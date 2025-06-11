#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "common.h"

bool update_file(const char* file_name, linked_list* list) {
	FILE* fp = fopen(file_name, "w+");
	User* user_data;
	Book* book_data;
	Lend_Return* borrow_data;
	node* current = list->head;
	while (current != NULL) {
		if (strcmp(file_name, USER_FILE) == 0) {
			user_data = (User*)current->data;
			fprintf(fp, "%s,%s,%s", user_data->name, user_data->studentId, user_data->password);
			for (int i = 0; i < 5 - user_data->lendAvailable; i++) {
				if (i == 0)fprintf(fp, ",%s", user_data->lentBids[i]);
				else fprintf(fp, ";%s", user_data->lentBids[i]);
			}
			if (user_data->lendAvailable == 5) {
				fprintf(fp, ",");
			}
			fprintf(fp, ",%d,%d,%c", user_data->lendAvailable, user_data->reserveAvailable, user_data->isOverdue);
		}
		else if (strcmp(file_name, BOOK_FILE) == 0) {
			book_data = (Book*)current->data;
			fprintf(fp, "%s,%s,%s,%c,%c,%s\n", book_data->title, book_data->author, book_data->bid, book_data->isAvailable, book_data->isReserveAvailable, book_data->studentId);
		}
		else if (strcmp(file_name, LEND_RETURN_FILE) == 0) {
			borrow_data = (Lend_Return*)current->data;
			fprintf(fp, "%s,%s,%s,%s\n", borrow_data->userid, borrow_data->bookBid, borrow_data->borrowDate, borrow_data->returnDate);
		}
		current = current->next;
	}
	fclose(fp);
	return true;
}

void add_violation_line(linked_list* list, char* line) {
	char* str = (char*)malloc(strlen(line) + 1);
	strcpy(str, line);
	insert_back(list, (void*)str);
}

char* strsep(char** stringp, const char* delim) {
	char* start = *stringp;
	if (start == NULL) return NULL;

	char* end = strpbrk(start, delim);
	if (end) {
		*end = '\0';
		*stringp = end + 1;
	}
	else {
		*stringp = NULL;
	}

	return start;
}

