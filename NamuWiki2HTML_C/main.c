#include "stdnamu.h"
#include <pthread.h> 

#define DocS 125000*CORE

pthread_t threads[CORE];

int ReadJSON(FILE *input, unsigned char *cache, unsigned char **document){ //[1~] : �� ������ ������ ��ȯ. [0] : ���� ������ �����Ͱ� ��� ����.(2��° �ε����ʹ� ���.)
	printf("Reading             \r");
	int iv, a;
	for (a = iv = 0; (Csize - 15000000) > a && iv != DocS; iv++) { //realloc�� ��Ÿ ����� �̿��Ͽ� ���� ó��. 
		*(document+iv) = &cache[a]; //{"n ���� n�� ��ȯ��. 
		while( (cache[a++] = getc(input)) != '\"' ){
			while( (cache[a++] = getc(input)) != '{' ){
				if(feof(input) != 0){
					*(document+iv+1) = &cache[a];
					return -iv;
					} // EOF ��. feof();
			}
		} 
	} // �������� ��� �ȵ�.
	*(document+iv) = &cache[a];
	return (--iv);
}

void *workthread(void *input){ //�����δ� cstring�ް� ��. 
	cstring io = (cstring)input;
	int i,Tnum = pthread_self();
	for(i=Tnum;i<=io.Ilen;i+=CORE){
		
	}
	//CORE*i+pthread_self() ��° ������ ó���Ѵ�.
	//i/o ������ �޴´�.
	//�������� ��ȯ. 
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
	if (IOret == 1) printf("���� ������ �� �� �����ϴ�.");
	if (IOret == 2) printf("�޸𸮰� �����մϴ�.");
	if (IOret == 3) printf("��� ������ �� �� �����ϴ�.");
	return IOret;
}
