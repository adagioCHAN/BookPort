#include "common.h"
#include <stdio.h>  // EOF 및 getchar() 인식 위해 필요
#include <string.h> 

// �α��� ���� �Ǵ�
int is_logged_in = 0; // �α��� �� 1
char current_user_id[MAX_ID] = "";

// 입력이 버퍼를 초과하였을 때 flush하는 기능
void flush_stdin() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}
