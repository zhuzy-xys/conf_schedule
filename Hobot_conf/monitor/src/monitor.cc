#include <fstream>

#include "monitor_options.h"
#include "monitor_const.h"
#include "monitor_log.h"
#include "monitor_process.h"
#include "monitor_work_thread.h"

int main(int argc, char** argv) {
  int ret = kSuccess;
  MonitorOptions *options = new MonitorOptions(kConfPath);
  if ((ret = options->Load()) != 0)
    return ret;

  WorkThread *work_thread = new WorkThread(options);

  process::options = options;
  process::need_restart = false;
  if (process::IsProcessRunning(process::kMonitorProcessName)) {
    LOG(LOG_ERROR, "Monitor is already running.");
    return kOtherError;
  }
  if (options->daemon_mode)
    process::Daemonize();

  if (options->auto_restart) {
    int child_exit_status = -1;
    int ret = process::ProcessKeepalive(child_exit_status, process::kPidFile);
    // Parent process
    if (ret > 0)
      return child_exit_status;
    else if (ret < 0)
      return kOtherError;
    else {
      std::ofstream pidstream(process::kPidFile);
      pidstream << getpid() << endl;
      pidstream.close();
    }
  }

  work_thread->Start();
  delete work_thread;

  return 0;
}
