/*
 ============================================================================
 Name        : Lab_1.c
 Author      : Jakob Danielsson
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "wrapper.h"
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <mqueue.h>
#include <errno.h>
#include <sys/stat.h>
/*part one*/

/*pthread_t thread;
pthread_t thread1;
int turn = 1;

pthread_mutex_t mymutex;

void* hello_world(void*param)
{
	while(1)
	{
		if(turn==1)
		{
			pthread_mutex_lock(&mymutex);


			for(int i=0;i<10;i++)
			{
				printf("Hello world\n");
				sleep(1);
			}
			pthread_mutex_unlock(&mymutex);
			turn =0;
		}
	}
}
	void* hello_moon(void*param)
	{
		while(1)
		{
			if(turn==0)
			{
				pthread_mutex_lock(&mymutex);
				for(int i=0;i<10;i++)
				{
					printf("Hello moon\n");
					usleep(200000);
				}
				pthread_mutex_unlock(&mymutex);
				turn =1;
			}
		}
	}


int main()
{
	pthread_mutex_init(&mymutex,0);
	pthread_create(&thread,NULL,&hello_world,NULL);
	pthread_create(&thread1,NULL,&hello_moon,NULL);

	pthread_join(thread,NULL);
	pthread_join(thread1,NULL);

    return 0;
}*/

#define QUEUE_NAME "/my_roll"
#define MAX_SIZE 1024
int turn = 1;


mqd_t mq;
planet_type massege;
pthread_t myserver, myclient;
pthread_cond_t c =PTHREAD_COND_INITIALIZER;
pthread_mutex_t mymutex =PTHREAD_MUTEX_INITIALIZER;



void *server(void*parm)

{


	printf("starting receiving messages:\n");

	if (MQcreate(&mq,QUEUE_NAME)!=1)
	{
		printf("Error creating message queue\n");
		return NULL;
	}
	while(1)
	{
		if(turn==1)
				{
		pthread_mutex_lock(&mymutex);
		printf("enter your message, type end to exit\n");
		scanf("%s",massege.name);

		MQwrite(&mq,(void*)&massege);

		if(strcmp(massege.name,"end")==0){
			int value= MQclose(&mq, QUEUE_NAME);
			if (value==1){
				printf("The queue is closed\n");
			}else{printf("the queue is not closed\n");}
			pthread_mutex_unlock(&mymutex);

			return NULL;
		}
		pthread_cond_wait(&c, &mymutex);
		pthread_mutex_unlock(&mymutex);
		turn=0;
				}
	}
}

void *client(void *param)
{

	usleep(1);
	if (MQconnect(&mq,QUEUE_NAME)!=1)
	{
		printf("something went wrong\n");
		return NULL;
	}

	while(1)
	{
		if(turn==0)
					{
		pthread_mutex_lock(&mymutex);

		if (mq !=(mqd_t)-1)
		{
		if(MQread(&mq,(void*)&massege)>=0){
			if(strcmp(massege.name,"end")==0){
				MQclose(&mq, QUEUE_NAME);
				pthread_mutex_unlock(&mymutex);
				return NULL;
			}
			printf("MESSAGES ARE: %s\n",massege.name);
		}
		}
		pthread_cond_signal(&c); //signal server to continue
		pthread_mutex_unlock(&mymutex);
					turn=1;
					}
		usleep(20);

	}
}

int main()
{
	pthread_mutex_init(&mymutex,NULL);
	pthread_create(&myserver,NULL,&server,NULL);
	pthread_create(&myclient,NULL,&client,NULL);

	pthread_join(myserver,NULL);
	pthread_join(myclient,NULL);

	pthread_exit(NULL);

	return 0;
}
