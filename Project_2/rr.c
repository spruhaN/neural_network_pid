#include "schedule.h"

void print_node(struct Node *node) {
    if (node == NULL) {
        fprintf(stdout, "Node is NULL\n");
        return;
    }
	fprintf(stdout, "Pid: %d\n", node->child_pid);
    fprintf(stdout, "Filename: %s\n", node->filename);
    fprintf(stdout, "Arguments: ");
	int i = 0;
	if (node->args_list != NULL) {
		for (;node->args_list[i] != NULL; i++) {
			fprintf(stdout, "%s  ", node->args_list[i]);
		}
		fprintf(stdout, "\n");
	}
}

void print_queue(struct Node *nodeArray[]) {
    if (front == -1) {
        fprintf(stdout, "Queue is empty.\n");
    } else {
		int i = front;
        fprintf(stdout, "Queue:\n");
        while (i != end) {
            fprintf(stdout, "Node %d-\n", i + 1);
        	print_node(nodeArray[i]);
            i = (i + 1) % MAX_PROCESSES;
        }
        fprintf(stdout, "Node %d-\n", i + 1);
        print_node(nodeArray[i]);
    }
}

void clean_node(struct Node *node) {
	if (node == NULL) {
    	return;
    }
    if (node->args_list != NULL) {
        free(node->args_list);
    }

	if(node->filename != NULL) {
		free(node->filename);
	}

    free(node); // free node itself
}

void child_handler(int signum){
	// fprintf(stderr,"Child stopped or child ended.\n");
	sig_child_flag = 1;
}

void timer_handler(int signum) {
    // fprintf(stderr, "Timer expired. Stopping process.\n");  // always use fprintf(stderr)
	kill(pid, SIGSTOP);
	sig_alarm_flag = 1;
}

// in parent 
void setting_up_sigchild(){
	// Set up SIGCHILD handler
	signal(SIGCHLD, child_handler);
	signal(SIGALRM, timer_handler);
}

void setting_up_timer(int quantum){
    // Setting up timer with quantum
	struct itimerval it;
    it.it_value.tv_sec = quantum / 1000;
    it.it_value.tv_usec = (quantum % 1000) * 1000;
    it.it_interval.tv_sec = 0;
    it.it_interval.tv_usec = 0;

    if(setitimer(ITIMER_REAL, &it, NULL) == -1) {
        fprintf(stderr, "setitimer");
        exit(EXIT_FAILURE);
    }
}

void enqueue(struct Node *nodeArray[], struct Node *node) {
	if ((end + 1) % MAX_PROCESSES == front) {
		fprintf(stderr, "Queue is full");
	}
	else {
		// if we have an empty queue will set front to -1 
		// now we will set it to 0 to show that we have added element 
		if(front == -1) {
			front = 0;
		}
		// add 1 to end and add node
		end = (end + 1) % MAX_PROCESSES;
		nodeArray[end] = node;
	}
}

struct Node *dequeue(struct Node *nodeArray[]) {
	struct Node *node = NULL; // node we will give back
	if(front == -1 || end == -1) {	// if queue is empty!
		fprintf(stderr, "Empty Queue - no more space to add.\n");
	}
	else {
		node = nodeArray[front]; // grab node from front 
		nodeArray[front] = NULL; // set the spot in list to NUll
		if (front == end) {	// if we now have empty queue (front = end) set it to -1 to show this
			front = -1;
			end = -1;
		} else {
			front = (front + 1) % MAX_PROCESSES;
		}
	}
	return node;
}