linked_list* read_user_data(bool* file_integrity) {
	linked_list* list = (linked_list*)calloc(1, sizeof(linked_list));
	linked_list* violation_lines = (linked_list*)calloc(1, sizeof(linked_list));
	FILE* fp = fopen(USER_FILE, "r");
	char line[1 << 10];
	char* rest = line;
	char line_copy[1 << 10];
	if (fp == NULL) {
		fp = fopen(USER_FILE, "w+");
		fclose(fp);
		return list;
	}
	while (fgets(line, sizeof(line), fp)) {
		strcpy(line_copy, line);
		rest = line;
		User* data = (User*)calloc(1, sizeof(User));
		line[strcspn(line, "\n")] = 0;
		char* token = strsep(&rest, ",");
		if (token == NULL) { add_violation_line(violation_lines, line_copy); *file_integrity = false; continue; }
		strcpy(data->name, token);

		token = strsep(&rest, ",");
		if (token == NULL) { add_violation_line(violation_lines, line_copy); *file_integrity = false; continue; }
		strcpy(data->studentId, token);

		token = strsep(&rest, ",");
		if (token == NULL) { add_violation_line(violation_lines, line_copy); *file_integrity = false; continue; }
		strcpy(data->password, token);

		token = strsep(&rest, ",");
		if (token == NULL) { add_violation_line(violation_lines, line_copy); *file_integrity = false; continue; }
		int idx = 0;
		char tmp[1 << 10];
		char* tmp_p = tmp;
		strcpy(tmp, token);

		token = strsep(&rest, ",");
		if (token == NULL) { add_violation_line(violation_lines, line_copy); *file_integrity = false; continue; }
		char* bid = strsep(&tmp_p, ";");

		while (bid != NULL) {
			strcpy(data->lentBids[idx], bid);
			bid = strsep(&tmp_p, ";");
			idx++;
		}

		data->lendAvailable = token[0] - '0';
		token = strsep(&rest, ",");
		if (token == NULL) { add_violation_line(violation_lines, line_copy); *file_integrity = false; continue; }
		data->reserveAvailable = token[0] - '0';

		token = strsep(&rest, ",");
		if (token == NULL) { add_violation_line(violation_lines, line_copy); *file_integrity = false; continue; }
		data->isOverdue = token[0];

		bool stop_flag = false;
		int len = strlen(data->name);
		if (len < 1 || len > 100) {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
		}
		for (int i = 0; i < len; i++) {
			if (!islower(data->name[i])) {
				add_violation_line(violation_lines, line_copy); *file_integrity = false; stop_flag = true; break;
			}
		}

		if (strlen(data->studentId) != 9) {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
		}
		if (data->studentId[0] == '0') {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
		}
		for (int i = 0; i < 9; i++) {
			if (!isdigit(data->studentId[i])) {
				add_violation_line(violation_lines, line_copy); *file_integrity = false; stop_flag = true; break;
			}
		}
		for (char d = '0'; d <= '9'; d++) {
			int count = 0;
			for (int i = 0; data->studentId[i]; i++) {
				if (data->studentId[i] == d) count++;
				else count = 0;
				if (count >= 8) {
					add_violation_line(violation_lines, line_copy); *file_integrity = false; stop_flag = true; break;
				}
			}
		}

		int cnt_alpha = 0;
		int cnt_num = 0;
		int cnt_arr[256] = { 0 };
		len = strlen(data->password);
		if (len < 5 || len > 20) {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
		}
		for (int i = 0; i < len; i++) {
			if (isspace(data->password[i])) {
				add_violation_line(violation_lines, line_copy); *file_integrity = false; stop_flag = true; break;
			}
			if (isdigit(data->password[i])) cnt_num++;
			if (isalpha(data->password[i])) cnt_alpha++;
			cnt_arr[(unsigned char)data->password[i]]++;
		}
		if (cnt_num <= 0 || cnt_alpha <= 0) {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
		}
		for (int i = 0; i < 128; i++) {
			if (cnt_arr[i] > 5) {
				add_violation_line(violation_lines, line_copy); *file_integrity = false; stop_flag = true; break;
			}
		}

		if (data->lendAvailable < 0 || data->lendAvailable>5) {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
		}
		if (data->reserveAvailable < 0 || data->reserveAvailable>3) {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
		}
		if (data->isOverdue != 'Y' && data->isOverdue != 'N') {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
		}

		for (int i = 0; i < 5 - data->lendAvailable; i++) {
			if (strlen(data->lentBids[i]) == 0) {
				add_violation_line(violation_lines, line_copy); *file_integrity = false; stop_flag = true; continue;
			}
			if (data->lentBids[i][1] != '-') {
				add_violation_line(violation_lines, line_copy); *file_integrity = false; stop_flag = true; continue;
			}
			int pos;
			for (int j = 2; data->lentBids[i][j]; j++) {
				if (!isdigit(data->lentBids[i][j])) {
					if (data->lentBids[i][j] != '.') {
						add_violation_line(violation_lines, line_copy); *file_integrity = false; stop_flag = true; pos = i; break;
					}
					else {
						pos = i;
						break;
					}
				}
			}
			if (pos + 1 == strlen(data->lentBids[i])) {
				add_violation_line(violation_lines, line_copy); *file_integrity = false; stop_flag = true; continue;
			}
			for (int j = pos + 1; data->lentBids[i][j]; j++) {
				if (!isdigit(data->lentBids[i][j])) {
					add_violation_line(violation_lines, line_copy); *file_integrity = false; stop_flag = true; break;
				}
			}
		}

		if (stop_flag) {
			continue;
		}

		insert_back(list, (void*)data);
	}
	fclose(fp);

	if (*file_integrity) {
		free(violation_lines);
		return list;
	}
	else {
		return violation_lines;
	}
}

