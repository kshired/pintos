/* additional.c */

#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

int main(int argc, char *argv[])
{
    int i;

    // "additional num1 num2 num3 num4" => argc : 5
    if (argc != 5)
    {
        printf("usage: additional num1 num2 num3 num4\n");
        return EXIT_FAILURE;
    }

    // store num1 ~ num4
    int nums[4];

    for (i = 1; i < 5; i++)
    {
        // convert string to integer
        nums[i - 1] = atoi(argv[i]);
    }

    // call fibonacci, max_of_four_int system call!
    int fibo = fibonnaci(nums[0]);
    int max_val = max_of_four_int(nums[0], nums[1], nums[2], nums[3]);

    // show values
    printf("%d %d\n", fibo, max_val);
    return EXIT_SUCCESS;
}
