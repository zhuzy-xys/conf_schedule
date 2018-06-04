#ifndef HCONF_CONST_H
#define HCONF_CONST_H

#include "hconf_common.h"

// agent version
#define HCONF_AGENT_VERSION                 "1.0.2"

#define HCONF_AGENT_NAME                    "hconf_agent"

// status of process exit
#define HCONF_EXIT_NORMAL                   50

// error: zookeeper operation failed
#define HCONF_ERR_ZOO_FAILED                61

// error: failed to execute command
#define HCONF_ERR_CMD                       71

// error: local is is null
#define HCONF_ERR_NULL_LOCAL_IDC            81

// error: failed to operate file
#define HCONF_ERR_FAILED_OPEN_FILE          101
#define HCONF_ERR_FAILED_READ_FILE          102

// error: feedback timeout
#define HCONF_ERR_FB_TIMEOUT                111

// error: script callback
#define HCONF_ERR_SCRIPT_TIMEOUT            115
#define HCONF_ERR_SCRIPT_NOT_EXIST          116
#define HCONF_ERR_SCRIPT_DIR_NOT_INIT       117

// error: gray level submission
#define HCONF_ERR_GRAY_WATCH_FAILED         118
#define HCONF_ERR_GRAY_GET_NOTIFY_NODE      119
#define HCONF_ERR_GRAY_GET_NOTIFY_CONTENT   120
#define HCONF_ERR_GRAY_FAKE_SHM_FAILED      121

// status for getting data from zookeeper
#define HCONF_INVALID_LEN                   -1
#define HCONF_NODE_NOT_EXIST                -2
#define HCONF_NODE_EXIST                    -3

#define HCONF_SEND_BUF_MAX_LEN              4096
#define HCONF_IP_MAX_LEN                    256

#define HCONF_ZOO_LOG                       "zoo.err.log"

// zookeeper default recv timeout(unit:millisecond)
#define HCONF_ZK_DEFAULT_RECV_TIMEOUT       3000

#define HCONF_DEFAULT_REGIESTER_PREFIX      "/hconf/__hconf_register_hosts"

#define HCONF_STATUS_CHK_PREFIX             "/hconf/__status_chk"

#define	HCONF_KEY_DAEMON_MODE               "daemon_mode"
#define	HCONF_KEY_ZKRECVTIMEOUT             "zookeeper_recv_timeout"
#define HCONF_KEY_REGISTER_NODE_PREFIX 	    "register_node_prefix"
#define HCONF_KEY_ZKLOG_PATH                "zk_log"
#define HCONF_KEY_FEEDBACK_ENABLE           "feedback_enable"
#define HCONF_KEY_FEEDBACK_URL              "feedback_url"
#define	HCONF_KEY_SCEXECTIMEOUT             "script_execute_timeout"

// no need feedbcak nodes
#define HCONF_ANCHOR_NODE                   "/hconf/__hconf_anchor_node"

//aware nofication from zookeeper
#define HCONF_NOTIFY_CLIENT_PREFIX          "/hconf/__hconf_notify/client"
#define HCONF_NOTIFY_CONTENT_PREFIX         "/hconf/__hconf_notify/content"

#define HCONF_GET_VALUE_RETRY_TIMES         3

// valid conf key name
#define HCONF_KEY_DAEMON_MODE               "daemon_mode"
#define HCONF_KEY_LOG_FMT                   "log_fmt"
#define HCONF_KEY_LOG_LEVEL                 "log_level"
#define HCONF_KEY_HCONF_DIR                 "hconf_dir"
#define HCONF_KEY_CHECK_IDC                 "idc"
#define HCONF_KEY_SHM_KEY                   "shm_key"
#define HCONF_KEY_MSG_QUEUE_KEY             "msg_queue_key"
#define HCONF_KEY_MAX_REPEAT_READ_TIMES     "max_repeat_read_times"
#define HCONF_KEY_LOCAL_IDC                 "local_idc"

//shared memory size
#define SHARED_MEMORY_SIZE                  "shared_memory_size"

/* trigger type */
#define HCONF_TRIGGER_TYPE_ADD_OR_MODIFY    '0'
#define HCONF_TRIGGER_TYPE_REMOVE           '2'
#define HCONF_TRIGGER_TYPE_RESET            '3'

/* zk constants */
#define HCONF_GET_RETRIES                   3

/* log constans */
#define HCONF_LOG_DIR                       "logs"

/* script callback constants */
#define HCONF_SCRIPT_SEPARATOR              '#'
#define HCONF_SCRIPT_SUFFIX                 ".sh"

/* feedback constants */
#define HCONF_FB_RETRIES                    3
#define HCONF_FB_RESULT                     "0"

#define HCONF_STOP_MSG                      "__STOP__"

#endif
