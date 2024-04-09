#include <stdio.h>

int main()
{
    int sum = 0;
    int times = 0;

    times++;
    sum += 1;
    printf("%d: sum=%d\n", times, sum);

    times++;
    sum += 2;
    printf("%d: sum=%d\n", times, sum);

    times++;
    sum += 3;
    printf("%d: sum=%d\n", times, sum);

    times++;
    sum += 4;
    printf("%d: sum=%d\n", times, sum);
}
