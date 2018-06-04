#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <iostream>

#include "hconf_zoo.h"
#include "hconf_log.h"
#include "hconf_shm.h"
#include "hconf_cmd.h"
#include "hconf_dump.h"
#include "hconf_const.h"
#include "hconf_config.h"
#include "hconf_daemon.h"
#include "hconf_script.h"
#include "hconf_watcher.h"
#include "hconf_feedback.h"

using namespace std;

extern int maxSlotsNum;
const string HCONF_PID_FILE("/pid");
const string HCONF_LOG_FMT("/logs/hconf.log.%Y-%m-%d-%H");

static void sig_handler(int sig);
static int hconf_agent_init(const string &agent_dir, const string &log_dir);
static void hconf_agent_destroy();

#define STRING_(str) #str
#define STRING(str) STRING_(str)

static void Usage() {
  LOG_INFO("Usage: \n ./hconf_agent --localidc=idcname\n"
           "We support one argument for now, idcname is "
           "your idc name in agent.conf");
}

int main(int argc, char* argv[])
{
    string agent_dir("..");
#ifdef HCONF_AGENT_DIR
    agent_dir = STRING(HCONF_AGENT_DIR);
#endif

    hconf_set_log_level(HCONF_LOG_INFO);
    LOG_INFO("agent_dir:%s", agent_dir.c_str());

    // check whether agent is running
    // 单活判断
    int pid_fd = 0;
    string pid_file = agent_dir + HCONF_PID_FILE;
    int ret = check_proc_exist(pid_file, pid_fd);
    if (HCONF_OK != ret) return ret;

    // load configure
    // Check localidc argv
    std::string localidc;
    if (argc > 1) {
      if (argc == 2) {
        std::string localidc_s(argv[1]);
        if (localidc_s.empty()) {
          Usage();
        }
        size_t pos = localidc_s.find("=");
        localidc.assign(localidc_s.substr(pos + 1));
      } else {
        Usage();
      }
      LOG_INFO("localidc: %s", localidc.c_str());
    }
    ret = hconf_load_conf(agent_dir, localidc);
    if (HCONF_OK != ret)
    {
        LOG_FATAL_ERR("Failed to load configure!");
        return ret;
    }

    // daemonize
    string value;
    long daemon_mode = 1;
    ret = get_agent_conf(HCONF_KEY_DAEMON_MODE, value); 
    if (HCONF_OK == ret) get_integer(value, daemon_mode);

    string log_fmt = agent_dir + HCONF_LOG_FMT;
    ret = get_agent_conf(HCONF_KEY_LOG_FMT, value);
    if (HCONF_OK == ret) log_fmt = value;
    size_t pos = log_fmt.find_last_of('/');
    string log_dir = (string::npos == pos) ? "." : string(log_fmt.c_str(), pos);

    long log_level = HCONF_LOG_ERR;
    ret = get_agent_conf(HCONF_KEY_LOG_LEVEL, value);
    if (HCONF_OK == ret) get_integer(value, log_level);
    hconf_log_init(log_fmt, log_level);

    if (daemon_mode != 0)
    {
        close(pid_fd);
        ret = hconf_agent_daemon_keepalive(pid_file);
        if (HCONF_OK != ret)
        {
            hconf_destroy_log();
            return ret;
        }
    }
    else
    {
        write_pid(pid_fd, getpid());
    }

    // TODO: using socket
    // Set the signal process handler
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT,  sig_handler);
    signal(SIGHUP,  sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGUSR1, sig_handler);
    signal(SIGUSR2, sig_handler);

    // Environment initialize
    ret = get_agent_conf(SHARED_MEMORY_SIZE, value);
    if (ret == HCONF_OK) {
        maxSlotsNum = atoi(value.c_str());
    }
    else {
        maxSlotsNum = HCONF_MAX_SLOTS_NUM;
    }

    ret = hconf_agent_init(agent_dir, log_dir);
    if (HCONF_OK != ret)
    {
        LOG_ERR("Failed to init hconf agent!");
        return ret;
    }
    // main
    watcher_setting_start();
   
    hconf_agent_destroy();

    return HCONF_OK;
}