linked_list* read_book_data(bool* file_integrity) {
	linked_list* list = (linked_list*)calloc(1, sizeof(linked_list));
	linked_list* violation_lines = (linked_list*)calloc(1, sizeof(linked_list));
	FILE* fp = fopen(BOOK_FILE, "r");
	char line[1 << 10];
	char* rest;
	char line_copy[1 << 10];
	if (fp == NULL) {
		fp = fopen(BOOK_FILE, "w+");
		fclose(fp);
		return list;
	}
	while (fgets(line, sizeof(line), fp)) {
		strcpy(line_copy, line);
		rest = line;
		Book* data = (Book*)calloc(1, sizeof(Book));
		line[strcspn(line, "\n")] = 0;
		char* token = strsep(&rest, ",");
		if (token == NULL) { add_violation_line(violation_lines, line_copy); *file_integrity = false; continue; }
		strcpy(data->title, token);
		token = strsep(&rest, ",");
		if (token == NULL) { add_violation_line(violation_lines, line_copy); *file_integrity = false; continue; }
		strcpy(data->author, token);
		token = strsep(&rest, ",");
		if (token == NULL) { add_violation_line(violation_lines, line_copy); *file_integrity = false; continue; }
		strcpy(data->bid, token);
		token = strsep(&rest, ",");
		if (token == NULL) { add_violation_line(violation_lines, line_copy); *file_integrity = false; continue; }
		data->isAvailable = token[0];
		token = strsep(&rest, ",");
		if (token == NULL) { add_violation_line(violation_lines, line_copy); *file_integrity = false; continue; }
		data->isReserveAvailable = token[0];
		token = strsep(&rest, ",");
		if (token == NULL) { add_violation_line(violation_lines, line_copy); *file_integrity = false; continue; }
		strcpy(data->studentId, token);


		bool stop_flag = false;
		if (strlen(data->title) == 0) {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
		}
		for (int i = 0; data->title[i]; i++) {
			if ((isspace(data->title[i]) && data->title[i] != ' ')) {
				add_violation_line(violation_lines, line_copy); *file_integrity = false; stop_flag = true; break;
			}
		}
		if (data->title[0] == ' ') {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
		}
		if (data->title[strlen(data->title) - 1] == ' ') {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
		}

		if (strlen(data->author) == 0) {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
		}
		for (int i = 0; data->author[i]; i++) {
			if (!isalpha(data->author[i])) {
				add_violation_line(violation_lines, line_copy); *file_integrity = false; stop_flag = true; break;
			}
		}

		if (strlen(data->bid) == 0) {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
		}
		for (int i = 0; data->bid[i]; i++) {
			if (isspace(data->bid[i]) && !(data->bid[i] == '-' || data->bid[i] == ':' || data->bid == '.')) {
				add_violation_line(violation_lines, line_copy); *file_integrity = false; stop_flag = true; break;
			}
		}

		if (data->isAvailable != 'Y' && data->isAvailable != 'N') {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; stop_flag = true; continue;
		}

		if (data->isReserveAvailable != 'Y' && data->isReserveAvailable != 'N') {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; stop_flag = true; continue;
		}

		if (strlen(data->studentId) != 0) {
			if (strlen(data->studentId) != 9) {
				add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
			}
			if (data->studentId[0] == '0') {
				add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
			}
			for (int i = 0; i < 9; i++) {
				if (!isdigit(data->studentId[i])) {
					add_violation_line(violation_lines, line_copy); *file_integrity = false; stop_flag = true; break;
				}
			}
			for (char d = '0'; d <= '9'; d++) {
				int count = 0;
				for (int i = 0; data->studentId[i]; i++) {
					if (data->studentId[i] == d) count++;
					else count = 0;
					if (count >= 8) {
						add_violation_line(violation_lines, line_copy); *file_integrity = false; stop_flag = true; break;
					}
				}
			}
		}

		if (stop_flag) {
			continue;
		}

		insert_back(list, (void*)data);
	}
	fclose(fp);
	if (*file_integrity) {
		free(violation_lines);
		return list;
	}
	else
		return violation_lines;

}

