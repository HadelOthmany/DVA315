#include "wrapper.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>
#include <unistd.h>

#define MAX_SIZE 1024
static struct mq_attr attr;

int MQcreate(mqd_t *mq, char *name)
{
    attr.mq_flags   = 0;
    attr.mq_maxmsg  = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    *mq = mq_open(name, O_CREAT | O_RDWR, 0666, &attr);
    if (*mq == (mqd_t) -1) {
        perror("cannot create");
        return 0;
    }
    printf("MQcreate: success\n");
    return 1;
}

int MQconnect(mqd_t *mq, char *name)
{


    *mq = mq_open(name, O_RDWR, 0666, NULL);
    if (*mq == (mqd_t) -1) {
        perror("cannot connect");
        return 0;
    }
    return 1;
}

int MQread(mqd_t mq, void *buffer)
{
    // Return the number of bytes read, or 0 on error
    int byt_nr = mq_receive(mq, (char *)buffer, MAX_SIZE, NULL);
    if (byt_nr == -1) {
        if (errno != EAGAIN) { // EAGAIN just means "would block" in non-blocking mode
            perror("receive error");
        }
        return 0;
    }
    return byt_nr;
}

int MQwrite(mqd_t mq, void *data)
{
    // Here you always send sizeof(planet_type).
    // If you want variable-sized messages, pass a length argument instead.
   // int byt_nr = mq_send(mq, (const char*)data, strlen((char*)data) + 1, 0);
    // If you truly want to send a planet_type struct, you might do:
       int byt_nr = mq_send(mq, (const char*)data, sizeof(planet_type), 0);

    if (byt_nr == -1) {
        perror("send error");
        return 0;
    }
    return byt_nr;
}

int MQclose(mqd_t *mq, char *name)
{
    int ret1 = mq_close(*mq);
    int ret2 = mq_unlink(name);
    if (ret1 == 0 && ret2 == 0) {
        return 1;
    }
    return 0;
}
