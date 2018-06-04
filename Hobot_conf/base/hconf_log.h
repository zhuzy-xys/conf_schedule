#ifndef HCONF_LOG_H
#define HCONF_LOG_H

#include <string>

#define HCONF_LOG_LVL_MAX           10
#define HCONF_LOG_FATAL_ERR         5
#define HCONF_LOG_ERR               4
#define HCONF_LOG_WARN              3
#define HCONF_LOG_INFO              2
#define HCONF_LOG_TRACE             1
#define HCONF_LOG_DEBUG             0

#define LOG_FATAL_ERR(format, ...) hconf_print_log(__FILE__, __LINE__, HCONF_LOG_FATAL_ERR, format, ## __VA_ARGS__)
#define LOG_ERR(format, ...)       hconf_print_log(__FILE__, __LINE__, HCONF_LOG_ERR, format, ## __VA_ARGS__)
#define LOG_WARN(format, ...)      hconf_print_log(__FILE__, __LINE__, HCONF_LOG_WARN, format, ## __VA_ARGS__)
#define LOG_INFO(format, ...)      hconf_print_log(__FILE__, __LINE__, HCONF_LOG_INFO, format, ## __VA_ARGS__)
#define LOG_TRACE(format, ...)     hconf_print_log(__FILE__, __LINE__, HCONF_LOG_TRACE, format, ## __VA_ARGS__)
#define LOG_DEBUG(format, ...)     hconf_print_log(__FILE__, __LINE__, HCONF_LOG_DEBUG, format, ## __VA_ARGS__)

#define HCONF_FUNC_IN() LOG_INFO("[ %s ] in ...", __FUNCTION__)
#define HCONF_FUNC_OUT() LOG_INFO("[ %s ] out ...", __FUNCTION__)


void hconf_log_init(const std::string &log_path_fmt, int level);
void hconf_destroy_log();
void hconf_set_log_fmt(const std::string &log_path_fmt);
void hconf_set_log_level(int level);
void hconf_close_log_stream();
void hconf_print_log(const char* file_path, int line_no, int log_level, const char* format, ...);

/**
 * print tblkey information
 */
void hconf_print_key_info(const char* file_path, int line_no, const std::string &tblkey, const char *format, ...);

#endif
