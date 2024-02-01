#include "lwp.h"

// Creates new Lightweight Process which executes given function with given argument
// returns LWP id of new thread or NO_THREAD if thread cannot be created 
tid_t lwp_create(lwmpfun function, void *argument);


// Starts LWP system - converts calling thread into LWP 
void lwp_start(void);

// Yields control to another LWP
void lwp_yield(void);

// Terminates current LWP and yields to whichever thread the scheduler choose
void lwp_exit(void);
void lwp_exit(int exitval);
tid_t lwp_wait(int *status);
tid_t lwp_gettid(void);
thread tid2thread(tid_t tid);
void lwp_set_scheduler(scheduler sched);
scheduler lwp_get_scheduler(void);