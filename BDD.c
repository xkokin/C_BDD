#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

typedef struct BDD {
	unsigned char index;
	int VarCnt;				//pocet premennych z booleovskej funkcie
	int* NodeCnt;
	int layer;				//hlbka polozky
	int isRight;			//identifikator ci je odkaz pravym alebo l'avym odkazom 

	struct BDD* parent;		//ukazovatel na parent
	struct BDD* right;		//ukazovatel na praveho syna
	struct BDD* left;		//ukazovatel na l'aveho syna
}bdd;

typedef struct BF {					//bf = booleovska funkcia
	unsigned char* starter;			//starter - sequence znakov vstupnej funkcie
	unsigned char* seq;				//vystupny vektor
	int vars;						//pocet premennych funkcie
	unsigned char* Vname;			//array z menami tych premennych

}bf;

typedef struct TABLE {			//tb = table pre parsing funkcie na vektor
	unsigned char* seq;
	unsigned char* index;
}tb;

bf* parseFunc(unsigned char* starter) {
	int sl = strlen(starter);	//sl = starter length
	int vars = 0;
	unsigned char* varsName = (unsigned char*)calloc(32, sizeof(unsigned char));
	bf* func = (bf*)calloc(1, sizeof(bf));

	func->starter = (unsigned char*)calloc(sl+1, sizeof(unsigned char));
	strcpy(func->starter, starter);
	func->Vname = varsName;
		
	for (int i = 0; i < strlen(starter); i++) {		//najprv zapiseme vsetky premenne a ich pocet
		int rep = 0;
		if (isalpha(starter[i])) {
			for (int v = 0; v < vars; v++) {
				if (starter[i] == varsName[v]) {
					rep ++;
				}
			}
			if (rep == 0) {
				vars++;
				varsName[vars - 1] = starter[i];
			}
			else {
				rep--;
			}
			
		}
	}
	int sym = (int)pow(2, vars);				//sym - pocet odkazov posledneho layera alebo pocet symbolov vektora
	tb* table = (tb*)calloc(sl, sizeof(tb));
	for (int p = 0; p < vars; p++) {
		unsigned char* temp = (unsigned char*)calloc(sym+1, sizeof(unsigned char));
		for (int r = 0; r < pow(2, p); r++) {	//zgenerujeme tabul'ku hodnot pre premenne 
			for (int v = 0; v < (sym / pow(2, p)); v++) {
				if (v < ((sym / pow(2, p)) / 2)) {
					temp =strcat(temp, "0");
				}
				else {
					temp =strcat(temp, "1");
				
				}
			}
		}
		//printf("\n");
		table[p].seq = (unsigned char*)calloc((sym+1), sizeof(unsigned char));
		table[p].index = (unsigned char*)calloc((sl+1), sizeof(unsigned char));
		strcpy(table[p].seq, temp);
		table[p].index[0] = varsName[p];
		//printf("%s:\t%s", table[p].index, table[p].seq);

		free(temp);
	}

	int caps=0;				//pocet zatvoriek
	int curPos = vars-1;		//pozicia v tabule
	int plus = 0;			//pocet plusov 
	int neg = 0;			//identifikator pre negaciu

	//kazdu zatvorku budeme riesit v novej polozke tabuli,
	//budeme posuvat dalej prave vtedy, ked uvidime novu zatvorku '('
	//potom, ak dojdeme do konca funkcie tak urobime '+' pre kazdu polozku tiej tabuli z zatvorkami

	for (int i = 0; i < strlen(starter); i++) {
		if (starter[i] == '.') {
			int s = 0;
			if (starter[i + 1] == '!') {		//ak stretneme sa z '!' tak vymenime hodnoty urcitej premennej 
												//urobime vypocet a potom vratime hodnoty spat
				while (starter[i + 2] != table[s].index[0]) {
					s++;
				}
				for (int i = 0; i < sym; i++) {
					if (table[s].seq[i] == '1')table[s].seq[i] = '0';
					else table[s].seq[i] = '1';
				}
			}
			//------------------------------------------
			else {
				while (starter[i + 1] != table[s].index[0]) {
					s++;
				}
			}
			strcat(table[curPos].index, ".");
			strcat(table[curPos].index, table[s].index);
			for (int p = 0; p < sym; p++) {
				if (((table[curPos].seq[p]) == (table[s].seq[p])) && ((table[curPos].seq[p]) != '0')) {
					table[curPos].seq[p] = '1';
				}
				else table[curPos].seq[p] = '0';
			}
			if (starter[i + 1] == '!') {
				for (int v = 0; v < sym; v++) {
					if (table[s].seq[v] == '1')table[s].seq[v] = '0';
					else table[s].seq[v] = '1';
				}
			}
			
		}
		
		else if (starter[i] == '(') {
			int s = 0;
			caps++;
			curPos++;
			if (isalpha(starter[i+1])) {
				while (starter[i + 1] != varsName[s]) {
					s++;
				}
				table[curPos].seq = (unsigned char*)calloc(sym+1, sizeof(unsigned char));
				table[curPos].index = (unsigned char*)calloc(sl+1, sizeof(unsigned char));
				strcpy(table[curPos].seq, table[s].seq);
				strcpy(table[curPos].index, table[s].index);
			}
			else if (isalpha(starter[i + 2]) && starter[i + 1] == '!') {
				while (starter[i + 2] != varsName[s]) {
					s++;
				}
				table[curPos].seq = (unsigned char*)calloc(sym+1, sizeof(unsigned char));
				table[curPos].index = (unsigned char*)calloc(sl+1, sizeof(unsigned char));
				for (int k = 0; k < sym; k++) {
					if (table[s].seq[k] == '0')table[curPos].seq[k] = '1';
					else table[curPos].seq[k] = '0';
				}
				strcpy(table[curPos].index, table[s].index);
			}
			
		}
		else if (starter[i] == '!') {
			if (starter[i + 1] == '(') neg++;
		}
		else if (starter[i] == ')') {
			if (neg != 0) {
				for (int i = 0; i < sym; i++) {
					if (table[curPos].seq[i] == '1') table[curPos].seq[i] = '0';
					else if (table[curPos].seq[i] == '0') table[curPos].seq[i] = '1';
				}
				neg--;
			}
		}

		else if (starter[i] == '+') {
			plus++;
			
		}
	}

	if (plus != 0) {
		curPos++;
		table[curPos].seq = (unsigned char*)calloc(sym+1, sizeof(unsigned char));
		table[curPos].index = (unsigned char*)calloc(sl+1, sizeof(unsigned char));
		strcpy(table[curPos].seq, table[vars].seq);
		strcpy(table[curPos].index, starter);
		for (int i = vars + 1; i < curPos; i++) {
			for (int p = 0; p < sym; p++) {
				if ((table[curPos].seq[p]) == '1' || (table[i].seq[p]) == '1') {
					table[curPos].seq[p] = '1';
				}
				else {
					table[curPos].seq[p] = '0';
				}
			}
		}
	}

	//printf("\n%s parse: %s\n", table[curPos].index, table[curPos].seq);
	func->vars = vars;
	func->seq = (unsigned char*)calloc(sym, sizeof(unsigned char));
	strcpy(func->seq, table[curPos].seq);
	
	for (int i = 0; i < curPos; i++) {
		free(table[i].seq);
		free(table[i].index);

	}
	free(table);
	return func;
}

