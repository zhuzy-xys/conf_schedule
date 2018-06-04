#ifndef HCONF_COMMON_H
#define HCONF_COMMON_H

#include <zookeeper.h>

#ifndef __STRING_VECTOR_T_FLAGS__
#define __STRING_VECTOR_T_FLAGS__
typedef struct String_vector string_vector_t;
#endif

#define HCONF_ERR_OTHER                     -1
#define HCONF_OK                            0
#define HCONF_ERR_PARAM                     1
#define HCONF_ERR_MEM                       2 
#define HCONF_ERR_TBL_SET                   3
#define HCONF_ERR_GET_HOST                  4
#define HCONF_ERR_GET_IDC                   5
#define HCONF_ERR_BUF_NOT_ENOUGH            6
#define HCONF_ERR_DATA_TYPE                 7
#define HCONF_ERR_DATA_FORMAT               8
#define HCONF_ERR_NULL_VALUE                9
//key is not found in hash table
#define HCONF_ERR_NOT_FOUND                 10  
#define HCONF_ERR_OPEN_DUMP                 11
#define HCONF_ERR_OPEN_TMP_DUMP             12
#define HCONF_ERR_NOT_IN_DUMP               13
#define HCONF_ERR_RENAME_DUMP               14
#define HCONF_ERR_WRITE_DUMP                15
#define HCONF_ERR_SAME_VALUE                16
#define HCONF_ERR_LEN_NON_POSITIVE          17 
#define HCONF_ERR_TBL_DATA_MESS             18
#define HCONF_ERR_TBL_END                   19

// error of number
#define HCONF_ERR_OUT_OF_RANGE              20
#define HCONF_ERR_NOT_NUMBER                21
#define HCONF_ERR_OTHRE_CHARACTER           22

// error of ip and port
#define HCONF_ERR_INVALID_IP                30
#define HCONF_ERR_INVALID_PORT              31
#define HCONF_ERR_SET                       32

// error of msgsnd and msgrcv
#define HCONF_ERR_NO_MESSAGE                40
#define HCONF_ERR_E2BIG                     41
#define HCONF_ERR_MSGGET                    42
#define HCONF_ERR_MSGSND                    43
#define HCONF_ERR_MSGRCV                    44
#define HCONF_ERR_MSGIDRM                   45

// error of file
#define HCONF_ERR_OPEN                      51
#define HCONF_ERR_DEL_DUMP                  52 
#define HCONF_ERR_LOCK                      53
#define HCONF_ERR_MKDIR                     54
#define HCONF_ERR_READ                      55
#define HCONF_ERR_WRITE                     56

// error of hashmap
#define HCONF_ERR_INIT_MAP                  91

// error of share memory 
#define HCONF_ERR_SHMGET                    201
#define HCONF_ERR_SHMAT                     202
#define HCONF_ERR_SHMINIT                   203

#define HCONF_ERR_LOG_LEVEL                 211

// the buf length
#define HCONF_PATH_BUF_LEN                  2048
#define HCONF_IDC_MAX_LEN                   512
#define HCONF_HOST_MAX_LEN                  1024
#define HCONF_TBLKEY_MAX_LEN                2048
#define HCONF_MAX_BUF_LEN                   2048

// server status define
#define STATUS_UNKNOWN                      -1
#define STATUS_UP                           0
#define STATUS_OFFLINE                      1
#define STATUS_DOWN                         2

// define the md5 string length
#define HCONF_MD5_INT_LEN                   16
#define HCONF_MD5_INT_HEAD_LEN              19
#define HCONF_MD5_STR_LEN                   32
#define HCONF_MD5_STR_HEAD_LEN              35

#define HCONF_INVALID_SEM_ID                -1
#define HCONF_INVALID_KEY                   -1

// hconf share memory key
#define HCONF_DEFAULT_SHM_KEY               0x10cf21d1
#define HCONF_MAX_SLOTS_NUM                 800000 

#define HCONF_FILE_PATH_LEN                 2048

// data type
// '0' - '9' : type of users' data
// 'a' - 'z' : type of information that are only used by hconf 
// 'A' - 'Z' : default type
#define HCONF_DATA_TYPE_UNKNOWN             '0'
#define HCONF_DATA_TYPE_ZK_HOST             '1'
#define HCONF_DATA_TYPE_NODE                '2'
#define HCONF_DATA_TYPE_SERVICE             '3'
#define HCONF_DATA_TYPE_BATCH_NODE          '4'
#define HCONF_DATA_TYPE_LOCAL_IDC           'a'

// zookeeper default recv timeout(unit:millisecond)
#define HCONF_ZK_DEFAULT_RECV_TIMEOUT       3000

// hconf msg queue key
#define HCONF_DEFAULT_MSG_QUEUE_KEY         0x10cf56fe

// hconf message type
#define HCONF_MSG_TYPE                      0x10c

// max send repeat times when the queue     is full
#define HCONF_MAX_SEND_MSG_TIMES            3

// max length of one message
#define HCONF_MAX_MSG_LEN                   2048

// get the max number of messages one time
#define HCONF_MAX_NUM_MSG                   200

// max read times from the share memory
#define HCONF_MAX_READ_TIMES                100
// max read times when value is empty from the share memory
#define HCONF_MAX_EMPTY_READ_TIMES          5

// max value size of node info or children services
#define HCONF_MAX_VALUE_SIZE                1048577
#define HCONF_MAX_CHD_NODE_CNT              65535

#define HCONF_PREFIX                        "/hconf/"
#define HCONF_PREFIX_LEN                    (sizeof(HCONF_PREFIX) - 1)

#endif
