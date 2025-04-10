#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"

#define MAX_LINE 1024
#define MAX_USERS 1000
#define MAX_BOOKS 1000
#define MAX_LEND 1000

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

int is_valid_password(const char* pw) {
    int len = strlen(pw);
    if (len < 5 || len > 20) return 0;
    int has_alpha = 0, has_digit = 0, freq[256] = { 0 };
    for (int i = 0; i < len; i++) {
        unsigned char ch = (unsigned char)pw[i];

        if (ch > 127) return 0;
        if (isspace(ch)) return 0;
        if (isalpha(ch)) has_alpha = 1;
        if (isdigit(ch)) has_digit = 1;
        if (++freq[ch] >= 5) return 0;
    }
    return has_alpha && has_digit;
}

 int is_valid_bid(const char* bid) {
    if (!bid || strlen(bid) == 0) return 0;
    for (int i = 0; bid[i]; i++) {
       if (!(isalnum(bid[i]) || bid[i] == '-' || bid[i] == '.' || bid[i] == ':' || bid[i] == ' ')) return 0;
    }
    return 1;
}

int is_valid_date(const char* date) {
    if (!date) return 0;
    char clean[9];
    int y, m, d;
    if (sscanf(date, "%4d/%2d/%2d", &y, &m, &d) == 3 ||
        sscanf(date, "%4d-%2d-%2d", &y, &m, &d) == 3 ||
        sscanf(date, "%8s", clean) == 1 && sscanf(clean, "%4d%2d%2d", &y, &m, &d) == 3) {
        if (y < 1900 || y > 2100 || m < 1 || m > 12 || d < 1 || d > 31) return 0;
        return 1;
    }
    return 0;
}

int is_valid_flag(const char* flag) {
    return flag && (strcmp(flag, "Y") == 0 || strcmp(flag, "N") == 0);
}

void run_verify() {
    const char* files[] = { USER_FILE, BOOK_FILE, LEND_RETURN_FILE };
    for (int i = 0; i < 3; i++) {
        FILE* f = fopen(files[i], "r");
        if (!f) {
            f = fopen(files[i], "w");
            if (!f) {
                printf("Error: Cannot create file %s\n", files[i]);
                exit(1);
            }
            printf("[INFO] Empty file created: %s\n", files[i]);
            fclose(f);
        }
        else fclose(f);
    }

    FILE* uf = fopen(USER_FILE, "r");
    if (!uf) { printf("Error opening user file.\n"); exit(1); }
    User users[MAX_USERS];
    int user_count = 0, line_no = 0;
    char line[MAX_LINE];

    while (fgets(line, sizeof(line), uf)) {
        trim(line); line_no++;
        char* token = strtok(line, ",");
        char* fields[5]; int i = 0;
        while (token && i < 5) fields[i++] = token, token = strtok(NULL, ",");
        if (i != 5) {
            printf("Line %d: Syntax error in users_data.txt\n", line_no); exit(1);
        }

        User u;
        strncpy(u.name, fields[0], MAX_NAME);
        strncpy(u.studentId, fields[1], MAX_ID);
        strncpy(u.password, fields[2], MAX_PW);
        u.lendAvailable = atoi(fields[4]);

        if (!is_valid_student_id(u.studentId)) {
            printf("Line %d: Invalid student ID %s\n", line_no, u.studentId); exit(1);
        }
        if (!is_valid_password(u.password)) {
            printf("Line %d: Invalid password format\n", line_no); exit(1);
        }
        for (int j = 0; j < user_count; j++) {
            if (strcmp(users[j].studentId, u.studentId) == 0) {
                printf("Line %d: Duplicate student ID: %s\n", line_no, u.studentId); exit(1);
            }
        }
        users[user_count++] = u;
    }
    fclose(uf);

    FILE* bf = fopen(BOOK_FILE, "r");
    if (!bf) { printf("Error opening book file.\n"); exit(1); }
    Book books[MAX_BOOKS];
    int book_count = 0; line_no = 0;

    while (fgets(line, sizeof(line), bf)) {
        trim(line); line_no++;
        char* token = strtok(line, ",");
        char* fields[4]; int i = 0;
        while (token && i < 4) fields[i++] = token, token = strtok(NULL, ",");
        if (i != 4) {
            printf("Line %d: Syntax error in books_data.txt\n", line_no); exit(1);
        }

        Book b;
        strncpy(b.title, fields[0], MAX_TITLE);
        strncpy(b.author, fields[1], MAX_AUTHOR);
        strncpy(b.bid, fields[2], MAX_BID);
        b.isAvailable = (strcmp(fields[3], "Y") == 0) ? 0 : 1;

        if (!is_valid_bid(b.bid)) {
            printf("Line %d: Invalid BID: %s\n", line_no, b.bid); exit(1);
        }
        if (!is_valid_flag(fields[3])) {
            printf("Line %d: Invalid availability flag: %s\n", line_no, fields[3]); exit(1);
        }
        for (int j = 0; j < book_count; j++) {
            if (strcmp(books[j].bid, b.bid) == 0) {
                printf("Line %d: Duplicate BID: %s\n", line_no, b.bid); exit(1);
            }
        }
        books[book_count++] = b;
    }
    fclose(bf);

    FILE* lf = fopen(LEND_RETURN_FILE, "r");
    if (!lf) { printf("Error opening lend/return file.\n"); exit(1); }
    line_no = 0;

    while (fgets(line, sizeof(line), lf)) {
        trim(line); line_no++;
        char* token = strtok(line, ",");
        char* fields[5]; int i = 0;
        while (token && i < 5) fields[i++] = token, token = strtok(NULL, ",");
        if (i != 5) {
            printf("Line %d: Syntax error in lend_return_data.txt\n", line_no); exit(1);
        }
        if (!is_valid_student_id(fields[0]) || !is_valid_bid(fields[1])) {
            printf("Line %d: Invalid ID or BID\n", line_no); exit(1);
        }
        if (!is_valid_date(fields[2]) || (strlen(fields[3]) > 0 && !is_valid_date(fields[3]))) {
            printf("Line %d: Invalid date format\n", line_no); exit(1);
        }
        if (!is_valid_flag(fields[4])) {
            printf("Line %d: Invalid overdue flag\n", line_no); exit(1);
        }
    }
    fclose(lf);

    printf("All files passed integrity verification.\n");
}
