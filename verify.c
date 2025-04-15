#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"
#include "verify.h"

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

int is_correct_password(const char* id, const char* pw) {
    FILE* file = fopen("users.txt", "r");
    User user = { 0 };
    char password[MAX_PW];

    if (!file)
        return 0;

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        char* token = strtok(line, ",");
        if (token) strncpy(user.name, token, MAX_NAME);

        token = strtok(NULL, ",");
        if (token) strncpy(user.studentId, token, MAX_ID);

        if (strcmp(user.studentId, id) == 0) {
            token = strtok(NULL, ",");
            if (token) strncpy(user.password, token, MAX_PW);

            if (strcmp(user.password, pw) == 0) {
                fclose(file);
                return 1;
            }
            else {
                fclose(file);
                return 0;
            }
        }
    }

    fclose(file);
    return 0;
}

int is_valid_lendavailable(const int* lendAvailable) {
    return (*lendAvailable) >= 0 && (*lendAvailable) <= 5;
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

        if (!isalnum((unsigned char)bid[i]) && bid[i] != '-' && bid[i] != '.' && bid[i] != ':')
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
    return flag && (strcmp(flag, "Y") == 0);
}   // 대출 가능 = 1, 대출 불가능 = 0


// 대출/반납 정보 관련 함수
int is_valid_date(const char* date) {
    if (!date || strlen(date) < 8) return 0;

    int y, m, d;
    if (sscanf(date, "%4d/%2d/%2d", &y, &m, &d) == 3 ||
        sscanf(date, "%4d-%2d-%2d", &y, &m, &d) == 3 ||
        sscanf(date, "%4d%2d%2d", &y, &m, &d) == 3) {

        if (y < 1900 || y > 2100 || m < 1 || m > 12) return 0;

        int days_in_month[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
        if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0))) {
            days_in_month[1] = 29;
        }

        if (d < 1 || d > days_in_month[m - 1]) return 0;

        return 1;
    }

    return 0;
}

int is_valid_overdue(const char* overdue) {
    return overdue && (strcmp(overdue, "Y") == 0 || strcmp(overdue, "N") == 0);
}

int is_meaningful_overdue(const char* overdue) {
    return overdue && (strcmp(overdue, "Y") == 0);
}   // 연체 = 1, 연체되지 않음 = 0


void run_verify() {
    const char* files[] = { USER_FILE, BOOK_FILE, LEND_RETURN_FILE };
    const char* file_names[] = { "User", "Book", "Lend/Return" };
    int error_count = 0;

    char student_ids[MAX_USERS][MAX_ID];
    int student_id_count = 0;

    char bids[MAX_BOOKS][MAX_BID];
    int bid_count = 0;

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
            continue;
        }

        printf(">>> Verifying %s file...\n", file_names[i]);

        char line[MAX_LINE];
        int line_num = 0;

        while (fgets(line, sizeof(line), f)) {
            line_num++;
            trim(line);

            if (i == 0) { // USER_FILE
                User u;
                char* token = strtok(line, ",");
                if (!token || !is_valid_student_name(token)) {
                    printf("Invalid name in line %d\n", line_num); error_count++; continue;
                }
                strncpy(u.name, token, MAX_NAME);

                token = strtok(NULL, ",");
                if (!token || !is_valid_student_id(token)) {
                    printf("Invalid studentId in line %d\n", line_num); error_count++; continue;
                }
                strncpy(u.studentId, token, MAX_ID);

                // 중복 학번 검사
                for (int j = 0; j < student_id_count; j++) {
                    if (strcmp(student_ids[j], u.studentId) == 0) {
                        printf("Duplicate studentId in line %d: %s\n", line_num, u.studentId);
                        error_count++;
                        break;
                    }
                }
                strncpy(student_ids[student_id_count++], u.studentId, MAX_ID);

                token = strtok(NULL, ",");
                if (!token || !is_valid_password(token)) {
                    printf("Invalid password in line %d\n", line_num); error_count++; continue;
                }

                token = strtok(NULL, ",");
                int lendAv = token ? atoi(token) : -1;
                if (!is_valid_lendavailable(&lendAv)) {
                    printf("Invalid lendAvailable in line %d\n", line_num); error_count++; continue;
                }
            }
            else if (i == 1) { // BOOK_FILE
                Book b;
                char* token = strtok(line, ",");
                if (!token || !is_valid_book_title(token)) {
                    printf("Invalid title in line %d\n", line_num); error_count++; continue;
                }

                token = strtok(NULL, ",");
                if (!token || !is_valid_book_author(token)) {
                    printf("Invalid author in line %d\n", line_num); error_count++; continue;
                }

                token = strtok(NULL, ",");
                if (!token || !is_valid_bid(token)) {
                    printf("Invalid BID in line %d\n", line_num); error_count++; continue;
                }
                strncpy(b.bid, token, MAX_BID);

                // 중복 BID 검사
                for (int j = 0; j < bid_count; j++) {
                    if (strcmp(bids[j], b.bid) == 0) {
                        printf("Duplicate BID in line %d: %s\n", line_num, b.bid);
                        error_count++;
                        break;
                    }
                }
                strncpy(bids[bid_count++], b.bid, MAX_BID);

                token = strtok(NULL, ",");
                if (!token || !is_valid_flag(token)) {
                    printf("Invalid flag in line %d\n", line_num); error_count++; continue;
                }
            }
            else if (i == 2) { // LEND_RETURN_FILE
                Lend_Return lr;
                char* token = strtok(line, ",");
                if (!token || !is_valid_student_id(token)) {
                    printf("Invalid studentId in line %d\n", line_num); error_count++; continue;
                }

                token = strtok(NULL, ",");
                if (!token || !is_valid_bid(token)) {
                    printf("Invalid BID in line %d\n", line_num); error_count++; continue;
                }

                token = strtok(NULL, ",");
                if (!token || !is_valid_date(token)) {
                    printf("Invalid borrowDate in line %d\n", line_num); error_count++; continue;
                }

                token = strtok(NULL, ",");
                if (!token || !is_valid_date(token)) {
                    printf("Invalid returnDate in line %d\n", line_num); error_count++; continue;
                }

                token = strtok(NULL, ",");
                if (!token || !is_valid_overdue(token)) {
                    printf("Invalid overdue flag in line %d\n", line_num); error_count++; continue;
                }
            }
        }
        fclose(f);
    }

    if (error_count == 0) {
        printf(">>> All files are valid.\n");
    }
    else {
        printf(">>> A total of %d error(s) were found. Terminating program.\n", error_count);
        exit(1);
    }
}

