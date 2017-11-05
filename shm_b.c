/*
 * Нужно сделать 2 программы, shm_a и shm_b.
 * Запускаются обе программы одновременно.
 * В первую мы вводим число, которое должно передаться во вторую через shared memory.
 * Вторая программа возводит это число в квадрат и пишет ответ обратно в общую память.
 * Первая программа распечатывает результат.
 * Синхронизация программ через семафоры.
*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/shm.h>

int main() {

	int *array;
	int shmid;
	int number;
	char pathname[] = "shm_a.c";
	key_t key;
	
		if (( key = ftok (pathname, 0)) < 0) {
			printf ("Can't generate key\n");
			exit (-1);
		}

		if ((shmid = shmget (key, sizeof (int), 0666|IPC_CREAT|IPC_EXCL)) < 0) {
			if (errno != EEXIST) {
				printf ("Can't create shared memory");
				exit (-1);
			} 
			
			else if ((shmid = shmget (key, sizeof (int), 0)) < 0) {
					printf ("Can't find share memory");
					exit (-1);
			}
		}
		if ((array = (int*) shmat (shmid, NULL, 0)) == (int*)(-1)) {
			printf("Can't attach shared memory\n");
			exit(-1);
		}
		
	int semid;
	struct sembuf mybuf; 

		if ((semid = semget(key , 2, 0666 | IPC_CREAT)) < 0) {
			printf("Can`t get semid\n");
			exit(-1);
	}
	
	mybuf.sem_op = -1;
	mybuf.sem_flg = 0;
	mybuf.sem_num = 0;
	
		if (semop(semid , &mybuf , 1) < 0) {
			perror("Can`t wait for condition 3\n");
			exit(-1);
	}
	
	number = array[0];
	number = number * number;
		if (number < 0) {
			printf ("Can't square the number");
		}
	array[0] = number;
	
	mybuf.sem_op = 1;
	mybuf.sem_flg = 0;
	mybuf.sem_num = 1;
	
		if (semop(semid , &mybuf , 1) < 0) {
			perror("Can`t wait for condition 4\n");
			exit(-1);
	}

	return 0;
}

