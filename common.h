#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stdbool.h>

#define MAX_NAME 101
#define MAX_ID 10
#define MAX_PW 21

#define MAX_TITLE 50
#define MAX_AUTHOR 50
#define MAX_BID 50
#define MAX_DATE 11

#define USER_FILE "users_data.txt"             
#define BOOK_FILE "books_data.txt"             
#define LEND_RETURN_FILE "lend_return_data.txt" 

typedef struct {
	char name[MAX_NAME];        // 이름
	char studentId[MAX_ID];     // 학번
	char password[MAX_PW];      // 비밀번호    
    char lentBids[5][MAX_BID];  // 대여한 책 BID   
	int lendAvailable;          // 대여 가능 권수
	int reserveAvailable;       // 예약 가능 권수
	char isOverdue;            // 연체 여부
} User;

typedef struct {
	char title[MAX_TITLE];      // 책 제목
	char author[MAX_AUTHOR];	// 책 저자
	char bid[MAX_BID];		    // 책 BID
	char isAvailable;           // 책 대여 가능 여부
	char isReserveAvailable;    // 책 예약 가능 여부
	char studentId[MAX_ID];     // 예약한 학생 ID
} Book;

typedef struct node {
    struct node* next;
    struct node* prev;
    void* data;
} node;

typedef struct linked_list {
    struct node* head;
    struct node* tail;
    int counter;
} linked_list;


bool update_file(const char* file_name, linked_list* list);
linked_list* read_user_data(bool* file_integrity);
linked_list* read_book_data(bool* file_integrity);
linked_list* read_borrow_data(bool* file_integrity);
void insert_back(linked_list* list, void* data);
void insert_front(linked_list* list, void* data, int type);
void* find(linked_list* list, void* data, int type);
Book* find_by_bid(linked_list* list, const char* bid);
User* find_by_userId(linked_list* list, const char* userId);
void remove_node(linked_list* list, void* data, int type);
void print_list(linked_list* list, int type);
bool check_equality(void* data1, void* data2, int type);
Book* find_by_author(linked_list* list, const char* author);
Book* find_by_title(linked_list* list, const char* title);
bool check_empty(char* token, bool* file_integrity);
void add_violation_line(linked_list* list, char* line);

typedef struct {
    char userid[MAX_ID];
    char bookBid[MAX_BID];
    char borrowDate[MAX_DATE];
    char returnDate[MAX_DATE];
} Lend_Return;

extern int is_logged_in;
extern User current_user;
extern int penalty_day, current_year, current_month, current_day;

void run_help(const char* argument);
void run_verify();
void run_account();
void run_login();
void run_logout();
int run_search(int mode);
void run_borrow();
void run_return();
void run_myinfo();
void run_admin();
bool checkOverDue(const char* input);

void print_command_usage();
char* get_canonical_command(const char* input);
void trim(char* str);

// login.c
User login_user(); // 로그인 함수

// account.c
User register_user(); // 회원가입 함수

#endif