#include "all.h"

void print_int(int x)
{
    printf("%d\n", x);
}

void print_double(double f)
{
    printf("%lf\n", f);
}

#define good(...)\
    bad((param){__VA_ARGS__})\

#define CHOOSE(x) _Generic((x),\
                        int : print_int,\
                        double : print_double)(x)

struct ss {
    int a;
};

struct ss func()
{
    int a;
    struct ss s;
    s.a = 10;
    return s;
}

int main()
{
    struct ss s;
    s = func();
    printf("%d\n", s.a);
    return 0;
}
