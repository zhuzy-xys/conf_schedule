#ifndef HCONF_MSG_H
#define HCONF_MSG_H

#include <string>

#include "hconf_common.h"

#ifndef __PERMS_FLAGS__
#define __PERMS_FLAGS__
#define PERMS 0666
#endif

typedef struct
{
    long mtype;
    char mtext[HCONF_MAX_MSG_LEN];
} hconf_msgbuf;

int init_msg_queue(key_t key, int &msqid);
int create_msg_queue(key_t key, int &msqid);
int send_msg(int msqid, const std::string &msg);
int receive_msg(int msqid, std::string &msg);

#endif
