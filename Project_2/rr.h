#ifndef RR
#define RR
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

extern struct scheduler roundrobin;
extern scheduler RR; 

// add our linked list struct here 
// add global queueu variable here!

void rr_init();
void rr_shutdown();
void rr_admit(thread new);
void rr_remove(thread victim);
thread rr-next();
int qlen(void);
void enqueue();
void dequeue();


# endif


