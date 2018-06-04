#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <zookeeper.h>

#include <string>

#include "hconf.h"
#include "hconf_log.h"
#include "driver_api.h"
#include "hconf_errno.h"
#include "driver_common.h"

using namespace std;

#ifdef HCONF_INTERNAL
static int hconf_get_batch_keys_native_(const char *path, string_vector_t *nodes, const char *idc, int flags);
#endif

static int get_node_path(const string &path, string &real_path);
static int hconf_get_conf_(const char *path, char *buf, size_t buf_len, const char *idc, int flags);
static int hconf_get_batch_conf_(const char *path, hconf_batch_nodes *bnodes, const char *idc, int flags);
static int hconf_get_batch_keys_(const char *path, string_vector_t *nodes, const char *idc, int flags);
static int hconf_get_allhost_(const char *path, string_vector_t *nodes, const char *idc, int flags);
static int hconf_get_host_(const char *path, char *buf, size_t buf_len, const char *idc, int flags);

int hconf_init()
{
    srand(time(NULL));
    return init_hconf_env();
}

int hconf_destroy()
{
    return HCONF_OK;
}

int init_string_vector(string_vector_t *nodes)
{
    if (NULL == nodes) return HCONF_ERR_PARAM;

    memset((void*)nodes, 0, sizeof(string_vector_t));
    return HCONF_OK;
}

int destroy_string_vector(string_vector_t *nodes)
{
    if (NULL == nodes) return HCONF_ERR_PARAM;

    deallocate_String_vector((String_vector*)nodes);

    nodes->count = 0;
    nodes->data = NULL;
    return HCONF_OK;
}

int hconf_get_conf(const char *path, char *buf, unsigned int buf_len, const char *idc)
{
    return hconf_get_conf_(path, buf, buf_len, idc, HCONF_WAIT);
}

int hconf_aget_conf(const char *path, char *buf, unsigned int buf_len, const char *idc)
{
    return hconf_get_conf_(path, buf, buf_len, idc, HCONF_NOWAIT);
}

int hconf_get_batch_conf(const char *path, hconf_batch_nodes *bnodes, const char *idc)
{
    return hconf_get_batch_conf_(path, bnodes, idc, HCONF_WAIT);
}

int hconf_aget_batch_conf(const char *path, hconf_batch_nodes *bnodes, const char *idc)
{
    return hconf_get_batch_conf_(path, bnodes, idc, HCONF_NOWAIT);
}

int hconf_get_batch_keys(const char *path, string_vector_t *nodes, const char *idc)
{
    return hconf_get_batch_keys_(path, nodes, idc, HCONF_WAIT);
}

int hconf_aget_batch_keys(const char *path, string_vector_t *nodes, const char *idc)
{
    return hconf_get_batch_keys_(path, nodes, idc, HCONF_NOWAIT);
}

#ifdef HCONF_INTERNAL
int hconf_get_batch_keys_native(const char *path, string_vector_t *nodes, const char *idc)
{
    return hconf_get_batch_keys_native_(path, nodes, idc, HCONF_WAIT);
}

int hconf_aget_batch_keys_native(const char *path, string_vector_t *nodes, const char *idc)
{
    return hconf_get_batch_keys_native_(path, nodes, idc, HCONF_NOWAIT);
}
#endif

int hconf_get_allhost(const char *path, string_vector_t *nodes, const char *idc)
{
    return hconf_get_allhost_(path, nodes, idc, HCONF_WAIT);
}

int hconf_aget_allhost(const char *path, string_vector_t *nodes, const char *idc)
{
    return hconf_get_allhost_(path, nodes, idc, HCONF_NOWAIT);
}

int hconf_get_host(const char *path, char *buf, unsigned int buf_len, const char *idc)
{
    return hconf_get_host_(path, buf, buf_len, idc, HCONF_WAIT);
}

int hconf_aget_host(const char *path, char *buf, unsigned int buf_len, const char *idc)
{
    return hconf_get_host_(path, buf, buf_len, idc, HCONF_NOWAIT);
}

const char* hconf_version()
{
    return HCONF_DRIVER_CC_VERSION;
}

static int hconf_get_host_(const char *path, char *buf, size_t buf_len, const char *idc, int flags)
{
    if (NULL == path || '\0' == *path || NULL == buf)
        return HCONF_ERR_PARAM;

    int ret = HCONF_OK;
    string_vector_t nodes;

    init_string_vector(&nodes);
    ret = hconf_get_allhost_(path, &nodes, idc, flags); 
    if (HCONF_OK != ret)
    {
        LOG_ERR("Failed to call get_allhost_! ret:%d", ret);
        return ret;
    }

    if (0 == nodes.count)
    {
        buf[0] = '\0';
        return ret;
    }

    unsigned int r = rand() % nodes.count;
    size_t node_len = strlen(nodes.data[r]);
    if (node_len >= buf_len)
    {
        destroy_string_vector(&nodes);
        return HCONF_ERR_BUF_NOT_ENOUGH;
    }

    memcpy(buf, nodes.data[r], node_len);
    buf[node_len] = '\0';

    destroy_string_vector(&nodes);

    return ret;
}

