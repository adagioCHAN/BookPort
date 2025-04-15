/*파일 입출력 구현*/
#define _CRT_SECURE_NO_WARNINGS
#include "common.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

bool update_file(char* file_name, linked_list* list) {
	FILE* fp = fopen(file_name, "w+");
	User* user_data;
	Book* book_data;
	Lend_Return* borrow_data;
	node* current = list->head;
	while (current != NULL) {
		if (file_name == USER_FILE) {
			user_data = (User*)current->data;
			fprintf(fp, "%s,%s,%s", user_data->name, user_data->studentId, user_data->password);
			for (int i = 0; i < 5 - user_data->lendAvailable; i++) {
				if (i == 0)fprintf(fp, "%s", user_data->lentBids + i);
				else fprintf(fp, ";%s", user_data->lentBids + i);
			}
			fprintf(fp, ",%d\n", user_data->lendAvailable);
		}
		else if (file_name == BOOK_FILE) {
			book_data = (Book*)current->data;
			fprintf(fp, "%s,%s,%s,%d\n", book_data->title, book_data->author, book_data->bid, book_data->isAvailable);
		}
		else if (file_name == LEND_RETURN_FILE) {
			borrow_data = (Lend_Return*)current->data;
			fprintf(fp, "%s,%s,%s,%s,%d\n", borrow_data->userid, borrow_data->bookBid, borrow_data->borrowDate, borrow_data->returnDate, borrow_data->isOverdue);
		}
		current = current->next;
	}
	fclose(fp);
	return true;
}

linked_list* read_user_data() {
	linked_list* list = (linked_list*)calloc(1, sizeof(linked_list));
	FILE* fp = fopen(USER_FILE, "r");
	char line[1 << 10];
	char* saveptr1, saveptr2;
	if (fp == NULL) {
		fp = fopen(USER_FILE, "w+");
		fclose(fp);
		return list;
	}
	while (fgets(line, sizeof(line), fp)) {
		User* data = (User*)calloc(1, sizeof(User));
		line[strcspn(line, "\n")] = 0;
		char* token = strtok(line, ",");
		strcpy(data->name, token);
		token = strtok(NULL, ",");
		strcpy(data->studentId, token);
		token = strtok(NULL, ",");
		strcpy(data->password, token);
		token = strtok(NULL, ",");
		int idx = 0;
		char tmp[1 << 10];
		strcpy(tmp, token);
		token = strtok(NULL, ",");
		char* bid = strtok(tmp, ";");
		while (bid != NULL) {
			strcpy(data->lentBids[idx], bid);
			bid = strtok(NULL, ";");
			idx++;
		}
		data->lendAvailable = token[0] - '0';
		insert_back(list, (void*)data);
	}
	fclose(fp);
	return list;
}

linked_list* read_book_data() {
	linked_list* list = (linked_list*)calloc(1, sizeof(linked_list));
	FILE* fp = fopen(BOOK_FILE, "r");
	char line[1 << 10];
	if (fp == NULL) {
		fp = fopen(BOOK_FILE, "w+");
		fclose(fp);
		return list;
	}
	while (fgets(line, sizeof(line), fp)) {
		Book* data = (Book*)calloc(1, sizeof(Book));
		line[strcspn(line, "\n")] = 0;
		char* token = strtok(line, ",");
		strcpy(data->title, token);
		token = strtok(NULL, ",");
		strcpy(data->author, token);
		token = strtok(NULL, ",");
		strcpy(data->bid, token);
		token = strtok(NULL, ",");
		data->isAvailable = token[0] - '0';
		insert_back(list, (void*)data);
	}
	fclose(fp);
	return list;
}

linked_list* read_borrow_data() {
	linked_list* list = (linked_list*)calloc(1, sizeof(linked_list));
	FILE* fp = fopen(LEND_RETURN_FILE, "r");
	char line[1 << 10];
	if (fp == NULL) {
		fp = fopen(LEND_RETURN_FILE, "w+");
		fclose(fp);
		return list;
	}
	while (fgets(line, sizeof(line), fp)) {
		Lend_Return* data = (Lend_Return*)calloc(1, sizeof(Lend_Return));
		line[strcspn(line, "\n")] = 0;
		char* token = strtok(line, ",");
		strcpy(data->userid, token);
		token = strtok(NULL, ",");
		strcpy(data->bookBid, token);
		token = strtok(NULL, ",");
		strcpy(data->borrowDate, token);
		token = strtok(NULL, ",");
		strcpy(data->returnDate, token);
		token = strtok(NULL, ",");
		data->isOverdue = token[0] - '0';
		insert_back(list, (void*)data);
	}
	fclose(fp);
	return list;
}

void insert_back(linked_list* list, void* data) {
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

void* find(linked_list* list, void* data, int type) {
	User* user_data;
	Book* book_data;
	Lend_Return* borrow_data;
	node* current = list->head;
	switch (type) {
	case 1:
		user_data = (User*)data;
		while (current != NULL) {
			if ((User*)current->data == user_data) {
				return current->data;
			}
			current = current->next;
		}
		break;
	case 2:
		book_data = (Book*)data;
		while (current != NULL) {
			if ((Book*)current->data == book_data) {
				return current->data;
			}
			current = current->next;
		}
		break;
	case 3:
		borrow_data = (Lend_Return*)data;
		while (current != NULL) {
			if ((Lend_Return*)current->data == borrow_data) {
				return current->data;
			}
			current = current->next;
		}
		break;
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
	node* current = list->head;
	while (current != NULL) {
		if (type == 1) {
			user_data = (User*)current->data;
			printf("%s,%s,%s", user_data->name, user_data->studentId, user_data->password);
			for (int i = 0; i < 5 - user_data->lendAvailable; i++) {
				if (i == 0)printf("%s", user_data->lentBids[i]);
				else printf(";%s", user_data->lentBids[i]);
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
		current = current->next;
	}
}