#ifndef VERIFY_H
#define VERIFY_H

#include "common.h"  // User �� ��� ����

#define MAX_LINE 500
#define MAX_USERS 1000
#define MAX_BOOKS 1000

// ����� ���� ���� �Լ�
//	�̸� �˻�
int is_valid_student_name(const char* name);					// ���� ��Ģ

//	�й� �˻�
int is_valid_student_id(const char* id);						// ���� ��Ģ
int is_unique_student_id(const char* id);						// �й� �ߺ� Ȯ��

//	��й�ȣ �˻�
int is_valid_password(const char* pw);							// ���� ��Ģ
int is_correct_password(const char* id, const char* pw);		// �ùٸ� pw Ȯ��

//	���� ���� ���� ���� �˻�
int is_valid_lendavailable(const int* lendAvailable);			// ���� ��Ģ

// ���� ���� ���� �Լ�
// ������ �˻�
int is_valid_book_title(const char* title);						// ���� ��Ģ

// ���� �˻�
int is_valid_book_author(const char* author);					// ���� ��Ģ

// ���� ���� BID �˻�
int is_valid_bid(const char* bid);								// ���� ��Ģ
int is_unique_bid(const char* bid);								// bid �ߺ� Ȯ��

// ���� ���� ���� �˻�
int is_valid_flag(const char* flag);							// ���� ��Ģ
int is_meaningful_flag(const char* flag);						// �ǹ� ��Ģ


// ����/�ݳ� ���� ���� �Լ�
// �й�, BID�� ���� �Լ��� ���

// ������ �˻�
int is_valid_date(const char* date);							// ���� & �ǹ� ��Ģ

// ��ü ����
int is_valid_overdue(const char* overdue);						// ���� ��Ģ
int is_meaningful_overdue(const char* overdue);					// �ǹ� ��Ģ

#endif