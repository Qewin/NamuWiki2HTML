#include <stdio.h>
#include "stdnamu.h"

#define CORE 3 //코어 수 - 1 
#define Csize  CORE*50*1000*1000

int ReadJSON(FILE *input){ //[1~] : 각 문서의 포인터 반환. [0] : 원래 문서의 포인터가 들어 있음.(2번째 로딩부터는 사용.)
	char *cache;
	int *document;
	if( (cache = (char *)malloc(Csize)) == NULL) return NULL;
	if( (document = (int *)malloc(sizeof(int)*500000)) == NULL) return NULL;
	int iv, a;
	for (a = iv = 0; (Csize - a) >10000; iv++) {
		document[iv] = &cache[a]; //{"n 에서 n이 반환됨. 
		while( (cache[a++] = getc(input)) != '\"' ) while( (cache[a++] = getc(input)) != '{' ); // EOF 도. feof();
	} // 마지막은 기록 안됨. 
	document[iv+1] = '\0'; 
	printf("%d,%d",iv,a); //for(a = 0; a < iv;a++)printf("%d,",document[a]);
	return document;
}


int JsonIO(){
	//구조를 하나의 큰 malloc에서 나머지로 편성하는 방법 사용하기 
	FILE *input;
	if((input = fopen("namu.json","r")) == NULL) return 1;
	int *dptr;
	if( (dptr = ReadJSON(input) ) == NULL) return 2;
	dptr = worker(dptr);
	
	//워커 쓰레드 만들기 
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
	
	if (IOret == 1) printf("덤프 파일이 없거나 손상되었습니다.");
	return IOret;
}
