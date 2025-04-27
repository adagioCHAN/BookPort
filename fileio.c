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
			fprintf(fp, ",%d\n", user_data->lendAvailable);
		}
		else if (strcmp(file_name, BOOK_FILE) == 0) {
			book_data = (Book*)current->data;
			fprintf(fp, "%s,%s,%s,%c\n", book_data->title, book_data->author, book_data->bid, book_data->isAvailable);
		}
		else if (strcmp(file_name, LEND_RETURN_FILE) == 0) {
			borrow_data = (Lend_Return*)current->data;
			fprintf(fp, "%s,%s,%s,%s,%c\n", borrow_data->userid, borrow_data->bookBid, borrow_data->borrowDate, borrow_data->returnDate, borrow_data->isOverdue);
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
		if (token == NULL) { add_violation_line(violation_lines, line_copy); *file_integrity = false; continue; }
		data->isOverdue = token[0];
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
		if (strcmp(borrow_data1->userid, borrow_data2->userid) != 0 || strcmp(borrow_data1->bookBid, borrow_data2->bookBid) != 0 || strcmp(borrow_data1->borrowDate, borrow_data2->borrowDate) != 0 || strcmp(borrow_data1->returnDate, borrow_data2->returnDate) != 0 || borrow_data1->isOverdue != borrow_data2->isOverdue) {
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
		}
		else if (type == 2) {
			book_data = (Book*)current->data;
			printf("%s,%s,%s,%d\n", book_data->title, book_data->author, book_data->bid, book_data->isAvailable);
		}
		else if (type == 3) {
			borrow_data = (Lend_Return*)current->data;
			printf("%s,%s,%s,%s,%d\n", borrow_data->userid, borrow_data->bookBid, borrow_data->borrowDate, borrow_data->returnDate, borrow_data->isOverdue);
		}
		else if (type == 4) {
			str = (char*)current->data;
			printf("%s", str);
		}
		current = current->next;
	}
}