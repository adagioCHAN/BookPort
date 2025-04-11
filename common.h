#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#define MAX_NAME 101
#define MAX_ID 10
#define MAX_PW 21

#define MAX_TITLE 50
#define MAX_AUTHOR 50
#define MAX_BID 50

#define MAX_DATE 11

// 파일 경로
#define USER_FILE "users_data.txt"             // 사용자 정보 파일
#define BOOK_FILE "books_data.txt"             // 도서 정보 파일
#define LEND_RETURN_FILE "lend_return_data.txt" // 대여 정보 파일

// 사용자 구조체
typedef struct {
    char name[MAX_NAME];                      // 이름
    char studentId[MAX_ID];                  // 학번
    char password[MAX_PW];                   // 비밀번호
    char lentBids[5][MAX_BID];               // 대여한 책 BID 최대 5권 (';' 없이 배열로 처리)
    int lendAvailable;                       // 대여 가능 권수
} User;

// 도서 구조체
typedef struct {
    char title[MAX_TITLE];                   // 제목
    char author[MAX_AUTHOR];                 // 저자
    char bid[MAX_BID];                       // 도서 고유 BID
    char isAvailable;                         // 대여 가능 여부 ("Y": 대출 가능, "N": 대출 불가능)
} Book;

// 대출/반납 구조체
typedef struct {
    char userid[MAX_ID];                     // 사용자 학번
    char bookBid[MAX_BID];                   // 도서 BID
    char borrowDate[MAX_DATE];               // 대출일 (yyyy-mm-dd)
    char returnDate[MAX_DATE];               // 반납일 (yyyy-mm-dd)
    char isOverdue;                           // 연체 여부 (0: 정상, 1: 연체)
} Lend_Return;

// 로그인 여부와 사용자 ID
extern int is_logged_in;
extern char current_user_id[MAX_ID];

// run 함수 선언
void run_help(const char* argument);
void run_verify();
void run_account();
void run_login();
void run_logout();
void run_search();
void run_borrow();
void run_return();
void run_myinfo();

// 공통적으로 쓰이는 함수들
void print_command_usage();                     // 표 출력 함수
char* get_canonical_command(const char* input); // 동의어 → 대표명령어
void trim(char* str);

#endif
