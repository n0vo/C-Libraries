#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "queue.h"

#define POOLSIZE 10

struct thread_data {
	struct Task *task;
	struct Queue *queue;
};

void *worker(void *q)
{
	struct thread_data *td = q;
	while ((td->task = queue_pop(td->queue)) != NULL) {
		printf("Popped '%s' off of the queue.\nSize: %d\n", 
					(char *)td->task->object, 
					td->queue->size);
		sleep(1);
	}
	
	return NULL;
}

int main(int argc, char *argv[])
{
	int i, j;
	char *c;
	char *data;
	char item[80];
	char *file = argv[1];
	FILE *fn = fopen(file, "r");
	struct Queue *queue = queue_init();
	
	while (fgets(item, sizeof(item), fn) != NULL) {
		if ((c = strchr(item, '\n')) != NULL) *c = '\0';
		data = strdup(item);
		queue_push((void *)data, queue);
		printf("Size: %d - added '%s' to queue\n", queue->size, data);
	}
	fclose(fn);
	printf("\nTotal queue size: %d\n", queue->size);	
	/* now the queue is full */

	/* begin workers */
	pthread_t workers[POOLSIZE];
	// data to be passed to the worker threads
	struct thread_data qdata;
	for (i = 0; i < POOLSIZE; i++) {
		// make sure each thread has access to the queue
		qdata.queue = queue;
		// create the worker threads
		pthread_create(&workers[i], NULL, worker, &qdata);
	}
	/* join worker threads */
	for (j = 0; j < POOLSIZE; j++)
		pthread_join(workers[j], NULL);

	/* free up the queue */
	queue_exit(queue);
	return EXIT_SUCCESS;
}

