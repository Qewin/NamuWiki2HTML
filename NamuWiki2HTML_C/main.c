#include <stdio.h>
#include "stdnamu.h"

#define CORE 3 //�ھ� �� - 1 
#define Csize  CORE*50*1000*1000
//#define forcores for(short i = 0; i<CORE; i++) // �ھ� ���� ���� ȣ�� 

int JsonIO(){
	//������ �ϳ��� ū malloc���� �������� ���ϴ� ��� ����ϱ� 
	FILE *input;
	if((input = fopen("namu.json","r")) == NULL) return 1;
	char *cache;
	if((cache = (char *)malloc(Csize)) == NULL) return 2;
	//read near 50MB * (core - 1) (maybe 4000 documents?)
	int document[500000];
	int iv, a;
	a = 0;
	
	document[0] = &cache[0];
	for (iv = 1; (Csize - (&cache[a] - cache)) >10000; iv++) {
		while( (cache[a] = getc(input)) != '}' ) a++; // EOF ��. feof();
		a++; 
		document[iv] = &cache[a];
	}
	document[iv+1] = '\0';
	//printf("%s\n",cache);
	//printf("%d",(&cache[5]-cache));
	printf("%d",iv);
	scanf("%d",&a);
	//send to pointer
	//while all the pointer is not null : 
	// until 40% of memory left
	//  read near 50MB
	//  find title, namespace, document
	//start new parse thread
	free (cache);
	return 0;
}

int ParseManager(){
	//
	
	return 0;
}

int main(){
	//JsonIO
	int IOret = JsonIO();
	//start worker - (core - 1) threads
	
	if (IOret == 1) printf("���� ������ ���ų� �ջ�Ǿ����ϴ�.");
	return IOret;
}
