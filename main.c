﻿#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"


void trim(char* str) {
    char* start = str;
    while (isspace((unsigned char)*start)) start++;
    if (*start == '\0') {
        *str = '\0';
        return;
    }

    char* end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';


    if (start != str) memmove(str, start, end - start + 2);
}




char* get_canonical_command(char* token) {
    struct {
        const char* synonyms[20];
        const char* canonical;
    } cmd_table[] = {
        { { "?", "help", "hel", "he", "h" }, "help" },
        { { ".", "quit" }, "quit" },
        { { "!", "verify", "verif", "ver", "v" }, "verify" },
        { { "a", "account", "accoun", "acc", "ac" }, "account" },
        { { "in", "login", "logi", "logo", "i" }, "login" },
        { { "out", "logout", "logou", "ou", "o" }, "logout" },
        { { "/", "search", "searc", "se", "sea", "s" }, "search" },
        { { "$", "borrow", "borro", "borr", "bor", "bo", "b" }, "borrow" },
        { { "r", "return", "retur", "ret", "re" }, "return" },
        { { "info", "myinfo", "myinf", "myin", "myi", "my", "m" }, "myinfo" }
    };

    for (int i = 0; i < sizeof(cmd_table) / sizeof(cmd_table[0]); i++) {
        for (int j = 0; cmd_table[i].synonyms[j]; j++) {
            if (strcmp(token, cmd_table[i].synonyms[j]) == 0)
                return (char*)cmd_table[i].canonical;
        }
    }
    return NULL;
}

// ���ɾ� ǥ ���
void print_command_usage() {
    printf("--------------------------------------------------------------\n");
    printf(" Command | Arguments              | Description               \n");
    printf("--------------------------------------------------------------\n");
    printf(" ? help      | None or one command        | Show help for all or a specific command\n");
    printf(" ! verify    | None                       | Proceed with file integrity verification\n");
    printf(" in login    | None                       | Move to login prompt      \n");
    printf(" out logout  | None                       | Logout                    \n");
    printf(" / search    | None                       | Display books matching a keyword\n");
    printf(" $ borrow    | None                       | Move to borrow prompt     \n");
    printf(" a account   | None                       | Move to create account prompt\n");
    printf(" r return    | None                       | Move to return prompt     \n");
    printf(" info myinfo | None                       | Show member's information");
    printf("--------------------------------------------------------------\n");
}

int main() {
    char input[100];

    while (1) {
        printf("BookPort >");
        if (!fgets(input, sizeof(input), stdin)) break;

        char* context = NULL;
        char* token = strtok_s(input, " \t\n", &context);
        if (!token) {
            print_command_usage();
            continue;
        }

        char* cmd = get_canonical_command(token);
        if (!cmd) {
            print_command_usage();
            continue;
        }

        char* argument = strtok_s(NULL, " \t\n", &context);
        char* extra = strtok_s(NULL, " \t\n", &context);

        // help
        if (strcmp(cmd, "help") == 0) {
            if (extra != NULL) {
                printf("Error: Too many arguments. Please enter only one command at a time.\n");
                print_command_usage();
                continue;
            }
            run_help(argument); // help.c�� ���ǵ�
        }
        // �ٸ� ���ɾ�� (���� ������ ����)
        else if (strcmp(cmd, "quit") == 0 ||
            strcmp(cmd, "verify") == 0 ||
            strcmp(cmd, "account") == 0 ||
            strcmp(cmd, "login") == 0 ||
            strcmp(cmd, "logout") == 0 ||
            strcmp(cmd, "search") == 0 ||
            strcmp(cmd, "borrow") == 0 ||
            strcmp(cmd, "return") == 0 ||
            strcmp(cmd, "myinfo") == 0) {

            if (argument != NULL || extra != NULL) {
                printf("Error: No arguments should be provided.\n");
                continue;
            }

            // ����
            if (strcmp(cmd, "quit") == 0) {
                printf("Exiting program...\n");
                break;
            }

            else if (strcmp(cmd, "verify") == 0) run_verify();
            else if (strcmp(cmd, "account") == 0) run_account();
            else if (strcmp(cmd, "login") == 0) run_login();
            else if (strcmp(cmd, "logout") == 0) run_logout();
            else if (strcmp(cmd, "search") == 0) run_search(1);
            else if (strcmp(cmd, "borrow") == 0) run_borrow();
            else if (strcmp(cmd, "return") == 0) run_return();

            else if (strcmp(cmd, "myinfo") == 0) run_myinfo();
        }
    }

    return 0;
}