#ifndef VERIFY_H
#define VERIFY_H

#define MAX_LINE 500
#define MAX_USERS 1000
#define MAX_BOOKS 1000

#include "common.h" 

int is_valid_student_name(const char* name);				

int is_valid_student_id(const char* id);					
int is_unique_student_id(const char* id);				

int is_valid_password(const char* pw);							
int is_correct_password(const char* id, const char* pw);		

int check_input(const char* str);

int is_valid_lendavailable(const int* lendAvailable);			

int is_valid_book_title(const char* title);					

int is_valid_book_author(const char* author);			

int is_valid_bid(const char* bid);								
int is_unique_bid(const char* bid);								

int is_valid_flag(const char* flag);						
int is_meaningful_flag(const char* flag);						

int is_valid_date(const char* date);							

int is_valid_overdue(const char* overdue);					
int is_meaningful_overdue(const char* overdue);				

#endif