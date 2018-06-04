#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <string>
#include <vector>

#include "hconf_common.h"
#include "hconf_format.h"

using namespace std;

static int tblval_to_vectorval(const string &tblval, char data_type, string_vector_t &nodes, string &idc, string &path);

static void hconf_append_idc(string &tblkey, const string &idc);
static void hconf_append_path(string &tblkey, const string &path);
static void hconf_append_host(string &tblkey, const string &host);
static void hconf_append_nodeval(string &tblkey, const string &nodeval);

static int hconf_sub_idc(const string &tblkey, size_t &pos, string &idc);
static int hconf_sub_path(const string &tblkey, size_t &pos, string &path);
static int hconf_sub_host(const string &tblkey, size_t &pos, string &host);
static int hconf_sub_nodeval(const string &tblkey, size_t &pos, string &nodeval);
static int hconf_sub_vectorval(const string &tblval, size_t &pos, string_vector_t &nodes);

int serialize_to_tblkey(char data_type, const string &idc, const string &path, string &tblkey)
{
    tblkey.assign(1, data_type);
    switch (data_type)
    {
    case HCONF_DATA_TYPE_NODE:
    case HCONF_DATA_TYPE_SERVICE:
    case HCONF_DATA_TYPE_BATCH_NODE:
        hconf_append_idc(tblkey, idc);
        hconf_append_path(tblkey, path);
        return HCONF_OK;
    case HCONF_DATA_TYPE_ZK_HOST:
        hconf_append_idc(tblkey, idc);
        return HCONF_OK;
    case HCONF_DATA_TYPE_LOCAL_IDC:
        return HCONF_OK;
    default:
        return HCONF_ERR_DATA_TYPE;
    }
}

int deserialize_from_tblkey(const string &tblkey, char &data_type, string &idc, string &path)
{
    size_t pos = 0;

    if (tblkey.size() <= 0) return HCONF_ERR_DATA_FORMAT;
    data_type = tblkey[0];

    pos = 1;

    switch (data_type)
    {
        case HCONF_DATA_TYPE_NODE:
        case HCONF_DATA_TYPE_SERVICE:
        case HCONF_DATA_TYPE_BATCH_NODE:
            if (HCONF_OK != hconf_sub_idc(tblkey, pos, idc) ||
                    HCONF_OK != hconf_sub_path(tblkey, pos, path)) 
                return HCONF_ERR_DATA_FORMAT;
            return HCONF_OK;
        case HCONF_DATA_TYPE_ZK_HOST:
            if (HCONF_OK != hconf_sub_idc(tblkey, pos, idc))
                return HCONF_ERR_DATA_FORMAT;
            return HCONF_OK;
        case HCONF_DATA_TYPE_LOCAL_IDC:
            return HCONF_OK;
        default:
            return HCONF_ERR_DATA_TYPE;
    }
}

int localidc_to_tblval(const string &key, const string &local_idc, string &tblval)
{
    tblval.clear();
    hconf_append_idc(tblval, local_idc);
    tblval.append(key);

    return HCONF_OK;
}

int nodeval_to_tblval(const string &key, const string &nodeval, string &tblval)
{
    tblval.clear();
    hconf_append_nodeval(tblval, nodeval);
    tblval.append(key);

    return HCONF_OK;
}

int chdnodeval_to_tblval(const string &key, const string_vector_t &nodes, string &tblval, const vector<char> &valid_flg)
{
    HCONF_VECTOR_COUNT_TYPE valid_cnt = 0;
    HCONF_HOST_PATH_SIZE_TYPE node_size = 0;
    char buf[HCONF_VECTOR_COUNT_LEN] = {0};

    // set total children nodes count
    for (int i = 0; i < nodes.count; ++i)
    {
        if (STATUS_UP == valid_flg[i]) ++valid_cnt;
    }

    tblval.clear();
    hconf_encode_num(buf, valid_cnt, HCONF_VECTOR_COUNT_TYPE);
    tblval.append(buf, HCONF_VECTOR_COUNT_LEN);
    for (int i = 0; i < nodes.count; ++i)
    {
        if (STATUS_UP == valid_flg[i])
        {
            node_size = strlen(nodes.data[i]);
            hconf_encode_num(buf, node_size, HCONF_HOST_PATH_SIZE_TYPE);
            tblval.append(buf, HCONF_HOST_PATH_SIZE_LEN);
            tblval.append(nodes.data[i], node_size);
        }
    }
    tblval.append(key);

    return HCONF_OK;
}

