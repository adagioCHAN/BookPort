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

// ���� ���
#define USER_FILE "users_data.txt"             // ����� ���� ����
#define BOOK_FILE "books_data.txt"             // ���� ���� ����
#define LEND_RETURN_FILE "lend_return_data.txt" // �뿩 ���� ����

// ����� ����ü
typedef struct {
    char name[MAX_NAME];                      // �̸�
    char studentId[MAX_ID];                  // �й�
    char password[MAX_PW];                   // ��й�ȣ
    char lentBids[5][MAX_BID];               // �뿩�� å BID �ִ� 5�� (';' ���� �迭�� ó��)
    int lendAvailable;                       // �뿩 ���� �Ǽ�
} User;

// ���� ����ü
typedef struct {
    char title[MAX_TITLE];                   // ����
    char author[MAX_AUTHOR];                 // ����
    char bid[MAX_BID];                       // ���� ���� BID
    char isAvailable;                         // �뿩 ���� ���� ("Y": ���� ����, "N": ���� �Ұ���)
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
linked_list* read_user_data();
linked_list* read_book_data();
linked_list* read_borrow_data();
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

// ����/�ݳ� ����ü
typedef struct {
    char userid[MAX_ID];                     // ����� �й�
    char bookBid[MAX_BID];                   // ���� BID
    char borrowDate[MAX_DATE];               // ������ (yyyy-mm-dd)
    char returnDate[MAX_DATE];               // �ݳ��� (yyyy-mm-dd)
    char isOverdue;                           // ��ü ���� (0: ����, 1: ��ü)
} Lend_Return;

// 로그인 여부와 사용자 ID
extern int is_logged_in;
extern User current_user;

// run �Լ� ����
void run_help(const char* argument);
void run_verify();
void run_account();
void run_login();
void run_logout();
int run_search(int mode);
void run_borrow();
void run_return();
void run_myinfo();

// ���������� ���̴� �Լ���
void print_command_usage();                     // ǥ ��� �Լ�
char* get_canonical_command(const char* input); // ���Ǿ� �� ��ǥ���ɾ�
void trim(char* str);

#endif