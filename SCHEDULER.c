#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "THREAD.h"
#include <sys/types.h>
#include <sys/mman.h>
#include <signal.h>


// Wrapping thread with statuses
typedef enum STATUS {
	STOPPED,
	READY,
	RUNNING,
	FINISHED
};

void debugger(void){
	raise(SIGINT);
}

int currSize;
/*
The ARM64 calling convention specifies that the first eight parameters to a function are passed in registers x0 through x7. 
Additional parameters are passed on the stack. 
x9 to x15 - More temporary registers, often used for local variables.
The return value is passed back in register x0, or in x1 as well if it's 128 bits. 
The x19 to x30 and sp registers must be preserved across function calls.
*/
typedef struct {
	uint64_t *pc; // program counter (r15) -> current instruction executed
	uint64_t *sp; // stack pointer (sp)
	uint64_t *lr; // return addr  (x30)
	uint64_t *fp; // frame pointer (x29)
	uint64_t *x0; 
	uint64_t *x1; 
	uint64_t *x2; 
	uint64_t *x3; 
	uint64_t *x4; 
	uint64_t *x5; 
	uint64_t *x6; 
	uint64_t *x7; 
	uint64_t *x8; 
	uint64_t *x9;
	uint64_t *x10;
	uint64_t *x11;
	uint64_t *x12;
	uint64_t *x13;
	uint64_t *x14;
	uint64_t *x15;
	uint64_t *x16;
	uint64_t *x17;
	uint64_t *x18;
	uint64_t *x19;
	uint64_t *x20;
	uint64_t *x21;
	uint64_t *x22;
	uint64_t *x23;
	uint64_t *x24;
	uint64_t *x25;
	uint64_t *x26;
	uint64_t *x27;
	uint64_t *x28;
	uint64_t *testlr;
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
	printf("DIEDED THREAD. Setting %d index thread to FINISHED\n", idx);
	threads_arr[idx].status = FINISHED;
	int nextThread = getNextThreadIndexToHandleIndex(idx);
	idx = nextThread;
	printf("Next thread: %d\n", idx);
	// Context switch should be here
	if ( idx != -1 && idx < size ){ 
		if(threads_arr[idx].status == READY){
		printf("Next should handle thread index %d\n", idx);
			uint64_t* new_sp = 0;
			new_sp = mmap(NULL, 1024, PROT_READ|PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0 );
			uint64_t dieded_addr = &thread_dieded;
			__asm__ volatile ("mov lr, %0" : : "r" (dieded_addr) : "lr"); // copy var to sp
			// printf("[be4 init] setting new SP:  %p\n", new_sp);
			__asm__ volatile ("mov sp, %0" : : "r" (new_sp) : "sp"); // copy var to sp
			threads_arr[idx].ctx.sp = new_sp;
			threads_arr[idx].ctx.lr = dieded_addr;
			__asm__ volatile ( "BR %0" : : "r" (threads_arr[idx].entry_point):); // TODO - solve argument that's printed
	}
	// arriving here when a thread was yielded and now need to resume
		else if(threads_arr[idx].status == STOPPED){ // need to CONTEXT SWITCH! 
		__asm__ volatile ("mov lr, %0" : : "r" (threads_arr[idx].ctx.lr) : "lr"); // copy var to lr
			printf("NEED TO CONTEXT SWITCH and continue thread %d yo, val: %d\n", idx, threads_arr[idx].ctx.sp);
			// printf("PC VAL: %x", threads_arr[idx].ctx.pc);
		__asm__ volatile (
		"mov x0, %0\n\t"
		"mov x1, %1\n\t"
		"mov x2, %2\n\t"
		"mov x3, %3\n\t"
		"mov x4, %4\n\t"
		"mov x5, %5\n\t"
		"mov x6, %6\n\t"
		"mov x7, %7\n\t"
		"mov x8, %8\n\t"
		"mov x9, %9\n\t"
		"mov x10, %10\n\t"
		"mov x11, %11\n\t"
		"mov x12, %12\n\t"
		:
		: "r" (threads_arr[idx].ctx.x0), "r" (threads_arr[idx].ctx.x1),
		"r" (threads_arr[idx].ctx.x2), "r" (threads_arr[idx].ctx.x3),
		"r" (threads_arr[idx].ctx.x4), "r" (threads_arr[idx].ctx.x5),
		"r" (threads_arr[idx].ctx.x6), "r" (threads_arr[idx].ctx.x7),
		"r" (threads_arr[idx].ctx.x8), "r" (threads_arr[idx].ctx.x9),
		"r" (threads_arr[idx].ctx.x10), "r" (threads_arr[idx].ctx.x11),
		"r" (threads_arr[idx].ctx.x12)
		: "x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12"
		);
		// TODO - fix registers issue - registers aren't updated!
		// __asm__ volatile (
		// 	"mov x10, %0\n\t"
		// 	"mov x11, %1\n\t"
		// 	"mov x12, %2\n\t"
		// 	"mov x13, %3\n\t"
		// 	"mov x14, %4\n\t"
		// 	"mov x15, %5\n\t"
		// 	"mov x16, %6\n\t"
		// 	"mov x17, %7\n\t"
		// 	"mov x18, %8\n\t"
		// 	"mov x19, %9\n\t"
		// 	:
		// 	: "r" (threads_arr[idx].ctx.x10), "r" (threads_arr[idx].ctx.x11),
		// 	"r" (threads_arr[idx].ctx.x12), "r" (threads_arr[idx].ctx.x13),
		// 	"r" (threads_arr[idx].ctx.x14), "r" (threads_arr[idx].ctx.x15),
		// 	"r" (threads_arr[idx].ctx.x16), "r" (threads_arr[idx].ctx.x17),
		// 	"r" (threads_arr[idx].ctx.x18), "r" (threads_arr[idx].ctx.x19)
		// 	: "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x18", "x19"
		// );
			// tODO - rest of registers
			// debugger();
			__asm__ volatile (
				"mov fp, %0\n\t"  // Move the value to the frame pointer
				"mov sp, %1\n\t"  // Move the value to the stack pointer
				"br %2\n\t"       // Branch to the address contained in pc
				:
				: "r" (threads_arr[idx].ctx.fp), "r" (threads_arr[idx].ctx.sp), "r" (threads_arr[idx].ctx.pc)
				: "fp", "sp" // Specify clobbered registers if any specific ones are affected
			);
			// __asm__ volatile ("mov fp, %0" : : "r" (threads_arr[idx].ctx.fp) : "fp"); // copy var to sp
			// __asm__ volatile ("mov sp, %0" : : "r" (threads_arr[idx].ctx.sp) : "sp"); // copy var to sp
			// __asm__ volatile ("mov %0, lr" : "=r"(newContext.lr) ::); 
			// __asm__ volatile ("mov %0, sp" : "=r"(newContext.sp) ::); // TODO - maybe I shouldn't change sp at that point since I mmap' it 2 functions before?
	}
	else{
		printf("Weird state in thread_dieded.%d Debug program, we shouldn't arrive here", threads_arr[idx].status);
	}
}
	// next thread is -1, need to TERMINATE!
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
	idx = nextThreadToHandleIndex;
	if (idx != -1){
		uint64_t curr_sp = 0;
		uint64_t curr_lr = 0;
		__asm__ volatile (
		"mov %0, sp\n\t"  // Copy sp to curr_sp
		"mov %1, lr"      // Copy lr to curr_lr
		: "=r" (curr_sp), "=r" (curr_lr) // Output operands
		: // No input operands
		: // No clobber list needed as we are not modifying the registers, just reading from them
	);

		uint64_t* new_sp = 0;
		new_sp = mmap(NULL, 1024, PROT_READ|PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0 );
		uint64_t dieded_addr = &thread_dieded;
		threads_arr[idx].ctx.sp = new_sp;
		printf("NEW SP: %p, IDX: %d, VAL: %d\n", new_sp, idx, threads_arr[idx].ctx.sp);
		threads_arr[idx].ctx.lr = dieded_addr;
		threads_arr[idx].status = RUNNING;
		__asm__ volatile (
			"mov lr, %0\n\t"  // copy var to lr
			"mov sp, %1"      // copy var to sp
			: // No output operands
			: "r" (dieded_addr), "r" (new_sp) // Input operands
			: "lr", "sp" // Clobber list indicating that lr and sp are modified
		);

		// printf("[be4 init] setting new SP:  %p\n", new_sp);
		__asm__ volatile ( "BR %0" : : "r" (threads_arr[idx].entry_point):); // TODO - solve argument that's printed
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
	context newContext = {};
	uint64_t returnAddr; // 1 line before yield()
	uint64_t *prev_sp; //previous stack pointer to restore
	uint64_t *fp; // one of the registers to restore TODO - maybe I should restore it from sp (?)
	uint64_t *local_sp; // 1 line after field()
	// __asm__ volatile ("mov %0, lr" : "=r"(newContext.testlr) ::); 
	__asm__ volatile ("mov %0, lr" : "=r"(newContext.lr) ::); // TODO - maybe I shouldn't change sp at that point since I mmap' it 2 functions before?
	__asm__ volatile ("mov %0, fp" : "=r"(newContext.fp) ::); // TODO - maybe I shouldn't change sp at that point since I mmap' it 2 functions before?
	// __asm__ volatile ("mov %0, lr" : "=r"(newContext.lr) ::); 

	newContext.lr = *(newContext.fp+1);
	// printf("NOI FP: %llx\n", newContext.testlr);
	// printf("NOI2 FP: %x\n", *(newContext.fp+1));
	newContext.pc = newContext.lr; // MIND = BLOWN!! by that I will continue to the rest of the function by calling to lr

	__asm__ volatile (
    "mov %0, x0\n\t"
    "mov %1, x1\n\t"
    "mov %2, x2\n\t"
    "mov %3, x3\n\t"
    "mov %4, x4\n\t"
    "mov %5, x5\n\t"
    "mov %6, x6\n\t"
    "mov %7, x7\n\t"
    "mov %8, x8\n\t"
    "mov %9, x9\n\t"
    "mov %10, x10\n\t"
    "mov %11, x11\n\t"
    "mov %12, x12\n\t"
    "mov %13, x13\n\t"
    "mov %14, x14\n\t"
    "mov %15, x15\n\t"
    "mov %16, x16\n\t"
    "mov %17, x17\n\t"
    "mov %18, x18\n\t"
    "mov %19, x19\n\t"
    "mov %20, x20\n\t"
    "mov %21, x21\n\t"
    "mov %22, x22\n\t"
    "mov %23, x23\n\t"
    "mov %24, x24\n\t"
    "mov %25, x25\n\t"
    "mov %26, x26\n\t"
    "mov %27, x27\n\t"
    "mov %28, x28"
    : "=r"(newContext.x0), "=r"(newContext.x1),
      "=r"(newContext.x2), "=r"(newContext.x3),
      "=r"(newContext.x4), "=r"(newContext.x5),
      "=r"(newContext.x6), "=r"(newContext.x7),
      "=r"(newContext.x8), "=r"(newContext.x9),
      "=r"(newContext.x10), "=r"(newContext.x11),
      "=r"(newContext.x12), "=r"(newContext.x13),
      "=r"(newContext.x14), "=r"(newContext.x15),
      "=r"(newContext.x16), "=r"(newContext.x17),
      "=r"(newContext.x18), "=r"(newContext.x19),
      "=r"(newContext.x20), "=r"(newContext.x21),
      "=r"(newContext.x22), "=r"(newContext.x23),
      "=r"(newContext.x24), "=r"(newContext.x25),
      "=r"(newContext.x26), "=r"(newContext.x27),
      "=r"(newContext.x28)
    :: // No clobbered registers specified here, as we're only reading from them
);

	// __asm__ volatile ("mov %0, lr" : "=r"(newContext.lr) ::); // we shouldn't change lr since it should return to dieded func
	threads_arr[idx].status = STOPPED;
	//  CHANGING HERE! setting SP to FP plus 16 bytes
	newContext.sp = newContext.fp+2; // preserve sp
	printf("NEW CONTEXT: fp: %x sp: %x ", newContext.sp, newContext.fp);
	threads_arr[idx].ctx = newContext;
	// printf("Curr sp from yield: %d ", newContext.sp);
    int threadToStartOrResumeIndex = getNextThreadIndexToHandleIndex(idx);
	idx = threadToStartOrResumeIndex; // changing here since we use global variable and can't access threadToStartOrResumeIndex because we change the stack and it's a stack variable!
	printf("YO in yield! nextThreadToHandleIndex is: %d\n", threadToStartOrResumeIndex);
	// Check if need to handle more threads
	if( idx != -1 ){
		if ( threads_arr[idx].status == READY){
			// // Start thread regularly
			uint64_t curr_sp = 0;
			uint64_t curr_lr = 0;
			__asm__ volatile (
			"mov %0, sp\n\t"  // Copy sp to curr_sp
			"mov %1, lr"      // Copy lr to curr_lr
			: "=r"(curr_sp), "=r"(curr_lr) // Output operands
			: // No input operands
			: // No clobbered registers
			);

			uint64_t* new_sp = 0;
			new_sp = mmap(NULL, 1024, PROT_READ|PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0 );
			uint64_t dieded_addr = &thread_dieded;
			*(new_sp) = dieded_addr;// avoid stack frame stp / ldp fp,lr issue
			*(new_sp+1) = 0x33333333;// avoid stack frame stp / ldp fp,lr issue
			threads_arr[idx].ctx.sp = new_sp;
			printf("[yield]NEW SP: %d, IDX: %d, VAL: %d\n", new_sp, idx, threads_arr[idx].ctx.sp);
			threads_arr[idx].ctx.lr = dieded_addr;
		
			__asm__ volatile (
				"mov lr, %0\n\t"  // Set lr to dieded_addr
				"mov sp, %1"      // Set sp to new_sp
				: // No output operands
				: "r" (dieded_addr), "r" (new_sp) // Input operands
				: "lr", "sp" // Clobbered registers
			);
				// trying to fix bug and push return addr
		// 	__asm__ volatile (
		// 	"sub sp, sp, #8\n\t" // Decrement the stack pointer by 8 bytes for a 64-bit value
		// 	"str %0, [sp]"       // Store the value at the new top of the stack (address in sp)
		// 	:                     // No output operands
		// 	: "r"(dieded_addr)          // Input operand: the value to push
		// 	: "sp"                // Clobber list: indicates that this modifies the stack pointer
		// );


			threads_arr[idx].status = RUNNING;
			// threads_arr[idx].entry_point += 8;
			__asm__ volatile ( "BR %0" : : "r" (threads_arr[idx].entry_point):); // TODO - solve argument that's printed
		}
		// Resume thread
		else if ( threads_arr[idx].status == STOPPED){
			// TODO - difficult part
			printf("in yield, now need to handle thread index %d. Exiting\n", idx);
			exit(1);
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
