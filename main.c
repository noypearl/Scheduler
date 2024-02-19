#include "SCHEDULER.h"
#include <stdio.h>
#include <stdlib.h>

int thread_1(void *arg)
{
	printf("[T1 START] Hello from thread 1 arg is a pointer to 0x%08x!\r\n", arg);
	int number3 = 3;
	int number4 = 4;
	int number5 = 5;
	int number6 = 6;
	// SCHEDULER__yield(); // in yield I need to check the running process, set it to stopped and run the next one
	while(1){
		printf("A");
		SCHEDULER__yield(); // in yield I need to check the running process, set it to stopped and run the next one
	}
	printf("[T1] number (3) : %d\n", number3);
	printf("[T1] number (4) : %d\n", number4);
	// printf("[T1] number (5) : %d\n", number5);
	printf("[T1 END] Hello from thread 1 again!\r\n");
	// printf("[T1] number (6) : %d\n", number6);
	return 0;
}

int thread_2(void *arg)
{
	printf("[T2 START] Hello from thread 2! arg is a pointer to 0x%08x\r\n", arg);
	while(1){
		printf("B");
		SCHEDULER__yield(); // in yield I need to check the running process, set it to stopped and run the next one
	}
	int number6 = 6;
	int number7 = 7;
	int number8 = 8;
	// printf("number (6) : %d\n", number6);
	// printf("number (7) : %d\n", number7);
	printf("number (8) : %d\n", number8);
	printf("[T2 END] Hello from thread 2 again!\r\n");

	return 0;
}

int thread_3(void *arg)
{
	printf("[T3 START] Hello from thread 3! arg is a pointer to 0x%08x\r\n", arg);
	int number8 = 8;
	int number9 = 9;
	// printf("[T3] number (8) : %d\n", number8);
	printf("[T3] number (9) : %d\n", number9);
	printf("[T3 END] Hello from thread 3 again!\r\n");

	return 0;
}

int thread_1_arg = 0;
int thread_2_arg = 1;
int thread_3_arg = 1337;

struct
{
	THREAD__entry_point_t *entry_point;
	void *arg;
} threads[] = {
	{thread_1, &thread_1_arg},
	{thread_2, &thread_2_arg},
	{thread_3, &thread_3_arg},
};

int main(void)
{
	SCHEDULER__init();
	/* Add all threads to the scheduler. */
	for (int i = 0; i < sizeof(threads) / sizeof(threads[0]); i++)
	{
		SCHEDULER__add_thread(threads[i].entry_point, threads[i].arg);
	}
	SCHEDULER__schedule_threads();

	return 0;
}
