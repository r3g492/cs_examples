#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "t1.h"
#include "t2.h"

void io_1(){
    int work = 10;
    for(int i = 0; i < work; i++){
        printf("io_1\n");
        sleep(1);
    }
};

void io_2(){
    int work = 10;
    for(int i = 0; i < work; i++){
        printf("io_2\n");
        sleep(1);
    }
};

void io_3(){
    int work = 10;
    for(int i = 0; i < work; i++){
        printf("io_3\n");
        sleep(1);
    }
};

void nio_1(){
    int work = 10;
    for(int i = 0; i < work; i++){
        printf("io_1\n");
        sleep(1);
    }
};

void nio_2(){
    int work = 10;
    for(int i = 0; i < work; i++){
        printf("io_2\n");
        sleep(1);
    }
};

void nio_3(){
    int work = 10;
    for(int i = 0; i < work; i++){
        printf("io_3\n");
        sleep(1);
    }
};

int event_state = 1;

func_ptr blocking_handlers[] = {io_1, io_2, io_3};
func_ptr non_blocking_handlers[] = {nio_1, nio_2, nio_3};

void event_loop(){
    while(event_state == 1) {
        printf("event loop\n");
        for (int i = 0; i < 3; i++) {
            blocking_handlers[i]();
        }
        sleep(1);
    }
}

