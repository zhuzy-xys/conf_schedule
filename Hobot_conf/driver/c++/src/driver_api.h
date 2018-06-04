#ifndef DRIVER_API_H
#define DRIVER_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <string>

#include "hconf_common.h"
#include "driver_common.h"

// zookeeper event type constants
#define CREATED_EVENT_DEF            1
#define DELETED_EVENT_DEF            2
#define CHANGED_EVENT_DEF            3
#define CHILD_EVENT_DEF              4
#define SESSION_EVENT_DEF            -1
#define NOTWATCHING_EVENT_DEF        -2

/**
 * initialize current hconf environment before using hconf 
 *
 * @return: if success, return HCONF_OK
 *          if failed, return HCONF_ERR_OTHER
 */
int init_hconf_env();

/**
 * get the value of path
 *
 * @param path: the path like '/a/b/c' that kept in the zookeeper
 * @parma buf: the place that will keep the value, *buf will be malloced here, and you 
 *              should free it if it's no used
 * @param idc:  the place to get the nodes
 * @param flags: this indicates that operation if there is no value in the share memory
 *               HCONF_WAIT: it will wait until there is value in the share memory during certain time
 *               HCONF_NOWAIT: it will return immediately if there is no value in share memory
 *
 * @return: if success, return HCONF_OK
 *          if path exists in the zookeeper, but its value is NULL, return HCONF_ERR_NULL_VALUE, this may not be error
 *          if path is NULL or buf or buf_len is NULL, return HCONF_ERR_PARAM
 *          if get idc failed, return HCONF_ERR_GET_IDC
 *          other failed, return HCONF_ERR_OTHER
 */
int hconf_get(const std::string &path, std::string &buf, const std::string &idc, int flags);

/**
 * get the available services of path
 *
 * @param path: the path like '/a/b/c' that kept in the zookeeper
 * @param nodes: the place to keep the available services, the definition of String_vector_t is 
 *               struct String_vector {
 *                  int32_t count;
 *                  char * *data;
 *               };
 *               the data will be created by the program, you should call function deallocate_String_vector
 *               if the nodes are no used;
 *               more use of this struct, please refer zookeeper document
 * @param idc:  the place to get the nodes
 * @param flags: this indicates that operation if there is no value in the share memory
 *               HCONF_WAIT: it will wait until there is value in the share memory during certain time
 *               HCONF_NOWAIT: it will return immediately if there is no value in share memory
 *
 * @return: if success, return HCONF_OK
 *          if path exists in the zookeeper, but no available services, return HCONF_ERR_NULL_VALUE, this may not be error
 *          if path is NULL or buf or buf_len is NULL, return HCONF_ERR_PARAM
 *          if get idc failed, return HCONF_ERR_GET_IDC
 *          other failed, return HCONF_ERR_OTHER
 */
int hconf_get_children(const std::string &path, string_vector_t &nodes, const std::string &idc, int flags);

/**
 * get the children nodes of path including node's key and node's value
 *
 * @param path: the path like '/a/b/c' that kept in the zookeeper
 * @param len: the length of path
 * @param bnodes: the place to keep the children nodes including node's key and node's value
 * @param new_idc: the idc that will be used to get the nodes
 * @param flags: this indicates that operation if there is no value in the share memory
 *               HCONF_WAIT: it will wait until there is value in the share memory during certain time
 *               HCONF_NOWAIT: it will return immediately if there is no value in share memory
 *
 * @return: if success, return HCONF_OK
 *          if path exists in the zookeeper, but no children nodes, return HCONF_ERR_NULL_VALUE, this may not be error
 *          if path is NULL or buf or buf_len is NULL, return HCONF_ERR_PARAM
 *          if get idc failed, return HCONF_ERR_GET_IDC
 *          other failed, return HCONF_ERR_OTHER
 */
int hconf_get_batchnode(const std::string &path, hconf_batch_nodes &bnodes, const std::string &idc, int flags);

/**
 * get the children nodes of path only including nodes' key
 *
 * @param path: the path like '/a/b/c' that kept in the zookeeper
 * @param nodes: the place to keep the children nodes only including node's key
 * @param idc: the place to get the nodes
 * @param flags: this indicates that operation if there is no value in the share memory
 *               HCONF_WAIT: it will wait until there is value in the share memory during certain time
 *               HCONF_NOWAIT: it will return immediately if there is no value in share memory
 *
 * @return: if success, return HCONF_OK
 *          if path exists in the zookeeper, but no children nodes, return HCONF_ERR_NULL_VALUE, this may not be error
 *          if path is NULL or buf or buf_len is NULL, return HCONF_ERR_PARAM
 *          if get idc failed, return HCONF_ERR_GET_IDC
 *          other failed, return HCONF_ERR_OTHER
 */
int hconf_get_batchnode_keys(const std::string &path, string_vector_t &nodes, const std::string &idc, int flags);

#ifdef __cplusplus
}
#endif

#endif
