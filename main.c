#include "all.h"

int line = 1;
int main()
{
    printf("%*s\n",
            7-(line>4? line-4: 4-line),
            "*******"+2*(line>4? line-4:4-line)
          );
    if(++line != 8)
        main();
    return 0;
}
