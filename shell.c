#include "user.h"

void main(void) {
    printf("----- SHELL -----\n");
    printf("\n");
    while (1) {
    prompt:
        printf(">");
        char cmdline[255];
        for (int i =0;;) {
            int ch = getc();
            if (ch == -1) {
                yield();
                continue;};
            printc((char) ch); //echo
            if (i >= 255) goto prompt;
            if (ch == '\n') {
                printf("\r");
                cmdline[i] = '\0';
                break;
            }
            cmdline[i++] = (char) ch;
        }

        if (strcmp(cmdline, "hello") == 0) {
            printf("Hello world from shell!\n");
        }
        else if (strcmp(cmdline, "exit") == 0)
            exit();
    }
}