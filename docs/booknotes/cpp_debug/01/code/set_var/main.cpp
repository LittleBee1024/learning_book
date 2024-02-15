#include <stdio.h>

FILE *fp = nullptr;
int gFlags = 0;

int Logme(int var)
{
    fprintf(fp, "gFlags=%d, var=%d\n", gFlags, var);
    return 0;
}

int main()
{
    fp = fopen("test.log", "w");

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

    fclose(fp);
}
