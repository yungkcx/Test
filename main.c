#include <stdio.h>

#define $(ret, def) ({ret $$ def &$$;})

typedef void* (*common_func) (void*);

void* output(int x) {
    return $(int, (int y) {
                return x + y;
}

int main() {
    // common_func fn = output(3);
    // printf("%d\n", (int)fn((void*)4));
    int a[10][10] = {{1, 2}};
    int **pp = a;
    printf("%d, %p\n", pp, *pp);

    return 0;
}