bdd* bddInit(bdd* head,bdd* one, bdd* zero, int vars, int cnt, int* red, bdd* parent, int right,unsigned char* seq, int* pos) {
	if (cnt == vars) {		// posledne odkazy nastavime na odkazy '0' alebo '1' podla vektora a ho pozicii
							//init je rekurzivne spraveni tak kazdy odkaz bude nastaveny postupne
		if (seq[*pos] == '1') {
			(*pos) ++;
			return one;
		}
		else if (seq[*pos] == '0') {
			(*pos) ++;
			return zero;
		}
	}
	if (head == NULL) {
		head = (bdd*)calloc(1, sizeof(bdd));
		head->index = '2';
		head->parent = parent;
		head->isRight = right;
		cnt++;
		//printf("node has been created\n");
	}
	head->VarCnt = vars; 
	head->NodeCnt = red;
	head->left = bddInit(head->left,one,zero, vars, cnt, red, head, 0,seq, pos);
	head->right = bddInit(head->right,one,zero, vars, cnt, red, head, 1,seq, pos);
	return head;

}

void bddReduceR(bdd* head, int* cnt);

bdd* bddCreate(bf* func) {
	int red = 0;
	float percents = 0;
	int pos = 0;
	//printf("func vars %d\n", func->vars);

	bdd* head = NULL;
	bdd* one = (bdd*)calloc(1, sizeof(bdd));
	one->index = '1';
	one->left = one->right = NULL;
	bdd* zero = (bdd*)calloc(1, sizeof(bdd));
	zero->index = '0';
	zero->left = zero->right = NULL;
	
	//najprv vytvorime ten strom
	head = bddInit(head,one,zero, func->vars, 0, &red, NULL, -1, func->seq, &pos);
	red = (pow(2, head->VarCnt) + 1);
	percents = red;
	printf("amount of nodes = %d\n", red);
		
	if (head != NULL) printf("Diagram for %d variables has been created\n", head->VarCnt);
	//potom reduce
	bddReduceR(head, &red);
	percents = 100.0-((red / percents) * 100);
	printf("reduced to %d nodes\n", red);
	printf("reduction percents: %.2f\n", percents);

	return head;
}

