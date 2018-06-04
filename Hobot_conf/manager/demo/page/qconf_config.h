#ifndef HCONF_CONFIG_H
#define HCONF_CONFIG_H

#include <string>
#include <map>

#define HCONF_OK                            0
#define HCONF_ERR_OTHER                     -1
#define HCONF_ERR_PARAM                     1
#define HCONF_ERR_NOT_FOUND                 10  
#define HCONF_ERR_INVALID_IP                30
#define HCONF_ERR_OPEN                      51

/**
 * Read configuration from file
 */
int hconf_load_conf();

/**
 * Get host value of idc
 */
int get_host(const std::string &idc, std::string &value);

/**
 * get all idcs
 */
const std::map<std::string, std::string> get_idc_map();

/**
 * Destory environment
 */
void hconf_destroy_conf_map();

#endif
