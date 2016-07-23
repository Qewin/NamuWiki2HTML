#include <stdio.h>
#include "stdnamu.h"

#define CORE 3 //�ھ� �� - 1 
#define Csize  CORE*50*1000*1000

int ReadJSON(FILE *input){ //[1~] : �� ������ ������ ��ȯ. [0] : ���� ������ �����Ͱ� ��� ����.(2��° �ε����ʹ� ���.)
	char *cache;
	int *document;
	if( (cache = (char *)malloc(Csize)) == NULL) return NULL;
	if( (document = (int *)malloc(sizeof(int)*500000)) == NULL) return NULL;
	int iv, a;
	for (a = iv = 0; (Csize - a) >10000; iv++) {
		document[iv] = &cache[a]; //{"n ���� n�� ��ȯ��. 
		while( (cache[a++] = getc(input)) != '\"' ) while( (cache[a++] = getc(input)) != '{' ); // EOF ��. feof();
	} // �������� ��� �ȵ�. 
	document[iv+1] = '\0'; 
	printf("%d,%d",iv,a); //for(a = 0; a < iv;a++)printf("%d,",document[a]);
	return document;
}


int JsonIO(){
	//������ �ϳ��� ū malloc���� �������� ���ϴ� ��� ����ϱ� 
	FILE *input;
	if((input = fopen("namu.json","r")) == NULL) return 1;
	int *dptr;
	if( (dptr = ReadJSON(input) ) == NULL) return 2;
	dptr = worker(dptr);
	
	//��Ŀ ������ ����� 
	free (dptr[0]);
	
	//send to pointer
	//while all the pointer is not null : 
	// until 40% of memory left
	//  read near 50MB
	//  find title, namespace, document
	//start new parse thread
	return 0;
}

int worker(int *doc){
	int *Cdoc;
	if( (Cdoc = (int *)malloc(sizeof(int)*500000)) == NULL) return NULL;
	int a, i;
	for (a=0; doc[a] == '/0'; a++);
	#pragma omp parallel for
	for (i=0; i < a; i++) Cdoc[i] = doc[i];
	return Cdoc;
}

int main(){
	//JsonIO
	int IOret = JsonIO();
	//start worker - (core - 1) threads
	
	if (IOret == 1) printf("���� ������ ���ų� �ջ�Ǿ����ϴ�.");
	return IOret;
}
