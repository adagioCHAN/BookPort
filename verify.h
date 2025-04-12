#ifndef RULE.H
#define RULE.H

#include "common.h"  // User 등 사용 가능

// 사용자 정보 관련 함수
//	이름 검사
int is_valid_student_name(const char* name);					// 문법 규칙

//	학번 검사
int is_valid_student_id(const char* id);						// 문법 규칙
int is_unique_student_id(const char* id);						// 의미 규칙

//	비밀번호 검사
int is_valid_password(const char* name);						// 문법 규칙

//	대출 가능 도서 개수 검사
int is_valid_lendavailable(const int* lendAvaiable);			// 문법 규칙



// 도서 정보 관련 함수
// 도서명 검사
int is_valid_book_title(const char* title);						// 문법 규칙

// 저자 검사
int is_valid_book_author(const char* author);					// 문법 규칙

// 도서 고유 BID 검사
int is_valid_bid(const char* bid);								// 문법 규칙
int is_unique_bid(const char* bid);								// 의미 규칙

// 대출 가능 여부 검사
int is_valid_flag(const char* flag);							// 문법 규칙
int is_meaningful_flag(const char* flag);						// 의미 규칙



// 대출/반납 정보 관련 함수
// 학번, BID는 위의 함수들 사용

// 대출일 검사
int is_valid_date(const char* date);							// 문법 & 의미 규칙

// 연체 여부
int is_valid_overdue(const char* overdue);						// 문법 규칙
int is_meaningful_overdue(const char* overdue);					// 의미 규칙

#endif