#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common.h"
#include "verify.h"

#define ADMIN_PASSWORD "@admin_1pw$"

char* get_admin_canonical_command(char* input) {
    struct {
        const char* synonyms[10];
        const char* canonical;
    } admin_table[] = {
        { { "add", "ad", "a" }, "add" },
        { { "delete", "delet", "dele", "del", "de", "d" }, "delete" },
        { { "exit", "exi", "ex", "e" }, "exit" }
    };

    for (int i = 0; i < sizeof(admin_table) / sizeof(admin_table[0]); ++i) {
        for (int j = 0; admin_table[i].synonyms[j]; ++j) {
            if (strcmp(input, admin_table[i].synonyms[j]) == 0)
                return (char*)admin_table[i].canonical;
        }
    }

    return NULL;
}

void add() {
    char title[100], author[100], bid[50];
    while (1) {
        printf("Enter book title: ");
        fgets(title, sizeof(title), stdin);
        title[strcspn(title, "\n")] = 0;
        if (!is_valid_book_title(title)) {
            printf(".!! Error: Invalid title.\n");
            continue;
        }
        break;
    }
    while (1) {
        printf("Enter author name: ");
        fgets(author, sizeof(author), stdin);
        author[strcspn(author, "\n")] = 0;
        if (!is_valid_book_author(author)) {
            printf(".!! Error: Invalid author.\n");
            continue;
        }
        break;
    }
    while (1) {
        printf("Enter BID: ");
        if (!fgets(bid, sizeof(bid), stdin)) return;
        bid[strcspn(bid, "\n")] = 0;
        if (is_valid_bid(bid) == 7) {
            printf(".!! Error: BID cannot be an empty string\n");
            continue;
        }

        if (is_valid_bid(bid) == 8) {
            printf(".!! Error: BID cannot consist of only whitespace characters\n");
            continue;
        }

        if (is_valid_bid(bid) == 9) {
            printf(".!! Error: A tab character cannot be placed between the first and last valid characters of the BID\n");
            continue;
        }

        if (is_valid_bid(bid) == 0) {
            printf(".!! Error: BID contains invalid characters\n");
            continue;
        }
        if (!is_unique_bid(bid)) {
            printf("Error: A book with the specified BID already exists.\n");
            continue;
        }
        break;
    }

    char confirm[10];
    printf("Confirm adding this book? (No to cancel): ");
    fgets(confirm, sizeof(confirm), stdin);
    confirm[strcspn(confirm, "\n")] = 0;
    if (_stricmp(confirm, "no") == 0) {
        printf("Addition cancelled.\n");
        return;
    }
    bool book_integrity = true;
    linked_list* book_list = read_book_data(&book_integrity);
    Book* new_book = (Book*)malloc(sizeof(Book));

    strcpy(new_book->title, title);
    strcpy(new_book->author, author);
    strcpy(new_book->bid, bid);
    new_book->isAvailable = 'Y';
    new_book->isReserveAvailable = 'N';
    strcpy(new_book->studentId, "");

    insert_back(book_list, new_book);
    update_file(BOOK_FILE, book_list);
    printf("Book added successfully.\n");
}


