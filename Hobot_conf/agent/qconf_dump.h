#ifndef HCONF_DUMP_H
#define HCONF_DUMP_H

#include <string>

#include "qlibc.h"
#include "hconf_common.h"

int hconf_init_dump_file(const std::string &agent_dir);
int hconf_init_dbf();
void hconf_destroy_dbf();
void hconf_destroy_dump_lock();
int hconf_dump_get(const std::string &key, std::string &value);
int hconf_dump_set(const std::string &key, const std::string &value);
int hconf_dump_delete(const std::string &tblkey);
int hconf_dump_clear();
int hconf_dump_tbl(qhasharr_t *tbl);

#endif
