#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "common.h"
#define MAX_KEYWORDS 10

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

int run_search(int mode) {
    while (1) {
        char input[101];
        char* keywords[MAX_KEYWORDS];
        int keyword_count = 0;

        printf("BookPort: Enter search keyword >");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';
        trim(input);

        if (strlen(input) == 0) {
            printf(".!! No search results.\n\n");
            if (mode == 0) return 0;
            continue; // 인자가 0개면 다시 입력
        }

        char* context = NULL;
        char* token = strtok_s(input, " \t", &context);
        while (token && keyword_count < MAX_KEYWORDS) {
            keywords[keyword_count++] = token;
            token = strtok_s(NULL, " \t", &context);
        }

        bool book_integrity = true;
        linked_list* book_list = read_book_data(&book_integrity);
        if (!book_integrity) {
            print_list(book_list, 4);
            return -1;
        }

        node* current = book_list->head;
        int found = 0;
        int count_y = 0;

        printf("\n[Search Result]\n");

        while (current) {
            Book* book = (Book*)current->data;
            int match_all = 1;
            for (int i = 0; i < keyword_count; ++i) {
                if (!(match_string(book->title, keywords[i]) ||
                    match_string(book->author, keywords[i]) ||
                    strcmp(book->bid, keywords[i]) == 0)) {
                    match_all = 0;
                    break;
                }
            }

            if (match_all) {
                if (mode == 1) {
                    printf("> Title: %s\n  Author: %s\n  BID: %s\n\n", book->title, book->author, book->bid);
                    found = 1;
                    if (book->isAvailable == 'Y') {
                        count_y++;
                    }
                }
                else if (mode == 0) {
                    printf("> Title: %s\n  Author: %s\n  BID: %s\n  Availibility: %c\n\n", book->title, book->author, book->bid, book->isAvailable);
                    found = 1;
                    if (book->isAvailable == 'Y') {
                        count_y++;
                    }
                }
            }
            current = current->next;
        }

        if (!found) {
            printf(".!! No search results.\n\n");
            if (mode == 0) return 0;
            continue;
        }

        if (count_y == 0) {
            return 2;
        }

        return 1;
    }
}