static int hconf_agent_init(const string &agent_dir, const string &log_dir)
{
    string value;
    int ret = HCONF_OK;

    // init zookeeper log
    ret = get_agent_conf(HCONF_KEY_ZKLOG_PATH, value);
    if (HCONF_OK != ret)
        hconf_init_zoo_log(log_dir);
    else
        hconf_init_zoo_log(log_dir, value);

    // init cmd env
    ret = hconf_init_cmd_env(agent_dir);
    if (HCONF_OK != ret)
    {
        LOG_FATAL_ERR("Failed to init cmd environment");
        return ret;
    }

    // init dump env
    ret = hconf_init_dump_file(agent_dir);
    if (HCONF_OK != ret)
    {
        LOG_FATAL_ERR("Failed to init dump file");
        return ret;
    }

    // init share memory table
    ret = hconf_init_shm_tbl();
    if (HCONF_OK != ret)
    {
        LOG_FATAL_ERR("Failed to init share memory!");
        return ret;
    }

    // init message queue
    ret = hconf_init_msg_key();
    if (HCONF_OK != ret)
    {
        LOG_FATAL_ERR("Failed to init message queue!");
        return ret;
    }

    // init register node prefix
    string node_prefix(HCONF_DEFAULT_REGIESTER_PREFIX);
    ret = get_agent_conf(HCONF_KEY_REGISTER_NODE_PREFIX, value);
    if (HCONF_OK == ret) node_prefix = value;
    hconf_init_rgs_node_pfx(node_prefix);

    // init zookeeper operation timeout
    long zk_timeout = 3000;
    ret = get_agent_conf(HCONF_KEY_ZKRECVTIMEOUT, value);
    if (HCONF_OK == ret) get_integer(value, zk_timeout);
    hconf_init_recv_timeout(static_cast<int>(zk_timeout));

    // init local idc
    ret = get_agent_conf(HCONF_KEY_LOCAL_IDC, value);
    if (HCONF_OK != ret)
    {
        LOG_FATAL_ERR("Failed to get local idc!");
        return ret;
    }
    ret = hconf_init_local_idc(value);
    if (HCONF_OK != ret)
    {
        LOG_FATAL_ERR("Failed to set local idc!");
        return ret;
    }

    // init script dir
    hconf_init_script_dir(agent_dir);
    
    // init script execute timeout
    long sc_timeout = 3000;
    ret = get_agent_conf(HCONF_KEY_SCEXECTIMEOUT, value);
    if (HCONF_OK == ret) get_integer(value, sc_timeout);
    hconf_init_scexec_timeout(static_cast<int>(sc_timeout));

#ifdef HCONF_CURL_ENABLE
    long fd_enable = 0;
    ret = get_agent_conf(HCONF_KEY_FEEDBACK_ENABLE, value);
    if (HCONF_OK == ret) get_integer(value, fd_enable);
    if (1 == fd_enable)
    {
        hconf_init_fb_flg(true);
       
        ret = get_agent_conf(HCONF_KEY_FEEDBACK_URL, value);
        if (HCONF_OK != ret)
        {
            LOG_FATAL_ERR("Failed to get feedback url!");
            return ret;
        }

        ret = hconf_init_feedback(value);
        if (HCONF_OK != ret)
        {
            LOG_FATAL_ERR("Failed to init feedback!");
            return ret;
        }
    }
#endif

    return HCONF_OK;
}

static void hconf_agent_destroy()
{
#ifdef HCONF_CURL_ENABLE
    hconf_destroy_feedback();
#endif
    hconf_destroy_zk();
    hconf_destroy_conf_map();
    hconf_destroy_dbf();
    hconf_destroy_dump_lock();
    hconf_destroy_qhasharr_lock();
    hconf_destroy_zoo_log();
    hconf_destroy_log();
}

static void sig_handler(int sig)
{
    switch(sig)
    {
    case SIGINT:
        break;
    case SIGTERM:
    case SIGUSR2:
        hconf_thread_exit();
        //hconf_agent_destroy();
        //exit(0);
        break;
    case SIGHUP:
        break;
    case SIGUSR1:
        hconf_cmd_proc();
        break;
    default:
        break;
    }
}
