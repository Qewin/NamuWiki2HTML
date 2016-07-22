#include <stdio.h>
#include "stdnamu.h"

#define CORE 3 //코어 수 - 1 
#define Csize  CORE*50*1000*1000
//#define forcores for(short i = 0; i<CORE; i++) // 코어 수에 맞춰 호출 

int JsonIO(){
	//구조를 하나의 큰 malloc에서 나머지로 편성하는 방법 사용하기 
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
		while( (cache[a] = getc(input)) != '}' ) a++; // EOF 도. feof();
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
	
	if (IOret == 1) printf("덤프 파일이 없거나 손상되었습니다.");
	return IOret;
}
