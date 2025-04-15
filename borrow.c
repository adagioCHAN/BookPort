#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "common.h"

#define TEMP_FILE "temp_user.txt"

void run_borrow() {
    if (!is_logged_in) {
        printf("You must login first to borrow books.\n");
        return;
    }
    if (current_user.lendAvailable <= 0) {
        printf("You cannot borrow more books.\n");
        return;
    }
    run_search();
    char bid_input[MAX_BID];
    Lend_Return lend;
    while (1) {
        printf("Enter BID of the book to borrow > ");
        fgets(bid_input, sizeof(bid_input), stdin);
        trim(bid_input);

        // BID 유효성 검사
        if (!is_valid_bid(bid_input)) {
            continue;
        }
        // 검색된 책 중 BID 일치 여부 확인
        FILE* file = fopen(BOOK_FILE, "r");
        if (!file) {
            printf("Cannot open file.\n");
            return;
        }
        char status_char;
        int found = 0;
        Book book;
        printf("\n[Search Result]\n");

        while (fscanf_s(file, "%49[^,],%29[^,],%104[^,],%c\n",
            book.title, (unsigned)(sizeof(book.title)),
            book.author, (unsigned)(sizeof(book.author)),
            book.bid, (unsigned)(sizeof(book.bid)),
            &status_char, 1) == 4) {
            if (!strcmp(book.bid, bid_input)) {
                found++;
                strcpy(lend.bookBid, book.bid);
                if (status_char != 'Y') {
                    printf("Error: already 이미 대출된 책\n");
                    found++;
                }
                break;
            }
        }
        fclose(file);
        if (found == 0) {
            printf("Error: no book 해당하는 책 없음");
            continue;
        }
        else if (found == 2) {
            continue;
        }
        break;
    }

    char loan_date[MAX_DATE];
    while (1)
    {
        printf("Enter loan date > ");
        fgets(loan_date, sizeof(loan_date), stdin);
        trim(loan_date);
        if (!is_valid_date(loan_date)) {
            continue;
        }
        strcpy(lend.borrowDate, loan_date);
        break;
    }

    printf("Do you really want to borrow this book? (Enter to confirm / No to cancel) > ");
    char confirm[5];
    fgets(confirm, sizeof(confirm), stdin);
    trim(confirm);

    if (_stricmp(confirm, "No") == 0) {
        printf("Borrowing cancelled.\n");
        return;
    }
    if (!updata_file(lend)) {
        return;
    }
}

int updata_file(Lend_Return lend) {
    //파일 수정
    FILE* file_user = fopen(USER_FILE, "r");
    FILE* file_temp = fopen(TEMP_FILE, "w");

    if (!file_user) {
        printf("Cannot open file_user.\n");
        return 0;
    }
    if (!file_temp) {
        printf("user.\n");
        return 0;
    }

    char name[100], studentId[20], password[20], bids[102];
    int available;
    char temp[102];
    while (fscanf(file_user, "%99[^,],%19[^,],%19[^,],%101[^,],%d\n",
        name, studentId, password, bids, &available) == 5) {
        if (strcmp(studentId, current_user.studentId) == 0) {
            // 이 사용자가 대상이면 수정된 정보로 기록
            strcat(bids, ";");
            strcat(bids, lend.bookBid);
            fprintf(file_temp, "%s,%s,%s,%s,%d\n", name, studentId, password, bids, available - 1);
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
        printf("Cannot open file_books.\n");
        return 0;
    }
    char title[50], author[30], bid[105];
    char isAvailable;
    while (fscanf(file_books, "%49[^,],%29[^,],%104[^,],%c\n",
        title, author, bid, &isAvailable) == 4) {
        if (strcmp(bid, lend.bookBid) == 0) {
            // 이 사용자가 대상이면 수정된 정보로 기록
            fprintf(file_temp1, "%s,%s,%s,%c\n", title, author, bid, 'N');
        }
        else {
            // 그 외 사용자는 그대로 복사
            fprintf(file_temp1, "%s,%s,%s,%c\n", title, author, bid, isAvailable);
        }
    }
    fclose(file_books);
    fclose(file_temp1);
    remove(BOOK_FILE);
    if (rename(TEMP_FILE, BOOK_FILE) != 0) {
        perror("rename BOOK_FILE");
        return 0;
    }
    FILE* file_lend = fopen(LEND_RETURN_FILE, "a");
    if (!file_lend) {
        printf("Cannot open file_lend.\n");
        return 0;
    }
    fprintf(file_lend, "%s,%s,%s,%s,%c\n", current_user.studentId, lend.bookBid, lend.borrowDate, "0", 'N');
    fclose(file_lend);

    return 1;
}