int batchnodeval_to_tblval(const string &key, const string_vector_t &nodes, string &tblval)
{
    HCONF_VECTOR_COUNT_TYPE size = 0;
    char buf[HCONF_VECTOR_COUNT_LEN] = {0};

    tblval.clear();
    size = nodes.count;
    hconf_encode_num(buf, size, HCONF_VECTOR_COUNT_TYPE);
    tblval.append(buf, HCONF_VECTOR_COUNT_LEN);
    for (int i = 0; i < nodes.count; ++i)
    {
        size = strlen(nodes.data[i]);
        hconf_encode_num(buf, size, HCONF_HOST_PATH_SIZE_TYPE);
        tblval.append(buf, HCONF_HOST_PATH_SIZE_LEN);
        tblval.append(nodes.data[i], size);
    }
    tblval.append(key);

    return HCONF_OK;
}

int idcval_to_tblval(const string &key, const string &host, string &tblval)
{
    tblval.clear();
    hconf_append_host(tblval, host);
    tblval.append(key);

    return HCONF_OK;
}

char get_data_type(const string &value)
{
    if (value.empty()) return HCONF_DATA_TYPE_UNKNOWN;
    return value[0];
}

int tblval_to_localidc(const string &tblval, string &idc)
{
    size_t pos = 0;

    // idc
    if (HCONF_OK != hconf_sub_idc(tblval, pos, idc)) return HCONF_ERR_DATA_FORMAT;

    // data type
    if (tblval.size() < pos + 1 || tblval[pos] != HCONF_DATA_TYPE_LOCAL_IDC)
        return HCONF_ERR_DATA_FORMAT;

    return HCONF_OK;
}

int tblval_to_idcval(const string &tblval, string &host)
{
    size_t pos = 0;

    // host
    if (HCONF_OK != hconf_sub_host(tblval, pos, host)) return HCONF_ERR_DATA_FORMAT;
    
    // data type
    if (tblval.size() < pos + 1 || tblval[pos] != HCONF_DATA_TYPE_ZK_HOST)
        return HCONF_ERR_DATA_FORMAT;

    return HCONF_OK;
}

int tblval_to_idcval(const string &tblval, string &host, string &idc)
{
    size_t pos = 0;

    // host
    if (HCONF_OK != hconf_sub_host(tblval, pos, host)) return HCONF_ERR_DATA_FORMAT;

    // data type
    if (tblval.size() < pos + 1 || tblval[pos] != HCONF_DATA_TYPE_ZK_HOST)
        return HCONF_ERR_DATA_FORMAT;
    pos++;

    // idc
    if (HCONF_OK != hconf_sub_idc(tblval, pos, idc)) return HCONF_ERR_DATA_FORMAT;

    return HCONF_OK;
}

int tblval_to_nodeval(const string &tblval, string &nodeval)
{
    size_t pos = 0;
    int ret = HCONF_ERR_OTHER;
    
    // nodeval
    if (HCONF_OK != (ret = hconf_sub_nodeval(tblval, pos, nodeval)))
        return ret;
    
    // data type
    if (tblval.size() < pos + 1 || tblval[pos] != HCONF_DATA_TYPE_NODE)
        return HCONF_ERR_DATA_FORMAT;
    
    return HCONF_OK;
}

int tblval_to_nodeval(const string &tblval, string &nodeval, string &idc, string &path)
{
    size_t pos = 0;
    int ret = HCONF_ERR_OTHER;

    // nodeval
    if (HCONF_OK != (ret = hconf_sub_nodeval(tblval, pos, nodeval)))
        return ret;

    // data type
    if (tblval.size() < pos + 1 || tblval[pos] != HCONF_DATA_TYPE_NODE)
        return HCONF_ERR_DATA_FORMAT;
    pos++;

    // idc, path
    if (HCONF_OK != hconf_sub_idc(tblval, pos, idc) || 
            HCONF_OK != hconf_sub_path(tblval, pos, path))
        return HCONF_ERR_DATA_FORMAT;

    return HCONF_OK;
}

