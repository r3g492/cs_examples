//
// Created by Gunwoo on 8/9/2024.
//

#ifndef CS_EXAMPLES_T2_H
#define CS_EXAMPLES_T2_H




void io_1();
void io_2();
void io_3();

extern int event_state;
void event_loop();

void thread_pool_init_v2();

void thread_pool_add_task_v2(void (*function)(void*), void* argument);

void example_task_v2(void* arg);

#endif //CS_EXAMPLES_T2_H