linked_list* read_borrow_data(bool* file_integrity) {
	linked_list* list = (linked_list*)calloc(1, sizeof(linked_list));
	linked_list* violation_lines = (linked_list*)calloc(1, sizeof(linked_list));
	FILE* fp = fopen(LEND_RETURN_FILE, "r");
	char line[1 << 10];
	char* rest = line;
	char line_copy[1 << 10];
	if (fp == NULL) {
		fp = fopen(LEND_RETURN_FILE, "w+");
		fclose(fp);
		return list;
	}
	while (fgets(line, sizeof(line), fp)) {
		strcpy(line_copy, line);
		rest = line;
		Lend_Return* data = (Lend_Return*)calloc(1, sizeof(Lend_Return));
		line[strcspn(line, "\n")] = 0;
		char* token = strsep(&rest, ",");
		if (token == NULL) { add_violation_line(violation_lines, line_copy); *file_integrity = false; continue; }
		strcpy(data->userid, token);
		token = strsep(&rest, ",");
		if (token == NULL) { add_violation_line(violation_lines, line_copy); *file_integrity = false; continue; }
		strcpy(data->bookBid, token);
		token = strsep(&rest, ",");
		if (token == NULL) { add_violation_line(violation_lines, line_copy); *file_integrity = false; continue; }
		strcpy(data->borrowDate, token);
		token = strsep(&rest, ",");
		if (token == NULL) { add_violation_line(violation_lines, line_copy); *file_integrity = false; continue; }
		strcpy(data->returnDate, token);
		token = strsep(&rest, ",");

		bool stop_flag = false;
		if (strlen(data->userid) != 9) {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
		}
		if (data->userid[0] == '0') {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
		}
		for (int i = 0; i < 9; i++) {
			if (!isdigit(data->userid[i])) {
				add_violation_line(violation_lines, line_copy); *file_integrity = false; stop_flag = true; break;
			}
		}
		for (char d = '0'; d <= '9'; d++) {
			int count = 0;
			for (int i = 0; data->userid[i]; i++) {
				if (data->userid[i] == d) count++;
				else count = 0;
				if (count >= 8) {
					add_violation_line(violation_lines, line_copy); *file_integrity = false; stop_flag = true; break;
				}
			}
		}

		if (strlen(data->bookBid) == 0) {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
		}
		for (int i = 0; data->bookBid[i]; i++) {
			if (isspace(data->bookBid[i]) && !(data->bookBid[i] == '-' || data->bookBid[i] == ':' || data->bookBid == '.')) {
				add_violation_line(violation_lines, line_copy); *file_integrity = false; stop_flag = true; break;
			}
		}

		if (strlen(data->borrowDate) != 8) {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
		}
		for (int i = 0; i < 8; i++) {
			if (!isdigit(data->borrowDate[i])) {
				add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
			}
		}
		int year = (data->borrowDate[0] - '0') * 1000 + (data->borrowDate[1] - '0') * 100 + (data->borrowDate[2] - '0') * 10 + (data->borrowDate[3] - '0');
		int month = (data->borrowDate[4] - '0') * 10 + (data->borrowDate[5] - '0');
		int day = (data->borrowDate[6] - '0') * 10 + (data->borrowDate[7] - '0');

		if (month < 1 || month > 12) {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
		}

		int days_in_month[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
		if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) days_in_month[1] = 29;

		if (day < 1 || day > days_in_month[month - 1]) {
			add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
		}
		if (strlen(data->returnDate) != 0) {
			if (strlen(data->returnDate) != 8) {
				add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
			}
			for (int i = 0; i < 8; i++) {
				if (!isdigit(data->returnDate[i])) {
					add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
				}
			}
			year = (data->returnDate[0] - '0') * 1000 + (data->returnDate[1] - '0') * 100 + (data->returnDate[2] - '0') * 10 + (data->returnDate[3] - '0');
			month = (data->returnDate[4] - '0') * 10 + (data->returnDate[5] - '0');
			day = (data->returnDate[6] - '0') * 10 + (data->returnDate[7] - '0');

			if (month < 1 || month > 12) {
				add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
			}

			if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) days_in_month[1] = 29;

			if (day < 1 || day > days_in_month[month - 1]) {
				add_violation_line(violation_lines, line_copy); *file_integrity = false; continue;
			}
		}

		if (stop_flag) {
			continue;
		}

		insert_back(list, (void*)data);
	}
	fclose(fp);
	if (*file_integrity) {
		free(violation_lines);
		return list;
	}
	else {
		return violation_lines;
	}

}

