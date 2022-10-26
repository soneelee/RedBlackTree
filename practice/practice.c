// Valgrind 명령어 : C/C++ 프로그램에서 발생할 수 있는 Memory leak 문제를 찾을 수 있는 명령어
// https://hiseon.me/c/valgrind-tutorial/
// 해당 tree가 사용했던 메모리를 전부 반환해야 합니다. 

#include <stdlib.h>

void f(void){
    int* x = malloc(10*sizeof(int));
    free(x);
}

int main(void){
    f();
    return 0;
}