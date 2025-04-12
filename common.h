#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#define MAX_NAME 100
#define MAX_ID 10
#define MAX_PW 20
#define MAX_BID 105

#define MAX_TITLE 50
#define MAX_AUTHOR 30

#define MAX_DATE 11

#define MIN_PW_LEN 5
#define MAX_PW_LEN 20

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
    int isAvailable;                         // �뿩 ���� ���� (0: ����, 1: �뿩 ��)
} Book;

// ����/�ݳ� ����ü
typedef struct {
    char userid[MAX_ID];                     // ����� �й�
    char bookBid[MAX_BID];                   // ���� BID
    char borrowDate[MAX_DATE];              // ������ (yyyy-mm-dd)
    char returnDate[MAX_DATE];              // �ݳ��� (yyyy-mm-dd)
    int isOverdue;                           // ��ü ���� (0: ����, 1: ��ü)
} Lend_Return;

// �α��� ���ο� ����� ID
extern int is_logged_in;
extern char current_user_id[MAX_ID];

// run �Լ� ����
void run_help(const char* argument);
void run_verify();
void run_account();
void run_login();
void run_logout();
void run_search();
void run_borrow();
void run_return();
void run_myinfo();

// ���������� ���̴� �Լ���
void print_command_usage();                     // ǥ ��� �Լ�
char* get_canonical_command(const char* input); // ���Ǿ� �� ��ǥ���ɾ�
void trim(char* str);

void flush_stdin();

#endif
