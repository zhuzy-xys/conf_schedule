#ifndef HCONF_DAEMON_H
#define HCONF_DAEMON_H

#include <string>

int check_proc_exist(const std::string &pid_file, int &pid_fd);
void write_pid(int fd, pid_t chd_pid);
int hconf_agent_daemon_keepalive(const std::string &pid_file);

#endif
