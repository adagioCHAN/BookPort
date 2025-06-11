#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "common.h"
#define MAX_KEYWORDS 10

/* 기존 util 함수 _ 2차 기능 추가 전 함수*/
void remove_spaces(char* str) {
    char* p1 = str;
    char* p2 = str;
    while (*p2) {
        if (*p2 != ' ') {
            *p1++ = *p2;
        }
        p2++;
    }
    *p1 = '\0';
}

void to_lowercase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

bool match_string(const char* target, const char* keyword) {
    char target_copy[101];
    char keyword_copy[101];

    strncpy(target_copy, target, sizeof(target_copy));
    strncpy(keyword_copy, keyword, sizeof(keyword_copy));
    target_copy[sizeof(target_copy) - 1] = '\0';
    keyword_copy[sizeof(keyword_copy) - 1] = '\0';

    // 공백 제거
    remove_spaces(target_copy);
    remove_spaces(keyword_copy);

    // 소문자로 변환
    to_lowercase(target_copy);
    to_lowercase(keyword_copy);

    // 부분 문자열
    return strstr(target_copy, keyword_copy) != NULL;
}

/* 2차 구현 파트 */
typedef struct {
    char* title;
    char* author;
    char* bid;
} SearchInput;

void freeSearchInput(SearchInput* input) {
    if (input->title) free(input->title);
    if (input->author) free(input->author);
    if (input->bid) free(input->bid);
}

SearchInput parseSearchInput(const char* rawInput) {
    SearchInput input = { NULL, NULL, NULL };
    const char* p = rawInput;

    while (*p) {
        while (isspace(*p)) p++;

        const char* key_start = p;
        while (*p && *p != ':') p++;

        if (*p != ':') break;

        size_t key_len = p - key_start;
        if (key_len == 0 || key_len >= 16) {
            printf("!! Error: Invalid field key\n");
            freeSearchInput(&input);
            return input;
        }

        char key[16] = { 0 };
        strncpy(key, key_start, key_len);
        key[key_len] = '\0';

        p++;  // skip ':'

        const char* val_start = p;
        while (*p && !(strncmp(p, "title:", 6) == 0 || strncmp(p, "author:", 7) == 0 || strncmp(p, "bid:", 4) == 0)) {
            p++;
        }

        size_t val_len = p - val_start;

        // 뒤쪽 공백 제거
        while (val_len > 0 && isspace(val_start[val_len - 1])) {
            val_len--;
        }

        // 빈 값 예외 처리
        if (val_len == 0) {
            printf("!! Error: Empty value for field '%s'\n", key);
            freeSearchInput(&input);
            return input;
        }

        // 안전하게 메모리 할당
        char* value = (char*)malloc(val_len + 1);
        if (!value) {
            printf("!! Error: Memory allocation failed.\n");
            freeSearchInput(&input);
            return input;
        }

        // strncpy는 널문자 자동 추가 안 함
        strncpy(value, val_start, val_len);
        value[val_len] = '\0';

        // 중복 필드 체크 및 저장
        if (strcmp(key, "title") == 0) {
            if (input.title) {
                printf("!! Error: Duplicate field 'title'\n");
                free(value);
                freeSearchInput(&input);
                return input;
            }
            input.title = value;
        }
        else if (strcmp(key, "author") == 0) {
            if (input.author) {
                printf("!! Error: Duplicate field 'author'\n");
                free(value);
                freeSearchInput(&input);
                return input;
            }
            input.author = value;
        }
        else if (strcmp(key, "bid") == 0) {
            if (input.bid) {
                printf("!! Error: Duplicate field 'bid'\n");
                free(value);
                freeSearchInput(&input);
                return input;
            }
            input.bid = value;
        }
        else {
            printf("!! Error: Invalid field '%s'\n", key);
            free(value);
            freeSearchInput(&input);
            return input;
        }
    }

    return input;
}


Book* filterBooks(const Book* allBooks, int totalCount, SearchInput input, int* resultCount) {
    Book* results = (Book*)malloc(sizeof(Book) * totalCount);
    int count = 0;

    for (int i = 0; i < totalCount; i++) {
        const Book* book = &allBooks[i];
        trim(input.bid);

        // 조건 체크
        if (input.title && !match_string(book->title, input.title)) continue;
        if (input.author && !match_string(book->author, input.author)) continue;
        if (input.bid && strcmp(book->bid, input.bid) != 0) continue;

        results[count++] = *book; // 조건 만족 시 복사
    }

    *resultCount = count;

    if (count == 0) {
        free(results);
        return NULL;
    }

    return results;
}

/* printSearchResults 함수 (fileio.c 완성 후 가능 여부, 예약 중인 사용자 학번 추가 예정)*/
void printSearchResults(const Book* results, int count) {
    printf("\n[Search Result]\n");

    for (int i = 0; i < count; i++) {
        const Book* book = &results[i];
        printf("> Title: %s\n", book->title);
        printf("  Author: %s\n", book->author);
        printf("  BID: %s\n", book->bid);
        printf("  Availibility: %c\n\n", book->isAvailable);
    }
}

int run_search(int mode) {
    while (1) {
        char inputBuf[201];

        printf("BookPort: Enter search keyword > ");
        if (!fgets(inputBuf, sizeof(inputBuf), stdin)) return -1;
        inputBuf[strcspn(inputBuf, "\n")] = '\0';

        if (strlen(inputBuf) == 0) {
            printf("!! Error: Empty input\n\n");
            if (mode == 0) return 0;
            continue;
        }

        SearchInput input = parseSearchInput(inputBuf);
        if (!input.title && !input.author && !input.bid) {
            printf("!! Error: No valid field detected.\n\n");
            if (mode == 0) return 0;
            continue;
        }

        bool book_integrity = true;
        linked_list* book_list = read_book_data(&book_integrity);
        if (!book_integrity) {
            printf(book_list);
            freeSearchInput(&input);
            return -1;
        }

        int totalCount = 0;
        node* cur = book_list->head;
        while (cur) {
            totalCount++;
            cur = cur->next;
        }

        Book* allBooks = (Book*)malloc(sizeof(Book) * totalCount);
        cur = book_list->head;
        for (int i = 0; i < totalCount && cur; i++, cur = cur->next) {
            allBooks[i] = *(Book*)cur->data;
        }

        int resultCount = 0;
        Book* filtered = filterBooks(allBooks, totalCount, input, &resultCount);

        if (resultCount > 0) {
            printSearchResults(filtered, resultCount);

            int count_y = 0;
            for (int i = 0; i < resultCount; i++) {
                if (filtered[i].isAvailable == 'Y') count_y++;
            }

            free(filtered);
            free(allBooks);
            freeSearchInput(&input);

            return count_y == 0 ? 2 : 1;
        }
        else {
            printf("!! Error: No matching data found.\n\n");
            free(allBooks);
            freeSearchInput(&input);
            if (mode == 0) return 0;
            continue;
        }
    }
}
