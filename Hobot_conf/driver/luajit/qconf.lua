--[[
-- 1. current hconf just supports luajit
--
-- 2. before the program, please make sure that libhconf.so has been installed
--
--]]

local ffi = require('ffi')
-- local hconf = ffi.load("/absolute/libhconf.so")
local hconf = ffi.load("libhconf.so")

ffi.cdef[[
struct string_vector
{
    int count;      // the number of services
    char **data;    // the array of services
};
typedef struct string_vector string_vector_t;

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

int hconf_init();
int hconf_destroy();
int hconf_get_conf(const char *path, char *buf, int buf_len, const char *idc);
int hconf_get_batch_conf(const char *path, hconf_batch_nodes *bnodes, const char *idc);
int hconf_get_batch_keys(const char *path, string_vector_t *nodes, const char *idc);

int init_string_vector(string_vector_t *nodes);
int destroy_string_vector(string_vector_t *nodes);
int init_hconf_batch_nodes(hconf_batch_nodes *bnodes);
int destroy_hconf_batch_nodes(hconf_batch_nodes *bnodes);
int hconf_get_allhost(const char *path, string_vector_t *nodes, const char *idc);
int hconf_get_host(const char *path, char *buf, int buf_len, const char *idc);

const char* hconf_version();
]]

-- define the max length of conf buffer which is 1M
-- define the max length of one ip and port which is 256
local hconf_conf_buf_max_len = 1048576
local hconf_host_buf_max_len = 256

-- define hconf lua version
local hconf_version = "1.2.2"

-- define the hconf errors flags
local hconf_errors = {
    ["-1"]     = 'HCONF_ERR_OTHER',
    ["0"]      = 'HCONF_OK',
    ["1"]      = 'HCONF_ERR_PARAM',
    ["2"]      = 'HCONF_ERR_MEM',
    ["3"]      = 'HCONF_ERR_TBL_SET',
    ["4"]      = 'HCONF_ERR_GET_HOST',
    ["5"]      = 'HCONF_ERR_GET_IDC',
    ["6"]      = 'HCONF_ERR_BUF_NOT_ENOUGH',
    ["7"]      = 'HCONF_ERR_DATA_TYPE',
    ["8"]      = 'HCONF_ERR_DATA_FORMAT',
    ["9"]      = 'HCONF_ERR_NULL_VALUE',
    -- key is not found in hash table
    ["10"]     = 'HCONF_ERR_NOT_FOUND',
    ["11"]     = 'HCONF_ERR_OPEN_DUMP',
    ["12"]     = 'HCONF_ERR_OPEN_TMP_DUMP',
    ["13"]     = 'HCONF_ERR_NOT_IN_DUMP',
    ["14"]     = 'HCONF_ERR_RENAME_DUMP',
    ["15"]     = 'HCONF_ERR_WRITE_DUMP',
    ["16"]     = 'HCONF_ERR_SAME_VALUE',
    -- error of number
    ["20"]     = 'HCONF_ERR_OUT_OF_RANGE',
    ["21"]     = 'HCONF_ERR_NOT_NUMBER',
    ["22"]     = 'HCONF_ERR_OTHRE_CHARACTER',
    -- error of ip and port
    ["30"]     = 'HCONF_ERR_INVALID_IP',
    ["31"]     = 'HCONF_ERR_INVALID_PORT',
    -- error of msgsnd and msgrcv
    ["40"]     = 'HCONF_ERR_NO_MESSAGE',
    ["41"]     = 'HCONF_ERR_E2BIG',
    -- error of hostname
    ["71"]     = 'HCONF_ERR_HOSTNAME',
    -- error, not init while using hconf c library
    ["81"]     = 'HCONF_ERR_CC_NOT_INIT',
}

-- init the hconf environment
local ret = hconf.hconf_init()
if ret ~= 0 then
    os.exit(-1)
end

-- hconf interned buff
-- Lua string is an (interned) copy of the data and bears no relation to the original data area anymore. 
-- http://luajit.org/ext_ffi_api.html#ffi_string
local buff = ffi.new("char[?]", hconf_conf_buf_max_len, {0})

-- hconf get_conf function
local function get_conf(key, idc)
    local ret = hconf.hconf_get_conf(key, buff, hconf_conf_buf_max_len, idc)

    if ret == 0 then
        return ret, ffi.string(buff)
    else
        return ret, hconf_errors[tostring(ret)]
    end
end

-- hconf get_allhost function
local function get_allhost(key, idc)
    local nodes = ffi.new("string_vector_t")
    local ret = hconf.init_string_vector(nodes)
    if ret ~= 0 then
        return ret, hconf_errors[tostring(ret)]
    end

    ret = hconf.hconf_get_allhost(key, nodes, idc)
    if ret ~= 0 then
        return ret, hconf_errors[tostring(ret)]
    end

    local arr = {}

    for v = 0, nodes.count - 1 do
        arr[v + 1] = ffi.string(nodes.data[v])
    end

    hconf.destroy_string_vector(nodes)

    return ret, arr
end

-- hconf get_host function
local function get_host(key, idc)
    local ret = hconf.hconf_get_host(key, buff, hconf_host_buf_max_len, idc)

    if ret == 0 then
        return ret, ffi.string(buff)
    else
        return ret, hconf_errors[tostring(ret)]
    end
end

-- hconf get_batch_conf function
local function get_batch_conf(key, idc)
    local bnodes = ffi.new("hconf_batch_nodes")
    local ret = hconf.init_hconf_batch_nodes(bnodes)
    if ret ~= 0 then
        return ret, hconf_errors[tostring(ret)]
    end

    ret = hconf.hconf_get_batch_conf(key, bnodes, idc)
    if ret ~= 0 then
        return ret, hconf_errors[tostring(ret)]
    end

    local arr = {}

    for v = 0, bnodes.count - 1 do
        arr[ffi.string(bnodes.nodes[v].key)] = ffi.string(bnodes.nodes[v].value)
    end

    hconf.destroy_hconf_batch_nodes(bnodes)

    return ret, arr
end

-- hconf get_batch_keys function
local function get_batch_keys(key, idc)
    local nodes = ffi.new("string_vector_t")
    local ret = hconf.init_string_vector(nodes)
    if ret ~= 0 then
        return ret, hconf_errors[tostring(ret)]
    end

    ret = hconf.hconf_get_batch_keys(key, nodes, idc)
    if ret ~= 0 then
        return ret, hconf_errors[tostring(ret)]
    end

    local arr = {}

    for v = 0, nodes.count - 1 do
        arr[v + 1] = ffi.string(nodes.data[v])
    end

    hconf.destroy_string_vector(nodes)

    return ret, arr
end

-- hconf version
local function version()
    return 0, hconf_version
end

return {
    get_conf = get_conf,
    get_allhost = get_allhost,
    get_host = get_host,
    get_batch_conf = get_batch_conf,
    get_batch_keys = get_batch_keys,
    version = version
}
