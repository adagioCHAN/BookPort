#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "common.h"
#include "verify.h"

User register_user() {
    User new_user = { 0 };
    char buffer[200];

    // 이름 입력
    while (1) {
        printf("BookPort: Enter name >");
        fgets(buffer, sizeof(buffer), stdin);               // 이름 입력
        buffer[strcspn(buffer, "\n")] = 0;

        int validResult = is_valid_student_name(buffer);

        switch (validResult) {                              // 오류 처리
        case 0:
            strncpy(new_user.name, buffer, MAX_NAME);
            break;
        case 1:
            printf(".!! Error: Name cannot be an empty string.\n");
            break;
        case 2:
            printf(".!! Error: The name must be 1 to 100 characters long.\n");
            break;
        case 3:
            printf(".!! Error: Name cannot be entered in any language other than English.\n");
            break;
        case 4:
            printf(".!! Error: Name does not allow spaces except spaces through the space bar.\n");
            break;
        default:
            printf(".!! Error: An unknown error occured.\n");
            break;
        }

        if (validResult == 0)
            break;
    }

    // 학번 입력
    while (1) {
        printf("BookPort: Enter student ID >");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        int validResult = is_valid_student_id(buffer);
        int uniqueResult = is_unique_student_id(buffer);

        // 회원가입-학번입력 시 디버깅용
        /*
        fprintf(stderr, "[DEBUG] 입력된 학번: %s\n", buffer);

        printf("[DEBUG] 유효성 검사 결과: %d\n", validResult);
        fflush(stdout);

        printf("[DEBUG] 중복 검사 결과: %d\n", uniqueResult);
        fflush(stdout);

        printf("[DEBUG] strlen(buffer) = %zu\n", strlen(buffer));
        fflush(stdout);
        */

        // 학번 등록 및 오류 처리
        switch (validResult) {
        case 0:
            if (uniqueResult == 1)
                strncpy(new_user.studentId, buffer, MAX_ID);
            else if (uniqueResult == 0)
                printf(".!! Error: User information with the student ID you entered already exists\n");
            break;
        case 1:
            printf(".!! Error: Student ID cannot be an empty string.\n");
            break;
        case 2:
            printf(".!! Error: Student ID must consist of 9 digits.\n");
            break;
        case 3:
            printf(".!! Error: The first number of student ID caanot be zero.\n");
            break;
        case 4:
            printf(".!! Error: Student ID cannot contain spaces.\n");
            break;
        case 5:
            printf(".!! Error: Student ID can only be entered in numbers.\n");
            break;
        case 6:
            printf(".!! Error: A student ID cannot consist of more than eight identical numbers.\n");
            break;
        default:
            printf(".!! Error: An unknown error occured.\n");
            break;
        }

        if (validResult == 0 && uniqueResult == 1)
            break;
    }

    // 비밀번호 입력
    while (1) {
        printf("BookPort: Enter password >");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        int validResult = is_valid_password(buffer);

        // 비밀번호 등록 및 오류 처리
        switch (validResult) {
        case 0:
            strncpy(new_user.password, buffer, MAX_ID);
            break;
        case 1:
            printf(".!! Error: Password cannot be an empty string.\n");
            break;
        case 2:
            printf(".!! Error: The password must be 5 to 20 characters long.\n");
            break;
        case 3:
            printf(".!! Error: Password cannot contain spaces\n");
            break;
        case 4:
            printf(".!! Error: The password cannot contain 5 or more of the same letter, digit, or special character.\n");
            break;
        case 5:
            printf(".!! Error: Password must be at least 1 character long and include at least 1 digit\n");
            break;
        case 6:
            printf(".!! Error: Password input can only be used in English, numbers, and special characters (ASCII standard characters only)\n");
            break;
        default:
            printf(".!! Error: An unknown error occured\n");
            break;
        }

        if (validResult == 0)
            break;
    }

    // 회원가입 의사 확인

    while (1) {
        printf("\n⇒ Student ID: %s\n", new_user.studentId);
        printf("   Password: %s\n", new_user.password);
        printf("BookPort: Do you really want to sign up? (.../No) >");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (check_input(buffer) == 0) {
            printf("Account creation canceled.\n");
            User empty_user = { 0 };
            return empty_user; // 빈 사용자 구조체 반환
        }
        else {
            for (int i = 0; i < 5; i++)
                new_user.lentBids[i][0] = '\0'; // 대여한 책 BID 초기화
            new_user.lendAvailable = 5; // 대여 가능 권수 초기화

            printf("Account successfully created.\n");
            return new_user; // 성공적으로 생성된 사용자 구조체 반환

        }
    }
}