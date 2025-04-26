#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "common.h"
#include "verify.h"

User get_user_by_id(const char* id) {
    FILE* file = fopen(USER_FILE, "r");
    User user = { 0 };

    if (!file)
        return user;

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        char* token = strtok(line, ",");
        if (token) strncpy(user.name, token, MAX_NAME);

        token = strtok(NULL, ",");
        if (token) strncpy(user.studentId, token, MAX_ID);

        if (strcmp(user.studentId, id) == 0) {
            token = strtok(NULL, ",");
            if (token) strncpy(user.password, token, MAX_PW);

            token = strtok(NULL, ",");
            if (token) {
                char* bidToken = strtok(token, ";");
                for (int i = 0; i < 5; i++) {
                    if (bidToken) {
                        strncpy(user.lentBids[i], bidToken, MAX_BID);
                        bidToken = strtok(NULL, ";");
                    }
                    else
                        user.lentBids[i][0] = '\0'; // 대여한 책 BID 초기화
                }
            }


            user.lendAvailable = 5;
            for (int i = 0; i < 5; i++) {
                if (user.lentBids[i][0] != '\0') {
                    user.lendAvailable--;
                }
            }

            fclose(file);
            return user;
        }
    }

    fclose(file);
    User empty_user = { 0 };
    return empty_user; // 사용자 정보가 없을 경우 빈 사용자 구조체 반환
}

User login_user() {
    char buffer[200];
    char studentId[MAX_ID];
    char password[MAX_PW];
    User user = { 0 };

    // 학번 입력
    while (1) {
        printf("Enter student ID >");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        int validResult = is_valid_student_id(buffer);
        int uniqueResult = is_unique_student_id(buffer);

        // 로그인-학번입력 시 디버깅용
        /*
        fprintf(stderr, "[DEBUG] 입력된 학번: %s\n", buffer);

        printf("[DEBUG] 유효성 검사 결과: %d\n", validResult);
        fflush(stdout);

        printf("[DEBUG] 중복 검사 결과: %d\n", uniqueResult);
        fflush(stdout);

        printf("[DEBUG] strlen(buffer) = %zu\n", strlen(buffer));
        fflush(stdout);
        */

        // 학번 오류 처리
        if (validResult != 0) {
            switch (validResult) {
            case 1:
                printf(".!! Error: Student ID cannot be an empty string.\n");
                break;
            case 2:
                printf(".!! Error: Student ID must consist of 9 digits.\n");
                break;
            case 3:
                printf(".!! Error: The first number of student ID cannot be zero.\n");
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
        }

        // 학번 존재 여부 확인

        if (uniqueResult == 0) {
            strncpy(studentId, buffer, MAX_ID);
            break;
        }
        else {
            printf(".!! Error: No user information exists for the entered student ID.\n");
            continue;
        }
    }

    // 학번 디버깅용
    /*
    printf("[DEBUG] studentId = '%s'\n", studentId);
    fflush(stdout);
    */

    // 비밀번호 입력
    while (1) {
        printf("Enter password >");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        int validResult = is_valid_password(buffer);
        int correctResult = is_correct_password(studentId, buffer);

        // 비밀번호 디버깅용
        /*
        fprintf(stderr, "[DEBUG] 입력된 pw: %s\n", buffer);

        printf("[DEBUG] 유효성 검사 결과: %d\n", validResult);
        fflush(stdout);

        printf("[DEBUG] 중복 검사 결과: %d\n", correctResult);
        fflush(stdout);
        */

        // 비밀번호 오류 처리
        if (validResult != 0) {
            switch (validResult) {
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
            default:
                printf(".!! Error: An unknown error occured\n");
                break;
            }
        }
        else {
            if (correctResult == 1) {
                strncpy(password, buffer, MAX_PW);
                break;
            }
            else {
                printf(".!! Error: Password does not match.\n");
                continue;
            }
        }


    }


    // 로그인 의사 확인
    while (1) {
        printf("\nStudent ID: %s\n", studentId);
        printf("Password: %s\n", password);
        printf("Are you sure you want to login? (.../No) >");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (check_input(buffer) == 0)
            return user; // 빈 사용자 구조체 반환
        else {
            user = get_user_by_id(studentId);
            return user; // 성공적으로 생성된 사용자 구조체 반환
        }
    }

}