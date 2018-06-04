#ifndef DRIVER_COMMON_H
#define DRIVER_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#ifndef __HCONF_BATCH_NODES__
#define __HCONF_BATCH_NODES__
typedef struct hconf_node
{
    char *key;
    char *value;
} hconf_node;

typedef struct hconf_batch_nodes
{
    int count;
    hconf_node *nodes;
} hconf_batch_nodes;
#endif


/**
 * Free hconf_batch_nodes according to the free_size
 */
void free_hconf_batch_nodes(hconf_batch_nodes *bnodes, size_t free_size);

/**
 * Init the nodes array for keeping batch conf
 * @Note: the function should be called before calling hconf_get_batchconf
 *
 * @param bnodes: the array for keeping batch nodes
 *
 * @return HCONF_Ok: if success
 *         HCONF_ERR_PARAM: if nodes is null
 *         HCONF_ERR_OTHER: other failed
 */
int init_hconf_batch_nodes(hconf_batch_nodes *bnodes);

/**
 * Destroy the array for keeping batch nodes
 * @Note: the function should be called after the last use of calling hconf_get_batchconf
 *
 * @param bnodes: the array for keeping batch nodes
 *
 * @return HCONF_Ok: if success
 *         HCONF_ERR_PARAM: if nodes is null
 *         HCONF_ERR_OTHER: other failed
 */
int destroy_hconf_batch_nodes(hconf_batch_nodes *bnodes);

#ifdef __cplusplus
}
#endif

#endif
