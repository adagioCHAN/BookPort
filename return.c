#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "common.h"
#include "verify.h"

#define TEMP_FILE "temp_user.txt"
int updata_file_2(Lend_Return);

void run_return() {
    if (!is_logged_in) {
        printf("You must login first to return books.\n");
        return;
    }
    if (current_user.lendAvailable == 5) {
        printf("You have no books to return.\n");
        return;
    }
    printf("\n[Here is the list of books you have borrowed]\n");
    bool user_integrity = true;
    linked_list* user_list = read_user_data(&user_integrity);
    if (!user_integrity) {
        print_list(user_list, 4);
        return -1;
    }
    node* current_u = user_list->head;
    while (current_u) {
        User* user = (User*)current_u->data;
        if (!strcmp(user->studentId, current_user.studentId)) {
            bool book_integrity = true;
            linked_list* book_list = read_book_data(&book_integrity);
            if (!book_integrity) {
                print_list(book_list, 4);
                return -1;
            }
            for (int i = 0; i < 5 - (user->lendAvailable); i++) {
                node* current_b = book_list->head;
                while (current_b) {
                    Book* book = (Book*)current_b->data;
                    if (!strcmp(book->bid, user->lentBids[i])) {
                        printf("> Title: %s\n  Author: %s\n  BID: %s\n\n", book->title, book->author, book->bid);
                    }
                    current_b = current_b->next;
                }
            }
            break;
        }
        current_u = current_u->next;
    }

    char bid_input[MAX_BID];
    Lend_Return lend;
    while (1) {
        printf("Enter BID of the book to return > ");
        fgets(bid_input, sizeof(bid_input), stdin);
        trim(bid_input);

        // BID 유효성 검사
        if (!is_valid_bid(bid_input)) {
            continue;
        }
    }

    char return_date[MAX_DATE];
    while (1)
    {
        printf("Enter return date > ");
        fgets(return_date, sizeof(return_date), stdin);
        trim(return_date);
        if (!is_valid_date(return_date)) {
            continue;
        }
        strcpy(lend.returnDate, return_date);
        break;
    }
    printf("\n[He following book matches the entered BID]\n");
    linked_list* book_list = read_book_data();
    Book* current = find_by_bid(book_list, bid_input);
    printf("> Title: %s\n  Author: %s\n  BID: %s\n\n", current->title, current->author, current->bid);

    printf("Do you really want to return this book? (Enter to confirm / No to cancel) > ");
    char confirm[5];
    fgets(confirm, sizeof(confirm), stdin);
    trim(confirm);

    if (_stricmp(confirm, "No") == 0) {
        printf("Returning cancelled.\n");
        return;
    }
    if (!updata_file(lend)) {
        return;
    }
}

int updata_file_2(Lend_Return lend) {
    //파일 수정
    FILE* file_user = fopen(USER_FILE, "r");
    FILE* file_temp = fopen(TEMP_FILE, "w");
    if (!file_user || !file_temp) {
        printf("Cannot open file.\n");
        return 0;
    }
    char name[100], studentId[20], password[20], bids[102];
    int available;
    char temp[102];
    while (fscanf(file_user, "%99[^,],%19[^,],%19[^,],%101[^,],%d\n",
        name, studentId, password, bids, &available) == 5) {
        if (strcmp(studentId, current_user.studentId) == 0) {
            // 이 사용자가 대상이면 수정된 정보로 기록
            char updated_bids[102] = "";
            char* token = strtok(bids, ";");
            int first = 1;

            while (token) {
                if (strcmp(token, lend.bookBid) != 0) {
                    if (!first) strcat(updated_bids, ";");
                    strcat(updated_bids, token);
                    first = 0;
                }
                token = strtok(NULL, ";");
            }
            fprintf(file_temp, "%s,%s,%s,%s,%d\n", name, studentId, password, updated_bids, available + 1);
        }
        else {
            // 그 외 사용자는 그대로 복사
            fprintf(file_temp, "%s,%s,%s,%s,%d\n", name, studentId, password, bids, available);
        }
    }
    fclose(file_user);
    fclose(file_temp);
    remove(USER_FILE);
    if (rename(TEMP_FILE, USER_FILE) != 0) {
        perror("rename BOOK_FILE");
        return 0;
    }
    FILE* file_books = fopen(BOOK_FILE, "r");
    FILE* file_temp1 = fopen(TEMP_FILE, "w");
    if (!file_books || !file_temp1) {
        printf("Cannot open file.\n");
        return 0;
    }
    char title[50], author[30], bid[105];
    char isAvailable;
    while (fscanf(file_books, "%49[^,],%[^,],%29[^,],%104[^,],%c\n",
        title, author, bid, &isAvailable) == 4) {
        if (strcmp(bid, lend.bookBid) == 0) {
            // 이 사용자가 대상이면 수정된 정보로 기록
            fprintf(file_temp1, "%s,%s,%s,%c\n", title, author, bid, 'Y');
        }
        else {
            // 그 외 사용자는 그대로 복사
            fprintf(file_temp1, "%s,%s,%s,%s,%c\n", title, author, bid, isAvailable);
        }
    }
    fclose(file_books);
    fclose(file_temp1);
    remove(BOOK_FILE);
    if (rename(TEMP_FILE, BOOK_FILE) != 0) {
        perror("rename BOOK_FILE");
        return 0;
    }
    FILE* file_lend = fopen(LEND_RETURN_FILE, "r");
    FILE* file_temp2 = fopen(TEMP_FILE, "w");
    if (!file_lend || !file_temp2) {
        printf("Cannot open file.\n");
        return 0;
    }
    char userid[10], bookBid[105], borrowDate[11], returnDate[11];
    char isOverdue;
    while (fscanf(file_lend, "%9[^,],%104[^,],%10[^,],%10[^,],%c\n",
        userid, bookBid, borrowDate, returnDate, &isOverdue) == 5) {
        if (strcmp(bid, lend.bookBid) == 0) {
            // 이 사용자가 대상이면 수정된 정보로 기록
            fprintf(file_temp2, "%s,%s,%s,%s,%c\n", userid, bookBid, borrowDate, lend.returnDate, 'N');
        }
        else {
            // 그 외 사용자는 그대로 복사
            fprintf(file_temp2, "%s,%s,%s,%s,%c\n", userid, bookBid, borrowDate, returnDate, 'N');
        }
    }
    fclose(file_lend);
    fclose(file_temp2);
    remove(LEND_RETURN_FILE);
    if (rename(TEMP_FILE, LEND_RETURN_FILE) != 0) {
        perror("rename BOOK_FILE");
        return 0;
    }
    return 1;
}