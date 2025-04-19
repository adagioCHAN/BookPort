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
    if (len == 0)   return 1;
    if (len < 1 || len > 100) return 2;
    for (int i = 0; i < len; i++) {
        unsigned char ch = name[i];

        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
            continue;  // 영어 알파벳 OK
        else if (isspace(ch))
            if (ch != ' ') return 4;  // 스페이스바 공백만 허용
        else
            return 3;  // 알파벳도 아니고 공백도 아님 → 외국어/기호 등
    }

    return 0;
}

int is_valid_student_id(const char* id) {
    if (!id) return 1;
    if (strlen(id) != 9) return 2;
    if (id[0] == '0') return 3;

    int count[10] = { 0 };
    for (int i = 0; i < 9; i++) {
        if (id[i] == ' ') return 4;
        if (!isdigit(id[i])) return 5;

        count[id[i] - '0']++;
    }

    for (int i = 0; i < 10; i++) if (count[i] >= 8) return 6;

    return 0;
}

int is_unique_student_id(const char* id) {
    FILE* file = fopen(USER_FILE, "r");
    if (!file) {
        printf("[DEBUG] 사용자 파일 열기 실패: %s\n", USER_FILE);
        return 1;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        User user;
        char* token = strtok(line, ",");
        if (token) strncpy(user.name, token, MAX_NAME);

        token = strtok(NULL, ",");
        if (token) strncpy(user.studentId, token, MAX_ID);

        // 학번 검사 디버깅용
        /*
        printf("[DEBUG] 전달된 id: '%s'\n", id);
        printf("[DEBUG] 파일에서 읽은 id: '%s'\n", user.studentId);
        printf("[DEBUG] strcmp 결과: %d\n", strcmp(user.studentId, id));
        */

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
    if (!pw) return 1;
    if (len < 5 || len > 20) return 2;

    int has_alpha = 0, has_digit = 0, freq[256] = { 0 };
    for (int i = 0; i < len; i++) {
        if (isspace(pw[i])) return 3;
        if (isalpha(pw[i])) has_alpha = 1;
        if (isdigit(pw[i])) has_digit = 1;
        if (++freq[(unsigned char)pw[i]] >= 5) return 4;
    }

    if (has_alpha == 1 && has_digit == 1) return 0;
    else return 5;
}

int is_correct_password(const char* id, const char* pw) {
    FILE* file = fopen(USER_FILE, "r");
    if (!file) {
        printf("[DEBUG] 사용자 파일 열기 실패: %s\n", USER_FILE);
        return 0;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        User user;
        char* token = strtok(line, ",");
        if (token) strncpy(user.name, token, MAX_NAME);

        token = strtok(NULL, ",");
        if (token) strncpy(user.studentId, token, MAX_ID);

        if (strcmp(user.studentId, id) == 0) {
            token = strtok(NULL, ",");
            if (token) strncpy(user.password, token, MAX_PW);

            // 비밀번호 디버깅용
            /*
            printf("[Debug] 비밀번호 확인: %s\n", user.password);
            printf("[Debug] 입력된 비밀번호: %s\n", pw);
            */

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
    return *lendAvailable >= 0 && *lendAvailable <= 5;
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
}
// 대출 가능 = 1, 대출 불가능 = 0

// 대출/반납 정보 관련 함수
int is_valid_date(const char* date) {
    if (!date) return 0;

    char clean[9] = { 0 };
    int y, m, d;

    if (
        sscanf(date, "%4d/%2d/%2d", &y, &m, &d) == 3 ||
        sscanf(date, "%4d-%2d-%2d", &y, &m, &d) == 3 ||
        (sscanf(date, "%8s", clean) == 1 && sscanf(clean, "%4d%2d%2d", &y, &m, &d) == 3)
        ) {
        if (y < 1900 || y > 2100 || m < 1 || m > 12) return 0;

        int day_in_month[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
        if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0))) {
            day_in_month[1] = 29;  // 윤년
        }

        if (d < 1 || d > day_in_month[m - 1]) return 0;

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
    int error_count = 0;

    // === 사용자 정보 무결성 검사 ===
    bool user_integrity = true;
    linked_list* user_list = read_user_data(&user_integrity);

    printf(">>> Verifying User file...\n");

    if (!user_integrity) {
        printf(" - File integrity failed. Invalid lines:\n");
        print_list(user_list, 4);
        error_count++;
    }
    else {
        char student_ids[MAX_USERS][MAX_ID] = { 0 };
        int student_id_count = 0;
        node* current = user_list->head;

        while (current) {
            User* u = (User*)current->data;

            trim(u->name);
            trim(u->studentId);
            trim(u->password);
            for (int i = 0; i < 5 - u->lendAvailable; i++) {
                trim(u->lentBids[i]);
            }

            int name_result = is_valid_student_name(u->name);
            if (name_result != 0) {
                printf(" - Invalid name: %s (code=%d)\n", u->name, name_result);
                error_count++;
            }

            int id_result = is_valid_student_id(u->studentId);
            if (id_result != 0) {
                printf(" - Invalid studentId: %s (code=%d)\n", u->studentId, id_result);
                error_count++;
            }

            for (int j = 0; j < student_id_count; j++) {
                if (strcmp(student_ids[j], u->studentId) == 0) {
                    printf(" - Duplicate studentId: %s\n", u->studentId);
                    error_count++;
                    break;
                }
            }
            strncpy(student_ids[student_id_count++], u->studentId, MAX_ID);

            int pw_result = is_valid_password(u->password);
            if (pw_result != 0) {
                printf(" - Invalid password for %s (code=%d)\n", u->studentId, pw_result);
                error_count++;
            }

            if (!is_valid_lendavailable(&u->lendAvailable)) {
                printf(" - Invalid lendAvailable for %s\n", u->studentId);
                error_count++;
            }

            current = current->next;
        }
    }

    // === 도서 정보 무결성 검사 ===
    bool book_integrity = true;
    linked_list* book_list = read_book_data(&book_integrity);

    printf(">>> Verifying Book file...\n");

    if (!book_integrity) {
        printf(" - File integrity failed. Invalid lines:\n");
        print_list(book_list, 4);
        error_count++;
    }
    else {
        char bids[MAX_BOOKS][MAX_BID] = { 0 };
        int bid_count = 0;
        node* current = book_list->head;

        while (current) {
            Book* b = (Book*)current->data;

            int title_result = is_valid_book_title(b->title);
            if (title_result != 1) {
                printf(" - Invalid title: %s (code=%d)\n", b->title, title_result);
                error_count++;
            }

            int author_result = is_valid_book_author(b->author);
            if (author_result != 1) {
                printf(" - Invalid author: %s (code=%d)\n", b->author, author_result);
                error_count++;
            }

            int bid_result = is_valid_bid(b->bid);
            if (bid_result != 1) {
                printf(" - Invalid BID: %s (code=%d)\n", b->bid, bid_result);
                error_count++;
            }

            for (int j = 0; j < bid_count; j++) {
                if (strcmp(bids[j], b->bid) == 0) {
                    printf(" - Duplicate BID: %s\n", b->bid);
                    error_count++;
                    break;
                }
            }
            strncpy(bids[bid_count++], b->bid, MAX_BID);

            if (!is_valid_flag(b->isAvailable ? "Y" : "N")) {
                printf(" - Invalid availability flag for BID %s\n", b->bid);
                error_count++;
            }

            current = current->next;
        }
    }

    // === 대출 정보 무결성 검사 ===
    bool borrow_integrity = true;
    linked_list* borrow_list = read_borrow_data(&borrow_integrity);

    printf(">>> Verifying Lend/Return file...\n");

    if (!borrow_integrity) {
        printf(" - File integrity failed. Invalid lines:\n");
        print_list(borrow_list, 4);
        error_count++;
    }
    else {
        node* current = borrow_list->head;

        while (current) {
            Lend_Return* lr = (Lend_Return*)current->data;

            trim(lr->userid);
            trim(lr->bookBid);
            trim(lr->borrowDate);
            trim(lr->returnDate);

            int id_result = is_valid_student_id(lr->userid);
            if (id_result != 0) {
                printf(" - Invalid studentId: %s (code=%d)\n", lr->userid, id_result);
                error_count++;
            }

            int bid_result = is_valid_bid(lr->bookBid);
            if (bid_result != 1) {
                printf(" - Invalid BID: %s (code=%d)\n", lr->bookBid, bid_result);
                error_count++;
            }

            int borrow_result = is_valid_date(lr->borrowDate);
            if (borrow_result != 1) {
                printf(" - Invalid borrowDate: %s (code=%d)\n", lr->borrowDate, borrow_result);
                error_count++;
            }

            int return_result = is_valid_date(lr->returnDate);
            if (return_result != 1) {
                printf(" - Invalid returnDate: %s (code=%d)\n", lr->returnDate, return_result);
                error_count++;
            }


            if (!is_valid_overdue(lr->isOverdue ? "Y" : "N")) {
                printf(" - Invalid overdue flag: %d\n", lr->isOverdue);
                error_count++;
            }

            current = current->next;
        }
    }

    // === 최종 출력 ===
    if (error_count == 0) {
        printf(">>> All files are valid.\n");
    }
    else {
        printf(">>> A total of %d error(s) found. Terminating program.\n", error_count);
        exit(1);
    }
}
