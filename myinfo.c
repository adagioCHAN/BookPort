#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"

char* get_myinfo_canonical_command(char* input) {
	struct {
		const char* synonyms[20];
		const char* canonical;
	} myinfo_table[] = {
		{ { "withdraw", "withdra", "withdr", "withd", "with", "wit", "wi", "w"}, "withdraw"},
		{ { "change", "chang", "chan", "cha", "ch", "c"}, "change"},
		{ { "manage", "manag", "mana", "man", "ma", "m"}, "manage"}
	};

	for (int i = 0; i < sizeof(myinfo_table) / sizeof(myinfo_table[0]); i++) {
		for (int j = 0; myinfo_table[i].synonyms[j]; j++) {
			if (strcmp(input, myinfo_table[i].synonyms[j]) == 0)
				return (char*)myinfo_table[i].canonical;
		}
	}
	return NULL;
}

char* get_manage_canonical_command(char* input) {
	struct {
		const char* synonyms[20];
		const char* canonical;
	} manage_table[] = {
		{ { "list", "lis", "li", "l"}, "list"},
		{ { "record", "recor", "reco", "rec", "re", "r"}, "record"}
	};

	for (int i = 0; i < sizeof(manage_table) / sizeof(manage_table[0]); i++) {
		for (int j = 0; manage_table[i].synonyms[j]; j++) {
			if (strcmp(input, manage_table[i].synonyms[j]) == 0)
				return (char*)manage_table[i].canonical;
		}
	}
	return NULL;
}

typedef struct {
	char date[9];
	char type[8];
	char bid[MAX_BID];
} RecordEntry;

int compare_record_entry(const void* a, const void* b) {
	return strcmp(((RecordEntry*)a)->date, ((RecordEntry*)b)->date);
}

void run_record() {
	RecordEntry* user_record = (RecordEntry*)malloc(2 * lend_return_count * sizeof(RecordEntry)); //각 파일 linecount, load필요
	int user_record_count = 0;
	for (int i = 0; i < lend_return_count; i++) {
		if (strcmp(current_user.studentId, Lend_Return[i].userid) == 0) {	
			strcpy(user_record[user_record_count].date, Lend_Return[i].borrowDate);
			strcpy(user_record[user_record_count].type, "borrow");
			strcpy(user_record[user_record_count].bid, Lend_Return[i].bookBid);
			user_record_count++;
			if (Lend_Return[i].returnDate[0] != '\0') {
				strcpy(user_record[user_record_count].date, Lend_Return[i].returnDate);
				strcpy(user_record[user_record_count].type, "return");
				strcpy(user_record[user_record_count].bid, Lend_Return[i].bookBid);
				user_record_count++;
			}
		}
	}
	qsort(user_record, user_record_count, sizeof(RecordEntry), compare_record_entry);
	if(user_record_count>0) printf("=> ");
	for (int i = 0; i < user_record_count; i++) {
		char* d = user_record[i].date;
		printf("[%s] %s %c%c/%c%c/%c%c\n",
			user_record[i].type,
			user_record[i].bid,
			d[2], d[3], d[4], d[5], d[6], d[7]);
	}
	free(user_record);
}

void run_manage() {
	char input[100];

	while (1) {
		printf("BookPort: My info - Enter command>");
		if (!fgets(input, sizeof(input), stdin)) break;
		trim(input);

		char* cmd = get_manage_canonical_command(input);
		if (!cmd) {
			printf(".!! Error: Wrong command entered\n");
			continue;
		}

		if (strcmp(cmd, "list") == 0) {
			for (int i = 0; i < 5; i++) {
				if (strlen(current_user.lentBids[i]) == 0) continue;

				for (int j = 0; j < bookcount; j++) {
					if (strcmp(Book[j].bookBid, current_user.lentBids[i]) == 0) {
						printf("Title: %s\n", Book[j].title);
						printf("Author: %s\n", Book[j].author);
						printf("BID: %s\n", Book[j].bid);
					}
				}
			}
			break;
		}
		else if (strcmp(cmd, "record") == 0) {
			run_record();
			break;
		}
	}
	return;
}

void run_myinfo() {
	if (is_logged_in == 1) {
		printf("[My Information]\n");
		printf("Name: %s\n", current_user.name);
		printf("ID: %s\n", current_user.studentId);

		char input[100];

		while (1) {
			printf("BookPort: My info - Enter command>");
			if(!fgets(input, sizeof(input), stdin)) break;
			trim(input);

			char* cmd = get_myinfo_canonical_command(input);
			if (!cmd) {
				printf(".!! Error: Wrong command entered\n");
				continue;
			}

			if (strcmp(cmd, "withdraw") == 0) {
				run_withdraw();
				break;
			}
			else if (strcmp(cmd, "change") == 0) {
				run_change();
				break;
			}
			else if(strcmp(cmd, "manage") == 0) {
				run_manage();
				break;
			}
		}
	}
	else {
		run_login();
	}
	return;
}
