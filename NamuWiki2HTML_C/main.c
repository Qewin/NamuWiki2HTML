#include "stdnamu.h"
#include <omp.h>

#define DocS 500000
#define ThreadNum omp_get_thread_num()

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



int worker(pstring doc, FILE *outfile){
	printf("Starting Worker\r");
	int full = doc.len, i;
	unsigned char *Cdoc[CORE]; //��ȯ ������ ���� 
	for (i=0; i < CORE; i++) if( (Cdoc[i] = (unsigned char *)malloc((int)(Csize*0.4)) ) == NULL) return 1;
	int CDIndex = 0;
	unsigned char *endpoint[CORE];
	unsigned char *temp;
	string title, text;
	int read, read2;
	int st = 1;
	printf("                 \r");
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
		printf("[%d/%d]\r",st++,(full+1));
	}
	for (i=0; i < CORE; i++) {
		printf("Writing(%d/%d)\r",(i+1),CORE);
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
	int output, doclen;
	unsigned char *cache;
	unsigned char **document;
	if((input = fopen("namu.json","r")) == NULL) return 1;
	if((outfile = fopen("Cnamu.txt","w")) == NULL) return 3;
	if( (cache = (unsigned char *)malloc(Csize)) == NULL) return 2;
	if( (document = (unsigned char **)malloc(DocS)) == NULL) return 2;
	
	doclen = ReadJSON(input,cache,document);
	while(doclen > 0){
		//printf("%c",*(*(doc.p+doc.len+1)-1));
		if ((output = worker((pstring){document,doclen},outfile)) != 0) return 2;
		doclen = ReadJSON(input,cache,document);
	}
	if ((output = worker((pstring){document,-doclen},outfile)) != 0) return 2;
	
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
