#include <errno.h>
#include <regex.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <map>
#include <string>
#include <iostream>
#include <sstream>

#include "hconf_config.h"

using namespace std;

//length of one line in config file and path
#define HCONF_LINE_MAX_LEN			        1024

// configure file name
#define HCONF_IDC_CONF_PATH     		    "conf/idc.conf"

static string strtrim(const string &cnt);
static int is_ip_port(const string &item);
static int get_first_host_by_name(const string &domain, string &ip_address);
static int is_domain_port(const string &item);
static int is_valid_idc(const string &idc, const string &value);
static int load_conf_(const string &conf_path);

//data structure
static map<string, string> _idc_conf_map;

typedef map<string, string>::iterator map_iterator;
typedef map<string, string>::const_iterator const_map_iterator;

const map<string, string> get_idc_map()
{
    return _idc_conf_map;
}

/**
 * Trim blank at the begin and end of content
 */
static string strtrim(const string &cnt)
{
    if (cnt.empty()) return cnt;

    string tmp_str;
    size_t cnt_len = cnt.size();
    const char *begin = cnt.data();
    const char *end = cnt.data() + cnt_len - 1;

    while (' ' == *begin && begin <= end) ++begin;
    while (' ' == *end && begin <= end) --end;

    if (begin > end) return tmp_str;

    tmp_str.assign(begin, end + 1 - begin);

    return tmp_str; 
}

/** 
 * Judge whether the content is ip_port
 */
static int is_ip_port(const string &item)
{
    if (item.empty()) return HCONF_ERR_PARAM;
 
    int status = 0;
    regex_t reg = {0};
    regmatch_t pmatch[1];
    const size_t nmatch = 1;
    char errbuf[1024] = {0};
    int cflags = REG_EXTENDED | REG_NOSUB;
    const char * pattern = "^([0-9]{1,3}\\.){3}([0-9]{1,3}){1}:[0-9]{1,5}$";

    status = regcomp(&reg, pattern, cflags);
    if (0 != status)
    {
        regerror(status, &reg, errbuf, sizeof(errbuf));
        return HCONF_ERR_OTHER;
    }
    
    status = regexec(&reg, item.c_str(), nmatch, pmatch, 0);
    if (0 == status)
    {
        regfree(&reg);
        return HCONF_OK;
    }
    else
    {
        regerror(status, &reg, errbuf, sizeof(errbuf));
        regfree(&reg);
        return HCONF_ERR_OTHER;
    }
}

static int get_first_host_by_name(const string &domain, string &ip_address)
{
    char **pptr;
    const char *ip_res;
    struct hostent *hptr;
    char str[32];

    if(NULL == (hptr = gethostbyname(domain.c_str())))
        return HCONF_ERR_OTHER;

    switch(hptr->h_addrtype)
    {
        case AF_INET:
        case AF_INET6:
            pptr=hptr->h_addr_list;
            if (pptr)
                ip_res = inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str));
            if (ip_res)
            {
                ip_address.assign(ip_res);
                return HCONF_OK;
            }
            break;
        default:
            break;
    }

    return HCONF_ERR_OTHER;
}

/**
 * Judge whether the content is ip_port
 */
static int is_domain_port(const string &item)
{
    if (item.empty()) return HCONF_ERR_PARAM;

    string::size_type spos;
    string domain, _port, ip_address;

    spos = item.find(':');
    if (spos == string::npos)
        return HCONF_ERR_OTHER;
    domain = item.substr(0, spos);
    _port = item.substr(spos);

    if (HCONF_OK != get_first_host_by_name(domain, ip_address))
        return HCONF_ERR_OTHER;
    if (HCONF_OK != is_ip_port(ip_address + _port))
        return HCONF_ERR_OTHER;

    return HCONF_OK;
}

/**
 * Validation for idc map
 */
static int is_valid_idc(const string &idc, const string &value)
{
    if (idc.empty() || value.empty()) return HCONF_ERR_PARAM;

    string idc_address;
    stringstream ss(value);

    while (getline(ss, idc_address, ','))
    {
        if ((HCONF_OK != is_ip_port(idc_address)) && (HCONF_OK != is_domain_port(idc_address)))
            return HCONF_ERR_INVALID_IP;
    }

    return HCONF_OK;
}

int hconf_load_conf()
{
    string idc_conf_path =  HCONF_IDC_CONF_PATH;

    // init conf map
    int ret = load_conf_(idc_conf_path);
    return ret;
}

static int load_conf_(const string &conf_path)
{
    if (conf_path.empty()) return HCONF_ERR_PARAM;

    //open config file
    FILE *conf_file = fopen(conf_path.c_str(), "r"); 
    if (NULL == conf_file)
    {
        return HCONF_ERR_OPEN;
    }

    //read config file
    size_t line_num = 0;
    pair<map_iterator, bool> ret;
    char line[HCONF_LINE_MAX_LEN] = {0};
    while (NULL != fgets(line, HCONF_LINE_MAX_LEN, conf_file))
    {
        line_num++;
        //empty line or comment line
        if ('\0' == line[0] || '#' == line[0] || '\n' == line[0]) continue;

        //remove the LF at the end of line
        line[strlen(line) - 1] = '\0';

        //format error line
        char *delimiter = NULL;
        if (NULL == (delimiter = strchr(line, '='))) continue;

        string key(line, delimiter - line);
        string value(delimiter + 1);

        key = strtrim(key);
        value = strtrim(value);

        //idc map
        if (0 != key.find("zookeeper.")) continue;

        string idc(key, key.find(".") + 1);
        if (HCONF_OK != is_valid_idc(idc, value)) continue;

        ret = _idc_conf_map.insert(make_pair<string, string>(idc, value));
        if (!ret.second) continue;
    }
    fclose(conf_file);
    conf_file = NULL;

    return HCONF_OK;
}


/**
 * Get host value of idc
 */
int get_host(const string &idc, string &value)
{
    if (idc.empty()) return HCONF_ERR_PARAM;

    map_iterator it = _idc_conf_map.find(idc);
    if (it == _idc_conf_map.end()) return HCONF_ERR_NOT_FOUND;
    
    value.assign(it->second);
    return HCONF_OK;
}

/**
 * Destroy the configuration environment
 */
void hconf_destroy_conf_map()
{
    _idc_conf_map.clear();
}
