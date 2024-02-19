#include "SCHEDULER.h"
#include <stdio.h>
#include <stdlib.h>

int thread_1(void *arg)
{
	printf("Hello from thread 1 arg is a pointer to 0x%08x!\r\n", arg);
	int number = 3;
	int number2 = 4;
	int number3 = 5;
	int number4 = 6;
	SCHEDULER__yield(); // in yield I need to check the running process, set it to stopped and run the next one
        // for (int i = 0; i < 10; i++) {
	printf("Hello from thread 1 again!\r\n");
	printf("number (3) : %d\n", number);
	printf("number (3) addr : %p\n", &number);
	printf("number2 (4) : %d\n", number2);
	printf("number2 (4) addr : %p\n", &number2);
	uint64_t curr_sp = 0;
	__asm__ volatile ("mov %0, sp" : "=r"(curr_sp) ::);  // copy sp to var
	printf("T1: curr_sp : %p\n", curr_sp);
	return 0;
}

int thread_2(void *arg)
{
	printf("Hello from thread 2! arg is a pointer to 0x%08x\r\n", arg);

	uint64_t curr_sp = 0;
	__asm__ volatile ("mov %0, sp" : "=r"(curr_sp) ::);  // copy sp to var
	printf("T2: curr_sp : %p\n", curr_sp);
	// SCHEDULER__yield();
	// thread_1(NULL);

	printf("Hello from thread 2 again!\r\n");

	return 0;
}

int thread_3(void *arg)
{
	uint64_t curr_sp = 0;
	__asm__ volatile ("mov %0, sp" : "=r"(curr_sp) ::);  // copy sp to var
	printf("T3: curr_sp : %p\n", curr_sp);
	// printf("Hello from thread 3! arg is a pointer to 0x%08x\r\n", arg);

	/*SCHEDULER__yield();*/

	// printf("Hello from thread 3 again!\r\n");

	return 0;
}
				

// int thread_1_arg = 0;
// int thread_2_arg = 1;
// int thread_3_arg = 1337;

int thread_1_arg = 24;
int thread_2_arg = 12;
int thread_3_arg = 1337;

struct {
	THREAD__entry_point_t *entry_point;
	void *arg;
} threads[] = {
	{thread_1, &thread_1_arg},
	{thread_2, &thread_2_arg},
	{thread_3, &thread_3_arg},
};

int main(void)
{
	// size_t i = 0;

	SCHEDULER__init();
	// thread_2(&thread_1_arg);
	/* Add all threads to the scheduler. */
	for (int i = 0; i < sizeof(threads)/sizeof(threads[0]); i++) {
 	// printf("[main] Adding threads at %d, entry_point: %p, arg: 0x%08x, \n", i, threads[i].entry_point, threads[i].arg);
 	SCHEDULER__add_thread(threads[i].entry_point,threads[i].arg);
 }
 	// SCHEDULER__print_threads();
 	SCHEDULER__schedule_threads();

	return 0;
}
