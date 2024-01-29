#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    size_t buf_size = 6;
    char *buf = NULL;
    getline(&buf, &buf_size, stdin);
    printf("%c %c %c", buf[0], buf[2], buf[4] == 'Z' ? 'A' : buf[4] + 1);
    free(buf);
    return 0;
}
