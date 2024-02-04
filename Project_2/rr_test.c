#include <assert.h>
#include "rr.h"  // Include the header file where your functions are declared

struct scheduler test_roundrobin = {NULL, NULL, rr_admit, rr_remove, rr_next, rr_qlen};

void test_admit_remove() {
    // THREAD 1
    thread a_thread = (thread)malloc(sizeof(thread));
    tid_t t1 = 1111;
    a_thread->tid = t1;

    test_roundrobin.admit(a_thread);
    assert(test_roundrobin.next()->tid == 1111);

    // THREAD 2
    thread a_thread2 = (thread)malloc(sizeof(thread));
    tid_t t2 = 2222;
    a_thread2->tid = t2;
    test_roundrobin.admit(a_thread2);

    // REMOVE THREAD 1
    test_roundrobin.remove(a_thread);
    assert(test_roundrobin.next()->tid == 2222);
}


void test_qlen() {
    assert(test_roundrobin.qlen() == 1);

    // THREAD 3
    thread thread3 = (thread)malloc(sizeof(thread));
    tid_t t3 = 3333;
    thread3->tid = t3;
    test_roundrobin.admit(thread3);

    assert(test_roundrobin.qlen() == 2);
    test_roundrobin.remove(thread3);

    assert(test_roundrobin.qlen() == 1);

    test_roundrobin.remove(thread3);

    assert(test_roundrobin.qlen() == 0);
}



void test_next() {
    // THREAD 4
    thread thread4 = (thread)malloc(sizeof(thread));
    tid_t t4 = 4444;
    thread4->tid = t4;
    test_roundrobin.admit(thread4);

    assert(test_roundrobin.next()->tid == 4444);

}

int main() {
    test_admit_remove();
    test_qlen();
    test_next();

    printf("All tests passed!\n");
    return 0;
}
