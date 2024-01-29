#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    unsigned int oldW = 0, newW = 0;
    scanf("%u %u\n", &oldW, &newW);

    char contentChar = 0, borderChar = 0;
    char *buf = NULL; size_t buf_size = 0;
    for (int i = 0; i < oldW; i++) {
        getline(&buf, &buf_size, stdin);
        if (i == 1) {
            borderChar = buf[0];
            contentChar = buf[1];
        }
        memset(buf, 0, buf_size);
    }
    free(buf);

    for (int i = 0; i < newW; i++) {
        for (int j = 0; j < newW; j++) {
            putchar((i == 0 || i == newW - 1 || j == 0 || j == newW - 1) ? borderChar : contentChar);
        }
        putchar('\n');
    }

    return EXIT_SUCCESS;
}
