#include "stdnamu.h"
#include <pthread.h> 

#define DocS 12500*CORE*MUL
#define Csize  CORE*MUL*5*1000*1000

pthread_t threads[CORE];
int ReadJSON(FILE *input, unsigned char *cache, unsigned char **document, unsigned char *oldend){ //[1~] : �� ������ ������ ��ȯ. [0] : ���� ������ �����Ͱ� ��� ����.(2��° �ε����ʹ� ���.)
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
	for (a = iv = 0; (Csize - 15000000) > a && iv != DocS; iv++) {
		*(document+iv) = &cache[a];
		while(cache[a++] != '\"') while(cache[a++] != '{') if((Csize-10) <= a) goto fin;
	}
	fin:
	*(document+iv) = &cache[a];
	if (readlen == readsize) return (--iv);
	else {
		*(document+iv+1) = cache + readlen;
		return -iv;
	}
}

#define title (string) {&document[24] , ((ttlend-4)-24)}
#define text (string) {&document[ttlend+7] , ((txtend-17) - (ttlend+7))}
void *workthread(void *input){ //�����δ� cstring�ް� ��. 
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
#undef title
#undef text
int worker(pstring doc, FILE *outfile, unsigned char *Cdocv[]){
	int i,j,outlen;
	printf("Converting\r");
	cstring args;
	for(i=0;i<CORE;i++) args.Cdoc[i] = Cdocv[i];
	args.Ilen = doc.len;
	args.Ip = doc.p;
	for(i=0;i<CORE;i++) pthread_create(&threads[i], NULL, &workthread, &args);
	for(i=0;i<CORE;i++) {
		pthread_join(threads[i],(void **) &outlen);
		//for(j=0;j<args.Olen[outlen];j++)putc(Cdocv[outlen][j],outfile);//olen�� ������ ���� �۱��� ���ԵǹǷ� outlen ����. 
		fwrite(Cdocv[outlen],sizeof(char),args.Olen[outlen],outfile); //olen�� ������ ���� �۱��� ���ԵǹǷ� outlen ����. 0���� ����. 
	}
	return 0;
}
int JsonIO(){
	FILE *input;
	FILE *outfile;
	int sum, output, doclen;
	unsigned char *cache;
	unsigned char **document;
	unsigned char *Cdoc[CORE];
	if((input = fopen("namu.json","r")) == NULL) return 1;
	if((outfile = fopen("Cnamu.txt","w")) == NULL) return 3;
	if( (cache = (unsigned char *)malloc(Csize)) == NULL) return 2;
	if( (document = (unsigned char **)malloc(DocS)) == NULL) return 2;
	int i;
	for(i=0; i<CORE; i++) if( (Cdoc[i] = (unsigned char *)malloc((int)(Csize*0.4)) ) == NULL) return 2;
	printf("[Main]Threads:%d\n",CORE);
	
	doclen = ReadJSON(input,cache,document,cache);
	sum = doclen;
	while(doclen > 0){
		if ((output = worker((pstring){document,doclen},outfile,Cdoc)) != 0) return 2;
		doclen = ReadJSON(input,cache,document,document[doclen+1]);
		sum += doclen;
	}
	if ((output = worker((pstring){document,-doclen},outfile,Cdoc)) != 0) return 2;
	
	free(cache);
	free(document);
	fclose(input);
	fclose(outfile);
	printf("�� ����:%d",sum);
	
	return 0;
}

int main(){
	int IOret = JsonIO();
	if (IOret == 1) printf("���� ������ �� �� �����ϴ�.");
	if (IOret == 2) printf("�޸𸮰� �����մϴ�.");
	if (IOret == 3) printf("��� ������ �� �� �����ϴ�.");
	return IOret;
}
