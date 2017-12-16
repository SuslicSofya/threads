#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

struct mymsgbuf 
{
	long mtype;
	int number;
    pid_t pid;	
}mybuf;
 

int main (int argc, char** argv) 
{
	if(argc != 2)
	{
		printf("Wrong number of arguments\n");
		exit(-1);
	}
	
	int msqid;
	char pathname[] = "server.c";
	key_t key;
	size_t size = sizeof(struct mymsgbuf) - sizeof(long);
	
	if ((key = ftok (pathname, 0)) < 0) 
	{
		printf ("Can't generate key\n");
		exit (-1);
	}
		
	if ((msqid = msgget (key, 0666| IPC_CREAT)) < 0)
	{
		printf ("Can't get msqid\n");
		exit (-1);
	}
		
	mybuf.pid = getpid();
	mybuf.mtype = 1;
	mybuf.number = atoi(argv[1]);
	
	if ((msgsnd (msqid, (struct msgbuf *) &mybuf, size, 0)) < 0)
	{
		printf ("Can't printf a message to a queue\n");
		exit (-1);
	}
	if (mybuf.number < 0) exit(1);	
	if ((msgrcv (msqid, (struct msgbuf*) &mybuf, size, getpid(), 0)) < 0) 
	{
	    printf ("Can't receive the answer\n");
	    exit (-1);
	}
	
	printf ("Result: %d\n", mybuf.number);
	fflush(stdout);
    exit(1);
}
