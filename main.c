#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "t1.h"
#include "t2.h"
#include "t3.h"

void execute_t1();

int main(void) {
    // execute_t1();
    // execute_t2();
    // execute_t3();
    execute_t3_real_time();
    return 0;
}