static int hconf_get_allhost_(const char *path, string_vector_t *nodes, const char *idc, int flags)
{
    if (NULL == path || '\0' == *path || NULL == nodes)
        return HCONF_ERR_PARAM;

    string tmp_buf;
    string tmp_idc;
    int ret = HCONF_OK;
    string real_path;

    ret = get_node_path(string(path), real_path);
    if (HCONF_OK != ret) return ret;

    if (0 != nodes->count) destroy_string_vector(nodes);

    if (NULL != idc) tmp_idc.assign(idc);

    ret = hconf_get_children(real_path, *nodes, tmp_idc, flags);
    if (HCONF_OK != ret)
    {
        LOG_ERR("Failed to get children services! ret:%d", ret);
        return ret;
    }

    return ret;
}

static int hconf_get_conf_(const char *path, char *buf, size_t buf_len, const char *idc, int flags)
{
    if (NULL == path || '\0' == *path || NULL == buf)
        return HCONF_ERR_PARAM;

    string tmp_buf;
    string tmp_idc;
    int ret = HCONF_OK;
    string real_path;

    ret = get_node_path(string(path), real_path);
    if (HCONF_OK != ret) return ret;

    if (NULL != idc) tmp_idc.assign(idc);

    ret = hconf_get(real_path, tmp_buf, tmp_idc, flags);
    if (HCONF_OK != ret) return ret;

    if (tmp_buf.size() >= buf_len)
    {
        LOG_ERR("buf is not enough! value len:%zd, buf len:%zd",
                tmp_buf.size(), buf_len);
        return HCONF_ERR_BUF_NOT_ENOUGH;
    }

    memcpy(buf, tmp_buf.data(), tmp_buf.size());
    buf[tmp_buf.size()] = '\0';

    return ret;
}

static int hconf_get_batch_conf_(const char *path, hconf_batch_nodes *bnodes, const char *idc, int flags)
{
    if (NULL == path || '\0' == *path || NULL == bnodes)
        return HCONF_ERR_PARAM;

    string tmp_buf;
    string tmp_idc;
    int ret = HCONF_OK;
    string real_path;

    ret = get_node_path(string(path), real_path);
    if (HCONF_OK != ret) return ret;

    if (0 != bnodes->count) destroy_hconf_batch_nodes(bnodes);

    if (NULL != idc) tmp_idc.assign(idc);

    ret = hconf_get_batchnode(real_path, *bnodes, tmp_idc, flags);
    if (HCONF_OK != ret)
    {
        LOG_ERR("Failed to call hconf_get_batchnode! ret:%d", ret);
        return ret;
    }

    return ret;
}

static int hconf_get_batch_keys_(const char *path, string_vector_t *nodes, const char *idc, int flags)
{
    if (NULL == path || '\0' == *path || NULL == nodes)
        return HCONF_ERR_PARAM;

    string tmp_buf;
    string tmp_idc;
    int ret = HCONF_OK;
    string real_path;

    ret = get_node_path(string(path), real_path);
    if (HCONF_OK != ret) return ret;

    if (0 != nodes->count) destroy_string_vector(nodes);

    if (NULL != idc) tmp_idc.assign(idc);

    ret = hconf_get_batchnode_keys(real_path, *nodes, tmp_idc, flags);
    if (HCONF_OK != ret)
    {
        LOG_ERR("Failed to get batchnode keys! ret:%d", ret);
        return ret;
    }

    return ret;
}

#ifdef HCONF_INTERNAL
static int hconf_get_batch_keys_native_(const char *path, string_vector_t *nodes, const char *idc, int flags)
{
    if (NULL == path || '\0' == *path || NULL == nodes)
        return HCONF_ERR_PARAM;

    int ret = HCONF_OK;
    string tmp_idc;

    if (0 != nodes->count) destroy_string_vector(nodes);

    if (NULL != idc) tmp_idc.assign(idc);

    ret = hconf_get_batchnode_keys(string(path), *nodes, tmp_idc, flags);
    if (HCONF_OK != ret)
    {
        LOG_ERR("Failed to get batchnode keys! ret:%d", ret);
        return ret;
    }

    return ret;
}
#endif

static int get_node_path(const string &path, string &real_path)
{
    if (0 == path.size()) return HCONF_ERR_PARAM;

    char delim = '/';
    size_t deal_path_len = 0;
    const char *end_pos = NULL;
    const char *start_pos = NULL;

    start_pos = path.data();
    end_pos = path.data() + path.size() - 1;

    while (*start_pos == delim && start_pos <= end_pos)
    {
        start_pos++;
    }
    while (*end_pos == delim && start_pos <= end_pos)
    {
        end_pos--;
    }
    
    if (start_pos > end_pos)
    {
        LOG_ERR("path:%s is not right", path.c_str());
        return HCONF_ERR_PARAM;
    }

    deal_path_len = end_pos + 1 - start_pos;

#ifdef HCONF_INTERNAL
    real_path.assign(HCONF_PREFIX);
    real_path.append(start_pos, deal_path_len);
#else
    real_path.assign(1, delim);
    real_path.append(start_pos, deal_path_len);
#endif
   
    return HCONF_OK;
}
