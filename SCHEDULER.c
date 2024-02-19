#include <stdio.h>
#include "THREAD.h"

// Wrapping thread with statuses
typedef enum STATUS {
	WAITING,
	STOPPED,
	RUNNING,
	FINISHED
};

#define MAX_THREADS = 5;
int currSize;
struct {
	THREAD__entry_point_t *entry_point;
	void *arg;
	enum STATUS status;
} threads[3];



int idx = 0;

void SCHEDULER__init(void){
	printf("%s", "in SCHEDULER__init\n");
	currSize = 0;
	// threads[0].entry_point = 0;
	// threads[0].arg = 'a';

	// TODO - initiallize threads array
}


void SCHEDULER__print_thread(int index){
	printf("print_thread arg: %p, status: %d", threads[index].arg, threads[index].status);
}

/* Initialize a specific thread and add it to the thread pool. */
void SCHEDULER__add_thread(THREAD__entry_point_t *entry_point,
						    void *arg){
	
	printf("adding thread, index: %d\n", idx);
	threads[1].entry_point = entry_point;
	threads[1].arg = arg;
	threads[1].status = WAITING;
	// for(int i = 0; idx < currSize+1; idx++){
	// printf("i: %d, index: %d", i, idx);
	// if(idx == currSize){
	printf("adding to threads[ %d ]\n",idx);
		// threads[idx] = {entry_point, &arg};
	// idx++;
	// }
	// }	
}