int tblval_to_chdnodeval(const string &tblval, string_vector_t &nodes)
{
    size_t pos = 0;
    int ret = hconf_sub_vectorval(tblval, pos, nodes);
    
    if (HCONF_OK != ret) return ret;
    
    // data type
    if (tblval.size() < pos + 1 || tblval[pos] != HCONF_DATA_TYPE_SERVICE)
        return HCONF_ERR_DATA_FORMAT;
    
    return HCONF_OK;
}

int tblval_to_batchnodeval(const string &tblval, string_vector_t &nodes)
{
    size_t pos = 0;
    int ret = hconf_sub_vectorval(tblval, pos, nodes);
    
    if (HCONF_OK != ret) return ret;
    
    // data type
    if (tblval.size() < pos + 1 || tblval[pos] != HCONF_DATA_TYPE_BATCH_NODE)
        return HCONF_ERR_DATA_FORMAT;

    return HCONF_OK;
}

int tblval_to_chdnodeval(const string &tblval, string_vector_t &nodes, string &idc, string &path)
{
    return tblval_to_vectorval(tblval, HCONF_DATA_TYPE_SERVICE, nodes, idc, path);
}

int tblval_to_batchnodeval(const string &tblval, string_vector_t &nodes, string &idc, string &path)
{
    return tblval_to_vectorval(tblval, HCONF_DATA_TYPE_BATCH_NODE, nodes, idc, path);
}

static int tblval_to_vectorval(const string &tblval, char data_type, string_vector_t &nodes, string &idc, string &path)
{
    size_t pos = 0;
    int ret = HCONF_OK;

    // nodes
    ret = hconf_sub_vectorval(tblval, pos, nodes);
    if (HCONF_OK != ret) return ret;

    // data type
    if (tblval.size() < pos + 1 || tblval[pos] != data_type)
        return HCONF_ERR_DATA_FORMAT;
    pos++;

    // idc
    if (HCONF_OK != hconf_sub_idc(tblval, pos, idc) || 
            HCONF_OK != hconf_sub_path(tblval, pos, path))
        return HCONF_ERR_DATA_FORMAT;

    return HCONF_OK;
}

void serialize_to_idc_host(const string &idc, const string &host, string &dest)
{
    dest.clear();
    hconf_append_idc(dest, idc);
    hconf_append_host(dest, host);
}

int deserialize_from_idc_host(const string &idc_host, string &idc, string &host)
{
    size_t pos = 0;
    if (HCONF_OK != hconf_sub_idc(idc_host, pos, idc) || 
            HCONF_OK != hconf_sub_host(idc_host, pos, host))
        return HCONF_ERR_DATA_FORMAT;

    return HCONF_OK;
}

static void hconf_append_idc(string &tblkey, const string &idc)
{
    hconf_string_append(tblkey, idc, HCONF_IDC_SIZE_TYPE);
}

static void hconf_append_path(string &tblkey, const string &path)
{
    hconf_string_append(tblkey, path, HCONF_HOST_PATH_SIZE_TYPE);
}

static void hconf_append_host(string &tblkey, const string &host)
{
    hconf_string_append(tblkey, host, HCONF_HOST_PATH_SIZE_TYPE);
}

static void hconf_append_nodeval(string &tblkey, const string &nodeval)
{
    hconf_string_append(tblkey, nodeval, HCONF_VALUE_SIZE_TYPE);
}

static int hconf_sub_idc(const string &tblkey, size_t &pos, string &idc)
{
    int ret = HCONF_ERR_OTHER;
    hconf_string_sub(tblkey, pos, idc, HCONF_IDC_SIZE_TYPE, ret);
    return ret;
}

static int hconf_sub_host(const string &tblkey, size_t &pos, string &host)
{
    int ret = HCONF_ERR_OTHER;
    hconf_string_sub(tblkey, pos, host, HCONF_HOST_PATH_SIZE_TYPE, ret);
    return ret;
}

