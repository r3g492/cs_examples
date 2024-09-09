//
// Created by Gunwoo on 8/9/2024.
//

#ifndef CS_EXAMPLES_T1_H
#define CS_EXAMPLES_T1_H

extern void execute_t1();

int add_program(int a, int b, unsigned long* pc1, unsigned long* pc2);

struct context {
    int a;
    int b;
    int result;
    int code_depth;
};

struct context switching_add(struct context c, int a, int b);

struct context switching_sub(struct context c, int a, int b);

void funcA(int* resA);
void funcB(int* resB);


#define THREAD_POOL_SIZE 4
#define QUEUE_SIZE 10

typedef struct {
    void (*function)(void*);
    void *argument;
} Task;

typedef struct {
    Task task_queue[QUEUE_SIZE];
    int queue_size;
    int front;
    int rear;
    pthread_mutex_t mutex;
    pthread_cond_t cond_var;
} ThreadPool;

void* thread_function(void* arg);

void thread_pool_init();

void thread_pool_add_task(void (*function)(void*), void* argument);

void example_task(void* arg);


#endif //CS_EXAMPLES_T1_H
