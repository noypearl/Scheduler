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
	uint64_t fp; // frame pointer (x29)
	uint64_t x0; 
	uint64_t x1; 
	uint64_t x2; 
	uint64_t x3; 
	uint64_t x4; 
	uint64_t x5; 
	uint64_t x6; 
	uint64_t x7; 
	uint64_t x8; 
	uint64_t x9;
	uint64_t x10;
	uint64_t x11;
	uint64_t x12;
	uint64_t x13;
	uint64_t x14;
	uint64_t x15;
	uint64_t x16;
	uint64_t x17;
	uint64_t x18;
	uint64_t x19;
	uint64_t x20;
	uint64_t x21;
	uint64_t x22;
	uint64_t x23;
	uint64_t x24;
	uint64_t x25;
	uint64_t x26;
	uint64_t x27;
	uint64_t x28;
} context;

struct {
	THREAD__entry_point_t *entry_point;
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

// Loops all thread array and returns the index of the next thread we shuold handle, 
// return -1 if no threads to handle
int getNextThreadIndexToHandleIndex(int currIdx){
	for (int i = 0; i < sizeof(threads_arr)/sizeof(threads_arr[0]); i++)
	{
		enum STATUS threadStatus = threads_arr[i].status;
		if(( threadStatus != FINISHED ) && (threadStatus != RUNNING) && (i != currIdx)){
			printf("Returning from getNextThreadIndexToHandleIndex, i: %d\n", i);
			return i;
		}
	}
	return -1;
}

// Program should arrive here at the end
void terminateProgram(void){
	printf("No more threads to handle! YAY! \n");
	exit(0);
}

void thread_dieded(){	
	threads_arr[idx].status = FINISHED;
	printf("DIEDED THREAD. Setting %d index thread to FINISHED\n", idx);
	int nextThread = getNextThreadIndexToHandleIndex(idx);
	// Context switch should be here
	if ( nextThread != -1 && nextThread < size ){ 
		printf("Next should handle thread index %d\n", nextThread);
			uint64_t curr_sp = 0;
			uint64_t curr_lr = 0;
			__asm__ volatile ("mov %0, sp" : "=r"(curr_sp) ::);  // copy sp to var
			__asm__ volatile ("mov %0, lr" : "=r"(curr_lr) ::);  // copy sp to var
			uint64_t* new_sp = 0;
			new_sp = mmap(NULL, 1024, PROT_READ|PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0 );
			uint64_t dieded_addr = &thread_dieded;
			__asm__ volatile ("mov lr, %0" : : "r" (dieded_addr) : "lr"); // copy var to sp
			// printf("[be4 init] setting new SP:  %p\n", new_sp);
			__asm__ volatile ("mov sp, %0" : : "r" (new_sp) : "sp"); // copy var to sp
			threads_arr[nextThread].ctx.sp = new_sp;
			threads_arr[nextThread].ctx.lr = dieded_addr;
			idx = nextThread; // setting index of running thread
			threads_arr[nextThread].status = RUNNING;
			__asm__ volatile ( "BR %0" : : "r" (threads_arr[nextThread].entry_point):); // TODO - solve argument that's printed
	}
	else{
		terminateProgram();
	}
	// should run the next thread - do the context switch
	// check if no threads to run - then exit(0)
}
// DO
/* Start the scheduler. This function will return only when all
 * threads finished their work. */
void SCHEDULER__schedule_threads(void){
    // printf("%s", "in schedule_threads\n");
    int nextThreadToHandleIndex = getNextThreadIndexToHandle();
	if (nextThreadToHandleIndex != -1){
			uint64_t curr_sp = 0;
			uint64_t curr_lr = 0;
			__asm__ volatile ("mov %0, sp" : "=r"(curr_sp) ::);  // copy sp to var
			__asm__ volatile ("mov %0, lr" : "=r"(curr_lr) ::);  // copy sp to var
			uint64_t* new_sp = 0;
			new_sp = mmap(NULL, 1024, PROT_READ|PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0 );
			uint64_t dieded_addr = &thread_dieded;
			// dieded_addr = dieded_addr & 0xfffffff;
			// new_sp[0] = &thread_dieded;
			// new_sp[1] = &thread_dieded;
			__asm__ volatile ("mov lr, %0" : : "r" (dieded_addr) : "lr"); // copy var to sp
			// printf("[be4 init] setting new SP:  %p\n", new_sp);
			__asm__ volatile ("mov sp, %0" : : "r" (new_sp) : "sp"); // copy var to sp
			threads_arr[nextThreadToHandleIndex].ctx.sp = new_sp;
			threads_arr[nextThreadToHandleIndex].ctx.lr = dieded_addr;
			idx = nextThreadToHandleIndex; // setting index of running thread
			__asm__ volatile ( "BR %0" : : "r" (threads_arr[nextThreadToHandleIndex].entry_point):); // TODO - solve argument that's printed
			threads_arr[nextThreadToHandleIndex].status = RUNNING;
			// __asm__ volatile ("mov sp, %0" : : "r" (curr_sp) : "sp"); // copy var to sp
			// __asm__ volatile ("mov lr, %0" : : "r" (curr_lr) : "lr"); // copy var to sp
        }
	else {
		terminateProgram();
	}
    // while((nextThreadToHandleIndex = getNextThreadIndexToHandle()) != -1){ // index of the next thread we should start or resume. Stop while all threads finished.
        // enum STATUS threadStatus = threads_arr[nextThreadToHandleIndex].status;
        // if( threadStatus == STOPPED ){
                // printf("Stopped thread\n");
                // resumeThread(nextThreadToHandleIndex);
                
                // apply stopped from yield - recover frame address, set thread to RUNNING, call it
        // }
        // if ( threadStatus == READY ){ //TODO - treat status STOPPED
                // printf("Ready thread\n");
			// printf("[in schedule threads loop] Jumping to : %d\n", nextThreadToHandleIndex);
        sleep(1);
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
	uint64_t *lr = 0; // 1 line after field()
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
	__asm__ volatile ("mov %0, x11" : "=r"(newContext.x11) ::); // save regs
	__asm__ volatile ("mov %0, x12" : "=r"(newContext.x12) ::); // save regs
	__asm__ volatile ("mov %0, x13" : "=r"(newContext.x13) ::); // save regs
	__asm__ volatile ("mov %0, x14" : "=r"(newContext.x14) ::); // save regs
	__asm__ volatile ("mov %0, x15" : "=r"(newContext.x15) ::); // save regs
	__asm__ volatile ("mov %0, x16" : "=r"(newContext.x16) ::); // save regs
	__asm__ volatile ("mov %0, x17" : "=r"(newContext.x17) ::); // save regs
	__asm__ volatile ("mov %0, x18" : "=r"(newContext.x18) ::); // save regs
	__asm__ volatile ("mov %0, x19" : "=r"(newContext.x19) ::); // save regs
	__asm__ volatile ("mov %0, x20" : "=r"(newContext.x20) ::); // save regs
	__asm__ volatile ("mov %0, x21" : "=r"(newContext.x21) ::); // save regs
	__asm__ volatile ("mov %0, x22" : "=r"(newContext.x22) ::); // save regs
	__asm__ volatile ("mov %0, x23" : "=r"(newContext.x23) ::); // save regs
	__asm__ volatile ("mov %0, x24" : "=r"(newContext.x24) ::); // save regs
	__asm__ volatile ("mov %0, x25" : "=r"(newContext.x25) ::); // save regs
	__asm__ volatile ("mov %0, x26" : "=r"(newContext.x26) ::); // save regs
	__asm__ volatile ("mov %0, x27" : "=r"(newContext.x27) ::); // save regs
	__asm__ volatile ("mov %0, x28" : "=r"(newContext.x28) ::); // save regs
	__asm__ volatile ("mov %0, lr" : "=r"(newContext.lr) ::); 
	__asm__ volatile ("mov %0, sp" : "=r"(newContext.sp) ::); // TODO - maybe I shouldn't change sp at that point since I mmap' it 2 functions before?
	newContext.pc = lr; // MIND = BLOWN!! by that I will continue to the rest of the function by calling to lr
	threads_arr[idx].status = STOPPED;
	threads_arr[idx].ctx = newContext;
	u_int64_t n;
	newContext.lr = newContext.lr & 0xfffffff;
    int threadToStartOrResumeIndex = getNextThreadIndexToHandleIndex(idx);
	idx = threadToStartOrResumeIndex; // changing here since we use global variable and can't access threadToStartOrResumeIndex because we change the stack and it's a stack variable!
	printf("YO in yield! nextThreadToHandleIndex is: %d\n", threadToStartOrResumeIndex);
	// Check if need to handle more threads
	if( idx != -1 ){
		if ( threads_arr[idx].status == READY){
			// // Start thread regularly
			uint64_t curr_sp = 0;
			uint64_t curr_lr = 0;
			__asm__ volatile ("mov %0, sp" : "=r"(curr_sp) ::);  // copy sp to var
			__asm__ volatile ("mov %0, lr" : "=r"(curr_lr) ::);  // copy sp to var
			uint64_t* new_sp = 0;
			new_sp = mmap(NULL, 1024, PROT_READ|PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0 );
			uint64_t dieded_addr = &thread_dieded;
			// dieded_addr = dieded_addr & 0xfffffff;
			// new_sp[0] = &thread_dieded;
			// new_sp[1] = &thread_dieded;
			__asm__ volatile ("mov lr, %0" : : "r" (dieded_addr) : "lr"); // copy var to sp
			// printf("[be4 init] setting new SP:  %p\n", new_sp);
			__asm__ volatile ("mov sp, %0" : : "r" (new_sp) : "sp"); // copy var to sp
			threads_arr[idx].ctx.sp = new_sp;
			threads_arr[idx].ctx.lr = dieded_addr;
			// printf("idx in YIELD::: %d", idx);
			threads_arr[idx].status = RUNNING;
			__asm__ volatile ( "BR %0" : : "r" (threads_arr[idx].entry_point):); // TODO - solve argument that's printed
		}
		// Resume thread
		else if ( threads_arr[idx].status == STOPPED){
			// TODO - difficult part
			printf("in yield, now need to handle thread index %d\n", idx);
		// ACTUALLY YIELD LOGIC - SWITCH CONTEXT WITH NEXT ONE
		}
		else{
		printf("Unexpected state of thread in yield\n");
		}
	}
	else{
		printf("in YIELD - no more threads\n");
		terminateProgram();
	}
	
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
