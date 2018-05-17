#include "monitor_check_thread.h"
#include "monitor_work_thread.h"
#include "monitor_options.h"
#include "monitor_const.h"
#include "monitor_log.h"
#include "monitor_zk.h"

// Traversal all the service under the service father to judge weather it's only one service up
static bool IsOnlyOneUp(const std::string &node, MonitorOptions *options) {
  bool ret = true;
  int alive = 0;
  std::string ip_port_path;
  std::string service_father = node.substr(0, node.rfind('/'));
  std::set<std::string> &ipset = options->service_father_to_ip[service_father];
  for (auto &ip_port : ipset) {
    ip_port_path = service_father + "/" + ip_port;
    int status = options->service_map[ip_port_path].status;
    if (status == kStatusUp) ++alive;
    if (alive > 1) return false;
  }
  return ret;
}

// Update service thread. comes first update first
void UpdateServiceFunc(void *arg) {
  UpdateServiceArgs *update_service_args= static_cast<UpdateServiceArgs *>(arg);
  std::string ip_port = update_service_args->ip_port;
  int new_status = update_service_args->new_status;
  MonitorOptions *options = update_service_args->options;
  delete update_service_args;

  int old_status = (options->service_map[ip_port]).status;

  // Compare the new status and old status to decide weather to update status
  if (new_status == kStatusDown && old_status == kStatusUp &&
      IsOnlyOneUp(ip_port, options)) {
    LOG(LOG_FATAL_ERROR, "Maybe %s is the last server that is up. \
        But monitor CAN NOT connect to it. its Status will not change!", ip_port.c_str());
    return;
  }

  // Update in zookeeper and ServiceItem
  MonitorZk *zk = WorkThread::GetZkInstance(options);
  int ret = zk->zk_modify(ip_port, to_string(new_status));
  if (ret == kSuccess)
    options->service_map[ip_port].status = new_status;
  else
    LOG(LOG_ERROR, "Can not update zookeeper.");
}
