#include "lwp.h"

// Creating new Lightweight Process, Adds to Scheduler, But does not run it 
tid_t lwp_create(lwmpfun function, void *argument) {
    // parameter function = has code to be executed by thread 
    // when function is called, the code will be executed until it either calls lwp_exit()
    // or function terminates with termination status 
    // thread function takes single arg (pointer to anything) aka parameter argument 

    // Creates new thread 
    // Admits it to the current scheduler 
    // Creates resources for it: context and stack (intilaize both and connect with scheduler)
    // DO NOT RUN THIS PROCESS! up to scheduler to do that 

    // returns LWP id of new thread or NO_THREAD if thread cannot be created 
}


// Starts LWP system
void lwp_start(void){
    // Starts LWP system by converting calling thread into LWP
    // allocates context for main thread and admits to scheduler 
    // yields control to whichever thread scheduler indicates 

    // NOTE: no need to make stack for this thread, it already has one! 
}

// Yields control to another thread 
void lwp_yield(void){
    // Yields control to another thread depending on scheduler 
    // Here is all the stack moving stuff happening
    // Saving current thread's context, restoring next thread's context

    // If no next thread, end entire program = call exit with termination status of calling thread
}

// Terminates current thread and yields to whichever thread the scheduler choose
void lwp_exit(int exitval) {
    // terminates calling thread!! 
    // termination status becomes low 8 bits of passed integer (exitval)
    

    // don't deallocate or anything - the next thread will deallocate it for us in lwp_wait()

    // yields control to next thread using lwp_yield()
}


// Waiting for thread to terminate 
tid_t lwp_wait(int *status){
    // waiting for thread to terminate
    // If multiple terminated threads, grab in FIFO terminated threads (aka oldest thread first)
    // If there are no terminated threads and still running ones, caller of lwp_wait() needs to block
    // We will deschedule it, sched->remove() , and place it on queue of waiting threads 
    // Once a thread FINALLY calls lwp_exit() - make it be known its the oldest thread somehow (us to decide)
    // and then remove the blocking process from our waiting queue and readmit it back into the schedule
    // with sched->admit() command so it can finish its lwp_wait() command


    // NOTE: if there are literally no threads left that could block, then we just return
    // NO_THREAD; we can tell this by using qlen(): check if qlen() > 1? (is this correct?)

    // deallocates resources of terminated LWP
    // NOTE: be careful don't deallocate stack of main thread

    // if status is non-NULL (status has termination status)? weird phrasing (need to do something with status)
    // reports termination status if not NULL

    // returns either tid of terminated threat or NO_THREAD 
    // if blocking forever (since no more runnable threads that could terminate)
}

// Getting tid of called LWP 
tid_t lwp_gettid(void){
    // returns either tid of thread or NO_THREAD if not called by LWP
}

// given a thread ID, return thread 
thread tid2thread(tid_t tid){
    // given a thread ID, return thread or NO_THREAD if ID is invalid
}

// LWP package uses scheduler to choose next process to run
void lwp_set_scheduler(scheduler sched) {
    // given scheduler transfer all threads from old scheduler to new one 
    // using the next() order
    // if scheduler is NULL, return back to round-robin scheduling
}

// Helps us get current scheduler 
scheduler lwp_get_scheduler(void){
    // return pointer to current scheduler
}

// current process we are running 
int main(void) {


    lwp_start();

}