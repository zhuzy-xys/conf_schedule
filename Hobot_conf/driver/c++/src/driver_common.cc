#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hconf_errno.h"
#include "driver_common.h"

int init_hconf_batch_nodes(hconf_batch_nodes *bnodes)
{
    if (NULL == bnodes) return HCONF_ERR_PARAM;

    memset((void*)bnodes, 0, sizeof(hconf_batch_nodes));

    return HCONF_OK;
}

int destroy_hconf_batch_nodes(hconf_batch_nodes *bnodes)
{
    if (NULL == bnodes) return HCONF_ERR_PARAM;

    free_hconf_batch_nodes(bnodes, bnodes->count);

    return HCONF_OK;
}

void free_hconf_batch_nodes(hconf_batch_nodes *bnodes, size_t free_size)
{
    if (NULL == bnodes) return;

    for (size_t i = 0; i < free_size; ++i)
    {
        free(bnodes->nodes[i].key);
        free(bnodes->nodes[i].value);
        bnodes->nodes[i].key = NULL;
        bnodes->nodes[i].value = NULL;
    }
    free(bnodes->nodes);
    bnodes->nodes = NULL;
    bnodes->count = 0;
}