int main(int argc, char *argv[]) {
	// Setting up everything 
	if(argc < 3) {
		// Fprintf formats to output stream; output stream is line-buffered when connected to a terminal
		fprintf(stderr, "Format is incorrect. Usage: schedule quantum [prog 1 [args] [: prog 2 [args] [: prog3 [args] [: ... ]]]].\n");
		return 1;
	}

	char* other;
	int quantum = strtol(argv[1], &other, 0); 	// formatting time for timer
	 if (*other != '\0' || other == argv[1]) {
		fprintf(stderr, "Format is incorrect. Either missing quantum or quantum needs to be integer.\n");
		return 1;
	}

	struct Node *nodeArray[MAX_PROCESSES]; 		// array to hold all process information 
  
	int i = 2;
	for(; i < argc; i++) {
		// setting up new node for process 
		struct Node *newNode = (struct Node *)malloc(sizeof(struct Node)); 		// made new node
		
		// Way to check if it is a bin command because we can't execute ls but we can execute "/bin/ls"
		if (access(argv[i] , X_OK) == 0) { 
			newNode->filename = (char *)malloc((strlen("./") + strlen(argv[i]) + 1) * sizeof(char));
			strcpy(newNode->filename, "./");
			strcat(newNode->filename, argv[i]);
		} else {
			newNode->filename = (char *)malloc((strlen(argv[i]) + 1) * sizeof(char));
			newNode->filename = strcpy(newNode->filename, argv[i]);
		}

		// making list to hold arguments for program + made it plus 1 for NULL
		newNode->args_list = (char **)malloc((MAX_ARG_COUNT + 1) * sizeof(char *));

		if (newNode->args_list == NULL) { // if error mallocing
			perror("Argument_List Malloc Error");
			return 1;
		}

		int args_list_count = 0;
		newNode->args_list[args_list_count++] = newNode->filename;

		// filling up args list 
		int j = i + 1;
		for(;j < argc; j++) {
			if(strcmp(argv[j], ":") == 0) {
				newNode->args_list[args_list_count++] = NULL;	// end array
				i = j;
				break;
			} // if not :, then let's append argument to argument list! 
			else {
				newNode->args_list[args_list_count++] = argv[j];	// add value to list
			}
		}
		if(j == argc){ // checking if broke out naturally or not
			newNode->args_list[args_list_count++] = NULL;	// end array
			i = j;
		}

		// Process each program and just open and automatically stop it!
		pid_t pid = fork();
		if(pid == -1) {		// issue
			fprintf(stderr,"Error during forking");
			return 1;
		} else if (pid > 0 ) { 	// parent
			newNode->child_pid = pid;
			int status;
			waitpid(pid, &status, WUNTRACED);
			//pause();
		} else { 	 // child
			raise(SIGSTOP);  // right away just get child in queue and SIGSTOP it 
			if(execvp(newNode->args_list[0], newNode->args_list) == -1){
				fprintf(stdout, "ERROR!! %d\n", getpid());
				fprintf(stderr, "Error running process");
				exit(-1);
			}
		}
		enqueue(nodeArray, newNode);
	}

	// red robin scheduling time!!
	setting_up_sigchild();	// parent can now handle sigchilds and sigalarms

	while(front != -1 && end != -1) {
		struct Node *node = dequeue(nodeArray);
		setting_up_timer(quantum); // setting up timer
		pid = node->child_pid;
		kill(node->child_pid, SIGCONT);

		int status;
		waitpid(node->child_pid, &status, WUNTRACED);
	
		if (node != NULL) {
			if (sig_alarm_flag) { // then alarm went off 
				//fprintf(stdout, "Sig_alarm.\n");
				enqueue(nodeArray, node); // if process isn't done put back into queue
				//fprintf(stdout, "Child readded.\n");
				//print_queue(nodeArray);
			} 
			else if (sig_child_flag) { // then if not alarm then child went off  
				//fprintf(stdout, "Sig_child.\n");
				clean_node(node);
				// process has been dequeued so nothing to do, just won't put it back
			}
			else {
				//fprintf(stderr, "Something else went wrong with the child program.\n");
				//fprintf(stderr, "%d\n", node->child_pid);
				kill(node->child_pid, SIGTERM); 
			}
		} else {
			fprintf(stdout, "node is null!!!\n");
		}
		//fprintf(stdout, "child flag %d\n", sig_child_flag);
		//fprintf(stdout, "alarm flag %d\n", sig_alarm_flag);
		sig_child_flag = 0;
		sig_alarm_flag = 0;
	}

	return 0;
}



// idea is to change this to deal with threads! 
// 