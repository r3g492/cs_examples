#include <stdio.h>

int add(int a, int b, int* pc1, int* pc2)
{
    int result;
    /*
     * __asm__
     * 내부 코드의 movl가 instruction set의 명령어이고,
     * 뒤에 따라오는 매개변수와 합쳐서 (32bit 기준, 대부분의 경우) 한 줄이 하나의 명령어(instruction)가 된다.
     */
    __asm__ (
        "label1:\n"
        "movl %1, %%eax;\n"
        "movl %2, %%ebx;\n"
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

int main(void) {
    /*
     * CPU는 다음 2가지만 한다.
     * 1. 메모리에서 명령어를 하나 가져온다.
     * 2. 명령어를 실행한 후 1번을 반복한다.
     */

    int a_add = 5;
    int b_add = 3;
    long pc1, pc2;
    int sum = add(a_add, b_add, &pc1, &pc2);

    printf("pc1: %ld\n", pc1);
    printf("pc2: %ld\n", pc2);
    printf("%d + %d = %d\n", a_add, b_add, sum);

    /*
     * PC 레지스터가 저장하는 주소는 기본적으로 1씩 자동으로 증가합니다.
     */

    /*
     * CPU는 만드는 사람마다 다르다.
     * instruction set도 만드는 제조사마다 다르다.
     *
     * ref1 온라인 MOS 6502 프로세서 어셈블리 이뮬레이터: http://txt3.de/
     * ref2 MOS 6502 프로세서 어셈블리 인스트럭션 셋: https://www.masswerk.at/6502/6502_instruction_set.html
     */


    /*
     * add 프로그램은 한번에 한 번만 실행할 수 있다.
     * 고전적인 CPU라면, add를 실행하는 동안 다른 프로그램은 실행할 수 없다.
     *
     */





    return 0;
}
