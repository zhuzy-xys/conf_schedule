#ifndef HCONF_ZK_H
#define HCONF_ZK_H

#include <string>
#include <vector>
#include "hconf_const.h"

/**
 *  Get conf from zookeeper
 */
int zk_get_node(zhandle_t *zh, const std::string &path, std::string &buf, int watcher);

/**
 * Create znode on zookeeper
 */
int zk_create_node(zhandle_t *zh, const std::string &path, const std::string &value, int flags);

    /**
 *  Get child nodes from zookeeper
 */
int zk_get_chdnodes(zhandle_t *zh, const std::string &path, string_vector_t &nodes);

/**
 *  Get child nodes together with their status
 */
int zk_get_chdnodes_with_status(zhandle_t *zh, const std::string &path, string_vector_t &nodes, std::vector<char> &status);

/**
 *  Create ephemeral node on zookeeper
 */
int zk_register_ephemeral(zhandle_t *zh, const std::string &path, const std::string &value);

/**
 * init zookeeper log stream
 */
int hconf_init_zoo_log(const std::string &log_dir, const std::string &zoo_log = HCONF_ZOO_LOG);

/**
 * Destroy zookeeper log stream
 */
void hconf_destroy_zoo_log();

/**
 * Check zookeeper path exists
 */
int zk_exists(zhandle_t *zh, const std::string &path);

#endif
