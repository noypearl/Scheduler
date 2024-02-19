#include <stdio.h>
#include "THREAD.h"

#define MAX_THREADS = 5;
int currSize;
struct {
	THREAD__entry_point_t *entry_point;
	void *arg;
} threads[5];

int idx = 0;

void SCHEDULER__init(void){
	printf("%s", "in SCHEDULER__init");
	currSize = 0;
	// threads[0].entry_point = 0;
	// threads[0].arg = 'a';

	// TODO - initiallize threads array
}

/* Initialize a specific thread and add it to the thread pool. */
void SCHEDULER__add_thread(THREAD__entry_point_t *entry_point,
						    void *arg){
	
	printf("adding thread, index: %d\n", idx);
	idx++;
	for(int i = 0; idx < currSize; idx++){
		printf("i: %d, index: %d", i, idx);
		if(idx == currSize){
			printf("adding to threads[ %d ]",idx);
			// threads[index] = {entry_point, arg};
		}
	}
}
