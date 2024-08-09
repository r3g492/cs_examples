#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "t1.h"

int add_program(int a, int b, unsigned long* pc1, unsigned long* pc2) {
    int result;
    /*
     * __asm__
     * 내부 코드의 movl가 instruction set의 명령어이고,
     * 뒤에 따라오는 매개변수와 합쳐서 (32bit 기준, 대부분의 경우) 한 줄이 하나의 명령어(instruction)가 된다.
     */
    __asm__ (
            "label1:\n"
            "movl %1, %%eax;\n" // 2 byte
            "movl %2, %%ebx;\n" // 2 byte
            "label2:\n"
            "addl %%ebx, %%eax;\n"
            "movl %%eax, %0;\n"
            "lea label1(%%rip), %%ecx;\n"
            "movl %%ecx, %3;\n"

            "lea label2(%%rip), %%ecx;\n"
            "movl %%ecx, %4;\n"

            : "=r" (result)
            : "r" (a), "r" (b), "m" (*pc1), "m" (*pc2)
            : "%eax", "%ebx"
            );

    /*
     * 위 문자열이 소스 파일이라고 하면,
     * 1. 컴파일러가 이를 실행 파일로 변환하고,
     * 2. 디스크에 저장한 후,
     * 3. 메모리에 로드되어 실시간으로 실행한다.
     */
    return result;
}

/*
 * 컨텍스트 스위칭 예시.
 * 한 줄의 명령어는 대부분의 경우 더 이상 분해될 수 없다.
 * 여러 줄의 명령어 사이에는 컨텍스트 스위칭이 발생할 수 있다.
 */

struct context switching_add(struct context c, int a, int b) {
    if (c.code_depth == 0) {
        c.a = a;
        c.code_depth++;
        printf("add line 0 executed\n");
    } else if (c.code_depth == 1) {
        c.b = b;
        c.code_depth++;
        printf("add line 1 executed\n");
    } else if (c.code_depth == 2) {
        c.result = c.a + c.b;
        c.code_depth++;
        printf("add line 2 executed\n");
    }
    return c;
}

struct context switching_sub(struct context c, int a, int b) {
    if (c.code_depth == 0) {
        c.a = a;
        c.code_depth++;
        printf("sub line 0 executed\n");
    } else if (c.code_depth == 1) {
        c.b = b;
        c.code_depth++;
        printf("sub line 1 executed\n");
    } else if (c.code_depth == 2) {
        c.result = c.a - c.b;
        c.code_depth++;
        printf("sub line 2 executed\n");
    }
    return c;
}




void funcA(int* resA) {
    *resA = 1;
}

void funcB(int* resB) {
    *resB = 2;
}


#define THREAD_POOL_SIZE 4
#define QUEUE_SIZE 10


ThreadPool pool;

void* thread_function(void* arg) {
    while (1) {
        Task task;

        pthread_mutex_lock(&pool.mutex);

        while (pool.queue_size == 0) {
            pthread_cond_wait(&pool.cond_var, &pool.mutex);
        }

        task = pool.task_queue[pool.front];
        pool.front = (pool.front + 1) % QUEUE_SIZE;
        pool.queue_size--;

        pthread_mutex_unlock(&pool.mutex);

        task.function(task.argument);
    }
    return NULL;
}

void thread_pool_init() {
    pool.queue_size = 0;
    pool.front = 0;
    pool.rear = 0;
    pthread_mutex_init(&pool.mutex, NULL);
    pthread_cond_init(&pool.cond_var, NULL);

    pthread_t threads[THREAD_POOL_SIZE];
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_create(&threads[i], NULL, thread_function, NULL);
    }
}

void thread_pool_add_task(void (*function)(void*), void* argument) {
    pthread_mutex_lock(&pool.mutex);

    if (pool.queue_size == QUEUE_SIZE) {
        printf("Task queue is full!\n");
        pthread_mutex_unlock(&pool.mutex);
        return;
    }

    pool.task_queue[pool.rear].function = function;
    pool.task_queue[pool.rear].argument = argument;
    pool.rear = (pool.rear + 1) % QUEUE_SIZE;
    pool.queue_size++;

    pthread_cond_signal(&pool.cond_var);
    pthread_mutex_unlock(&pool.mutex);
}

void example_task(void* arg) {
    int* num = (int*)arg;
    printf("Processing task with argument: %d\n", *num);
    sleep(1); // Simulate work
}

