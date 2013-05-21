#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc == 1) {
        fprintf(stderr, "getenv: Missing operand\n");
    } else {
        printf("%s\n", getenv(argv[1]));
    }

    return 0;
}
