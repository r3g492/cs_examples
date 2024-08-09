#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "t1.h"


void execute_t1();
void execute_t2();

int main(void) {
    execute_t1();
    execute_t2();

    return 0;
}

void execute_t2() {

}


void execute_t1() {
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

    struct context add_c;
    add_c.code_depth = 0;
    struct context sub_c;
    sub_c.code_depth = 0;

    while (add_c.code_depth < 3 || sub_c.code_depth < 3) {
        add_c = switching_add(add_c, 5, 3);
        sub_c = switching_sub(sub_c, 5, 3);
    }
    printf("add: %d\n", add_c.result);
    printf("sub: %d\n", sub_c.result);

    printf("\n");

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
    int* resA = malloc(sizeof(int));
    int* resB = malloc(sizeof(int));
    pthread_t threadA, threadB;
    pthread_create(&threadA, NULL, (void*)funcA, resA);
    pthread_create(&threadB, NULL, (void*)funcB, resB);
    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);

    printf("resA: %d\n", *resA);
    printf("resB: %d\n", *resB);

    /*
     * IPC는 커널 api를 호출해야 하지만, 스레드는 위와 같이 공용 라이브러리만 사용하면 프로그래머가 관리할 수 있다.
     * 긴 작업 하나에 스레드를 하나 생성해서 처리하는 것은 괜찮지만,
     * 작업이 작고 많다면, 스레드 생성 비용도 여전히 문제가 될 수 있다.
     * 따라서, 스레드 풀(thread pool)이라는 개념이 도입되었다.
     * 스레드 풀은 미리 스레드를 생성해두고, 작업을 처리할 때마다 스레드를 재사용하는 구조이다.
     * 스레드 생성 관리의 책임을 프로그래머가 아닌 라이브러리가 맡는다.
     */

    thread_pool_init();

    for (int i = 0; i < 20; i++) {
        int* num = malloc(sizeof(int));
        *num = i;
        thread_pool_add_task(example_task, num);
    }

    sleep(10);
}
