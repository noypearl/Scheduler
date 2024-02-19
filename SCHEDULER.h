#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "THREAD.h"

/* Yield function, to be used by the schedulers threads.
 * Once a thread calls yield, execution should continue from a 
 * different thread. */
void SCHEDULER__yield(void);

/* Start the scheduler. This function will return only when all
 * threads finished their work. */
void SCHEDULER__schedule_threads(void);

/* Initialize a specific thread and add it to the thread pool. */
void SCHEDULER__add_thread(THREAD__entry_point_t *entry_point,
						    void *arg);

void SCHEDULER__init(void);

// added new
void SCHEDULER__print_thread(int index);
#endif
