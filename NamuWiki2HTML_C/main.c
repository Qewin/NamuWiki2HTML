#include <stdio.h>
#include "stdnamu.h"

#define CORE 3 //코어 수 - 1 
#define Csize  50*1000*1000
#define forcores for(short i = 0; i<CORE; i++) // 코어 수에 맞춰 호출 

int JsonIO(){
	FILE *input;
	if((input = fopen("namu.json","r")) == NULL) return 1;
	char *cache[CORE];
	forcores if((cache[i] = (char *)malloc(Csize)) == NULL) return 2;
	//read near 50MB * (core - 1) (maybe 4000 documents?)
	forcores printf("%d\n",cache[i]);
	forcores{
		for(short w = 0; w<1000; w++){
			for(int k = 0 ; cache[i][k-1] != '}' ; k++) {
				cache[i][k] = getc(input);
			}
			//포인터 기록. 
		}
	}
	int a;
	scanf("%d",&a);
	forcores printf("%s",cache[i]);
	//send to pointer
	//while all the pointer is not null : 
	// until 40% of memory left
	//  read near 50MB
	//  find title, namespace, document
	//start new parse thread
	forcores free (cache[i]);
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
