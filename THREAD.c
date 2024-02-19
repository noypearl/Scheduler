#include <stdio.h>

int THREAD__entry_point_t(void *arg){
	printf("Doing something MALICOUS in THREAD__entry_point_t %p\n", arg);
	return *(int *)arg;
}