#include <stdio.h>
#include "stdnamu.h"
#include <omp.h>

string ReadJSON(FILE *input){ //[1~] : �� ������ ������ ��ȯ. [0] : ���� ������ �����Ͱ� ��� ����.(2��° �ε����ʹ� ���.)
	unsigned char *cache;
	int *document;
	if( (cache = (unsigned char *)malloc(Csize)) == NULL) return (string){0,0};
	if( (document = (int *)malloc(sizeof(int)*500000)) == NULL) return (string){0,0};
	int iv, a;
	for (a = iv = 0; (Csize - a) >10000; iv++) {
		document[iv] = &cache[a]; //{"n ���� n�� ��ȯ��. 
		while( (cache[a++] = getc(input)) != '\"' ) while( (cache[a++] = getc(input)) != '{' ); // EOF ��. feof();
	} // �������� ��� �ȵ�. 
	printf("%d,%d,%d\n",document,document[1],iv);
	return (string){document,(iv-1)};
}

int worker(string doc){
	printf("\n%d,%d\n",doc.p,doc.p[1]);
	int full, i;
	printf("%d",doc.len);
	unsigned char *Cdoc[4]; //��ȯ ������ ���� 
	for (i=0; i < CORE; i++) if( (Cdoc[i] = (unsigned char *)malloc((Csize*0.3)) ) == NULL) return NULL;
	int CDIndex = 0;
	unsigned char *temp;
	#pragma omp parallel for private(CDIndex, temp) schedule(dynamic , 50)
	for (i=0; i < full; i++) {//namespace = (char)-48,    omp_get_thread_num()
			temp = doc.p[i];
			if(temp[0] != 'n')continue;
			int read = 24;
			while( temp[read++] != '\"' ) while( temp[read++] != ',' );
			string title = {&temp[24] , ((read-4)-24)};
			int read2 = doc.p[i+1] - doc.p[i];
			while( temp[read2--] != '[' )( temp[read2--] != '\"' );
			string text ={&temp[read+7] , ((read2-17) - (read+7))};
			
			CDIndex += parse((int)(temp[12]-48),title,text,Cdoc[omp_get_thread_num()] + CDIndex);
			 //printf("(%c,%d,%c%c,%c)",temp[0],(temp[12]-48),temp[24],temp[25],temp[read+7]);
			 //parse((int)(temp[12]-48),&temp[24],&temp[read+7],Cdoc[i]);//namespace,title,text,cdocpointer
			
			//for (read = 0; read < title.len ;read++)printf("%c",temp[24+read]);
			//printf("\n");
			//unsigned char* ttl = title.p;
			//for (read = 0; read < title.len ;read++)printf("%c",ttl[read]);
			//if(&temp[24] != &title.p[0]) printf("aaa\n");
			//printf("\n");
		}
	return 0;
}


int JsonIO(){
	FILE *input;
	if((input = fopen("namu.json","r")) == NULL) return 1;
	string doc;
	int ret;
	doc = ReadJSON(input);
	//if( (ret = worker(doc)) != 0) return ret;
	free (doc.p[0]);
	free (doc.p);
	
	//send to pointer
	//while all the pointer is not null : 
	// until 40% of memory left
	//  read near 50MB
	//  find title, namespace, document
	//start new parse thread
	return 0;
}

int main(){
	
	
	//JsonIO
	int IOret = JsonIO();
	//start worker - (core - 1) threads
	
	if (IOret == 1) printf("���� ������ ���ų� �ջ�Ǿ����ϴ�.");
	return IOret;
}
