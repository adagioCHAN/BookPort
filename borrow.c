#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "common.h"
#include "verify.h"

void check_reserve_available(Book* bookBid, linked_list* book_list) {
    char buffer[200];

    bool user_integrity = true;
    linked_list* user_list = read_user_data(&user_integrity);
    User* user = find_by_userId(user_list, current_user.studentId);

    printf(".!!Error: The book is currently not available to borrow.\n");

    //책이 이미 예약 중
    if (bookBid->isReserveAvailable == 'N') {
        printf("The book cannot be reserved. It is already reserved.\n");
        return;
    }
    //사용자의 예약 가능 도서 개수 초과
    else if (user->reserveAvailable <= 0) {
        printf("The book cannot be reserved. Reservation limit reached.\n");
        return;
    }

    //예약할건지 여부 확인
    printf("BookPort: Do you want to reserve this book? (.../No) >");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';

    //No인 경우
    if (check_input(buffer) == 0) return;

    //No외 다른 것일 경우
    else {
        //현재 사용자의 <예약 가능 도서 개수>를 수정 
        user->reserveAvailable--;
        //해당 도서의 <예약 가능 여부>를 ‘N
        bookBid->isReserveAvailable = 'N';
        // <예약한 사용자 학번>을 추가 
        strcpy(bookBid->studentId, user->studentId);
        //file update
        update_file(USER_FILE, user_list);
        update_file(BOOK_FILE, book_list);

        // 예약되었다는 안내 메세지
        printf("The book has been successfully reserved.\n");
        // 주 프롬프트로 돌아갑니다.
        return;
    }
    return;
}


void run_borrow() {
    char bid_input[MAX_BID];
    Lend_Return lend;

    bool book_integrity = true;
    linked_list* book_list = read_book_data(&book_integrity);
    if (!book_integrity) {
        print_list(book_list, 4);
        return -1;
    }

    int search_result = run_search(0);
    
    if (!is_logged_in) {
        run_login();
        return;
    }
    if (current_user.lendAvailable <= 0) {
        printf("You cannot borrow more books.\n");
        return;
    }

    node* current = book_list->head;
    while (1) {
        while (search_result == 0 || search_result == 2) {
            if (search_result == 0) {
                search_result = run_search(0);
            }
            if (search_result == 2) {
                printf("Error: No available books found among the search results\n");
                search_result = run_search(0);
            }
        }
        printf("Enter BID of the book to borrow > ");
        fgets(bid_input, sizeof(bid_input), stdin);
        bid_input[strcspn(bid_input, "\n")] = '\0';

        int error_check = -1;

        // BID 유효성 검사
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

        trim(bid_input);
        // 검색된 책 중 BID 일치 여부 확인
        current = book_list->head;

        int found = 0;
        printf("\n[Search BID Result]\n");

        int count_y = 0;
        while (current) {
            Book* book = (Book*)current->data;
            int match_all = 1;
            if (!strcmp(book->bid, bid_input)) {
                found = 1;
                strcpy(lend.bookBid, book->bid);
                if (book->isAvailable != 'Y') {
                    printf("Error: The book is already borrowed.\n");
                    check_reserve_available(book, book_list);
                    found = 2;
                }
                book->isAvailable = 'N';
                break;
            }
            current = current->next;
        }

        if (found == 0) {
            printf("Error: No matching book found.\n");
            continue;
        }
        else if (found == 2) {
            return;
        }
        break;
    }

    char loan_date[MAX_DATE];

    while (1)
    {
        printf("Enter loan date > ");

        if (fgets(loan_date, sizeof(loan_date), stdin) == NULL) {
            printf("Input error!\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        // fgets는 줄바꿈('\n')을 포함할 수 있으므로 제거
        loan_date[strcspn(loan_date, "\n")] = '\0';

        // 버퍼 초과 방지: 입력이 가득 찼는데 줄바꿈 없으면 추가로 비워준다
        if (strlen(loan_date) == sizeof(loan_date) - 1 && loan_date[sizeof(loan_date) - 2] != '\n') {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }

        if (is_valid_date(loan_date) == 10) {
            printf(".!! Error: Loan date cannot be an empty string\n");
            continue;
        }
        if (is_valid_date(loan_date) == 11) {
            printf(".!! Error: Loan date cannot consist of only whitespace characters\n");
            continue;
        }
        if (is_valid_date(loan_date) == 12) {
            printf(".!! Error: A tab Character cannot be placed between the first and last valid characters of the loan date.\n");
            continue;
        }
        if (is_valid_date(loan_date) == 13) {
            printf(".!! Error: The date must exist in the Gregorian calendar\n");
            continue;
        }
        trim(loan_date);
        strcpy(lend.borrowDate, loan_date);
        break;
    }

    printf("Do you really want to borrow this book? (Enter to confirm / No to cancel) > ");
    char confirm[5];
    fgets(confirm, sizeof(confirm), stdin);
    trim(confirm);

    bool isoverdue = checkOverDue(loan_date);
    struct tm date = { 0 };
    int ny, nm, nd;
    if (isoverdue) {
        current_user.isOverdue = 'Y';
        date.tm_year = current_year - 1900;
        date.tm_mon = current_month - 1;
        date.tm_mday = current_day + penalty_day;

        mktime(&date);

        ny = date.tm_year + 1900;
        nm = date.tm_mon + 1;
        nd = date.tm_mday;
        printf("you got overdue! return your book first\n");
        return;
    }

    if (_stricmp(confirm, "No") == 0) {
        printf("Borrowing cancelled.\n");
        return;
    }

    // 1. Update book status
    Book* book = find_by_bid(book_list, lend.bookBid);
    if (book) {
        book->isAvailable = 'N';
    }
    update_file(BOOK_FILE, book_list);

    // 2. Update user data
    bool user_integrity = true;
    linked_list* user_list = read_user_data(&user_integrity);
    User* user = find_by_userId(user_list, current_user.studentId);
    if (user) {
        strcpy(user->lentBids[5 - user->lendAvailable], lend.bookBid);
        user->lendAvailable--;
        if (isoverdue) {
            user->isOverdue = 'Y';
        }
        else {
            user->isOverdue = 'N';
        }
        current_user = *user;
    }
    update_file(USER_FILE, user_list);
    printf("user data success");

    // 3. Append to lend_return_data
    bool lend_integrity = true;
    linked_list* lend_list = read_borrow_data(&lend_integrity);
    Lend_Return* new_lend = (Lend_Return*)malloc(sizeof(Lend_Return));

    strcpy(new_lend->userid, current_user.studentId);
    //printf("DEBUG: lend.bookBid = '%s'\n", lend.bookBid);
    strcpy(new_lend->bookBid, lend.bookBid);
    strcpy(new_lend->borrowDate, lend.borrowDate);
    strcpy(new_lend->returnDate, "");
    insert_back(lend_list, new_lend);
    update_file(LEND_RETURN_FILE, lend_list);

    printf("\nBook successfully borrowed!\n");
}