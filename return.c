#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "common.h"
#include "verify.h"

#define TEMP_FILE "temp_user.txt"
//int updata_file_2(Lend_Return);

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
        print_list(user_list, 5);
        return;
    }
    bool book_integrity = true;
    linked_list* book_list = read_book_data(&book_integrity);
    if (!book_integrity) {
        print_list(book_list, 4);
        return;
    }
    User* temp_u = NULL;
    Book* temp_b = NULL;
    Lend_Return* temp_l = NULL;
    node* current_u = user_list->head;
    while (current_u) {
        User* user = (User*)current_u->data;
        if (!strcmp(user->studentId, current_user.studentId)) {
            temp_u = user;
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
    while (1) {
        printf("Enter BID of the book to return > ");
        fgets(bid_input, sizeof(bid_input), stdin);
        trim(bid_input);

        // BID 유효성 검사
        if (!is_valid_bid(bid_input)) {
            continue;
        }

        int found = -1;
        for (int i = 0; i < 5 - (temp_u->lendAvailable); i++) {
            if (!strcmp(bid_input, temp_u->lentBids[i])) {
                found = i;
                break;
            }
        }
        temp_b = find_by_bid(book_list, bid_input);
        if (found == -1) {
            printf("Error: This book is not in your borrowed list.\n");
            continue;
        }
        else {
            for (int i = found; i < 4 - (temp_u->lendAvailable); i++) {
                strcpy(temp_u->lentBids[i], temp_u->lentBids[i + 1]);
            }
            temp_u->lentBids[4 - (temp_u->lendAvailable)][0] = '\0';
            break;
        }

    }
    bool lend_integrity = true;
    linked_list* lend_list = read_borrow_data(&lend_integrity);
    if (!lend_integrity) {
        print_list(lend_list, 5);
        printf("here Error");
        return;
    }
    node* current_l = lend_list->head;
    while (current_l) {
        Lend_Return* lend = (Lend_Return*)current_l->data;
        if (!strcmp(lend->bookBid, temp_b->bid)) {
            temp_l = lend;
        }
        current_l = current_l->next;
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
        strcpy(temp_l->returnDate, return_date);
        //temp_l->isOverdue = 'N';
        break;
    }
    printf("\n[He following book matches the entered BID]\n");

    printf("> Title: %s\n  Author: %s\n  BID: %s\n\n", temp_b->title, temp_b->author, temp_b->bid);

    printf("Do you really want to return this book? (Enter to confirm / No to cancel) > ");
    char confirm[5];
    fgets(confirm, sizeof(confirm), stdin);
    trim(confirm);

    if (_stricmp(confirm, "No") == 0) {
        printf("Returning cancelled.\n");
        return;
    }
    temp_u->lendAvailable++;
    temp_b->isAvailable = 'Y';

    if (!update_file(USER_FILE, user_list) || !update_file(BOOK_FILE, book_list) || !update_file(LEND_RETURN_FILE, lend_list)) {
        return;
    }
}