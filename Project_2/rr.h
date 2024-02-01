#ifndef SCHEDULE
#define SCHEDULE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

#define MAX_ARG_COUNT 10
#define MAX_PROCESSES 200

int sig_child_flag = 0; 
int sig_alarm_flag = 0;
pid_t pid = 0;
int front = -1;
int end = -1;

struct Node {
    pid_t child_pid;
    char *filename;
    char **args_list;
};

void setting_up_timer(int quantum);
void timer_handler(int signum);
void child_handler(int signum);
void setting_up_sigchild();

void print_node(struct Node *node);
void set_pause_timer();
void print_queue(struct Node *nodeArray[]);
void clean_node(struct Node *node);
void enqueue(struct Node *nodeArray[], struct Node *node);
struct Node *dequeue(struct Node *nodeArray[]);


# endif
