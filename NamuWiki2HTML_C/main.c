#include "stdnamu.h"
#include <omp.h>

#define ThreadNum omp_get_thread_num()

pstring ReadJSON(FILE *input){ //[1~] : �� ������ ������ ��ȯ. [0] : ���� ������ �����Ͱ� ��� ����.(2��° �ε����ʹ� ���.)
	unsigned char *cache;
	unsigned char **document;
	if( (cache = (unsigned char *)malloc(Csize)) == NULL) return (pstring){0,0};
	if( (document = (unsigned char **)malloc(10000)) == NULL) return (pstring){0,0};
	int iv, a;
	int size1 = Csize;
	for (a = iv = 0; iv < 10000; iv++) { //realloc�� ��Ÿ ����� �̿��Ͽ� ���� ó��. 
		*(document+iv) = &cache[a]; //{"n ���� n�� ��ȯ��. 
		while( (cache[a++] = getc(input)) != '\"' ){
			while( (cache[a] = getc(input)) != '{' ){
				if(cache[a++] == EOF) {
					*(document+iv+1) = &cache[a];
					return (pstring){document,iv};
					} // EOF ��. feof();
			}
		} 
		if (a >= (size1 - 1000000)){
			size1 += 10000000;
			realloc(cache,size1);
		}
	} // �������� ��� �ȵ�.
	*(document+iv) = &cache[a];
	cache = realloc(cache,a);
	printf("%d,%d,%d,%c\n",document,*(document+1),iv,cache[a-1]);
	return (pstring){document,(iv-1)};
}

int worker(pstring doc, FILE *outfile){
	int full = doc.len, i;
	printf("%d\n",doc.len);
	unsigned char *Cdoc[CORE]; //��ȯ ������ ���� 
	for (i=0; i < CORE; i++) if( (Cdoc[i] = (unsigned char *)malloc((int)(Csize*0.4)) ) == NULL) return 1;
	int CDIndex = 0;
	unsigned char *endpoint[CORE];
	unsigned char *temp;
	string title, text;
	int read, read2;
	#pragma omp parallel for private(temp, CDIndex, title, text, read, read2)
	for (i=0; i <= full; i++) {//namespace = (char)-48,    ThreadNum
		temp = *(doc.p+i);
		if(temp[0] != 'n')continue;
		read = 24;
		while( temp[read++] != '\"' ) while( temp[read++] != ',' );
		title = (string) {&temp[24] , ((read-4)-24)};
		read2 = doc.p[i+1] - doc.p[i];
		while( temp[read2--] != '[' ) while( temp[read2--] != '\"' );
		text = (string) {&temp[read+7] , ((read2-17) - (read+7))};
		CDIndex += parse((int)(temp[12]-48),title,text,Cdoc[ThreadNum] + CDIndex);
		endpoint[ThreadNum] = Cdoc[ThreadNum] + CDIndex;
	}
	
	for (i=0; i < CORE; i++) {
		int j, lim;
		lim = endpoint[i] - Cdoc[i];
		for(j=0;j < lim; j++)putc(Cdoc[i][j], outfile);
		free(Cdoc[i]);
	}
	return 0;
}


int JsonIO(){
	FILE *input;
	FILE *outfile;
	pstring doc;
	int output;
	
	if((input = fopen("namu.json","r")) == NULL) return 1;
	if((outfile = fopen("Cnamu.txt","w")) == NULL) return 3;
	do{
		doc = ReadJSON(input);
		//printf("%c",*(*(doc.p+doc.len+1)-1));
		if ((output = worker(doc,outfile)) != 0) return 2;
		free (*doc.p);
		free (doc.p);
	}while( *(*(doc.p+doc.len+1)-1) != EOF);
	fclose(input);
	fclose(outfile);
	return 0;
}

int main(){
	int IOret = JsonIO();
	if (IOret == 1) printf("���� ������ �� �� �����ϴ�.");
	if (IOret == 2) printf("��ȯ�� ĳ�� �޸𸮰� �����մϴ�.");
	if (IOret == 3) printf("��� ������ �� �� �����ϴ�.");
	return IOret;
}
