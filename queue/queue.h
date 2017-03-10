/* 
 * 2017 n0vo - n0vothedreamer@gmail.com
 * 
 * Queue implementation header file
 *
 * This header file can be included in any project where implementing
 * a queue is necessary
 */


struct Task {
	void *object;		/* pointer that can be anything (struct, function..) */
	struct Task *next;	/* linked-list */
};

struct Queue {
	int size;		/* keeps track of the queue size */
	struct Task *head;	/* points to the top of the queue */
	struct Task *tail;	/* points to the bottom of the queue */
	pthread_mutex_t lock;	/* mutual exclusion lock */
};


/*
 * This function creates an initializes a new queue.
 * 
 * Example:
 *	struct Queue *queue = queue_init();
 *
 * RETURN VALUE: This function returns a Task structure. Returns NULL upon failure to create the structure
 */
struct Queue *queue_init()
{
	struct Queue *queue = malloc(sizeof(struct Queue));
	if (queue == NULL) return NULL;
	pthread_mutex_init(&queue->lock, NULL);

	queue->size = 0;
	queue->head = queue->tail = NULL;
	return queue;
}

/*
 * Utility function for checking to see if the queue is empty or not.
 * This function takes only a Queue structure as an argument
 *
 * Example:
 *	struct Queue *queue = queue_init();
 *	// fill the queue and process
 *
 *	while ( *queue is full* ) {
 *		// process tasks
 *
 *		if (queue_empty(queue)) {
 *			puts("The queue is empty");
 *			exit(0);
 *		}
 *	}
 *
 * RETURN VALUE: This function returns 1 if the queue is empty, otherwise it returns 0. 
 * It returns -1 upon error.
 */
int queue_empty(struct Queue *queue)
{
	// if the queue is invalid
	if (queue == NULL) return -1;
	// if the queue size is 0
	if (queue->size == 0) return 1;
	else return 0;
}

/*
 * This function associates data with a task which gets added to the queue. Once the item
 * is added to the queue, the queue size is incremented by one. It takes a void pointer to 
 * any data you want, and a Queue structure as arguments
 *
 * Example:
 *	struct Task task;
 *	struct Queue *queue = queue_init();
 *
 *	while (1) {
 *		void *data = strdup(item);
 *		queue_push(data, queue);
 *	}
 *		
 * RETURN VALUE: This function returns 0 upon success, or -1 upon failure. 
 */
int queue_push(void *data, struct Queue *queue)
{
	// create a new task in memory
	struct Task *task = malloc(sizeof(struct Task));
	// if the queue or task are invalid
	if (queue == NULL) return -1;
	// initialize next task
	task->next = NULL;
	pthread_mutex_lock(&queue->lock);
	// associated the actual data with the task
	task->object = data;
	// if the queue is empty then set both ends to task
	if (queue->size == 0) {
		// since there's no tasks on queue yet
		queue->head = queue->tail = task;
	} else {
		// push task onto the tail of the queue
		queue->tail->next = task;
		queue->tail = task;
	}
	// increment queue size
	++queue->size;
	pthread_mutex_unlock(&queue->lock);
	return 0;
}

/*
 * This function takes full Queue structure as an argument and pops one item off of the 
 * queue at a time. It contains a lock so that only one thread may pop a task off of 
 * the queue at a time. Once an item has been popped off the queue the queue size is 
 * decremented by one. 
 *
 * Example:
 *
 *	struct Queue *queue = queue_init();
 *	struct Task *task = NULL;
 *	
 *	// fill the queue with tasks
 *
 *	while ((task = queue_pop(queue)) != NULL)
 *		printf("Thread: %d - task: %s\n", task->id, task->name);
 *
 * RETURN VALUE: This function returns a populated Task structure.
 */
struct Task *queue_pop(struct Queue *queue)
{
	pthread_mutex_lock(&queue->lock);
	// if the head of queue is NULL then it's empty
	if (queue_empty(queue)) {
		puts("Queue is empty");
		pthread_mutex_unlock(&queue->lock);
		return NULL;
	}
	// initialize pointer to head of queue
	struct Task *task = queue->head;
	// if the head and tail are the same set everything to NULL
	if (queue->head == queue->tail)
		queue->head = queue->tail = NULL;
	else 
		queue->head = queue->head->next;
	// return the task pointer
	--queue->size;
	pthread_mutex_unlock(&queue->lock);
	return task;
}

/*
 * Utility function for freeing up memory previously used by the full queue
 *
 * Example:
 *
 *	struct Queue *queue = queue_init();
 *
 *	// process items with the queue
 *
 *	queue_exit(queue);
 *
 * RETURN VALUE: This function has no return value.
 */
void queue_exit(struct Queue *queue)
{
	struct Task *task = NULL;
	// free all the tasks in the queue
	if (!queue_empty(queue)) {
		task = queue_pop(queue);
		free(task);
	}
	// free the queue itself
	free(queue);
}

