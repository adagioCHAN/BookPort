#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"

// 이거 login.c에 정의돼있을거같은데 그냥 썼음
#define MIN_PW_LEN 5
#define MAX_PW_LEN 20

extern void run_verify(); // 무결성 검사
extern void run_logout(); // 로그아웃

// 전역 로그인 상태 변수들
extern int is_logged_in;
extern char current_user_id[MAX_ID];
extern User current_user; // run_login() 시점에 설정되어 있어야 함


// 공백류 처리 (개행은 유지)
int is_custom_whitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r');
}
void trim(char* str) {
    while (is_custom_whitespace(*str)) memmove(str, str + 1, strlen(str));
    int len = strlen(str);
    while (len > 0 && is_custom_whitespace(str[len - 1])) str[--len] = '\0';
}


// 비밀번호 문법 검사
// 반환값 1 = 유효, 0 = 에러 (에러 메시지 출력 포함)
// 이거 login.c에 들어있을 것 같은데 여기선 그냥 썼음.
int validate_password(const char *pw) {
    int len = strlen(pw);

    // 1. 비어 있는 문자열
    if (len == 0) {
        printf(".!! Error: Password cannot be an empty string.\n");
        return 0;
    }

    // 2. 공백 문자만 존재하는지
    int only_whitespace = 1;
    for (int i = 0; i < len; i++) {
        if (!isspace(pw[i])) {
            only_whitespace = 0;
            break;
        }
    }
    if (only_whitespace) {
        printf(".!! Error: Password cannot consist of only whitespace characters.\n");
        return 0;
    }

    // 3. 탭 문자가 유효 문자 사이에 있는 경우 (↹ = \t)
    int first_valid = -1, last_valid = -1;
    for (int i = 0; i < len; i++) {
        if (!isspace(pw[i])) {
            if (first_valid == -1) first_valid = i;
            last_valid = i;
        }
    }
    for (int i = first_valid + 1; i < last_valid; i++) {
        if (pw[i] == '\t') {
            printf(".!! Error: A tab character cannot be placed between the first and last valid characters of the password.\n");
            return 0;
        }
    }

    // 4. 길이 제한
    if (len < MIN_PW_LEN || len > MAX_PW_LEN) {
        printf(".!! Error: Password must be 5 to 20 characters long.\n");
        return 0;
    }

    // 5. 영문자와 숫자 하나 이상 포함 여부
    int has_alpha = 0, has_digit = 0;
    for (int i = 0; i < len; i++) {
        if (isalpha(pw[i])) has_alpha = 1;
        if (isdigit(pw[i])) has_digit = 1;
    }
    if (!has_alpha || !has_digit) {
        printf(".!! Error: Password must contain at least one letter and at least one digit.\n");
        return 0;
    }

    // 6. 동일 문자 5회 이상 연속 사용 금지
    int counts[128] = {0};
    for (int i = 0; i < len; i++) {
        unsigned char ch = (unsigned char)pw[i];
        counts[ch]++;
        if (counts[ch] >= 5) {
            printf(".!! Error: The password cannot contain 5 or more of the same letter, digit, or special character.\n");
            return 0;
        }
    }

    // 모두 통과
    return 1;
}

// ### 비밀번호 변경 ###
void change_password() {
    if (!is_logged_in) {
        printf(".!! Error: Please log in first.\n");
        return;
    }

    char new_pw[100];
    printf("BookPort: Enter password > ");
    if (!fgets(new_pw, sizeof(new_pw), stdin)) return;
    new_pw[strcspn(new_pw, "\n")] = '\0';

    if (strcmp(new_pw, current_user.password) == 0) {
        printf(".!! Error: Password is the same as before.\n");
        return;
    }

    if (!validate_password(new_pw)) {
        return; // 오류 메시지는 내부에서 출력됨
    }

    // ===== 연결 리스트를 통한 파일 수정 =====

    typedef struct UserNode {
        char line[512];
        char studentId[MAX_ID];
        struct UserNode* next;
    } UserNode;

    FILE *fp = fopen(USER_FILE, "r");
    if (!fp) {
        printf(".!! Error: Failed to open user file.\n");
        return;
    }

    UserNode *head = NULL, *tail = NULL;
    char buf[512];

    while (fgets(buf, sizeof(buf), fp)) {
        UserNode* node = malloc(sizeof(UserNode));
        if (!node) continue;
        node->next = NULL;
        strcpy(node->line, buf);

        char copy[512];
        strcpy(copy, buf);
        strtok(copy, ",");                 // name
        char* sid = strtok(NULL, ",");     // student ID
        if (!sid) {
            free(node);
            continue;
        }
        strncpy(node->studentId, sid, MAX_ID);

        if (!head) head = node;
        else tail->next = node;
        tail = node;
    }
    fclose(fp);

    // 새 비밀번호를 반영해서 다시 파일에 쓰기
    fp = fopen(USER_FILE, "w");
    if (!fp) {
        printf(".!! Error: Failed to write user file.\n");
        return;
    }

    UserNode* cur = head;
    while (cur) {
        if (strcmp(cur->studentId, current_user_id) == 0) {
            // 비밀번호 변경된 줄 작성
            fprintf(fp, "%s,%s,%s,", current_user.name, current_user.studentId, new_pw);
            for (int i = 0; i < 5; i++) {
                fprintf(fp, "%s", current_user.lentBids[i]);
                if (i < 4) fprintf(fp, ";");
            }
            fprintf(fp, ",%d\n", current_user.lendAvailable);

            // current_user 비밀번호도 갱신
            strcpy(current_user.password, new_pw);
        } else {
            fputs(cur->line, fp);
        }
        cur = cur->next;
    }
    fclose(fp);

    // 메모리 해제
    while (head) {
        UserNode* tmp = head;
        head = head->next;
        free(tmp);
    }
    printf("\u2026 Your password successfully changed\n");
}

