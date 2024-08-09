#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "t1.h"
#include "t2.h"


void execute_t1();
void execute_t2();

int main(void) {
    // execute_t1();
    execute_t2();

    return 0;
}

void execute_t2() {
    // new thread to run event_loop function
    event_state = 1;
    pthread_t event_thread;
    pthread_create(&event_thread, NULL, (void*)event_loop, NULL);
    sleep(5);
    sleep(5);
    event_state = 0;
    /**
     * 다중 프로세스
     *
     * linux fork
     * 부모 프로세스가 자식 프로세스를 만든다.
     * 개별 프로세스의 주소 공간이 분리되어 있어 쉽다.
     * 다중 코어 사용 가능
     * IPC 통신이 필요하다.
     * 프로세스 생성 비용이 크다.
     *
     * 다중 스레드
     * 스레드는 주소 공간을 공유하여 IPC가 필요 없다.
     * 종료 시 모든 스레드가 같이 종료 된다.
     * 각 요청에 대응하는 스레드 생성 가능
     * 스레드 중 일부가 블로킹 되더라도 나머지 스레드는 계속 동작한다.
     * 스레드 안전 문제가 발생할 수 있다.
     *
     * C10K 문제를 해결하려면, 다중 스레드/ 스레드 풀만으로는 힘들다.
     * https://en.wikipedia.org/wiki/C10k_problem
     *
     * 그래서 .. event-based concurrency를 이용한 event driven programming 등장
     * 서버의 이벤트는 대부분 입출력에 관계 되어 있음
     * 예를 들면 네트워크 데이터의 수신 여부, 파일의 읽기/쓰기 여부 등
     * 이벤트 처리 함수를 보통 핸들러라고 부름.
     * 서버는 이벤트가 도착할때까지 기다렸다가 도착하면 이벤트에 맞는 핸들러를 호출한다.
     * 이벤트 루프는 이벤트를 기다리는 무한 루프이다.
     * 1. 함수 하나로 여러 이벤트를 받아야 한다.
     * 2. 핸들러 함수는 이벤트 루프와 동일한 스레드에서 실행될 필요가 있는 경우도 있고, 없는 경우도 있다.
     *
     * epoll로 해결한다. (사실 상 while loop로 해결한다는 말과 같다.)
     * https://man7.org/linux/man-pages/man7/epoll.7.html
     *
     * 이벤트 순환과 핸들러를 같은 스레드로 처리할 경우, cpu 자원이 많이 소모된다면,
     * 시스템 응답 시간이 저하된다. 이벤트 순환이 핸들러에 의해 느려지게 된다.
     *
     * 다중 스레드로 핸들러별로 스레드를 배정해야 하여 다중 코어를 활용하는 방식을 reactor pattern(반응자 패턴)라고 한다.
     * 케이스 별로 나눈다.
     * 1. 입출력 작업에 해당하는 논블로킹 인터페이스가 있는 경우
     * 2. 입출력 작업에 블로킹 인터페이스만 있는 경우: 이벤트 루프에서 절대로 블로킹 함수를 호출하면 안된다.
     * 그럴 경우 모든 이벤트가 중단된다.
     *
     * 최종 구조 그림 참조
     *
     * 논블로킹은 이벤트 루프에서 해도 되고,
     * 블로킹은 반드시 새로운 스레드에서 처리 해야함.
     * RPC : Remote Procedure Call
     * 프로그래머가 함수 호출하는 것처럼 외부 서버 호출. (Feign Client 같은 것)
     * v1: GetUserInfo(request, response);
     * v2: GetUserInfo(request, callback);
     *
     */

    /**
     * void handler_after_GetStorkInfo(response) {
     *  G;
     *  H;
     * }
     *
     * void handler_after_GetQueryInfo(response) {
     *  E;
     *  F;
     *  GetStorkInfo(request, handler_after_GetStorkInfo); // 서버 C 호출
     * }
     * void handler_after_GetUserInfo(response) {
     *  C;
     *  D;
     *  GetQueryInfo(request, handler_after_GetQueryInfo); // 서버 B 호출
     * }
     * void handler(request) {
     *  A;
     *  B;
     *  GetUserInfo(request, handler_after_GetUserInfo); // 서버 A 호출
     * }
     */

    /**
     * 효율적인 비동기와 간단한 동기를 합친 코루틴
     * handler를 코루틴에서 실행하도록 한다.
     * 코루틴 추가 후 서버의 전체 구조
     * 하드웨어: CPU
     * 커널 모드: 스레드
     * 유저 모드: 코루틴
     */

    /**
     * 한 줄 요약: 현대 서버 기술은 이벤트, 스레드, 코루틴을 적재적소에 사용한다.
     *
     * 서버 라는 요구사항은 많은 io를 병목 없이 해결하는게 목적이고,
     * 이를 위해 단순히 프로그램을 하나씩 실행하거나, 스레드 하나당 프로그램을 하나씩 사용하거나,
     * 요청 하나당 스레드를 생성하는 방법만으로는 최대 효율을 낼 수 없다.
     *
     * https://norvig.com/21-days.html
     */


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
