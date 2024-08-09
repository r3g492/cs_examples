#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "t1.h"
#include "t2.h"

void io_1(){
    sleep(1);
    sleep(1);
    sleep(1);
    sleep(1);
    sleep(1);
    sleep(1);
    sleep(1);
    sleep(1);
    sleep(1);
};

void io_2(){
    sleep(1);
    sleep(1);
    sleep(1);
    sleep(1);
    sleep(1);
    sleep(1);
    sleep(1);
    sleep(1);
    sleep(1);
    sleep(1);
};

void io_3(){
    sleep(1);
    sleep(1);
    sleep(1);
    sleep(1);
    sleep(1);
    sleep(1);
    sleep(1);
    sleep(1);
    sleep(1);
    sleep(1);
};

int event_state = 1;
void event_loop(){
    while(event_state == 1) {
        printf("event loop\n");
        sleep(1);
    }
}


ThreadPool pool_v2;

#define THREAD_POOL_SIZE_V2 4
#define QUEUE_SIZE_V2 10

void* thread_function_v2(void* arg) {
    while (1) {
        Task task;

        pthread_mutex_lock(&pool_v2.mutex);

        while (pool_v2.queue_size == 0) {
            pthread_cond_wait(&pool_v2.cond_var, &pool_v2.mutex);
        }

        task = pool_v2.task_queue[pool_v2.front];
        pool_v2.front = (pool_v2.front + 1) % QUEUE_SIZE_V2;
        pool_v2.queue_size--;

        pthread_mutex_unlock(&pool_v2.mutex);

        task.function(task.argument);
    }
}

void thread_pool_init_v2() {
    pool_v2.queue_size = 0;
    pool_v2.front = 0;
    pool_v2.rear = 0;
    pthread_mutex_init(&pool_v2.mutex, NULL);
    pthread_cond_init(&pool_v2.cond_var, NULL);

    pthread_t threads[THREAD_POOL_SIZE_V2];
    for (int i = 0; i < THREAD_POOL_SIZE_V2; i++) {
        pthread_create(&threads[i], NULL, thread_function_v2, NULL);
    }
}

void thread_pool_add_task_v2(void (*function)(void*), void* argument) {
    pthread_mutex_lock(&pool_v2.mutex);

    if (pool_v2.queue_size == QUEUE_SIZE_V2) {
        printf("Task queue is full!\n");
        pthread_mutex_unlock(&pool_v2.mutex);
        return;
    }

    pool_v2.task_queue[pool_v2.rear].function = function;
    pool_v2.task_queue[pool_v2.rear].argument = argument;
    pool_v2.rear = (pool_v2.rear + 1) % QUEUE_SIZE_V2;
    pool_v2.queue_size++;

    pthread_cond_signal(&pool_v2.cond_var);
    pthread_mutex_unlock(&pool_v2.mutex);
}

void example_task_v2(void* arg) {
    int* num = (int*)arg;
    printf("Processing task with argument: %d\n", *num);
    sleep(1); // Simulate work
}
