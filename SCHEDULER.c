#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "THREAD.h"
#include <sys/types.h>
#include <sys/mman.h>


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
	uint64_t x8; 
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

int idx = 0; // current running thread

void SCHEDULER__init(void){
	// printf("%s", "in SCHEDULER__init\n");
	currSize = 0;
}

void initThread(int index){
    // printf("initThread\n");
    threads_arr[index].status = RUNNING;
    int status = threads_arr[index].entry_point(threads_arr[index].arg);
	// __asm__ volatile ("mov sp, %0" : : "r" (curr_sp) : "sp"); // restore sp
	// __asm__ volatile ("mov lr, %0" : : "r" (curr_lr) : "lr"); // copy var to sp
	// idx = index; // setting index of running thread
				 // threads_arr[index].status = FINISHED; // TODO - there's maybe a bug when yield() was called and the state is now FINISHED instead of STOPPED
	threads_arr[index].status = FINISHED;
}
void resumeThread(int index){
    printf("resumeThread index %d\n", index);
	// TODO - restore registers & sp & jump to pc
    threads_arr[index].status = RUNNING;
    int status = threads_arr[index].entry_point(threads_arr[index].arg);

	// threads_arr[index].status = FINISHED; // TODO - there's maybe a bug when yield() was called and the state is now FINISHED instead of STOPPED

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
    // printf("%s", "in schedule_threads\n");
    int nextThreadToHandleIndex = -1;
    while((nextThreadToHandleIndex = getNextThreadIndexToHandle()) != -1){ // index of the next thread we should start or resume. Stop while all threads finished.
        enum STATUS threadStatus = threads_arr[nextThreadToHandleIndex].status;
        if( threadStatus == STOPPED ){
                // printf("Stopped thread\n");
                resumeThread(nextThreadToHandleIndex);
                
                // apply stopped from yield - recover frame address, set thread to RUNNING, call it
        }
        else if ( threadStatus == READY ){
                // printf("Ready thread\n");
			uint64_t curr_sp = 0;
			uint64_t curr_lr = 0;
			__asm__ volatile ("mov %0, sp" : "=r"(curr_sp) ::);  // copy sp to var
			__asm__ volatile ("mov %0, lr" : "=r"(curr_lr) ::);  // copy sp to var
			uint64_t* new_sp = 0;
			new_sp = mmap(NULL, 1024, PROT_READ|PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0 );
			printf("setting new SP:  %p\n", new_sp);
			__asm__ volatile ("mov sp, %0" : : "r" (new_sp) : "sp"); // copy var to sp
			threads_arr[nextThreadToHandleIndex].ctx.sp = new_sp;
			// __asm__ volatile ("mov lr, %0" : : "r" (curr_lr) : "lr"); // copy var to sp
			initThread(nextThreadToHandleIndex);
			__asm__ volatile ("mov sp, %0" : : "r" (curr_sp) : "sp"); // copy var to sp
			__asm__ volatile ("mov lr, %0" : : "r" (curr_lr) : "lr"); // copy var to sp
			idx = nextThreadToHandleIndex; // setting index of running thread
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

/* Yield function, to be used by the schedulers threads.
 * Once a thread calls yield, execution should continue from a 
 * different thread. 
 * ARM: We need to save the context and set LR   */
void SCHEDULER__yield(void){
	printf("in YIELD at 1st line!\n");
	context newContext;
	uint64_t returnAddr; // 1 line before yield()
	uint64_t *prev_sp; //previous stack pointer to restore
	uint64_t *fp; // one of the registers to restore TODO - maybe I should restore it from sp (?)
	uint64_t *sp = threads_arr[idx].ctx.sp; // one of the registers to restore TODO - maybe I should restore it from sp (?)
	uint64_t *lr; // 1 line after field()
	uint64_t *local_sp; // 1 line after field()
	__asm__ volatile ("mov %0, x0" : "=r"(newContext.x0) ::); // save regs
	__asm__ volatile ("mov %0, x1" : "=r"(newContext.x1) ::); // save regs
	__asm__ volatile ("mov %0, x2" : "=r"(newContext.x2) ::); // save regs
	__asm__ volatile ("mov %0, x3" : "=r"(newContext.x3) ::); // save regs
	__asm__ volatile ("mov %0, x4" : "=r"(newContext.x4) ::); // save regs
	__asm__ volatile ("mov %0, x5" : "=r"(newContext.x5) ::); // save regs
	__asm__ volatile ("mov %0, x6" : "=r"(newContext.x6) ::); // save regs
	__asm__ volatile ("mov %0, x7" : "=r"(newContext.x7) ::); // save regs
	__asm__ volatile ("mov %0, x8" : "=r"(newContext.x8) ::); // save regs
	__asm__ volatile ("mov %0, x9" : "=r"(newContext.x9) ::); // save regs
	__asm__ volatile ("mov %0, x10" : "=r"(newContext.x10) ::); // save regs
	__asm__ volatile ("mov %0, lr" : "=r"(newContext.lr) ::); 
	__asm__ volatile ("mov %0, sp" : "=r"(newContext.sp) ::); 
	newContext.pc = lr; // MIND = BLOWN!! by that I will continue to the rest of the function by calling to lr
	
	threads_arr[idx].ctx = newContext;
	u_int64_t n;
	newContext.lr = newContext.lr & 0xfffffff;
	// printf("addr of 1 line after the thread: %p\n", (newContext.lr));

	// printf("calling returnAddr %p\n", returnAddr);
	// void (*foo)(void) = (void (*)())returnAddr;
	// foo();
	// __asm__ volatile ("mov x1, 0"); // restore sp
	// __asm__ volatile ("add x1, x1 , pc"); 
    int threadToStartOrResumeIndex = -1;
	/* Now we need to run the next thread - I couldn't find a way to terminate this func so I'm 
	calling the next thread
	*/
    while((threadToStartOrResumeIndex = getNextThreadIndexToHandle()) != -1){ // index of the next thread we should start or resume. Stop while all threads finished.
        enum STATUS threadStatus = threads_arr[threadToStartOrResumeIndex].status;
        if( threadStatus == READY  && idx != threadToStartOrResumeIndex){ // so we won't run the thread forever
		// TODO - test if the addon is ok
			printf("[in yield while loop] Found thread to resume, number: %d\n", threadToStartOrResumeIndex);
			// uint64_t curr_sp = 0;
			// uint64_t curr_lr = 0;
			// __asm__ volatile ("mov %0, sp" : "=r"(curr_sp) ::);  // copy sp to var
			// __asm__ volatile ("mov %0, lr" : "=r"(curr_lr) ::);  // copy sp to var
			// uint64_t* new_sp = 0;
			// new_sp = mmap(NULL, 1024, PROT_READ|PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0 );
			// printf("setting new SP:  %p\n", new_sp);
			// __asm__ volatile ("mov sp, %0" : : "r" (new_sp) : "sp"); // copy var to sp
			// threads_arr[threadToStartOrResumeIndex].ctx.sp = new_sp;
			printf("initializing thread\n");
			initThread(threadToStartOrResumeIndex);
			// __asm__ volatile ("mov sp, %0" : : "r" (curr_sp) : "sp"); // copy var to sp
			// __asm__ volatile ("mov lr, %0" : : "r" (curr_lr) : "lr"); // copy var to sp
			break;
		}
		else if (threadStatus == STOPPED && idx != threadToStartOrResumeIndex){
			// TODO - understand how to context switch here
			resumeThread(threadToStartOrResumeIndex);
		}
	}
// TODO - schedule start scheduler
	return;
}


/* Initialize a specific thread and add it to the thread pool. */
void SCHEDULER__add_thread(THREAD__entry_point_t *entry_point,
						    void *arg){
	
	// printf("adding thread, size: %d, entry_point: %x, arg: %x\n",size, entry_point, arg);
	threads_arr[size].entry_point = entry_point;
	threads_arr[size].arg = arg;
	threads_arr[size].status = READY;
	size++;
}

void SCHEDULER__test(){
	int index = 1;
	printf("testing TEST addr %x\n", threads_arr[index].entry_point(index));
	// threads[index].entry_point(threads[index].arg);
}
