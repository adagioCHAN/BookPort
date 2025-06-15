#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "verify.h"

int is_logged_in = 0;
User current_user = { 0 };
int penalty_day, current_year, current_month, current_day;

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

                fprintf(file, "%s,%s,%s,%s,%d,%d,%c\n",
                    user.name,
                    user.studentId,
                    user.password,
                    lentBidsInfo,
                    user.lendAvailable,
                    user.reserveAvailable,
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

bool checkOverDue(const char* input) {
    struct tm date1 = { 0 }, date2 = { 0 }, date3 = { 0 };
    time_t t1 = 0;
    time_t t2 = 0;
    time_t t3 = 0;
    int tmp_penalty = 0, days;
    double seconds;
    bool file_integrity = true;
    bool all_returned = true;
    linked_list* borrow_list = read_borrow_data(&file_integrity);
    if (current_year == 0 && (strcmp(input, "") == 0)) return false;
    else {
        current_year = (input[0] - '0') * 1000 + (input[1] - '0') * 100 + (input[2] - '0') * 10 + (input[3] - '0');
        current_month = (input[4] - '0') * 10 + (input[5] - '0');
        current_day = (input[6] - '0') * 10 + (input[7] - '0');
        date2.tm_year = current_year - 1900;
        date2.tm_mon = current_month - 1;
        date2.tm_mday = current_day;
        node* current = borrow_list->head;
        while (current != NULL) {
            Lend_Return* borrow_data = (Lend_Return*)current->data;
            if (strcmp(borrow_data->userid, current_user.studentId) != 0) {
                current = current->next;
                continue;
            }
            int borrow_year = (borrow_data->borrowDate[0] - '0') * 1000 + (borrow_data->borrowDate[1] - '0') * 100 + (borrow_data->borrowDate[2] - '0') * 10 + (borrow_data->borrowDate[3] - '0');
            int borrow_month = (borrow_data->borrowDate[4] - '0') * 10 + (borrow_data->borrowDate[5] - '0');
            int borrow_day = (borrow_data->borrowDate[6] - '0') * 10 + (borrow_data->borrowDate[7] - '0');
            date1.tm_year = borrow_year - 1900;
            date1.tm_mon = borrow_month - 1;
            date1.tm_mday = borrow_day;
            if (strcmp(borrow_data->returnDate, "") != 0) {
                int return_year = (borrow_data->returnDate[0] - '0') * 1000 + (borrow_data->returnDate[1] - '0') * 100 + (borrow_data->returnDate[2] - '0') * 10 + (borrow_data->returnDate[3] - '0');
                int return_month = (borrow_data->returnDate[4] - '0') * 10 + (borrow_data->returnDate[5] - '0');
                int return_day = (borrow_data->returnDate[6] - '0') * 10 + (borrow_data->returnDate[7] - '0');
                
                date3.tm_year = return_year - 1900;
                date3.tm_mon = return_month - 1;
                date3.tm_mday = return_day;
                t1 = mktime(&date1);
                t2 = mktime(&date3);
                seconds = difftime(t2, t1);
                days = (int)(seconds / (60 * 60 * 24));
                if (days > 10) {
                    if (tmp_penalty < days - 10) {
                        tmp_penalty = days - 10;
                        t3 = mktime(&date3);
                    }
                }
            }
            else {
                all_returned = false;
                t1 = mktime(&date1);
                t2 = mktime(&date2);

                seconds = difftime(t2, t1);
                days = (int)(seconds / (60 * 60 * 24));
                if (days > 10) {
                    if (penalty_day < days - 10) {
                        penalty_day = days - 10;
                    }
                }
            }
            

            current = current->next;
        }
        if (!all_returned) {
            return penalty_day > 0;
        }
        else {
            if (t3 == 0) return false;
            seconds = difftime(t2, t3);
            days = (int)(seconds / (60 * 60 * 24));
            if (days > tmp_penalty) {
                penalty_day = 0;
                return false;
            }
            else {
                penalty_day = tmp_penalty;
                return true;
            }
        }
    }
}