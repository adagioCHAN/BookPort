#include <stdio.h>
#include <string.h>
#include "common.h"

void run_help(const char* argument) {
    if (argument == NULL) {
        print_command_usage();
        return;
    }

    char* canonical = get_canonical_command(argument);
    if (canonical == NULL) {
        print_command_usage();
        return;
    }

    // ���� ���� ���
    if (strcmp(canonical, "help") == 0)
        printf("help : Show help for all or a specific command\n");
    else if (strcmp(canonical, "quit") == 0)
        printf("quit : Exit the program immediately\n");
    else if (strcmp(canonical, "verify") == 0)
        printf("verify : Check data integrity of the 3 data files\n");
    else if (strcmp(canonical, "account") == 0)
        printf("account : Move to create account prompt\n");
    else if (strcmp(canonical, "login") == 0)
        printf("login : Move to login prompt\n");
    else if (strcmp(canonical, "logout") == 0)
        printf("logout : Logout current session\n");
    else if (strcmp(canonical, "search") == 0)
        printf("search : Search book info by keyword\n");
    else if (strcmp(canonical, "borrow") == 0)
        printf("borrow : Move to borrow prompt\n");
    else if (strcmp(canonical, "return") == 0)
        printf("return : Move to return prompt\n");
    else if (strcmp(canonical, "myinfo") == 0)
        printf("myinfo : Show currently logged-in user's info\n");
}