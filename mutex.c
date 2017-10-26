#include <pthread.h>
#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
// sleep

/*
Домашка на 14.10 (про мьютексы)
Есть 10 строителей (один строитель - один тред), которые приходят на стройку. На стройке есть 100 заданий (task_t). Одно задание делает строго один строитель.
Нужно смоделировать процесс того, как строители разбирают задание. Т.е. строитель берет задание, помечает что он его взял, другие строители проходят к следующим заданиям.
Задание - в нашем случае это просто sleep(tasks[i].duration).
Есть заготовка программы, чтобы вам было легче её доделать.
Нужно:
1) Доделать функцию work, чтобы выполнялось условие одного строителя на одно задание (добавить работу с мьютексом)
2) Каждый тред печатает, сколько заданий он обработал. Нужно проверить, что их сумма равна TASKS_N (т.е. проверить, что выполняется п.1)
*/

#define TASKS_N 100
#define THREADS_N 10
int summ = 0;

typedef enum {
	NEW,
	PROCESSING,
	DONE
} statuses;

typedef struct {
	int duration;
	int id;
	statuses status;
	pthread_t worker;
} task_t;

pthread_mutex_t lock;
task_t tasks[TASKS_N];

int j = 0;
void* work(void* x) {
	int done_tasks = 0;
	while(j < TASKS_N) {
		pthread_mutex_lock(&lock);
		j++;
		tasks[j].status = PROCESSING;
		pthread_mutex_unlock(&lock);
		usleep(tasks[j].duration);
		tasks[j].status = DONE;
		done_tasks++;
	}
	summ += done_tasks;
	printf("I, thread %u, have done %d tasks\n", (unsigned int)pthread_self(), done_tasks);
	
	return NULL;
}

int main() {
	srand(getpid());
	pthread_mutex_init(&lock, NULL);

	int i, result;
	pthread_t thread_id[THREADS_N];

	for (i = 0; i < TASKS_N; i++) {
		tasks[i].id = i;
		tasks[i].duration = rand() % 100000;
		tasks[i].status = NEW;
	}

	for (i = 0; i < THREADS_N; i++) {
		result = pthread_create(&thread_id[i], NULL, work, NULL);
		if (result) {
			printf("can't create thread\n");
			return 1;
		}
	}
	
	for (i = 0; i < THREADS_N; i++) {
		pthread_join(thread_id[i], NULL);
		
	}
	
	if (summ == TASKS_N) {
		printf ("100 tasks have done\n"); }
	else printf ("ooooooops\n");

	pthread_mutex_destroy(&lock);

	return 0;
}