void insert_back(linked_list* list, void* data) {
	list->counter++;
	node* new_node = (node*)calloc(1, sizeof(node));
	new_node->data = data;
	if (list->head == NULL) {
		list->head = new_node;
		list->tail = new_node;
	}
	else {
		list->tail->next = new_node;
		new_node->prev = list->tail;
		list->tail = new_node;
	}
}

void insert_front(linked_list* list, void* data, int type) {
	list->counter++;
	node* new_node = (node*)calloc(1, sizeof(node));
	new_node->data = data;
	if (list->head == NULL) {
		list->head = new_node;
		list->tail = new_node;
	}
	else {
		list->head->prev = new_node;
		new_node->next = list->head;
		list->head = new_node;
	}
}

bool check_equality(void* data1, void* data2, int type) {
	switch (type) {
	case 1:
	{
		User* user_data1;
		User* user_data2;
		user_data1 = (User*)data1;
		user_data2 = (User*)data2;
		if (strcmp(user_data1->name, user_data2->name) == 0 && strcmp(user_data1->studentId, user_data2->studentId) == 0 && strcmp(user_data1->password, user_data2->password) == 0) {
			if (user_data1->lendAvailable != user_data2->lendAvailable) return false;
			for (int i = 0; i < 5 - user_data1->lendAvailable; i++) {
				if (strcmp(user_data1->lentBids[i], user_data2->lentBids[i]) != 0) {
					return false;
				}
			}
		}
		else {
			return false;
		}
		break;
	}
	case 2:
	{
		Book* book_data1;
		Book* book_data2;
		book_data1 = (Book*)data1;
		book_data2 = (Book*)data2;
		if (strcmp(book_data1->title, book_data2->title) != 0 || strcmp(book_data1->author, book_data2->author) != 0 || strcmp(book_data1->bid, book_data2->bid) != 0 || book_data1->isAvailable != book_data2->isAvailable) {
			return false;
		}
		break;
	}
	case 3:
	{
		Lend_Return* borrow_data1;
		Lend_Return* borrow_data2;
		borrow_data1 = (Lend_Return*)data1;
		borrow_data2 = (Lend_Return*)data2;
		if (strcmp(borrow_data1->userid, borrow_data2->userid) != 0 || strcmp(borrow_data1->bookBid, borrow_data2->bookBid) != 0 || strcmp(borrow_data1->borrowDate, borrow_data2->borrowDate) != 0 || strcmp(borrow_data1->returnDate, borrow_data2->returnDate) != 0) {
			return false;
		}
		break;
	}
	}
	return true;
}

void* find(linked_list* list, void* data, int type) {
	node* current = list->head;
	while (current != NULL) {
		if (check_equality(current->data, data, type)) {
			return current->data;
		}
		current = current->next;
	}
	return NULL;
}

Book* find_by_bid(linked_list* list, const char* bid) {
	node* current = list->head;
	while (current != NULL) {
		if (strcmp(((Book*)current->data)->bid, bid) == 0) {
			return (Book*)current->data;
		}
		current = current->next;
	}
	return NULL;
}

Book* find_by_title(linked_list* list, const char* title) {
	node* current = list->head;
	while (current != NULL) {
		if (strcmp(((Book*)current->data)->title, title) == 0) {
			return (Book*)current->data;
		}
		current = current->next;
	}
	return NULL;
}