int compareNodes(bdd* head) {
	int cnt = 0;
	if (head->left == head->right) {
		//printf("pair detected: sons index is %c\n", head->left->index);
		if (head->isRight == 1) {
			head->parent->right = head->left;
			//printf("pair detected; reducing right \n");
			free(head);
			cnt++;
		}
		else if(head->isRight == 0) {
			head->parent->left = head->left;
			//printf("pair detected; reducing left\n");
			free(head);
			cnt++;
		}
	}
	return cnt;
}


void bddReduceR(bdd* head, int* cnt){
	//ak sme dostali do tych poslednych odkazov tak dalej uz nic nemame redukovat
	if (head->index == '1' || head->index == '0') return;

	if (head->right == head->left) {
		int t = compareNodes(head);		
		(*cnt) -= t; 
		return;
	}
	else {
		bddReduceR(head->left, cnt);	//ak nenasli par rovnakych synov - rekurzivne pokrocujeme
		bddReduceR(head->right, cnt);
		int t = compareNodes(head);
		(*cnt) -= t;
		return;
	}
}

char bddCheck(unsigned char* values, bf* func) {
	//spraveny rovnako ako parsing, ale najprv vymenime premenny na ich hodnoty

	int plus = 0;
	int curPos = -1;
	int bod = 0;
	int neg = 0;

	unsigned char* starter = (unsigned char*)calloc(strlen(func->starter)*2, sizeof(unsigned char));
	strcpy(starter, func->starter);
	tb* table = (tb*)calloc(strlen(func->seq)*2, sizeof(tb));
	for (int i = 0; i < strlen(func->Vname); i++) {
		for (int v = 0; v < strlen(starter); v++) {
			if (starter[v] == func->Vname[i]) {
				starter[v] = values[i];
			}
		}
	}
	//printf("new starter is %s\n", starter);
	for (int i = 0; i < strlen(starter);i++) {
		if (starter[i] == '(') {
			curPos++;
			if(isdigit(starter[i+1])) table[curPos].seq = starter[i + 1];
			else if (starter[i + 1] == '!') {
				if (starter[i + 2] == '0')table[curPos].seq = '1';
				else table[curPos].seq = '0';
			}
		}

		if (starter[i] == '!') {
			if (starter[i + 1] == '(')neg++;
		}
		if (starter[i] == ')') {
			if (neg != 0) {
				if (table[curPos].seq == '1') table[curPos].seq = '0';
				else table[curPos].seq = '1';
				neg--;
			}
		}
		if (starter[i] == '.') {
			
			if (starter[i + 1] == '!') {
				if (starter[i +2] == '1')starter[i +2] = '0';
				else starter[i +2] = '1';

				if (table[curPos].seq == '1' && starter[i + 2] == '1') table[curPos].seq = '1';
				else table[curPos].seq = '0';

				if (starter[i + 2] == '1')starter[i + 2] = '0';
				else starter[i + 2] = '1';
			}			
			else {
				if (table[curPos].seq == '1' && starter[i + 1] == '1') table[curPos].seq = '1';
				else table[curPos].seq = '0';
			}
		}
		if (starter[i] == '+') {
			plus++;
		}
	}
	if (plus != 0) {
		curPos++;
		
		table[curPos].seq = table[0].seq;

		for (int i = 1; i < curPos; i++) {
			if ((table[curPos].seq) == '1' || (table[i].seq) == '1') {
				table[curPos].seq = '1';
			}
			else table[curPos].seq = '0';
		}
	}
	char res;
	res = table[curPos].seq;
	free(starter);
	
	//printf("for values %s result from function is %c\n", values, res);
	free(table);
	return res;
}
char bddUse(bdd* head, unsigned char* values) {
	bdd* temp = head;
	for (int j = 0; j < strlen(values); j++) {
		if (temp->left == NULL)break;
		if (values[j] == '0')temp = temp->left;
		else if (values[j] == '1')temp = temp->right;
	}
	return temp->index;
}
unsigned char* toBinary(int num, int v) {
	unsigned char* res = (unsigned char*)calloc(v, sizeof(unsigned char));
	if (num > pow(2, v)) {
		printf("number is out of limits\n");
		return;
	}
	else {
		for (int i = 1; i <= v; i++) {
			if (num % 2 == 1)res[v - i] = '1';
			else if (num % 2 == 0)res[v - i] = '0';
			num /= 2;
		}
		return res;
	}
}

