#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>

typedef struct mymsgbuf 
{
	long mtype;
	int number;
	pid_t pid;		
} buf;
	
typedef struct arguments
{
	int number;
	int msqid;
	pid_t pid;
} argu;

typedef struct internal
{
    long mtype;
} intern_msg;

size_t size = sizeof(struct mymsgbuf) - sizeof(long);
	
void* func(void* arg)
{
	argu* copy = (argu*) arg; 
	buf ret;
	intern_msg msg;
	
	msg.mtype = 2;
	int time = copy->number;	
	ret.number = copy->number * copy->number;
	ret.mtype = copy->pid;
	
	if ((msgsnd(copy->msqid, (struct msgbuf*) &msg, 0, 0)) < 0)
	{
		perror("msgsnd");
		exit (-1);
	}
	
	sleep (time);
	
	if ((msgsnd(copy->msqid, (struct msgbuf*) &ret, size, 0)) < 0)
	{
		perror("msgsnd");
		exit (-1);
	}
	
	int k;
	pthread_exit(&k);
	}
	

int main (int argc, char** argv) 
{
	char pathname[] = "server.c";
	key_t key;
	pthread_t* id_thread = (pthread_t*)malloc(1000 * sizeof(pthread_t));
	argu* arg = (argu*)malloc(sizeof(argu));
	buf mybuf;
	intern_msg msg;
		
	if ((key = ftok (pathname, 0)) < 0)
	{
			printf ("Can't generate key\n");
			exit (-1);
	}
		
	if ((arg->msqid = msgget (key, 0666| IPC_CREAT)) < 0)
	{
		printf ("Can't get msqid\n");
		exit (-1);
	}
		
	int i = 0, d = 1;
		
	while (1)
	{
		if(i >= 1000)
		{
			id_thread = realloc(id_thread, (1000 + d) * sizeof(pthread_t));
			d++;
		}	
		
		if(msgrcv(arg->msqid, (struct msgbuf*)&mybuf, size, 1, 0) < 0)
		{
		    perror("msgrcv");
		    exit(-1);
		}
		
		if(mybuf.number < 0) break;
		
		arg->number = mybuf.number;
		arg->pid = mybuf.pid;
		
		if ((pthread_create(&id_thread[i], NULL, func, arg)) > 0 )
		{
			perror ("pthread_create");
			exit(-1);
		} 
		
		if(msgrcv(arg->msqid, (struct msgbuf*)&msg, 0, 2, 0) < 0)
		{
		    perror("msgrcv");
		    exit(-1);
		}
		i++; 
	}
	int j = 0;
	for(j = 0; j < i; j++)
	{
		pthread_join (id_thread[j], (void**)NULL);
	}
	if ((msgctl (arg->msqid, IPC_RMID, (struct msqid_ds *) NULL)) < 0) 
	{
		printf ("Can't delete queue\n");
		exit (-1);
	}
	
    return 0;
}