// ### 회원 탈퇴 ###
void withdraw_user() {
    printf("BookPort: Do you want to withdraw?(.../No) > ");
    char input[100];
    if (!fgets(input, sizeof(input), stdin)) return;
    input[strcspn(input, "\n")] = '\0';

    if (strcmp(input, "No") == 0) {
        return;
    }

    // 1. 연결 리스트 생성
    FILE *fp = fopen(USER_FILE, "r");
    if (!fp) {
        printf(".!! Error: Failed to open user data.\n");
        return;
    }

    typedef struct UserNode {
        char line[512];
        struct UserNode* next;
    } UserNode;

    UserNode* head = NULL;
    UserNode* tail = NULL;

    char buf[512];
    while (fgets(buf, sizeof(buf), fp)) {
        char copy[512];
        strcpy(copy, buf);

        // 학번 추출
        char* name = strtok(copy, ",");
        char* sid  = strtok(NULL, ",");
        if (!sid) continue;

        if (strcmp(sid, current_user_id) == 0) {
            continue;  // 탈퇴 대상 → 연결하지 않음
        }

        UserNode* newNode = (UserNode*)malloc(sizeof(UserNode));
        if (!newNode) continue;
        strcpy(newNode->line, buf);
        newNode->next = NULL;

        if (!head) head = newNode;
        else tail->next = newNode;
        tail = newNode;
    }
    fclose(fp);

    // 2. 덮어쓰기
    fp = fopen(USER_FILE, "w");
    if (!fp) {
        printf(".!! Error: Unable to write user data.\n");
        return;
    }

    UserNode* cur = head;
    while (cur) {
        fputs(cur->line, fp);
        cur = cur->next;
    }
    fclose(fp);

    // 3. 메모리 해제
    while (head) {
        UserNode* temp = head;
        head = head->next;
        free(temp);
    }

    printf("Withdrawal successful.\n");
    run_verify(); // 무결성 검사
    run_logout();
    // run_login(), run_logout()에서 밑 두 변수들 조정해주면 좋을듯.
    // is_logged_in = 0;
    // current_user_id[0] = '\0';
}

// run_myinfo: 메인 + 부 프롬프트
void run_myinfo() {
    
    // 1. 사용자 기본 정보 출력
    printf("[My Information]\n");
    printf("Name: %s\n", current_user.name);
    printf("ID: %s\n", current_user.studentId);

    // 2. 부 프롬프트 (withdraw / change / manage)
    char cmd[100];
    while (1) {
        printf("BookPort: My info - Enter command > ");
        if (!fgets(cmd, sizeof(cmd), stdin)) break;
        cmd[strcspn(cmd, "\n")] = '\0';
        trim(cmd);
        for (int i = 0; cmd[i]; i++) cmd[i] = tolower(cmd[i]);

        if (strncmp(cmd, "withdraw", strlen(cmd)) == 0) {
            withdraw_user();
            break;
        } else if (strncmp(cmd, "change", strlen(cmd)) == 0) {
            change_password();
            break;
        } else if (strncmp(cmd, "manage", strlen(cmd)) == 0) {
            // 제가 구현하는 부분이 아니라 남겨두었습니다.

            break;
        } else {
            printf(".!! Wrong command entered.\n");
        }
    }
}