void BGenerate(int variables) {
	FILE* file = fopen("funcs.txt", "w");
	srand(time(0));

	for (int i = 1; i <= variables; i++) {
		for (int l = 1; l <= 7; l++) {
			int zat;
			if (i <= 6)zat = 1 + (rand() % i);
			else zat = 4 + (rand() % 4);
			
			unsigned char* func = (unsigned char*)calloc(1024, sizeof(unsigned char));
			int p = 0;
			int check = 0;
			for (int v = 0; v < zat; v++) {
				if (rand() % 3 == 0) {
					func[p] = '!';
					p++;
				}
				func[p] = '(';
				p++;
				int alphaCnt;
				if (zat <= 3)alphaCnt = (abs(i - zat) + 1) + (rand() % i);
				else alphaCnt = 1+(rand() % (i/2));
				for (int c = 0; c < alphaCnt; c++) {

					if (check < i) {
						if (rand() % 7 == 0) {
							func[p] = '!';
							p++;
						}
						func[p] = 65 + check;
						p++;
						if ((c + 1) != alphaCnt) {
							func[p] = '.';
							p++;
						}
						check++;
					}
					else if (check>=i) {
						if (rand() % 7 == 0) {
							func[p] = '!';
							p++;
						}
						func[p] = 65 + (rand() % i);
						p++;
						if ((c + 1) != alphaCnt) {
							func[p] = '.';
							p++;
						}
					}
				}
				func[p] = ')';
				p++;
				if ((v + 1) != zat) {
					func[p] = '+';
					p++;
				}
			}

			fprintf(file, "%s\n", func);
			printf("pocet premennych: %d\t%s\n", i, func);
			free(func);
		}
	}
	fclose(file);
	return;
}
