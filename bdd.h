
typedef struct BF {					//bf = booleovska funkcia
	unsigned char* starter;			//starter - sequence znakov vstupnej funkcie
	unsigned char* seq;				//vystupny vektor
	int vars;						//pocet premennych funkcie
	unsigned char* Vname;			//array z menami tych premennych

}bf;
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

void BGenerate(int variables);
bf* parseFunc(unsigned char* starter);
bdd* bddCreate(bf* func);
char bddCheck(unsigned char* values, bf* func);
char bddUse(bdd* head, unsigned char* values);
unsigned char* toBinary(int num, int v);
