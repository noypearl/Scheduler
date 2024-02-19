#include <stdio.h>
#include <unistd.h>
#include "THREAD.h"


// Wrapping thread with statuses
typedef enum STATUS {
	STOPPED,
	READY,
	RUNNING,
	FINISHED
};

int currSize;
/*
The ARM64 calling convention specifies that the first eight parameters to a function are passed in registers x0 through x7. 
Additional parameters are passed on the stack. 
x9 to x15 - More temporary registers, often used for local variables.
The return value is passed back in register x0, or in x1 as well if it's 128 bits. 
The x19 to x30 and sp registers must be preserved across function calls.
*/
typedef struct {
	uint64_t pc; // program counter (r15) -> current instruction executed
	uint64_t sp; // stack pointer (sp)
	uint64_t lr; // return addr  (x30)
	uint64_t cpsr; // status pointer (r14)
	uint64_t x0; 
	uint64_t x1; 
	uint64_t x2; 
	uint64_t x3; 
	uint64_t x4; 
	uint64_t x5; 
	uint64_t x6; 
	uint64_t x7; 
	uint64_t fp; // frame pointer (x29)
	uint64_t x9;
	uint64_t x10;
	uint64_t x11;
	uint64_t x12;
	uint64_t x13;
	uint64_t x14;
	uint64_t x15;

} context;

struct {
	THREAD__entry_point_t *entry_point;
	void *pcb_addr;
	void *arg;
	enum STATUS status;
	context ctx;
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

// Return new context struct
context getNewContext(){
	context newContext;
	__asm__ volatile ("mov %0, x30" : "=r"(newContext.lr) ::);
	__asm__ volatile ("mov %0, sp" : "=r"(newContext.sp) ::);
	__asm__ volatile ("mov %0, x0" : "=r"(newContext.x0) ::);
	__asm__ volatile ("mov %0, x1" : "=r"(newContext.x1) ::);
	__asm__ volatile ("mov %0, x2" : "=r"(newContext.x2) ::);
	__asm__ volatile ("mov %0, x3" : "=r"(newContext.x3) ::);

    // printf("Program Counter: %p\n", newContext.pc);
    printf("Stack Pointer: %p\n", newContext.sp);
    printf("X2: %p\n", newContext.x2);
    printf("X1: %p\n", newContext.x1);
    printf("X0: %p\n", newContext.x0);
    printf("Ret addr: %p\n", newContext.lr);
	return newContext;
}

void setContext(context ctx){
	// does shinenigans with PCB & context block
	// And also jumps to the address of the ctx & sets esp
	// __asm__("movl (%%ebp), %0" : "=r"(pc)); // This gets the return address, close to current PC
    // __asm__("movl %%esp, %0" : "=r"(sp));

    // printf("Program Counter: %p\n", pc);
    // printf("Stack Pointer: %p\n", sp);
}
void mytest(int first, int second, int third, int fourth){
	context newContext;
	__asm__ volatile ("mov %0, x30" : "=r"(newContext.lr) ::);
	__asm__ volatile ("mov %0, sp" : "=r"(newContext.sp) ::);
	__asm__ volatile ("mov %0, x0" : "=r"(newContext.x0) ::);
	__asm__ volatile ("mov %0, x1" : "=r"(newContext.x1) ::);
	__asm__ volatile ("mov %0, x2" : "=r"(newContext.x2) ::);
	__asm__ volatile ("mov %0, x3" : "=r"(newContext.x3) ::);
	__asm__ volatile ("mov %0, x9" : "=r"(newContext.x9) ::);
	__asm__ volatile ("mov %0, x10" : "=r"(newContext.x10) ::);
	__asm__ volatile ("mov %0, x29" : "=r"(newContext.fp) ::);
	// int val = 3;
	// __asm__ volatile ("mov x9, %0" : "=r"(val) ::);

    // printf("Program Counter: %p\n", newContext.pc);
    printf("TEST\n");
    printf("Stack Pointer: %p\n", newContext.sp);
    printf("X9: %p\n", newContext.x9);
    printf("X5: %p\n", newContext.x5);
    printf("X4: %p\n", newContext.x4);
    printf("X3: %p\n", newContext.x3);
    printf("X2: %p\n", newContext.x2);
    printf("X1: %p\n", newContext.x1);
    printf("X0: %p\n", newContext.x0);
    printf("fp: %p\n", newContext.fp);
    printf("Ret addr: %p\n", newContext.lr);

}
/* Yield function, to be used by the schedulers threads.
 * Once a thread calls yield, execution should continue from a 
 * different thread. 
 * ARM: We need to save the context   */
void SCHEDULER__yield(void){
	mytest(3, 7,126,9);
    printf("in SCHEDULER__yield\n");
    int nextThreadIndex = -1;
    int i = 0;
    printf("in YIELD!\n");

	// context ctx = getNewContext();
    /*__asm__("movl %edx, %eax\n\t"*/
        /*"addl $2, %eax\n\t");*/
    
// TODO - schedule start scheduler
	/*for (i = 0; i < sizeof(threads_arr)/sizeof(threads_arr[0]); i++)*/
	/*{	*/

}

void resumeThread(int index){
    printf("resumeThread\n");
    threads_arr[index].status = RUNNING;
    /**((int *)(threads_arr[index].stopped_point));*/

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
        else if ( threadStatus == READY ){
                printf("Ready thread\n");
                initThread(nextThreadToHandleIndex);
        }
        else{
                printf("Weird state arrived at schedule_threads, state: %d,  index: %d\n", threadStatus, nextThreadToHandleIndex);
        }

		// TODO - next need to loop all the threads in memory PCB and if there's a thread that was saved - need to jump back to him. 

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
        sleep(1);
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
	threads_arr[size].status = READY;
	/*threads_arr[size].stopped_point = NULL;*/
	size++;
}

void SCHEDULER__test(){
	int index = 1;
	printf("testing TEST addr %x\n", threads_arr[index].entry_point(index));
	// threads[index].entry_point(threads[index].arg);
}
