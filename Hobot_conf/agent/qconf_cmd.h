#ifndef HCONF_CMD_H
#define HCONF_CMD_H

#include <string>

size_t hconf_cmd_proc();
int hconf_init_cmd_env(const std::string &hconf_dir);
int hconf_write_file(const std::string &file_str, const std::string &content, int append);

#endif
