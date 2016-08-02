#include "stdnamu.h"
#include <omp.h>

pstring ReadJSON(FILE *input){ //[1~] : 각 문서의 포인터 반환. [0] : 원래 문서의 포인터가 들어 있음.(2번째 로딩부터는 사용.)
	unsigned char *cache;
	unsigned char **document;
	if( (cache = (unsigned char *)malloc(Csize)) == NULL) return (pstring){0,0};
	if( (document = (unsigned char **)malloc(500000)) == NULL) return (pstring){0,0};
	int iv, a;
	for (a = iv = 0; (Csize - 100000) >a; iv++) { //realloc과 기타 등등을 이용하여 에러 처리. 
		*(document+iv) = &cache[a]; //{"n 에서 n이 반환됨. 
		while( (cache[a++] = getc(input)) != '\"' ) while( (cache[a++] = getc(input)) != '{' ); // EOF 도. feof();
	} // 마지막은 기록 안됨.
	*(document+iv) = &cache[a];
	cache = realloc(cache,a);
	printf("%d,%d,%d\n",document,*(document+1),iv);
	return (pstring){document,(iv-1)};
}

int worker(pstring doc){
	printf("\n%d,%d\n",doc.p,doc.p[1]);
	int full = doc.len, i;
	printf("%d\n",doc.len);
	unsigned char *Cdoc[4]; //변환 데이터 저장 
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
	if (IOret == 1) printf("덤프 파일이 없거나 손상되었습니다.");
	if (IOret == 2) printf("변환용 캐시 메모리가 부족합니다.");
	return IOret;
}
