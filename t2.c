#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include "t1.h"
#include "t2.h"


struct thread_worker {
    pthread_t thread;
    char* event;
    int target_work_id;
    int cur_work_id;
};

#define WORKER_SIZE 3
#define END_OF_WORK 10
#define WORK_SIZE 10

struct thread_worker workers[WORKER_SIZE];



struct work {
    char* event;
    int progress;
    bool is_done;
    int done_at;
};

struct work works[WORK_SIZE];

void init_workers() {
    for (int i = 0; i < WORKER_SIZE; i++) {
        workers[i].event = "idle";
        workers[i].target_work_id = -1;
        workers[i].cur_work_id = -1;
        workers[i].thread = pthread_create(&workers[i].thread, NULL, NULL, NULL);
    }
}

void init_works() {
    works[0] = (struct work){"non-blocking", 0, false};
    works[1] = (struct work){"non-blocking", 0, false};
    works[2] = (struct work){"non-blocking", 0, false};
    works[3] = (struct work){"non-blocking", 0, false};
    works[4] = (struct work){"non-blocking", 0, false};
    works[5] = (struct work){"blocking", 0, false};
    works[6] = (struct work){"blocking", 0, false};
    works[7] = (struct work){"blocking", 0, false};
    works[8] = (struct work){"blocking", 0, false};
    works[9] = (struct work){"blocking", 0, false};
}

void print_workers_and_works() {
    system("cls");

    for (int i = 0; i < WORKER_SIZE; i++) {
        printf("thread worker %d: / working on: %d / event: %s\n", i, workers[i].target_work_id, workers[i].event);
    }

    for (int i = 0; i < WORK_SIZE; i++) {
        printf("work %d: %s ", i, works[i].event);
        for (int j = 0; j < works[i].progress; j++) {
            printf("=");

        }
        if (works[i].is_done) {
            printf(" [done] end at time: %d", works[i].done_at);
        }
        printf("\n");
    }
}

int is_all_work_done() {
    for (int i = 0; i < WORK_SIZE; i++) {
        if (works[i].progress < END_OF_WORK) {
            return 0;
        }
    }
    return 1;
}

void do_work(int cur_time) {
    for (int i = 0; i < WORKER_SIZE; i++) {
        if (works[workers[i].cur_work_id].progress >= END_OF_WORK) {
            works[workers[i].cur_work_id].is_done = true;
            works[workers[i].cur_work_id].done_at = cur_time;
            workers[i].target_work_id = -1;
            workers[i].cur_work_id = -1;
            workers[i].event = "idle";
            continue;
        }

        if (works[workers[i].cur_work_id].is_done) {
            workers[i].target_work_id = -1;
            workers[i].cur_work_id = -1;
            workers[i].event = "idle";
            continue;
        }

        if (works[workers[i].cur_work_id].event != "blocking") {
            workers[i].cur_work_id = workers[i].target_work_id;
        }

        works[workers[i].cur_work_id].progress++;
    }
}

void worker_strategy_1() {
    for (int i = 0; i < WORKER_SIZE; i++) {
        if (workers[i].target_work_id == -1) {
            for (int j = 0; j < WORK_SIZE; j++) {
                if (works[j].is_done == false) {
                    workers[i].target_work_id = j;
                    workers[i].event = works[j].event;
                    break;
                }
            }
        }
    }
}

void worker_strategy_2() {
    int j = 0;
    for (int i = 0; i < WORK_SIZE; i++) {
        if (works[i].is_done == false && j < WORKER_SIZE) {
            workers[j].target_work_id = i;
            workers[j].event = works[i].event;
            j++;
        }
    }
}

void worker_strategy_3() {
    for (int i = 0; i < WORK_SIZE; i++) {
        if (works[i].is_done == false && works[i].event == "blocking") {
            workers[0].target_work_id = i;
            workers[0].event = works[i].event;
            break;
        }
    }

    int j = 1;
    for (int i = 0; i < WORK_SIZE; i++) {
        if (works[i].is_done == false && j < WORKER_SIZE) {
            workers[j].target_work_id = i;
            workers[j].event = works[i].event;
            j++;
        }
    }
}

void worker_strategy_4() {
    int j = 0;
    int i = 1;
    while (j < WORKER_SIZE) {
        int next = (workers[j].cur_work_id + i) % WORK_SIZE;
        if (next >= WORK_SIZE) {
            next = 0;
        }
        if (works[next].is_done == false) {
            workers[j].target_work_id = next;
            workers[j].event = works[next].event;
            j++;
        }
        i++;
    }
}

void worker_strategy_5() {
    int j = 0;
    int i = 1;
    while (j < 1) {
        int next = (workers[j].cur_work_id + i) % WORK_SIZE;
        if (next >= WORK_SIZE) {
            next = 0;
        }
        if (works[next].is_done == false && works[next].event == "blocking") {
            workers[j].target_work_id = next;
            workers[j].event = works[next].event;
            j++;
        }
        i++;
    }

    i = 1;
    int lap = 0;
    while (j < WORKER_SIZE && lap < 2) {
        int next = (workers[j].cur_work_id + i);
        if (next >= WORK_SIZE) {
            next = 0;
            lap++;
        }
        if (works[next].is_done == false && works[next].event == "non-blocking") {
            workers[j].target_work_id = next;
            workers[j].event = works[next].event;
            j++;
        }
        i++;
    }

    if (j == WORKER_SIZE) {
        return;
    }

    j = 0;
    i = 1;
    while (j < WORKER_SIZE) {
        int next = (workers[j].cur_work_id + i);
        if (next >= WORK_SIZE) {
            next = 0;
        }
        if (works[next].is_done == false) {
            workers[j].target_work_id = next;
            workers[j].event = works[next].event;
            j++;
        }
        i++;
    }
}


void execute_t2() {

    // 초기화
    init_workers();
    init_works();
    int time = 0;

    while (1) {
        if (is_all_work_done()) {
            break;
        }
        /**
         * 집중해서 하나씩
         */
        // worker_strategy_1();
        /**
         * 여러 개를 한번에
         */
        // worker_strategy_2();
        /**
         * 하나는 블로킹 전담, 나머지는 아무렇게나
         */
        // worker_strategy_3();
        /**
         * 블로킹 논블로킹 상관 없이 공평하게
         */
        worker_strategy_4();

        worker_strategy_5();

        do_work(time);
        print_workers_and_works();

        printf("spent time: %d\n", time);

        sleep((unsigned int) 1);
        time++;
    }

    do_work(time);
    print_workers_and_works();

    printf("spent time: %d\n", time);

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