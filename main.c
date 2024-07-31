#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


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

struct context {
    int a;
    int b;
    int result;
    int code_depth;
};

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

int resA;
int resB;

void funcA() {
    resA = 1;
}

void funcB() {
    resB = 2;
}


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

int main(void) {
    /*
     * CPU는 다음 2가지만 한다.
     * 1. 메모리에서 명령어를 하나 가져온다.
     * 2. 명령어를 실행한 후 1번을 반복한다.
     */

    int a_add = 5;
    int b_add = 3;
    unsigned long pc1, pc2;
    int sum = add_program(a_add, b_add, &pc1, &pc2);
    printf("pc1: %lu\n", pc1);
    printf("pc2: %lu\n", pc2);
    printf("%d + %d = %d\n", a_add, b_add, sum);

    printf("\n");

    /*
     * PC 레지스터가 저장하는 주소는 기본적으로 1씩 자동으로 증가한다.
     */

    /*
     * CPU instruction set은 제조사, 제품마다 다르다.
     * ref1 온라인 MOS 6502 프로세서 어셈블리 이뮬레이터: http://txt3.de/
     * ref2 MOS 6502 프로세서 어셈블리 인스트럭션 셋: https://www.masswerk.at/6502/6502_instruction_set.html
     * ref3 오픈소스 인스트럭션 아키텍처 risc-v https://github.com/riscv
     */

    /*
     * add 프로그램은 한번에 한 번만 실행할 수 있다.
     * 고전적인 CPU라면, add를 실행하는 동안 다른 프로그램은 실행할 수 없다.
     * 컨텍스트 스위칭을 간단하게 구현해본다면 ..
     */

    /*struct context add_c;
    add_c.code_depth = 0;
    struct context sub_c;
    sub_c.code_depth = 0;

    while (add_c.code_depth < 3 || sub_c.code_depth < 3) {
        add_c = switching_add(add_c, 5, 3);
        sub_c = switching_sub(sub_c, 5, 3);
    }
    printf("add: %d\n", add_c.result);
    printf("sub: %d\n", sub_c.result);

    printf("\n");*/

    /*
     * 프로세스 주소 공간은 커널에서 컨텍스트 스위칭과 프로그램 실행을 관리하기 위한 구조로 되어있다.
     * 스택 영역은 함수 호출 시 지역 변수를 저장하는 공간이다.
     * 힙 영역은 동적 할당을 위한 공간이다.
     * 데이터 영역은 전역 변수를 저장하는 공간이다.
     * 코드 영역은 프로그램 코드를 저장하는 공간이다.
     * 이 프로세스 저장 공간 간의 통신이 IPC(Inter-Process Communication)이다.
     * 다만, 프로세스는 비용이 크기 때문에 스레드라는 개념이 도입되었다.
     */

    /*
     * 스레드(thread)는 하나의 프로세스 주소 공간을 (일부) 공유하는 컨텍스트 스위칭 구조이다.
     */
    /*pthread_t threadA, threadB;
    pthread_create(&threadA, NULL, (void*)funcA, NULL);
    pthread_create(&threadB, NULL, (void*)funcB, NULL);
    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);

    printf("resA: %d\n", resA);
    printf("resB: %d\n", resB);
    */

    /*
     * IPC는 커널 api를 호출해야 하지만, 스레드는 위와 같이 공용 라이브러리만 사용하면 프로그래머가 관리할 수 있다.
     * 긴 작업 하나에 스레드를 하나 생성해서 처리하는 것은 괜찮지만,
     * 작업이 작고 많다면, 스레드 생성 비용도 여전히 문제가 될 수 있다.
     * 따라서, 스레드 풀(thread pool)이라는 개념이 도입되었다.
     * 스레드 풀은 미리 스레드를 생성해두고, 작업을 처리할 때마다 스레드를 재사용하는 구조이다.
     * 스레드 생성 관리의 책임을 프로그래머가 아닌 라이브러리가 맡는다.
     */

    /*thread_pool_init();

    for (int i = 0; i < 20; i++) {
        int* num = malloc(sizeof(int));
        *num = i;
        thread_pool_add_task(example_task, num);
    }

    sleep(10);*/


    return 0;
}