static int hconf_sub_path(const string &tblkey, size_t &pos, string &path)
{
    int ret = HCONF_ERR_OTHER;
    hconf_string_sub(tblkey, pos, path, HCONF_HOST_PATH_SIZE_TYPE, ret);
    return ret;
}

static int hconf_sub_nodeval(const string &tblkey, size_t &pos, string &nodeval)
{
    int ret = HCONF_ERR_OTHER;
    hconf_string_sub(tblkey, pos, nodeval, HCONF_VALUE_SIZE_TYPE, ret);
    return ret;
}

static int hconf_sub_vectorval(const string &tblval, size_t &pos, string_vector_t &nodes)
{
    HCONF_VECTOR_COUNT_TYPE size = 0;

    // nodes
    if (tblval.size() < pos + HCONF_VECTOR_COUNT_LEN)
        return HCONF_ERR_DATA_FORMAT;

    hconf_decode_num(tblval.data() + pos, size, HCONF_VECTOR_COUNT_TYPE);
    nodes.count = size;
    pos += HCONF_VECTOR_COUNT_LEN;
    if (0 == nodes.count)
    {
        nodes.data = NULL;
        return HCONF_OK;
    }

    nodes.data = (char**)calloc(nodes.count, sizeof(char*));
    if (NULL == nodes.data) return HCONF_ERR_MEM;

    for (int i = 0; i < nodes.count; ++i)
    {
        if (tblval.size() < pos + HCONF_HOST_PATH_SIZE_LEN) 
            return HCONF_ERR_DATA_FORMAT;
        hconf_decode_num(tblval.data() + pos, size, HCONF_HOST_PATH_SIZE_TYPE);
        pos += HCONF_HOST_PATH_SIZE_LEN;
        if (tblval.size() < pos + size) return HCONF_ERR_DATA_FORMAT;

        nodes.data[i] = (char*)calloc(size + 1, sizeof(char));
        if (NULL == nodes.data[i])
        {
            free_string_vector(nodes, i);
            return HCONF_ERR_MEM;
        }
        memcpy(nodes.data[i], tblval.data() + pos, size);
        nodes.data[i][size] = '\0';
        pos += size;
    }

    return HCONF_OK;
}

int graynodeval_to_tblval(const set<string> &nodes, string &tblval)
{
    HCONF_VECTOR_COUNT_TYPE size = 0;
    char buf[HCONF_VECTOR_COUNT_LEN] = {0};

    tblval.clear();
    size = nodes.size();
    hconf_encode_num(buf, size, HCONF_VECTOR_COUNT_TYPE);
    tblval.append(buf, HCONF_VECTOR_COUNT_LEN);
    for (set<string>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
    {
        HCONF_VALUE_SIZE_TYPE len= 0;
        len = (*it).size();
        hconf_encode_num(buf, len, HCONF_VALUE_SIZE_TYPE);
        tblval.append(buf, HCONF_VALUE_SIZE_LEN);
        tblval.append(*it);
    }
    return HCONF_OK;
}

int tblval_to_graynodeval(const string &tblval, set<string> &nodes)
{
    nodes.clear();

    // nodes
    size_t pos = 0;
    HCONF_VECTOR_COUNT_TYPE size = 0;
    if (tblval.size() < pos + HCONF_VECTOR_COUNT_LEN) return HCONF_ERR_OTHER;
    hconf_decode_num(tblval.data() + pos, size, HCONF_VECTOR_COUNT_TYPE);
    pos += HCONF_VECTOR_COUNT_LEN;
    if (0 == size) return HCONF_OK;

    for (int i = 0; i < size; ++i)
    {
        HCONF_VALUE_SIZE_TYPE len = 0;
        if (tblval.size() < pos + HCONF_VALUE_SIZE_LEN) return HCONF_ERR_OTHER;
        hconf_decode_num(tblval.data() + pos, len, HCONF_VALUE_SIZE_TYPE);
        pos += HCONF_VALUE_SIZE_LEN;
        if (tblval.size() < pos + len) return HCONF_ERR_OTHER;

        nodes.insert(tblval.substr(pos, len));
        pos += len;
    }
    return HCONF_OK;
}
