//
// Created by Gunwoo on 8/9/2024.
//

#ifndef CS_EXAMPLES_T2_H
#define CS_EXAMPLES_T2_H




void io_1();
void io_2();
void io_3();

extern int event_state;

typedef void (*func_ptr)();
extern func_ptr handlers[];

void event_loop();




#endif //CS_EXAMPLES_T2_H
