#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "verify.h"

int is_logged_in = 0;
User current_user = { 0 };

void run_login() {
    if (is_logged_in == 1)
        printf(".!! You are already logged in.\n");

    else {
        User user = login_user();
        if (user.studentId[0] == '\0')
            printf("Login canceled.\n");
        else {
            current_user = user;
            is_logged_in = 1;
            printf("Login success.\n");
        }
    }
}

void run_account() {
    if (is_logged_in == 1)
        printf(".!! You are already logged in.\n");

    else {
        User user = register_user();
        if (user.studentId[0] != '\0') {
            FILE* file = fopen("users_data.txt", "a");

            if (file == NULL)
                printf(".!! Error: Failed to open file.\n");
            else {
                char lentBidsInfo[MAX_BID] = "";
                
                int hasValue = 0;

                for (int i = 0; i < 5; i++) {
                    if (strlen(user.lentBids[i]) > 0) {
                        if (hasValue) strcat(lentBidsInfo, ";");
                        strcat(lentBidsInfo, user.lentBids[i]);
                        hasValue = 1;
                    }
                }

                if (strlen(lentBidsInfo) == 0) {
                    strcpy(lentBidsInfo, "");
                }

                fprintf(file, "%s,%s,%s,%s,%d,%d,%s\n",
                    user.name,
                    user.studentId,
                    user.password,
                    lentBidsInfo,
                    user.lendAvailable,
                    user.reserveAvailable;
                    user.isOverdue
                );

                fclose(file);
            }

        }
    }
}

void run_logout() {
    if (is_logged_in == 0)
        printf("....Cannot logout because you are not logged in\n");
    else {
        is_logged_in = 0;
        current_user = (User){ 0 };
        printf("...Logout completed\n");
    }
}