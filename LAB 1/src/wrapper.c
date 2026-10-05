#include "wrapper.h"

#define MAX_SIZE 1024
struct mq_attr myattr;
//Big note! If you think the arguments makes no sense, you are allowed to change them, as long as the basic functionality is kept
//In case you run in to blocking issues with reading from the queue, the option O_NONBLOCK is a hot tip

int MQcreate (mqd_t * mq, char * name)
{
 	//Should create a new messagequeue, use mq as reference pointer so you can reach the handle from anywhere
	//Should return 1 on success and 0 on fail



	myattr.mq_flags=0;
	myattr.mq_maxmsg=10;
	myattr.mq_msgsize=MAX_SIZE;
	*mq = mq_open(name,O_CREAT | O_NONBLOCK |O_RDWR,0666, &myattr);

	if(*mq ==(mqd_t) -1)
	{
		perror("error creating message queue");
		return 0; //failed
	}else
	{
		printf("message queue created successfully\n");
		return 1;//success
	}

}
int MQconnect (mqd_t * mq, char * name)
{
    /* Connects to an existing mailslot for writing Uses mq as reference pointer, so that you can 	reach the handle from anywhere*/
    /* Should return 1 on success and 0 on fail*/


	myattr.mq_flags=0;
		myattr.mq_maxmsg=10;
		myattr.mq_msgsize=MAX_SIZE;
		*mq = mq_open(name,O_RDWR,0666, &myattr);


		if(*mq ==(mqd_t) -1)
		{
			perror("error connecting to message queue");
			return 0; //failed
		}else
		{
			printf("Connected to message queue successfully\n");
			return 1;//success
		}
}

void*  MQread (mqd_t mq, void ** buffer)
{

    /* Read a msg from a mailslot, return nr Uses mq as reference pointer, so that you can 		reach the handle from anywhere */
    /* should return number of bytes read              */
	int result;
	result = mq_receive(mq,(char*)buffer, MAX_SIZE+1, NULL);
	if(result ==-1)
	{
		return NULL;
	}
	printf("message is received");
}

int MQwrite (mqd_t mq, void * sendBuffer)
{
    /* Write a msg to a mailslot, return nr Uses mq as reference pointer, so that you can 	     reach the handle from anywhere*/
    /* should return number of bytes read         */
	int result;
		result = mq_send(mq,(const char *)sendBuffer,sizeof(planet_type), 0);
		if(result ==-1)
		{
			result=0;
		}
		return result;
}

int MQclose(mqd_t * mq, char * name)
{
    /* close a mailslot, returning whatever the service call returns Uses mq as reference pointer, so that you can
    reach the handle from anywhere*/
    /* Should return 1 on success and 0 on fail*/
	int result;
	result = mq_close(*mq);
	result += mq_unlink(name);

	if(result==0)
	{
		return 1; //success
	}
	else
	{
		return 0; //fail
	}
}


