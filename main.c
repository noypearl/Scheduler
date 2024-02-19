#include "SCHEDULER.h"
#include <stdio.h>
#include <stdlib.h>

int thread_1(void *arg)
{
	printf("[T1 START] Hello from thread 1 arg is a pointer to 0x%08x!\r\n", arg);
	int number3= 3;
	int number4 = 4;
	int number5 = 5;
	int number6 = 6;
	int number7 = 7;
	// int number8 = 8;
	SCHEDULER__yield(); // in yield I need to check the running process, set it to stopped and run the next one
	// int number9 = 9;
	// int number10 = 10;
	// int number11 = 11;
	// int number12 = 12;
        // for (int i = 0; i < 10; i++) {
	printf("[T1 END] Hello from thread 1 again!\r\n");
	printf("number (3) : %d\n", number3);
	printf("number (4) : %d\n", number4);
	printf("number (5) : %d\n", number5);
	printf("number (6) : %d\n", number6);
	printf("number (7) : %d\n", number7);
	// printf("number (8) : %d\n", number8);
	// printf("number (9) : %d\n", number9);
	// printf("number (10) : %d\n", number10);
	// printf("number (11) : %d\n", number11);
	// printf("number (12) : %d\n", number12);
	// uint64_t curr_sp = 0;
	// __asm__ volatile ("mov %0, sp" : "=r"(curr_sp) ::);  // copy sp to var
	// printf("T1: curr_sp : %p\n", curr_sp);
	return 0;
}

int thread_2(void *arg)
{
	printf("[T2 START] Hello from thread 2! arg is a pointer to 0x%08x\r\n", arg);
	int number6 = 6;
	int number7 = 7;
	int number8 = 8;
	printf("number (6) : %d\n", number6);
	printf("number (7) : %d\n", number7);
	printf("number (8) : %d\n", number8);
	// SCHEDULER__yield();
	// thread_1(NULL);
	printf("[T2 END] Hello from thread 2 again!\r\n");

	return 0;
}

int thread_3(void *arg)
{
	// uint64_t curr_sp = 0;
	// __asm__ volatile ("mov %0, sp" : "=r"(curr_sp) ::);  // copy sp to var
	// printf("T3: curr_sp : %p\n", curr_sp);
	printf("[T3 START] Hello from thread 3! arg is a pointer to 0x%08x\r\n", arg);


	int number8 = 8;
	int number9 = 9;

	printf("[T3] number (8) : %d\n", number8);
	printf("[T3] number (9) : %d\n", number9);
	printf("[T3 END] Hello from thread 3 again!\r\n");

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
