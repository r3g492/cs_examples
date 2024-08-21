#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
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
    int blocking_count = 0;
    int non_blocking_count = 0;

    for (int i = 0; i < WORK_SIZE; i++) {
        if (!works[i].is_done) {
            if (strcmp(works[i].event, "blocking") == 0) {
                blocking_count++;
            } else {
                non_blocking_count++;
            }
        }
    }

    int blocking_assigned = 0;
    int non_blocking_assigned = 0;

    for (int i = 0; i < WORKER_SIZE; i++) {
        if (blocking_assigned < blocking_count) {
            for (int j = 0; j < WORK_SIZE; j++) {
                if (!works[j].is_done && strcmp(works[j].event, "blocking") == 0) {
                    workers[i].target_work_id = j;
                    workers[i].event = works[j].event;
                    blocking_assigned++;
                    break;
                }
            }
        }
        else if (non_blocking_assigned < non_blocking_count) {
            for (int j = 0; j < WORK_SIZE; j++) {
                if (!works[j].is_done && strcmp(works[j].event, "non-blocking") == 0) {
                    workers[i].target_work_id = j;
                    workers[i].event = works[j].event;
                    non_blocking_assigned++;
                    break;
                }
            }
        }
    }

    for (int i = 0; i < WORKER_SIZE; i++) {
        if (workers[i].target_work_id == -1) {
            for (int j = 0; j < WORK_SIZE; j++) {
                if (!works[j].is_done) {
                    workers[i].target_work_id = j;
                    workers[i].event = works[j].event;
                    break;
                }
            }
        }
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
         **/
        worker_strategy_1();

        /**
         * thread worker 0: / working on: -1 / event: idle
         * thread worker 1: / working on: -1 / event: idle
         * thread worker 2: / working on: -1 / event: idle
         * work 0: non-blocking ========== [done] end at time: 4
         * work 1: non-blocking ========== [done] end at time: 8
         * work 2: non-blocking ========== [done] end at time: 12
         * work 3: non-blocking ========== [done] end at time: 17
         * work 4: non-blocking ========== [done] end at time: 21
         * work 5: blocking ========== [done] end at time: 25
         * work 6: blocking ========== [done] end at time: 30
         * work 7: blocking ========== [done] end at time: 34
         * work 8: blocking ========== [done] end at time: 38
         * work 9: blocking ========== [done] end at time: 43
         * spent time: 43
         *
         **/

        /**
         * 여러 개를 동시에, 순차적으로
         **/
        // worker_strategy_2();
        /**
         * thread worker 0: / working on: -1 / event: idle
         * thread worker 1: / working on: -1 / event: idle
         * thread worker 2: / working on: -1 / event: idle
         * work 0: non-blocking ========== [done] end at time: 10
         * work 1: non-blocking ========== [done] end at time: 10
         * work 2: non-blocking ========== [done] end at time: 10
         * work 3: non-blocking ========== [done] end at time: 21
         * work 4: non-blocking ========== [done] end at time: 21
         * work 5: blocking ========== [done] end at time: 21
         * work 6: blocking ========== [done] end at time: 32
         * work 7: blocking ========== [done] end at time: 32
         * work 8: blocking ========== [done] end at time: 32
         * work 9: blocking ========== [done] end at time: 43
         * spent time: 43
         *
         **/

        /**
         * 하나는 블로킹 전담, 나머지는 아무렇게나
         */
        // worker_strategy_3();
        /**
         * thread worker 0: / working on: -1 / event: idle
         * thread worker 1: / working on: -1 / event: idle
         * thread worker 2: / working on: -1 / event: idle
         * work 0: non-blocking ========== [done] end at time: 10
         * work 1: non-blocking ========== [done] end at time: 10
         * work 2: non-blocking ========== [done] end at time: 21
         * work 3: non-blocking ========== [done] end at time: 21
         * work 4: non-blocking ========== [done] end at time: 32
         * work 5: blocking ========== [done] end at time: 10
         * work 6: blocking ========== [done] end at time: 21
         * work 7: blocking ========== [done] end at time: 27
         * work 8: blocking =========== [done] end at time: 34
         * work 9: blocking ========== [done] end at time: 40
         * spent time: 40
         **/

        /**
         * 블로킹 논블로킹 상관 없이 공평하게
         */
        // worker_strategy_4();
        /**
         * thread worker 0: / working on: -1 / event: idle
         * thread worker 1: / working on: -1 / event: idle
         * thread worker 2: / working on: -1 / event: idle
         * work 0: non-blocking =========== [done] end at time: 40
         * work 1: non-blocking ========== [done] end at time: 30
         * work 2: non-blocking ========== [done] end at time: 31
         * work 3: non-blocking ========== [done] end at time: 33
         * work 4: non-blocking =========== [done] end at time: 42
         * work 5: blocking ========== [done] end at time: 6
         * work 6: blocking ========== [done] end at time: 13
         * work 7: blocking ========== [done] end at time: 20
         * work 8: blocking ========== [done] end at time: 27
         * work 9: blocking ========== [done] end at time: 37
         * spent time: 42
         **/

        /**
         * 좀 복잡한 전략
         **/
        // worker_strategy_5();

        /**
         * thread worker 0: / working on: -1 / event: idle
         * thread worker 1: / working on: -1 / event: idle
         * thread worker 2: / working on: -1 / event: idle
         * work 0: non-blocking ========== [done] end at time: 21
         * work 1: non-blocking ========== [done] end at time: 27
         * work 2: non-blocking ========== [done] end at time: 32
         * work 3: non-blocking ========== [done] end at time: 37
         * work 4: non-blocking ========== [done] end at time: 41
         * work 5: blocking ========== [done] end at time: 4
         * work 6: blocking ========== [done] end at time: 8
         * work 7: blocking ========== [done] end at time: 12
         * work 8: blocking ========== [done] end at time: 18
         * work 9: blocking ========== [done] end at time: 29
         * spent time: 41
         **/

        do_work(time);
        print_workers_and_works();

        printf("spent time: %d\n", time);

        sleep((unsigned int) 1);
        time++;
    }

    do_work(time);
    print_workers_and_works();

    printf("spent time: %d\n", time);
}