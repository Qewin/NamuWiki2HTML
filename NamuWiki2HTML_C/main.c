#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h> 
#include "stdnamu.h"

typedef struct combinediostring{
	unsigned char *Cdoc[16]; //자기 쓰레드 번호에 집어넣으면 끝! 
	unsigned char **Ip; // 입력 포인터 
	int Ilen; //길이.(0부터 세었다.) 
	int Olen[16]; 
}cstring;

#define MUL 10 //10
#define CORE 4 //4
#define DocS 12500*CORE*MUL
#define Csize  CORE*MUL*5*1000*1000
#define TCsize (int)(Csize/CORE*1.1))

int ReadJSON(FILE *input, unsigned char *cache, unsigned char **document, unsigned char *oldend){ //[1~] : 각 문서의 포인터 반환. [0] : 원래 문서의 포인터가 들어 있음.(2번째 로딩부터는 사용.)
	printf("Reading             \r");
	int readsize = (oldend-cache), readlen;
	if(readsize) {
		memcpy(cache,oldend,(Csize - readsize));
		readlen = fread(cache+Csize-readsize,sizeof(char),readsize,input);
	}
	else {
		readsize = Csize;
		readlen = fread(cache,sizeof(char),readsize,input);
	}
	
	int iv, a;
	int full = (Csize - readsize) + readlen; //인덱스로 따지면 1 작아야 합니당! 
	for (a = iv = 0;iv < DocS && a<(full - 4); iv++) {
		document[iv] = &cache[a];
		do{
			while(cache[a++] != '{') if((full-4) <= a) goto fin;
		} while(cache[a++] != '\"');
	}
	fin:
	if (readlen == readsize) return (--iv);
	else {
		document[iv+1] = cache + full - 4;
		return -iv;
	}
}

#define title (string) {&document[24] , ((ttlend-4)-24)}
#define text (string) {&document[ttlend+7] , ((txtend-17) - (ttlend+7))}
void *workthread(void *input){ //실제로는 cstring받게 함. 
	cstring *io = (cstring*)input;
	int i,ttlend,txtend,index = 0,Tnum = pthread_self();
	Tnum = Tnum % CORE;
	unsigned char *document;
	unsigned char *Cdoc = io->Cdoc[Tnum];
	for(i=Tnum;i<=io->Ilen;i+=CORE){
		document = (io->Ip[i]);
		if(document[0] != 'n')continue;
		ttlend = 24;
		while(document[ttlend++] != '\"') while(document[ttlend++] != ',');
		txtend = io->Ip[i+1] - io->Ip[i];
		while( document[txtend--] != '[' ) while( document[txtend--] != '\"' );
		index += parse((int)(document[12]-48),title,text, Cdoc + index); 
	}
	io->Olen[Tnum] = index;
	return (void *)Tnum;
}
int worker(pstring doc, FILE *outfile, unsigned char *Cdocv[]){
	pthread_t threads[CORE];
	int i,j,outlen;
	printf("Converting\r");
	cstring args;
	for(i=0;i<CORE;i++) args.Cdoc[i] = Cdocv[i];
	args.Ilen = doc.len;
	args.Ip = doc.p;
	for(i=0;i<CORE;i++) pthread_create(&threads[i], NULL, &workthread, &args);
	for(i=0;i<CORE;i++) {
		pthread_join(threads[i],(void **) &outlen);
		//for(j=0;j<args.Olen[outlen];j++)putc(Cdocv[outlen][j],outfile);//olen은 구조상 범위 밖까지 포함되므로 outlen 제외. 
		fwrite(Cdocv[outlen],sizeof(char),args.Olen[outlen],outfile); //olen은 구조상 범위 밖까지 포함되므로 outlen 제외. 0부터 시작. 
	}
	return 0;
}
int sworker(pstring doc, FILE *outfile, unsigned char *Cdocv[]){
	printf("Converting[SingleThread]\r");
	
	int i,index = 0,ttlend,txtend;
	unsigned char *document;
	for(i=0;i<=doc.len;i++){
		document = doc.p[i];
		if(document[0] != 'n')continue;
		ttlend = 24;
		while(document[ttlend++] != '\"') while(document[ttlend++] != ',');
		txtend = doc.p[i+1] - doc.p[i];
		while( document[txtend--] != '[' ) while( document[txtend--] != '\"' );
		index += parse((int)(document[12]-48),title,text, Cdocv[0] + index);
	}
	fwrite(Cdocv[0],sizeof(char),index,outfile); //index는 구조상 범위 밖까지 포함되므로 index 제외. 0부터 시작. 
	return 0;
}

int JsonIO(){
	FILE *input;
	FILE *outfile;
	int sum = 0, output, doclen;
	unsigned char *cache;
	unsigned char **document;
	unsigned char *Cdoc[CORE];
	if((input = fopen("namu.json","rb")) == NULL) return 1;
	if((outfile = fopen("Cnamu.txt","w")) == NULL) return 3;
	if( (cache = (unsigned char *)malloc(Csize)) == NULL) return 2;
	if( (document = (unsigned char **)calloc(DocS,sizeof(unsigned char*))) == NULL) return 2;
	int i;
	for(i=0; i<CORE; i++) if( (Cdoc[i] = (unsigned char *)malloc(TCsize) == NULL) return 2;
	printf("[Main]Threads:%d\n",CORE);
	
	doclen = ReadJSON(input,cache,document,cache);
	while(doclen > 0){
		sum += doclen;
		if ((output = worker((pstring){document,doclen},outfile,Cdoc)) != 0) return 2;
		doclen = ReadJSON(input,cache,document,document[doclen+1]);
	}
	sum -= doclen;
	if ((output = worker((pstring){document,-doclen},outfile,Cdoc)) != 0) return 2;
	
	free(cache);
	free(document);
	fclose(input);
	fclose(outfile);
	printf("Documents:%d",sum);
	
	return 0;
}

int main(){
	int IOret = JsonIO();
	if (IOret == 1) printf("덤프 파일을 열 수 없습니다.");
	if (IOret == 2) printf("메모리가 부족합니다.");
	if (IOret == 3) printf("출력 파일을 열 수 없습니다.");
	return IOret;
}
