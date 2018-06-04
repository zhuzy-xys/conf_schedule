#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/msg.h>

#include <string>

#include "hconf_log.h"
#include "hconf_msg.h"
#include "hconf_format.h"

using namespace std;


int init_msg_queue(key_t key, int &msqid)
{
    msqid = msgget(key, PERMS);
    if (-1 == msqid)
    {
        return HCONF_ERR_MSGGET;
    }

    return HCONF_OK;
}

int create_msg_queue(key_t key, int &msqid)
{
    msqid = msgget(key, PERMS | IPC_CREAT);
    if (-1 == msqid)
    {
        LOG_FATAL_ERR("Faield to create msg queue of key:%#x! errno:%d", key, errno);
        return HCONF_ERR_MSGGET;
    }

    return HCONF_OK;
}

int send_msg(int msqid, const string &msg)
{
    int ret = HCONF_OK;
    int try_send_times = 1;

    if (msg.empty()) return HCONF_ERR_PARAM;

    if (msg.size() >= HCONF_MAX_MSG_LEN)
    {
        return HCONF_ERR_E2BIG;
    }

    hconf_msgbuf msgbuf;
    msgbuf.mtype = HCONF_MSG_TYPE;
    memcpy(msgbuf.mtext, msg.data(), msg.size());

    while (true)
    {
        ret = msgsnd(msqid, (void*)&msgbuf, msg.size(), IPC_NOWAIT);
        if (0 == ret) return HCONF_OK;

        if (EAGAIN == errno)
        {
            if (try_send_times < HCONF_MAX_SEND_MSG_TIMES)
            {
                usleep(5000);
                try_send_times++;
                continue;
            }
            return HCONF_ERR_MSGFULL;
        }
        return HCONF_ERR_MSGSND;
    }

    return HCONF_OK;
}

int receive_msg(int msqid, string &msg)
{
    ssize_t len = 0;
    hconf_msgbuf msgbuf;

    len = msgrcv(msqid, (void*)&msgbuf, HCONF_MAX_MSG_LEN, HCONF_MSG_TYPE, 0);
    if (-1 == len)
    {
        if (EIDRM == errno)
        {
            LOG_FATAL_ERR("Msg queue has been removed! msqid:%d, errno:%d", msqid, errno);
            return HCONF_ERR_MSGIDRM;
        }

        LOG_ERR("Failed to get message! msqid:%d, errno:%d", msqid, errno);
        return HCONF_ERR_MSGRCV;
    }

    msg.assign(msgbuf.mtext, len);
    return HCONF_OK;
}
