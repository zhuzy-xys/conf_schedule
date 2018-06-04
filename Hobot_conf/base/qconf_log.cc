#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>

#include <string>

#include "hconf_log.h"
#include "hconf_common.h"
#include "hconf_format.h"

using namespace std;

static const char *hconf_get_log_level(int level);
static int hconf_check_log(const struct tm *cur_tm);

static string           _hconf_log_path;
static string           _hconf_log_path_fmt;
static int              _hconf_log_level = HCONF_LOG_LVL_MAX; 
static FILE             *_hconf_log_fp = NULL;
static pthread_mutex_t  _hconf_log_mutex = PTHREAD_MUTEX_INITIALIZER;


void hconf_set_log_level(int level)
{
    _hconf_log_level = level;
}

void hconf_set_log_fmt(const string &log_path_fmt)
{
    if (!log_path_fmt.empty()) _hconf_log_path_fmt = log_path_fmt;
}

void hconf_log_init(const string &log_path_fmt, int level)
{
    hconf_set_log_fmt(log_path_fmt);
    hconf_set_log_level(level);
}

void hconf_close_log_stream()
{
    if (NULL != _hconf_log_fp && stderr != _hconf_log_fp)
    {
        fclose(_hconf_log_fp);
        _hconf_log_fp = NULL;
    }
}

void hconf_destroy_log()
{
    hconf_close_log_stream();
    pthread_mutex_destroy(&_hconf_log_mutex);
}

void hconf_print_log(const char* file_path, int line_no, int log_level, const char* format, ...)
{
    if (log_level < _hconf_log_level || NULL == file_path || NULL == format)
        return;

    int n = 0;
    time_t cur;
    int ret = HCONF_OK;
    struct tm cur_tm = {0};
    const char* file_name = NULL;
    char log_buf[HCONF_MAX_BUF_LEN] = {0};

    file_name = strrchr(file_path, '/');
    file_name = (file_name == NULL) ? file_path : (file_name + 1);

    pthread_mutex_lock(&_hconf_log_mutex);

    time(&cur);
    localtime_r(&cur, &cur_tm);

    ret = hconf_check_log(&cur_tm);
    if (HCONF_OK == ret)
    {
        n = snprintf(log_buf, sizeof(log_buf),
                     "%4d/%02d/%02d %02d:%02d:%02d - [pid: %d][%s][%s %d] - ",
                     cur_tm.tm_year + 1900,
                     cur_tm.tm_mon + 1,
                     cur_tm.tm_mday,
                     cur_tm.tm_hour,
                     cur_tm.tm_min,
                     cur_tm.tm_sec,
                     getpid(),
                     hconf_get_log_level(log_level),
                     file_name,
                     line_no);

        va_list arg_ptr;
        va_start(arg_ptr, format);
        vsnprintf(log_buf + n, sizeof(log_buf) - n - 2, format, arg_ptr);
        va_end(arg_ptr);
        strcat(log_buf, "\n");

        fwrite(log_buf, sizeof(char), strlen(log_buf), _hconf_log_fp);
        fflush(_hconf_log_fp);
    }

    pthread_mutex_unlock(&_hconf_log_mutex);
}

static int hconf_check_log(const struct tm *cur_tm)
{
    int ret = HCONF_OK;
    size_t len = 0;
    char log_path[HCONF_FILE_PATH_LEN] = {0};

    if (_hconf_log_path_fmt.empty())
    {
        _hconf_log_fp = _hconf_log_fp == NULL ? stderr : _hconf_log_fp;
        return HCONF_OK;
    }

    len = strftime(log_path, sizeof(log_path), _hconf_log_path_fmt.c_str(), cur_tm); 

    if (strncmp(log_path, _hconf_log_path.c_str(), len) != 0)
    {
        if (_hconf_log_fp != NULL && _hconf_log_fp != stderr)
        {
            fclose(_hconf_log_fp);
            _hconf_log_fp = NULL;
        }

        _hconf_log_path.assign(log_path);
        _hconf_log_fp = fopen(_hconf_log_path.c_str(), "a");
        ret = (_hconf_log_fp == NULL) ? HCONF_ERR_OPEN : ret;
    }
    else
    {
        if (NULL == _hconf_log_fp)
        {
            _hconf_log_fp = fopen(_hconf_log_path.c_str(), "a");
            ret = (_hconf_log_fp == NULL) ? HCONF_ERR_OPEN : ret;
        }
    }

    return ret;
}

static const char *hconf_get_log_level(int level)
{
    switch (level)
    {
    case HCONF_LOG_FATAL_ERR:
        return "FATAL ERROR";
    case HCONF_LOG_ERR:
        return "ERROR";
    case HCONF_LOG_WARN:
        return "WARNING";
    case HCONF_LOG_INFO:
        return "INFO";
    case HCONF_LOG_TRACE:
        return "TRACE";
    case HCONF_LOG_DEBUG:
        return "DEBUG";
    default:
        return "UNKNOWN";
    }
}

void hconf_print_key_info(const char* file_path, int line_no, const string &tblkey, const char *format, ...)
{
    string idc;
    string path;
    char data_type;
    char buf[HCONF_MAX_BUF_LEN] = {0};

    va_list arg_ptr;
    va_start(arg_ptr, format);
    int n = vsnprintf(buf, sizeof(buf), format, arg_ptr);
    va_end(arg_ptr);

    if (n >= (int)sizeof(buf)) return;

    deserialize_from_tblkey(tblkey, data_type, idc, path);
    snprintf(buf + n, sizeof(buf) - n, "; data type:%c, idc:%s, path:%s",
            data_type, idc.c_str(), path.c_str());
    hconf_print_log(file_path, line_no, HCONF_LOG_ERR, "%s", buf);
}
