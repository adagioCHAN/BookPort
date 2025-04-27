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
        printf(".!! Error: \"add\" is not a valid command.\n");
        print_command_usage();
        return;
    }

    // 세부 도움말 출력
    if (strcmp(canonical, "Help") == 0) {
        printf("Help : Show help for all or a specific command\n");
        printf("Synonyms:? Help Hel He h\n");
        printf("Arguments: one argument\n");
    }
    else if (strcmp(canonical, "quit") == 0) {
        printf("quit : Exit the program immediately\n");
        printf("Synonyms:. quit qui qu q\n");
        printf("Arguments: None\n");
    }
    else if (strcmp(canonical, "verify") == 0) {
        printf("verify : Check data integrity of the 3 data files\n");
        printf("Synonyms:! verify veri ver ve v !\n");
        printf("Arguments: None\n");
    }
    else if (strcmp(canonical, "account") == 0) {
        printf("Command that moves to the account prompt\n");
        printf("Synonyms: a account accoun accou acco acc ac\n");
        printf("Arguments: None\n");
        printf("Action: Moves to the account prompt.\n");
    }   
    else if (strcmp(canonical, "login") == 0) {
        printf("Command that moves to the login prompt\n");
        printf("Synonyms:in login logi i\n");
        printf("Arguments: None\n");
        printf("Action: Moves to the login prompt.\n");
    }
    else if (strcmp(canonical, "logout") == 0) {
        printf("Command that moves to the logout prompt\n");
        printf("Synonyms: out logout logou logo ou o\n");
        printf("Arguments: None\n");
        printf("Action: Moves to the logout prompt.\n Logout current session\n");
    }
    else if (strcmp(canonical, "search") == 0) {
        printf("Command that moves to the search prompt\n");
        printf("Synonyms:/ search searc sear sea se s\n");
        printf("Arguments: None\n");
        printf("Action: Moves to the search prompt.\n The user enters either the book title or the author's name\n The System displays information about books that match the input\n");
    }
    else if (strcmp(canonical, "borrow") == 0) {
        printf("Command that moves to the borrow prompt\n");
        printf("Synonyms:$ borrow borro borr bor bo b\n");
        printf("Arguments: None\n");
        printf("Action: Moves to the borrow prompt.\n");
    }
    else if (strcmp(canonical, "return") == 0) {
        printf("Command that moves to the return prompt\n");
        printf("Synonyms: r return retur retu ret re\n");
        printf("Arguments: None\n");
        printf("Action: Moves to the return prompt.\n");
    }
    else if (strcmp(canonical, "myinfo") == 0) {
        printf("Command that moves to the myinfo prompt\n");
        printf("Synonyms: info myinfo myinf myin myi my m\n");
        printf("Arguments: None\n");
        printf("Action: Moves to the myinfo prompt.\n Show currently logged-in user's info\n");
    }
        
}
