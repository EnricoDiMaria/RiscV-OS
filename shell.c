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
            //printf("%x", ch);
            printc((char) ch); //echo
            if (i >= 255) {
                printf("\nCommand too long!\n");
                //insert a wait/sleep function
                goto prompt;}
            if (ch == '\n') {
                printf("\r");
                cmdline[i] = '\0';
                break;
            }
            if (ch == '\x7f') {
                if (i > 0) {
                    printc('\b');
                    printc(' ');
                    printc('\b'); //it moves te pointer to the left, prints a blank character over the old one, and moves the pointer again to the left to make it possible to write over it
                    i--;
                }
                continue;
            }
            cmdline[i++] = (char) ch;
        }

        if (strcmp(cmdline, "hello") == 0) {
            printf("Hello world from shell!\n");
        }
        else if (strcmp(cmdline, "exit") == 0)
            exit();
        else {
            if (*cmdline != '\0') printf("unknown command: '%s'\n", cmdline);
        }
    }
}