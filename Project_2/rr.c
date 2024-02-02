roundrobin = {NULL, NULL, rr_admit, rr_remove, rr_next, rr_qlen}; 
RR = &roundr;
struct Queue *queue;


// adds node to tail 
void enqueue(thread new) {
    struct Node *new_tail = (struct Node *)malloc(sizeof(struct Node)); 
    // set up 
    new_tail.current_thread = new;
    new_tail.next = NULL;
    queue.tail.next = new_tail;
    queue.tail = new_tail;
}

// removes thread from queue 
thread dequeue() {
    struct Node *old_thread_node = queue.head; 
    if (queue.head == NULL){
        fprintf(stderr, "Dequeuing from empty queue\n");
        return NULL;
    }
    queue.head = queue.head.next;
    return old_thread_node;
}

// creates queue structure and puts current process [main] into queue 
//This is to be called before any threads are admitted to the scheduler. It’s to allow the scheduler to set up. 
// This one is allowed to be NULL, so don’t call it if it is.
void rr_init() {
    queue = (struct Queue *)malloc(sizeof(struct Queue)); 	
    struct Node *main = (struct Node *)malloc(sizeof(struct Node)); 
    struct threadinfo_st *thread_new = (struct threadinfo_st *)malloc(sizeof(struct threadinfo_st *)); 
    struct threadinfo_st thread_info = (struct threadinfo_st)malloc(sizeof(struct threadinfo_st)); 
    // how should we populate main
    thread_info.tid = 0;
    thread_info.stack = 0;
    thread_info.stacksize = 0;
    thread_info.state = 0; 
    thread_info.status = NULL:
    thread_info.lib_one = NULL;
    thread_info.lib_two = NULL;
    thread_info.sched_one = NULL;    
    thread_info.sched_two = NULL; 
    thread_info.exited = NULL;
    enqueue(thread thread_new);
}

// frees entire queue 
// Lwp library is done with a scheduler to allow it to clean up
// This, too, is allowed to be NULL, so don’t call it if it is.
void rr_shutdown() {
    while(queue.head){
        rr_remove(queue.head.current_thread);
    }
    free(queue);
}

// enqueues new thread to queue 
void rr_admit(thread new) {
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node)); 
    new_node.current_thread = new;
    enqueue(new_node);
}

// removes thread from front of queue 
void rr_remove(thread victim) {
    Node *victim_node = dequeue();
    free(&victim_node.current_thread);
    free(victim_node);
}

// moves queue over 
thread rr_next() {
    if(queue.head){
        return queue.head.current_thread;
    }
    else{
        return NULL;
    }
}

// gets length of queue 
int qlen(void) {
    int counter = 0;
    Node* temp = queue.head;
    while(temp){
        counter++;
        temp = temp.next;
    }
    return counter;
}


int main(void) {

    rr_init();

    return 0;

}