Book* find_by_author(linked_list* list, const char* author) {
	node* current = list->head;
	while (current != NULL) {
		if (strcmp(((Book*)current->data)->author, author) == 0) {
			return (Book*)current->data;
		}
		current = current->next;
	}
	return NULL;
}

User* find_by_userId(linked_list* list, const char* userId) {
	node* current = list->head;
	while (current != NULL) {
		if (strcmp(((User*)current->data)->studentId, userId) == 0) {
			return (User*)current->data;
		}
		current = current->next;
	}
	return NULL;
}

void remove_node(linked_list* list, void* data, int type) {
	User* user_data;
	Book* book_data;
	Lend_Return* borrow_data;
	node* current = list->head;
	switch (type) {
	case 1:
		user_data = (User*)data;
		while (current != NULL) {
			list->counter--;
			if ((User*)current->data == user_data) {
				if (current == list->head) {
					list->head = current->next;
					if (list->head != NULL) {
						list->head->prev = NULL;
					}
				}
				else if (current == list->tail) {
					list->tail = current->prev;
					if (list->tail != NULL) {
						list->tail->next = NULL;
					}
				}
				else {
					current->prev->next = current->next;
					current->next->prev = current->prev;
				}
				free(current);
				break;
			}
			current = current->next;
		}
		break;
	case 2:
		book_data = (Book*)data;
		while (current != NULL) {
			list->counter--;
			if ((Book*)current->data == book_data) {
				if (current == list->head) {
					list->head = current->next;
					if (list->head != NULL) {
						list->head->prev = NULL;
					}
				}
				else if (current == list->tail) {
					list->tail = current->prev;
					if (list->tail != NULL) {
						list->tail->next = NULL;
					}
				}
				else {
					current->prev->next = current->next;
					current->next->prev = current->prev;
				}
				free(current);
				break;
			}
			current = current->next;
		}
		break;
	case 3:
		borrow_data = (Lend_Return*)data;
		while (current != NULL) {
			list->counter--;
			if ((Lend_Return*)current->data == borrow_data) {
				if (current == list->head) {
					list->head = current->next;
					if (list->head != NULL) {
						list->head->prev = NULL;
					}
				}
				else if (current == list->tail) {
					list->tail = current->prev;
					if (list->tail != NULL) {
						list->tail->next = NULL;
					}
				}
				else {
					current->prev->next = current->next;
					current->next->prev = current->prev;
				}
				free(current);
				break;
			}
			current = current->next;
		}
		break;
	}
}

void print_list(linked_list* list, int type) {
	User* user_data;
	Book* book_data;
	Lend_Return* borrow_data;
	char* str;
	node* current = list->head;
	while (current != NULL) {
		if (type == 1) {
			user_data = (User*)current->data;
			printf("%s,%s,%s", user_data->name, user_data->studentId, user_data->password);
			for (int i = 0; i < 5 - user_data->lendAvailable; i++) {
				if (i == 0)printf("%s", user_data->lentBids[i]);
				else printf(";%s", user_data->lentBids[i]);
			}
			if (user_data->lendAvailable == 5) {
				printf(",");
			}
			printf(",%d\n", user_data->lendAvailable);
			printf(",%d\n", user_data->reserveAvailable);
			printf(",%s\n", user_data->studentId);
		}
		else if (type == 2) {
			book_data = (Book*)current->data;
			printf("%s,%s,%s,%d\n", book_data->title, book_data->author, book_data->bid, book_data->isAvailable);
		}
		else if (type == 3) {
			borrow_data = (Lend_Return*)current->data;
			printf("%s,%s,%s,%s\n", borrow_data->userid, borrow_data->bookBid, borrow_data->borrowDate, borrow_data->returnDate);
		}
		else if (type == 4) {
			if (current->data != NULL) {
				str = (char*)current->data;
				printf("%s", str);
			}
		}
		current = current->next;
	}
}