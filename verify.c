#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"
#include "verify.h"

#define MAX_LINE 500
#define MAX_USERS 1000
#define MAX_BOOKS 1000


// 사용자 정보 관련 함수
int is_valid_student_name(const char* name) {
    int len = strlen(name);
    if (!name || len < 1 || len > 100)   return 0;
    for (int i = 0; i < len; i++) {
        if (!isalpha(name[i]) && name[i] != ' ')
            return 0;
    }

    return 1;
}

int is_valid_student_id(const char* id) {
    if (!id || strlen(id) != 9 || id[0] == '0') return 0;

    int count[10] = { 0 };
    for (int i = 0; i < 9; i++) {
        if (!isdigit(id[i])) return 0;

        count[id[i] - '0']++;
    }

    for (int i = 0; i < 10; i++) if (count[i] >= 8) return 0;

    return 1;
}

int is_unique_student_id(const char* id) {
    FILE* file = fopen(USER_FILE, "r");
    if (!file)  return 1;

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
		line[strcspn(line, "\n")] = 0; // Remove newline character

        User user;
        char* token = strtok(line, ",");
        if (token) strncpy(user.name, token, MAX_NAME);

        token = strtok(NULL, ",");
        if (token) strncpy(user.studentId, token, MAX_ID);

        if (strcmp(user.studentId, id) == 0) {
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    return 1;
}

int is_valid_password(const char* pw) {
    int len = strlen(pw);
    if (!pw || len < 5 || len > 20) return 0;

    int has_alpha = 0, has_digit = 0, freq[256] = { 0 };
    for (int i = 0; i < len; i++) {
        if (isspace(pw[i])) return 0;
        if (isalpha(pw[i])) has_alpha = 1;
        if (isdigit(pw[i])) has_digit = 1;
        if (++freq[(unsigned char)pw[i]] >= 5) return 0;
    }
    return has_alpha && has_digit;
}

int is_valid_lendavailable(const int* lendAvailable) {
    return lendAvailable >= 0 && lendAvailable <= 5;
}


// 도서 정보 관련 함수
int is_valid_book_title(const char* title) {
	int len = strlen(title);
    if (len < 1) return 0;

    for (int i = 0; i < len; i++) {
        if (isspace((unsigned char)title[i]) && title[i] != ' ')
            return 0;
    }

    if (isspace((unsigned char)title[0]) || isspace((unsigned char)title[len - 1]))
        return 0;

    return 1;
}

int is_valid_book_author(const char* author) {
    int len = strlen(author);
    if (len < 1) return 0;

    for (int i = 0; i < len; i++) {
        if (isspace((unsigned char)author[i]))
            return 0;
    }

    if (isspace((unsigned char)author[0]) || isspace((unsigned char)author[len - 1]))
        return 0;

    return 1;
}

int is_valid_bid(const char* bid) {
    int len = strlen(bid);
    if (len < 1) return 0;

    for (int i = 0; i < len; i++) {
        if (isspace((unsigned char)bid[i]) && bid[i] != ' ')
            return 0;

		if (!isalnum((unsigned char)bid) && bid[i] != '-' && bid[i] != '.' && bid[i] != ':')
            return 0;
    }

    return 1;
}

int is_unique_bid(const char* bid) {
    FILE* file = fopen(BOOK_FILE, "r");
    if (!file)  return 1;

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        Book book;
        char* token = strtok(line, ",");
        if (token) strncpy(book.title, token, MAX_TITLE);
        token = strtok(NULL, ",");
        if (token) strncpy(book.author, token, MAX_AUTHOR);

        token = strtok(NULL, ",");
        if (token) strncpy(book.bid, token, MAX_BID);

        if (strcmp(book.bid, bid) == 0) {
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    return 1;
}

int is_valid_flag(const char* flag) {
    return flag && (strcmp(flag, "Y") == 0 || strcmp(flag, "N") == 0);
}

int is_meaningful_flag(const char* flag) {
    return flag && (strcmp(flag, "Y") == 0;
}   // 대출 가능 = 1, 대출 불가능 = 0


// 대출/반납 정보 관련 함수
int is_valid_date(const char* date) {
    if (!date) return 0;

    char clean[9];
    int y, m, d;
    if (sscanf(date, "%4d/%2d/%2d", &y, &m, &d) == 3 ||
        sscanf(date, "%4d-%2d-%2d", &y, &m, &d) == 3 ||
        sscanf(date, "%8s", clean) == 1 && sscanf(clean, "%4d%2d%2d", &y, &m, &d) == 3) {
        if (y < 1900 || y > 2100 || m < 1 || m > 12) return 0;

		int error[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
		if (d < 1 || d > error[m - 1]) return 0;
		if (m == 2 && d == 29) {
			if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) return 1;
			else return 0;
		}
    }

    return 0;
}

int is_valid_overdue(const char* overdue) {
    return overdue && (strcmp(overdue, "Y") == 0 || strcmp(overdue, "N") == 0);
}

int is_meaningful_overdue(const char* overdue) {
    return overdue && (strcmp(overdue, "Y") == 0;
}   // 연체 = 1, 연체되지 않음 = 0