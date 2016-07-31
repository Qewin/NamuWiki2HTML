#include "stdnamu.h"
#include <windows.h>

string ReadJSON(FILE *input){ //[1~] : �� ������ ������ ��ȯ. [0] : ���� ������ �����Ͱ� ��� ����.(2��° �ε����ʹ� ���.)
	unsigned char *cache;
	unsigned char *document;
	if( (cache = (unsigned char **)malloc(Csize)) == NULL) return (string){0,0};
	if( (document = (unsigned char **)malloc(sizeof(int)*500000)) == NULL) return (string){0,0};
	int iv, a;
	for (a = iv = 0; (Csize - a) >10000; iv++) {
		document[iv] = &cache[a]; //{"n ���� n�� ��ȯ��. 
		while( (cache[a++] = getc(input)) != '\"' ) while( (cache[a++] = getc(input)) != '{' ); // EOF ��. feof();
	} // �������� ��� �ȵ�. 
	string doc = {document,(iv-1)};
	printf("%d,%d,%d\n",document,document[1],iv);
	return doc;
}

int *worker(string doc){
	printf("\n%d,%d\n",doc.p,doc.p[1]);
	int full, i;
	printf("%d",doc.len);
	int *Cdoc; //��ȯ �������� ������ ���� 
	if( (Cdoc = (int *)malloc(sizeof(int) * full)) == NULL) return NULL;
	full--;
	#pragma omp parallel for
	for (i=0; i < full; i++) {//namespace = (char)-48, 
			unsigned char *temp = (unsigned char *)doc.p[i];
			if(temp[0] != 'n')continue;
			int read = 24;
			while( temp[read++] != '\"' ) while( temp[read++] != ',' );
			string title = {&temp[24] , ((read-4)-24)};
			int read2 = doc.p[i+1] - doc.p[i];
			while( temp[read2--] != '[' )( temp[read2--] != '\"' );
			string text ={&temp[read+7] , ((read2-17) - (read+7))};
			
			string conv = parse((int)(temp[12]-48),title,text);
			 //printf("(%c,%d,%c%c,%c)",temp[0],(temp[12]-48),temp[24],temp[25],temp[read+7]);
			 //parse((int)(temp[12]-48),&temp[24],&temp[read+7],Cdoc[i]);//namespace,title,text,cdocpointer
			free(conv.p);
			
			for (read = 0; read < title.len ;read++)printf("%c",temp[24+read]);
			printf("\n");
			//unsigned char* ttl = title.p;
			//for (read = 0; read < title.len ;read++)printf("%c",ttl[read]);
			//if(&temp[24] != &title.p[0]) printf("aaa\n");
			printf("\n");
		}
	return Cdoc;
}


int JsonIO(){
	FILE *input;
	if((input = fopen("namu.json","r")) == NULL) return 1;
	string doc;
	int *cptr;
	doc = ReadJSON(input);
	if( (cptr = worker(doc)) == NULL) return 3;
	free ((void*)doc.p[0]);
	free ((void*)cptr);
	
	//send to pointer
	//while all the pointer is not null : 
	// until 40% of memory left
	//  read near 50MB
	//  find title, namespace, document
	//start new parse thread
	return 0;
}

int main(){
	system("chcp 65001");
	//JsonIO
	int IOret = JsonIO();
	//start worker - (core - 1) threads
	
	if (IOret == 1) printf("���� ������ ���ų� �ջ�Ǿ����ϴ�.");
	return IOret;
}
