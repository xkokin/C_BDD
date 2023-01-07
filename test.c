#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

#include "bdd.h"

char* functions() {
	BGenerate(15);
	FILE* file = fopen("funcs.txt", "r");
	char** f = (char**)calloc(105, sizeof(char*));
	for (int i = 0; i < 105; i++) {
		f[i] = (char*)calloc(1025, sizeof(char));
		fgets(f[i], 1024, file);
		f[i] = strtok(f[i], "\n");
		
	}
	fclose(file);
	return f;
}


int main() {
	char check;
	char ch;
	int chc = 0;
	bf* func = NULL;
	bdd* head = NULL;
	char** f = functions();

	for (int i = 0; i < 105; i++) {
		printf("%s\n", f[i]);
		unsigned int start_time = clock();

		func = parseFunc(f[i]);
		head = bddCreate(func);
		unsigned int create_time = clock();
		printf("\n");
		unsigned char* con = NULL;

		for (int i = 0; i < pow(2, func->vars); i++) {
			
			con = toBinary(i, func->vars);
			//printf("to binary : %s\n", con);
			check = bddCheck(con, func);
			ch = bddUse(head, con);
			if (check != ch) {
				chc++;//printf("result must be %c, but it is %c\n", check, ch);
				printf("wrong for the %s, it is %c, but it needs to be %c\n", con, ch, check);
			}
			//else//("results are correct\n");
		}
		unsigned int finish_time = clock();
		if (chc != 0) printf("Binary diagram for %s is not correct\n\n", f[i]);
		else printf("Binary diagram for %s is correct\n\n", f[i]);

		printf("Time for creating %d variables diagram: %d;\nTime for checking whole diagram: %d\n\n",head->VarCnt, create_time - start_time, finish_time - create_time);

		chc = 0;
	}

	return 0;
}