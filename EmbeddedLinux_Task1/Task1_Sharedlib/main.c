#include <stdio.h>
#include "myMathSharedLib.h"

int main()
{
    int addition = sum(2,3);
    int diff = sub(5,3);
    int mod = modulu(-10);
    int product = multi(2,3);
    int division = div(10,5);

    printf("%d\n", addition);
    printf("%d\n", diff);
    printf("%d\n", mod);
    printf("%d\n", product);
    printf("%d\n", division);
}