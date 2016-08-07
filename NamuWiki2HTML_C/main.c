#include "stdnamu.h"
#include <pthread.h> 

#define DocS 125000*CORE

pthread_t threads[CORE];

int ReadJSON(FILE *input, unsigned char *cache, unsigned char **document){ //[1~] : 각 문서의 포인터 반환. [0] : 원래 문서의 포인터가 들어 있음.(2번째 로딩부터는 사용.)
	printf("Reading             \r");
	int iv, a;
	for (a = iv = 0; (Csize - 15000000) > a && iv != DocS; iv++) { //realloc과 기타 등등을 이용하여 에러 처리. 
		*(document+iv) = &cache[a]; //{"n 에서 n이 반환됨. 
		while( (cache[a++] = getc(input)) != '\"' ){
			while( (cache[a++] = getc(input)) != '{' ){
				if(feof(input) != 0){
					*(document+iv+1) = &cache[a];
					return -iv;
					} // EOF 도. feof();
			}
		} 
	} // 마지막은 기록 안됨.
	*(document+iv) = &cache[a];
	return (--iv);
}

void *workthread(void *input){ //실제로는 cstring받게 함. 
	cstring io = (cstring)input;
	int i,Tnum = pthread_self();
	for(i=Tnum;i<=io.Ilen;i+=CORE){
		
	}
	//CORE*i+pthread_self() 번째 문서만 처리한다.
	//i/o 포인터 받는다.
	//끝포인터 반환. 
}
int worker(pstring doc, FILE *outfile, unsigned char *Cdocv[]){
	int i;
	printf("\n");
	for(i=0;i<CORE;i++) printf("%d,",Cdocv[i]);
	printf("\n");
	printf("Converting\r");
	for(i=0;i<CORE;i++) pthread_create(&threads[i], NULL, &workthread, (void *)(cstring){Cdocv,doc.p,doc.len});
	return 0;
}
int JsonIO(){
	FILE *input;
	FILE *outfile;
	int output, doclen;
	unsigned char *cache;
	unsigned char **document;
	unsigned char *Cdoc[CORE];
	if((input = fopen("namu.json","r")) == NULL) return 1;
	if((outfile = fopen("Cnamu.txt","w")) == NULL) return 3;
	if( (cache = (unsigned char *)malloc(Csize)) == NULL) return 2;
	if( (document = (unsigned char **)malloc(DocS)) == NULL) return 2;
	int i;
	for(i=0; i<CORE; i++) if( (Cdoc[i] = (unsigned char *)malloc((int)(Csize*0.4)) ) == NULL) return 2;
	for(i=0;i<CORE;i++)printf("%d,",Cdoc[i]);
	printf("\n");
	
	doclen = ReadJSON(input,cache,document);
	while(doclen > 0){
		//printf("%c",*(*(doc.p+doc.len+1)-1));
		if ((output = worker((pstring){document,doclen},outfile,Cdoc)) != 0) return 2;
		doclen = ReadJSON(input,cache,document);
	}
	if ((output = worker((pstring){document,-doclen},outfile,Cdoc)) != 0) return 2;
	
	free(cache);
	free(document);
	fclose(input);
	fclose(outfile);
	
	return 0;
}

int main(){
	int IOret = JsonIO();
	if (IOret == 1) printf("덤프 파일을 열 수 없습니다.");
	if (IOret == 2) printf("메모리가 부족합니다.");
	if (IOret == 3) printf("출력 파일을 열 수 없습니다.");
	return IOret;
}
