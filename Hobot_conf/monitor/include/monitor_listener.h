#ifndef SERVICELISTENER_H
#define SERVICELISTENER_H

#include "monitor_options.h"
#include "monitor_service_item.h"
#include "monitor_load_balance.h"

//op of mpdify serviceFatherToIp
const int kDelete = 0;
const int kAdd = 1;

class ServiceListener {
 public:
  ServiceListener(MonitorOptions *options);
  ~ServiceListener();
  int Init();
  void LoadAllService();
  int ZkModify(const std::string &path, const std::string &value);

 private:
  // Implement of interface for callback handle
  struct BalanceZkHandle : public MonitorZk::ZkCallBackHandle {
    MonitorOptions *options_;
    MonitorZk *monitor_zk_;

    void ModifyServiceFatherToIp(const int &op, const std::string& path);
    void ProcessDeleteEvent(const std::string& path);
    void ProcessChildEvent(const std::string &path);
    void ProcessChangedEvent(const std::string &path);
  };

  MonitorOptions *options_;
  MonitorZk *monitor_zk_;
  BalanceZkHandle cb_handle;

  void GetAllIp();
  int AddChildren(const std::string &service_father, struct String_vector &children);
  int LoadService(std::string &ip_port_path, std::string service_father, std::string ip_port);
};
#endif
