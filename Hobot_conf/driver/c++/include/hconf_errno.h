#ifndef HCONF_ERRNO_H
#define HCONF_ERRNO_H

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
#define HCONF_ERR_NOT_FOUND                 10  //key is not found in hash table
#define HCONF_ERR_OPEN_DUMP                 11
#define HCONF_ERR_OPEN_TMP_DUMP             12
#define HCONF_ERR_NOT_IN_DUMP               13
#define HCONF_ERR_RENAME_DUMP               14
#define HCONF_ERR_WRITE_DUMP                15
#define HCONF_ERR_SAME_VALUE                16
#define HCONF_ERR_LEN_NON_POSITIVE          17 
#define HCONF_ERR_TBL_DATA_MESS             18

// error of number
#define HCONF_ERR_OUT_OF_RANGE              20
#define HCONF_ERR_NOT_NUMBER                21
#define HCONF_ERR_OTHRE_CHARACTER           22

// error of ip and port
#define HCONF_ERR_INVALID_IP                30
#define HCONF_ERR_INVALID_PORT              31

// error of msgsnd and msgrcv
#define HCONF_ERR_NO_MESSAGE                40
#define HCONF_ERR_E2BIG                     41
#define HCONF_ERR_MSGGET                    42
#define HCONF_ERR_MSGSND                    43
#define HCONF_ERR_MSGRCV                    44
#define HCONF_ERR_MSGIDRM                   45

// error of hostname
#define HCONF_ERR_HOSTNAME                  71

// error, not init while using hconf c library
#define HCONF_ERR_CC_NOT_INIT               81


// error, failed to operate message queue
#define HCONF_ERR_SEND_MSG_FAILED           91

//  operation if there is no value in share memory
#define HCONF_WAIT                          0
#define HCONF_NOWAIT                        1
#define HCONF_MAX_GET_TIMES                 100

#endif
