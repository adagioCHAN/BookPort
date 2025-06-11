#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common.h"
#include "verify.h"

#define ADMIN_PASSWORD "@admin_1pw$"

void print_book(Book* b) {
    printf("> Title: %s\n", b->title);
    printf("  Author: %s\n", b->author);
    printf("  BID: %s\n", b->bid);
}

void free_list(linked_list* list, int type) {
    if (!list) return;

    node* current = list->head;
    while (current != NULL) {
        node* next = current->next;

        switch (type) {
        case 1:
            free((User*)current->data);
            break;
        case 2:
            free((Book*)current->data);
            break;
        case 3:
            free((Lend_Return*)current->data);
            break;
        case 4:
            free((char*)current->data);
            break;
        default:
            break;
        }
        free(current);
        current = next;
    }
    free(list);
}

char* get_admin_canonical_command(char* input) {
    struct {
        const char* synonyms[10];
        const char* canonical;
    } admin_table[] = {
        { { "add", "ad", "a" }, "add" },
        { { "delete", "del", "de", "d" }, "delete" },
        { { "exit", "ex", "e" }, "exit" }
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
        fgets(bid, sizeof(bid), stdin);
        bid[strcspn(bid, "\n")] = 0;
        if (!is_valid_bid(bid) || !is_unique_bid(bid)) {
            printf(".!! Error: Invalid or duplicate BID.\n");
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
    strcpy(new_book->isAvailable, "Y");
    strcpy(new_book->isReserveAvailable, "N");
    strcpy(new_book->studentId, "");
    insert_back(book_list, new_book);
    update_file(BOOK_FILE, new_book);
    printf("Book added successfully.\n");
}

void delete() {
    char bid_input[50];
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

        bool book_integrity = true;
        linked_list* book_list = read_book_data(&book_integrity);
        if (!book_list) {
            printf(".!! Error: Failed to read book data.\n");
            return;
        }

        node* current = book_list->head;
        Book* target_book = NULL;

        while (current) {
            Book* b = (Book*)current->data;
            if (strcmp(b->bid, bid_input) == 0) {
                target_book = b;
                break;
            }
            current = current->next;
        }

        if (!target_book) {
            printf(".!! Error: The book does not exist.\n");
            free_list(book_list, 2);
            continue;
        }

        if (target_book->isAvailable != 'Y') {
            printf(".!! Error: The book that is on loan cannot be deleted.\n");
            free_list(book_list, 2);
            continue;
        }

        printf("...The following book matches the entered BID:\n");
        print_book(target_book);

        char confirm[10];
        printf("BookPort: Do you really want to delete this book? (...No) >");
        if (!fgets(confirm, sizeof(confirm), stdin)) {
            free_list(book_list, 2);
            return;
        }
        confirm[strcspn(confirm, "\n")] = '\0';

        if (strcmp(confirm, "No") == 0 || strcmp(confirm, "no") == 0 ||
            strcmp(confirm, "NO") == 0 || strcmp(confirm, "nO") == 0) {
            printf("...Delete cancelled.\n");
            free_list(book_list, 2);
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
            free_list(user_list, 1);
        }


        remove_node(book_list, target_book, 2);
        update_file(BOOK_FILE, book_list);
        printf("...Book deleted\n");

        free_list(book_list, 2);
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