void delete() {
    char bid_input[50];

    bool book_integrity = true;
    linked_list* book_list = read_book_data(&book_integrity);
    if (!book_list) {
        printf(".!! Error: Failed to read book data.\n");
        return;
    }

    while (1) {
        printf("BookPort: Administrator Mode - Enter BID of the book >");
        if (!fgets(bid_input, sizeof(bid_input), stdin)) return;
        bid_input[strcspn(bid_input, "\n")] = '\0';

        if (is_valid_bid(bid_input) == 7) {
            printf(".!! Error: BID cannot be an empty string\n");
            continue;
        }

        if (is_valid_bid(bid_input) == 8) {
            printf(".!! Error: BID cannot consist of only whitespace characters\n");
            continue;
        }

        if (is_valid_bid(bid_input) == 9) {
            printf(".!! Error: A tab character cannot be placed between the first and last valid characters of the BID\n");
            continue;
        }

        if (is_valid_bid(bid_input) == 0) {
            printf(".!! Error: BID contains invalid characters\n");
            continue;
        }

        node* current = book_list->head;
        Book* target_book = NULL;

        while (current) {
            Book* b = (Book*)current->data;
            //printf("DEBUG: BID in list = [%s]\n", b->bid); // 디버깅용
            if (strcmp(b->bid, bid_input) == 0) {
                target_book = b;
                break;
            }
            current = current->next;
        }

        if (!target_book) {
            printf(".!! Error: The book does not exist.\n");
            continue;
        }

        if (target_book->isAvailable != 'Y') {
            printf(".!! Error: The book that is on loan cannot be deleted.\n");
            continue;
        }

        printf("...The following book matches the entered BID:\n");
        printf("> Title: %s\n", target_book->title);
        printf("  Author: %s\n", target_book->author);
        printf("  BID: %s\n", target_book->bid);


        char confirm[10];
        printf("BookPort: Do you really want to delete this book? (...No) >");
        if (!fgets(confirm, sizeof(confirm), stdin)) {
            return;
        }
        confirm[strcspn(confirm, "\n")] = '\0';

        if (strcmp(confirm, "No") == 0 || strcmp(confirm, "no") == 0 ||
            strcmp(confirm, "NO") == 0 || strcmp(confirm, "nO") == 0) {
            printf("...Delete cancelled.\n");
            return;
        }

        if (target_book->isReserveAvailable == 'N') {
            bool user_integrity = true;
            linked_list* user_list = read_user_data(&user_integrity);
            User* reserver = find_by_userId(user_list, target_book->studentId);
            if (reserver != NULL) {
                reserver->reserveAvailable += 1;
            }
            update_file(USER_FILE, user_list);
            node* free_cursor = user_list->head;
            while (free_cursor != NULL) {
                node* next = free_cursor->next;
                free(free_cursor->data);
                free(free_cursor);
                free_cursor = next;
            }
            free(user_list);
        }


        remove_node(book_list, target_book, 2);
        update_file(BOOK_FILE, book_list);
        printf("...Book deleted\n");

        node* free_cursor = book_list->head;
        while (free_cursor != NULL) {
            node* next = free_cursor->next;
            free(free_cursor->data);
            free(free_cursor);
            free_cursor = next;
        }
        free(book_list);
        run_verify();
        return;
    }
}

bool exit_admin() {
    char input[50];
    printf("BookPort: Do you really want to exit administrator mode? (...No) > ");

    if (!fgets(input, sizeof(input), stdin)) {
        printf("...Exit cancelled\n");
        return false;
    }

    input[strcspn(input, "\n")] = '\0';

    if (strcmp(input, "No") == 0 || strcmp(input, "no") == 0 ||
        strcmp(input, "NO") == 0 || strcmp(input, "nO") == 0) {
        printf("...Exit cancelled\n");
        return false;
    }
    else {
        printf("...Exit administrator mode\n");
        return true;
    }
}


void run_admin() {
    if (is_logged_in) {
        printf(".!! Error: Access is not permitted. Logout and try again.\n");
        return;
    }

    char pw_input[100];
    printf("BookPort: Administrator Mode - Enter Password >");
    if (!fgets(pw_input, sizeof(pw_input), stdin)) return;
    pw_input[strcspn(pw_input, "\n")] = '\0';

    if (strcmp(pw_input, ADMIN_PASSWORD) != 0) {
        printf(".!! Error: It does not match the registered administrator password.\n");
        return;
    }
    printf("... Administrator authentication success\n");
    printf("[Administrator Mode]\n");
    printf("add: Add one book to the system.\n");
    printf("delete: Delete one book from the system.\n");
    printf("exit: Exit administrator mode.\n");

    char cmd_input[64];
    while (1) {
        printf("BookPort: Administrator Mode - Enter command >");
        if (!fgets(cmd_input, sizeof(cmd_input), stdin)) break;
        cmd_input[strcspn(cmd_input, "\n")] = '\0';

        char* cmd = get_admin_canonical_command(cmd_input);
        if (!cmd) {
            printf(".!! Error: Wrong command entered.\n");
            continue;
        }

        if (strcmp(cmd, "add") == 0) {
            add();
        }
        else if (strcmp(cmd, "delete") == 0) {
            delete();
        }
        else if (strcmp(cmd, "exit") == 0) {
            if (exit_admin()) break;
        }
    }
}