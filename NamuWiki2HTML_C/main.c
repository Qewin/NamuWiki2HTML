#include "stdnamu.h"
#include <omp.h>

pstring ReadJSON(FILE *input){ //[1~] : �� ������ ������ ��ȯ. [0] : ���� ������ �����Ͱ� ��� ����.(2��° �ε����ʹ� ���.)
	unsigned char *cache;
	unsigned char **document;
	if( (cache = (unsigned char *)malloc(Csize)) == NULL) return (pstring){0,0};
	if( (document = (unsigned char **)malloc(500000)) == NULL) return (pstring){0,0};
	int iv, a;
	for (a = iv = 0; (Csize - 100000) >a; iv++) { //realloc�� ��Ÿ ����� �̿��Ͽ� ���� ó��. 
		*(document+iv) = &cache[a]; //{"n ���� n�� ��ȯ��. 
		while( (cache[a++] = getc(input)) != '\"' ) while( (cache[a++] = getc(input)) != '{' ); // EOF ��. feof();
	} // �������� ��� �ȵ�.
	*(document+iv) = &cache[a];
	cache = realloc(cache,a);
	printf("%d,%d,%d\n",document,*(document+1),iv);
	return (pstring){document,(iv-1)};
}

int worker(pstring doc){
	printf("\n%d,%d\n",doc.p,doc.p[1]);
	int full = doc.len, i;
	printf("%d\n",doc.len);
	unsigned char *Cdoc[4]; //��ȯ ������ ���� 
	for (i=0; i < CORE; i++) if( (Cdoc[i] = (unsigned char *)malloc((int)(Csize*0.3)) ) == NULL) return 1;
	int CDIndex = 0;
	unsigned char *temp;
	#pragma omp parallel for private(CDIndex, temp) schedule(dynamic , 50)
	for (i=0; i <= full; i++) {//namespace = (char)-48,    omp_get_thread_num()
		temp = *(doc.p+i);
		if(temp[0] != 'n')continue;
		int read = 24;
		while( temp[read++] != '\"' ) while( temp[read++] != ',' );
		string title = {&temp[24] , ((read-4)-24)};
		int read2 = doc.p[i+1] - doc.p[i];
		while( temp[read2--] != '[' )( temp[read2--] != '\"' );
		string text = (string) {&temp[read+7] , ((read2-17) - (read+7))};
		//printf("%c%c",(Cdoc[omp_get_thread_num()] + CDIndex-1),(Cdoc[omp_get_thread_num()] + CDIndex));
		CDIndex += parse((int)(temp[12]-48),title,text,Cdoc[omp_get_thread_num()] + CDIndex);
		//if(CDIndex - (int)Cdoc[omp_get_thread_num()] > (int)(Csize*0.3) - 2000) CDIndex = &Cdoc[omp_get_thread_num()];
		//printf("(%c,%d,%c%c,%c)",temp[0],(temp[12]-48),temp[24],temp[25],temp[read+7]);
		
		//for (read = 0; read < title.len ;read++)printf("%c",temp[24+read]);
		//printf("\n");
		//unsigned char* ttl = title.p;
		//for (read = 0; read < title.len ;read++)printf("%c",ttl[read]);
		//printf("\n");
	}
	for (i=0; i < CORE; i++) free(Cdoc[i]);
	return 0;
}


int JsonIO(){
	FILE *input;
	pstring doc;
	int output;
	
	if((input = fopen("namu.json","r")) == NULL) return 1;
	doc = ReadJSON(input);
	while(doc.len != 0){
		if ((output = worker(doc)) != 0) return 2;
		free (*doc.p);
		free (doc.p);
		doc = ReadJSON(input);
	}
	return 0;
}

int main(){
	int IOret = JsonIO();
	if (IOret == 1) printf("���� ������ ���ų� �ջ�Ǿ����ϴ�.");
	if (IOret == 2) printf("��ȯ�� ĳ�� �޸𸮰� �����մϴ�.");
	return IOret;
}
