#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <errno.h>

#include <iostream>
#include <vector>
#include <string>

#include "hconf_log.h"
#include "hconf_shm.h"
#include "hconf_msg.h"
#include "hconf_errno.h"
#include "hconf_format.h"
#include "driver_api.h"

using namespace std;

static qhasharr_t *_hconf_hashtbl  = NULL;
static key_t _hconf_hashtbl_key    = HCONF_DEFAULT_SHM_KEY;

static int _hconf_msqid            = HCONF_INVALID_SEM_ID;
static key_t _hconf_msqid_key      = HCONF_DEFAULT_MSG_QUEUE_KEY;

static int init_shm(); 
static int init_msg();
static int send_msg_to_agent(int msqid, const string &idc, const string &path, char data_type);
static int hconf_get_(const string &path, string &tblval, char dtype, const string &idc, int flags);


int init_hconf_env()
{
    int ret = init_shm();
    if (HCONF_OK != ret) return ret;

    ret = init_msg();

    return ret; 
}

static int init_shm()
{
    int ret = HCONF_OK;

    if (NULL != _hconf_hashtbl) return ret;

    ret = init_hash_tbl(_hconf_hashtbl, _hconf_hashtbl_key, 0444, SHM_RDONLY);
    if (HCONF_OK != ret)
    {
        LOG_FATAL_ERR("Failed to init hash table! key:%#x", _hconf_hashtbl_key);
        return ret; 
    }

    return ret;
}

static int init_msg()
{
    int ret = HCONF_OK;

    if (HCONF_INVALID_SEM_ID != _hconf_msqid) return ret;

    ret = init_msg_queue(_hconf_msqid_key, _hconf_msqid);
    if (HCONF_OK != ret)
    {
        LOG_FATAL_ERR("Failed to init msg queue! key:%#x", _hconf_msqid_key);
        return ret;
    }

    return ret;
}

/**
 * Get child nodes from hash table(share memory)
 */
int hconf_get_children(const string &path, string_vector_t &nodes, const string &idc, int flags)
{
    if (path.empty()) return HCONF_ERR_PARAM;

    string tblval;

    int ret = hconf_get_(path, tblval, HCONF_DATA_TYPE_SERVICE, idc, flags);
    if (HCONF_OK != ret) return ret;

    ret = tblval_to_chdnodeval(tblval, nodes);
    return ret;
}

int hconf_get_batchnode(const string &path, hconf_batch_nodes &bnodes, const string &idc, int flags)
{
    if (path.empty()) return HCONF_ERR_PARAM;

    string buf;
    string child_path;
    int ret = HCONF_OK;
    string_vector_t nodes;

    memset(&nodes, 0, sizeof(string_vector_t));
    ret = hconf_get_batchnode_keys(path, nodes, idc, flags);

    if (HCONF_OK == ret)
    {
        bnodes.count = nodes.count;
        if (0 == bnodes.count)
        {
            bnodes.nodes = NULL;
            return ret;
        }

        bnodes.nodes = (hconf_node*) calloc(bnodes.count, sizeof(hconf_node));
        if (NULL == bnodes.nodes)
        {
            LOG_ERR("Failed to malloc bnodes->nodes! errno:%d", errno);
            free_string_vector(nodes, nodes.count);
            bnodes.count = 0;
            return HCONF_ERR_MEM;
        }

        for (int i = 0; i < nodes.count; i++)
        {
            child_path = path + "/" + nodes.data[i];

            ret = hconf_get(child_path, buf, idc, flags);
            if (HCONF_OK == ret)
            {
                bnodes.nodes[i].value = strndup(buf.c_str(), buf.size() + 1);
                if (NULL == bnodes.nodes[i].value)
                {
                    LOG_ERR("Failed to strdup value of path:%s! errno:%d",
                            child_path.c_str(), errno);
                    free_string_vector(nodes, nodes.count);
                    free_hconf_batch_nodes(&bnodes, i);
                    return HCONF_ERR_MEM;
                }

                bnodes.nodes[i].key = nodes.data[i];
                nodes.data[i] = NULL;
            }
            else
            {
                LOG_ERR("Failed to call hconf_get! ret:%d", ret);
                free_string_vector(nodes, nodes.count);
                free_hconf_batch_nodes(&bnodes, i);
                return ret;
            }
        }

        free_string_vector(nodes, 0);
    }
    else
    {
        LOG_ERR("Failed to get batch node keys! path:%s, idc:%s ret:%d",
                path.c_str(), idc.c_str(), ret);
    }

    return ret;
}

int hconf_get_batchnode_keys(const string &path, string_vector_t &nodes, const string &idc, int flags)
{
    if (path.empty()) return HCONF_ERR_PARAM;

    string tblval;

    int ret = hconf_get_(path, tblval, HCONF_DATA_TYPE_BATCH_NODE, idc, flags);
    if (HCONF_OK != ret) return ret;

    ret = tblval_to_batchnodeval(tblval, nodes);
    return ret;
}

/**
 * Get child nodes from hash table(share memory)
 */
int hconf_get(const string &path, string &buf, const string &idc, int flags)
{
    if (path.empty()) return HCONF_ERR_PARAM;

    string tblval;

    int ret = hconf_get_(path, tblval, HCONF_DATA_TYPE_NODE, idc, flags);
    if (HCONF_OK != ret) return ret;

    ret = tblval_to_nodeval(tblval, buf);
    return ret;
}

static int hconf_get_(const string &path, string &tblval, char dtype, const string &idc, int flags)
{
    int count = 0;
    string tblkey;
    int ret = HCONF_OK;

    ret = init_hconf_env();
    if (HCONF_OK != ret) return ret;

    string tmp_idc(idc);
    if (idc.empty())
    {
        ret = hconf_get_localidc(_hconf_hashtbl, tmp_idc);
        if (HCONF_OK != ret)
        {
            LOG_ERR("Failed to get local idc! ret:%d", ret);
            return ret;
        }
    }

    ret = serialize_to_tblkey(dtype, tmp_idc, path, tblkey);
    if (HCONF_OK != ret) return ret;

    // get value from tbl
    ret = hash_tbl_get(_hconf_hashtbl, tblkey, tblval);
    if (HCONF_OK == ret) return ret;

    // Not get batch keys from share memory, then send message to agent
    int ret_snd = send_msg_to_agent(_hconf_msqid, tmp_idc, path, dtype);
    if (HCONF_OK != ret_snd)
    {
        LOG_ERR("Failed to send message to agent, ret:%d", ret_snd);
        return ret_snd;
    }

    // If not wait, then return directly
    if (HCONF_NOWAIT == flags) return ret;

    while (count < HCONF_MAX_GET_TIMES)
    {
        usleep(5000);
        count++;
        
        ret = hash_tbl_get(_hconf_hashtbl, tblkey, tblval);
        if (HCONF_OK == ret)
        {
            LOG_ERR("Wait time:%d*5ms, type:%c, idc:%s, path:%s", 
                    count, dtype, tmp_idc.c_str(), path.c_str());
            return ret;
        }
    }

    if (count >= HCONF_MAX_GET_TIMES)
    {
        LOG_FATAL_ERR("Failed to get value! wait time:%d*5ms, type:%c, idc:%s, path:%s, ret:%d",
                count, dtype, tmp_idc.c_str(), path.c_str(), ret);
    }

    return ret;
}

static int send_msg_to_agent(int msqid, const string &idc, const string &path, char data_type)
{
    string tblkey;
    serialize_to_tblkey(data_type, idc, path, tblkey);
    
    int ret = send_msg(msqid, tblkey);
    return ret;
}
