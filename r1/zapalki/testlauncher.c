#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define BUF_SIZE 4096

const char *rel_path_to_actual_binary = "zapalki";
const char *rel_path_to_cmdline = "../testcmdline.txt";

int main(int argc, char **argv)
{
    char complete_path_to_actual_binary[BUF_SIZE] = { 0 };
    char complete_path_to_cmdline[BUF_SIZE] = { 0 };
    char binDirBuf[BUF_SIZE] = { 0 };

    int i = 0;
    int binDirLen  = 0;
    do {
        if(argv[0][i] == '/') binDirLen = i + 1; /* We want the trailing '/' */
    } while (argv[0][i++]);

    strncpy(binDirBuf, argv[0], binDirLen);
    binDirBuf[BUF_SIZE - 1] = '\0';

    strncpy(complete_path_to_cmdline, binDirBuf, BUF_SIZE);
    complete_path_to_cmdline[BUF_SIZE - 1] = '\0';
    strncpy(complete_path_to_actual_binary, binDirBuf, BUF_SIZE);
    complete_path_to_actual_binary[BUF_SIZE - 1] = '\0';

    strncat(complete_path_to_actual_binary, rel_path_to_actual_binary, strlen(complete_path_to_actual_binary) - strlen(rel_path_to_actual_binary) - 2);
    complete_path_to_actual_binary[BUF_SIZE - 1] = '\0';
    strncat(complete_path_to_cmdline, rel_path_to_cmdline, strlen(complete_path_to_cmdline) - strlen(rel_path_to_cmdline) - 2);
    complete_path_to_cmdline[BUF_SIZE - 1] = '\0';

    uint32_t max_argv_len = sysconf(_SC_ARG_MAX);
    char cmdline[max_argv_len + 1];
    memset(cmdline, 0, max_argv_len + 1);

    FILE *testcmdlinefile = fopen(complete_path_to_cmdline, "r");
    if (testcmdlinefile) {
        fread(cmdline, 1, max_argv_len, testcmdlinefile);
        fclose(testcmdlinefile);
        printf("[launcher] exec()ing %s with args from %s...\n", complete_path_to_actual_binary, complete_path_to_cmdline);
        if (execl(complete_path_to_actual_binary, cmdline) == -1) {
            fprintf(stderr, "[launcher] FAIL! Reason: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
    } else {
        fprintf(stderr, "[launcher] Failed to open '%s' for reading. Stop.\n", complete_path_to_cmdline);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
