#include <stdio.h>
#include <unistd.h>
#include "THREAD.h"

// Wrapping thread with statuses
typedef enum STATUS {
	STOPPED,
	WAITING,
	RUNNING,
	FINISHED
};

int currSize;
struct {
	THREAD__entry_point_t *entry_point;
	void *stopped_point;
	void *arg;
	enum STATUS status;
} threads_arr[3];

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
    printf("in SCHEDULER__yield");
    int nextThreadIndex = -1;
    int i = 0;
// TODO - schedule start scheduler
	for (i = 0; i < sizeof(threads_arr)/sizeof(threads_arr[0]); i++)
	{	
            // TODO - set stopped_point of thread object
            if(threads_arr[i].status == RUNNING){
                threads_arr[i].status = STOPPED;
            }
            // mark the index of the next thread we're going to resume/start
            else if((threads_arr[i].status == STOPPED || threads_arr[i].status == WAITING) && nextThreadIndex == -1){
                nextThreadIndex = i;
                printf("Frame addr: %x\n", __builtin_frame_address(1));
                // TODO - should I stop the thread?
                /*threads_arr[nextThreadIndex].status = READY; // TOOD - maybe redundant to set status and again set it after 4 lines #stupid?*/
                break;
        }
}
	if(nextThreadIndex == -1){
		/*printf("Last thread yielded at index %d - no more threads to start");*/
	}
	else{
		printf("Starting thread number %d\n", nextThreadIndex);
		threads_arr[nextThreadIndex].entry_point(threads_arr[nextThreadIndex].arg);
                threads_arr[nextThreadIndex].status = RUNNING;
    }
}

void resumeThread(int index){
    printf("resumeThread\n");
    threads_arr[index].status = RUNNING;
    *((int *)(threads_arr[index].stopped_point));

}

void initThread(int index){
    printf("initThread\n");
    threads_arr[index].status = RUNNING;
    threads_arr[index].entry_point(threads_arr[index].arg);
        
}
// Loops all thread array and returns the index of the next thread we shuold handle, 
// return -1 if no threads to handle
int getNextThreadIndexToHandle(void){
	for (int i = 0; i < sizeof(threads_arr)/sizeof(threads_arr[0]); i++)
	{
		enum STATUS threadStatus = threads_arr[i].status;
		if(threadStatus != FINISHED && threadStatus != RUNNING){
                    printf("Returning from getNextThreadIndexToHandle, i: %d\n", i);
			return i;
		}
	}
	return -1;
}
/* Start the scheduler. This function will return only when all
 * threads finished their work. */
void SCHEDULER__schedule_threads(void){
    printf("%s", "in schedule_threads\n");
    int nextThreadToHandleIndex = -1;
    while((nextThreadToHandleIndex = getNextThreadIndexToHandle()) != -1){ // index of the next thread we should start or resume. Stop while all threads finished.
        enum STATUS threadStatus = threads_arr[nextThreadToHandleIndex].status;
        if( threadStatus == STOPPED ){
                printf("Stopped thread\n");
                resumeThread(nextThreadToHandleIndex);
                
                // apply stopped from yield - recover frame address, set thread to RUNNING, call it
        }
        else if ( threadStatus == WAITING ){
                printf("Waiting thread\n");
                initThread(nextThreadToHandleIndex);
        }
        else{
                printf("Weird state arrived at schedule_threads, state: %d,  index: %d\n", threadStatus, nextThreadToHandleIndex);
        }
	// TODO - schedule start scheduler
	/*for (int i = 0; i < sizeof(threads_arr)/sizeof(threads_arr[0]); i++)*/
	/*{*/
            /*printf("in foor loop %d\n", i);*/
		/*// if thread finished - continue loop */
		/*if (threads_arr[i].status == FINISHED)*/
		/*{*/
            /*printf("thread at %d is FINISHED, skipping..\n", i);*/
			/*continue;*/
		/*}*/
		/*else{*/
			/*printf("Waiting for thread %d to be finished, its status is %d meanwhile, entry_point: %x\n", i*/
				/*, threads_arr[i].status, threads_arr[i].entry_point);*/
			/*// TODO - continue thread according to status*/
			/*if (threads_arr[i].status != RUNNING)*/
			/*{*/
				/*// RUN thread*/
				/*int ep = threads_arr[i].entry_point(threads_arr[i].arg);*/
				/*threads_arr[i].status = FINISHED;*/
			/*}*/
		/*}*/
	/*}*/
        sleep(5);
    }

}


void SCHEDULER__print_thread(int index){
	printf("print_thread arg: %p, status: %d\n", threads_arr[index].arg, threads_arr[index].status);
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
	
	// printf("adding thread, size: %d, entry_point: %x, arg: %x\n",size, entry_point, arg);
	threads_arr[size].entry_point = entry_point;
	threads_arr[size].arg = arg;
	threads_arr[size].status = WAITING;
	threads_arr[size].stopped_point = NULL;
	size++;
}

void SCHEDULER__test(){
	int index = 1;
	printf("testing TEST addr %x\n", threads_arr[index].entry_point(index));
	// threads[index].entry_point(threads[index].arg);
}
