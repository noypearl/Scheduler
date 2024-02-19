#include <stdio.h>
#include "THREAD.h"

// Wrapping thread with statuses
typedef enum STATUS {
	STOPPED,
        READY,
	WAITING,
	RUNNING,
	FINISHED
};

int currSize;
struct {
	THREAD__entry_point_t *entry_point;
	void *arg;
	enum STATUS status;
} threads_arr[2];

int size = 0;

int idx = 0;

void SCHEDULER__init(void){
	printf("%s", "in SCHEDULER__init\n");
	currSize = 0;
	// threads[0].entry_point = 0;
	// threads[0].arg = 'a';

	// TODO - initiallize threads array
}

/* Yield function, to be used by the schedulers threads.
 * Once a thread calls yield, execution should continue from a 
 * different thread. */
void SCHEDULER__yield(void){
    int nextThreadIndex = -1;
// TODO - schedule start scheduler
	for (int i = 0; i < sizeof(threads_arr)/sizeof(threads_arr[0])-1; i++)
	{	
            if(threads_arr[i].status == RUNNING){
                threads_arr[i].status = STOPPED;
            }
            // mark the index of the next thread we're going to resume/start
            else if((threads_arr[i].status == STOPPED || threads_arr[i].status == WAITING) && nextThreadIndex == -1){
                nextThreadIndex = i;
                // TODO - should I stop the thread?
                threads_arr[nextThreadIndex].status = READY;
        }
}
}

/* Start the scheduler. This function will return only when all
 * threads finished their work. */
void SCHEDULER__schedule_threads(void){
	// TODO - schedule start scheduler
	for (int i = 0; i < sizeof(threads_arr)/sizeof(threads_arr[0]); i++)
	{
            printf("in foor loop %d\n", i);
		// if thread finished - continue loop 
		if (threads_arr[i].status == FINISHED)
		{
			continue;
		}
		else{
			printf("Waiting for thread %d to be finished, its status is %d meanwhile, entry_point: %x\n", i
				, threads_arr[i].status, threads_arr[i].entry_point);
			// TODO - continue thread according to status
			if (threads_arr[i].status != RUNNING)
			{
				// RUN thread
				int ep = threads_arr[i].entry_point(threads_arr[i].arg);
                                printf("TEST\n");
				threads_arr[i].status = FINISHED;
                                // printf("TESTAAA\n");
			}
		}
	}
}



void SCHEDULER__print_thread(int index){
	printf("print_thread arg: %p, status: %d", threads_arr[index].arg, threads_arr[index].status);
}

void SCHEDULER__print_threads(void){
	for (int i = 0; i < sizeof(threads_arr)/sizeof(threads_arr[0]); i++)
	{
		printf("print_threads loop, index: %d, status: %d\n",i,threads_arr[i].status);	
	}
}

/* Initialize a specific thread and add it to the thread pool. */
void SCHEDULER__add_thread(THREAD__entry_point_t *entry_point,
						    void *arg){
	
	printf("adding thread, size: %d, entry_point: %x, arg: %x\n",size, entry_point, arg);
	threads_arr[size].entry_point = entry_point;
	threads_arr[size].arg = arg;
	threads_arr[size].status = WAITING;
	size++;
}

void SCHEDULER__test(){
	int index = 1;
	printf("testing TEST addr %x\n", threads_arr[index].entry_point(index));
	// threads[index].entry_point(threads[index].arg);
}