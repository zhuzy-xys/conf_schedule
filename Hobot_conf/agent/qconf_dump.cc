#include <gdbm.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/ipc.h>

#include <string>
#include <iostream>

#include "hconf_log.h"
#include "hconf_shm.h"
#include "hconf_dump.h"
#include "hconf_common.h"
#include "hconf_format.h"

using namespace std;

const string HCONF_DUMP_DIR("/dumps");
const string HCONF_DUMP_PATH("/_dump.gdbm");

static string _hconf_dump_file;
static GDBM_FILE _hconf_dbf = NULL;
static pthread_mutex_t _hconf_dump_mutx = PTHREAD_MUTEX_INITIALIZER;

static int hconf_init_dbf_(int flags);

int hconf_init_dump_file(const string &agent_dir)
{
    _hconf_dump_file = agent_dir + HCONF_DUMP_DIR;
    mode_t mode = 0755;
    if (-1 == access(_hconf_dump_file.c_str(), F_OK))
    {
        if (-1 == mkdir(_hconf_dump_file.c_str(), mode))
        {
            LOG_ERR("Failed to create dump directory! errno:%d", errno);
            return HCONF_ERR_MKDIR;
        }
    }

    _hconf_dump_file.append(HCONF_DUMP_PATH);

    return hconf_init_dbf();
}

int hconf_init_dbf()
{
    return hconf_init_dbf_(GDBM_WRCREAT);
}

static int hconf_init_dbf_(int flags)
{
    if (_hconf_dump_file.empty())
    {
        LOG_ERR("Not init dump_file\n");
        return HCONF_ERR_OPEN_DUMP;
    }

    int mode = 0644;
    _hconf_dbf = gdbm_open(_hconf_dump_file.c_str(), 0, flags, mode, NULL);
    if (NULL == _hconf_dbf)
    {
        LOG_FATAL_ERR("Failed to open gdbm file:%s; gdbm err:%s", 
                _hconf_dump_file.c_str(), gdbm_strerror(gdbm_errno));
        return HCONF_ERR_OPEN_DUMP;
    }

    int value = 1;
    int ret = gdbm_setopt(_hconf_dbf, GDBM_COALESCEBLKS, &value, sizeof(value));
    if (ret != 0) {
        LOG_FATAL_ERR("Failed to setopt of gdbm file:%s; gdbm err:%s", 
                _hconf_dump_file.c_str(), gdbm_strerror(gdbm_errno));
        return HCONF_ERR_OPEN_DUMP;
    }

    return HCONF_OK;
}

void hconf_destroy_dump_lock()
{
    pthread_mutex_destroy(&_hconf_dump_mutx);
}

void hconf_destroy_dbf()
{
    if (NULL != _hconf_dbf)
    {
        gdbm_close(_hconf_dbf);
        _hconf_dbf = NULL;
    }
}

int hconf_dump_get(const string &tblkey, string &tblval)
{
    if (tblkey.empty()) return HCONF_ERR_PARAM;

    datum gdbm_key;
    datum gdbm_val;

    gdbm_key.dptr = (char*)tblkey.data();
    gdbm_key.dsize = tblkey.size();

    pthread_mutex_lock(&_hconf_dump_mutx);
    assert(NULL != _hconf_dbf);

    gdbm_val = gdbm_fetch(_hconf_dbf, gdbm_key);
    if (NULL == gdbm_val.dptr)
    {
        LOG_ERR_KEY_INFO(tblkey, "Failed to gdbm fetch! gdbm err:%s",
                gdbm_strerror(gdbm_errno));

        pthread_mutex_unlock(&_hconf_dump_mutx);
        return HCONF_ERR_NOT_FOUND;
    }
    pthread_mutex_unlock(&_hconf_dump_mutx);

    tblval.assign(gdbm_val.dptr, gdbm_val.dsize);
    free(gdbm_val.dptr);

    return HCONF_OK;
}

int hconf_dump_set(const string &tblkey, const string &tblval)
{
    if (tblkey.empty()) return HCONF_ERR_PARAM;

    datum gdbm_key;
    datum gdbm_val;
    int ret = HCONF_OK;

    gdbm_key.dptr = (char*)tblkey.data();
    gdbm_key.dsize = tblkey.size();
    gdbm_val.dptr = (char*)tblval.data();
    gdbm_val.dsize = tblval.size();

    pthread_mutex_lock(&_hconf_dump_mutx);
    assert(NULL != _hconf_dbf);

    ret = gdbm_store(_hconf_dbf, gdbm_key, gdbm_val, GDBM_REPLACE);
    if (0 != ret)
    {
        LOG_ERR_KEY_INFO(tblkey, "Failed to gdbm store! gdbm err:%s",
                gdbm_strerror(gdbm_errno));

        pthread_mutex_unlock(&_hconf_dump_mutx);
        return HCONF_ERR_WRITE_DUMP;
    }
    gdbm_sync(_hconf_dbf);
    pthread_mutex_unlock(&_hconf_dump_mutx);

    return HCONF_OK;
}

int hconf_dump_delete(const string &tblkey)
{
    if (tblkey.empty()) return HCONF_ERR_PARAM;

    datum gdbm_key;
    int ret = HCONF_OK;

    gdbm_key.dptr = (char*)tblkey.data();
    gdbm_key.dsize = tblkey.size();

    pthread_mutex_lock(&_hconf_dump_mutx);
    assert(NULL != _hconf_dbf);

    ret = gdbm_delete(_hconf_dbf, gdbm_key);
    if (0 != ret && GDBM_ITEM_NOT_FOUND != gdbm_errno)
    {
        LOG_ERR_KEY_INFO(tblkey, "Failed to gdbm delete! gdbm err:%s",
                gdbm_strerror(gdbm_errno));

        pthread_mutex_unlock(&_hconf_dump_mutx);
        return HCONF_ERR_DEL_DUMP;
    }
    gdbm_sync(_hconf_dbf);
    pthread_mutex_unlock(&_hconf_dump_mutx);

    return HCONF_OK;
}

int hconf_dump_clear()
{
    pthread_mutex_lock(&_hconf_dump_mutx);
    if (NULL != _hconf_dbf)
    {
        gdbm_close(_hconf_dbf);
        _hconf_dbf = NULL;
    }
    int ret = hconf_init_dbf_(GDBM_NEWDB);
    pthread_mutex_unlock(&_hconf_dump_mutx);
    
    return ret;
}

int hconf_dump_tbl(qhasharr_t *tbl)
{
    if (NULL == tbl) return HCONF_ERR_PARAM;

    string tblkey;
    string tblval;
    int ret = HCONF_OK;
    int max_slots = 0, used_slots = 0;

    // get key count
    hash_tbl_get_count(tbl, max_slots, used_slots);

    for (int idx = 0; idx < max_slots;)
    {
        ret = hash_tbl_getnext(tbl, tblkey, tblval, idx);

        if (HCONF_OK == ret) 
        {
            // write dump
            ret = hconf_dump_set(tblkey, tblval);
            if (HCONF_OK != ret) break;
        }
        else if (HCONF_ERR_TBL_END == ret)
        {
            ret = HCONF_OK;
        }
        else
        {
            LOG_ERR("Failed to hash_tbl_getnext!");
        }
    }

    return ret;
}
