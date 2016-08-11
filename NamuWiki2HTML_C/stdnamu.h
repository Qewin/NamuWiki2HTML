#include <stdio.h>
#include <stdlib.h>

#define CORE 4
#define MUL 10
#define Csize  CORE*MUL*5*1000*1000
#define parsetitle(a,b,c) { if(a[b] == '\\'){ switch(a[++b]){ case 't': output[c] = '\t'; break; case 'u': c += u2utf8(a+b+1,output+c) - 1; b += 4; break; default: output[c] = a[b]; break;}} else output[c] = a[b]; c++; b++; }
#define Plain output[index2++] = txt[index]; break;

typedef struct typicalstring{
	unsigned char *p; //string 자체. 
	int len; //길이.(0부터 세었다.) 
}string;
typedef struct pointerstring{
	unsigned char **p; //string 자체. 
	int len; //길이.(0부터 세었다.) 
}pstring;
typedef struct combinediostring{
	unsigned char *Cdoc[CORE]; //자기 쓰레드 번호에 집어넣으면 끝! 
	unsigned char **Ip; // 입력 포인터 
	int Ilen; //길이.(0부터 세었다.) 
	int Olen[CORE]; 
}cstring;

short u2utf8(unsigned char *u, unsigned char* output){
	unsigned int i , ch = 0;
	for(i=0;i<=3;i++) {
		if ((unsigned int)u[i] <= 57) ch += ((int)u[i] - 48) << ((3-i)*4); // 하나로 합치기 
		else ch += ((unsigned int)u[i] - 97 + 10) << ((3-i)*4); //10부터 시작이라 10 더함. 그리고 소문자 영문. 
	}
	short index = 0;
    if (ch <= 0x7F) output[index++] = (unsigned char) ch;
    else if (ch <= 0x7FF){
    	output[index++] = (unsigned char) ((ch >> 6) + 0xC0); //110XXXXX
    	output[index++] = (unsigned char) ((ch & 63) + 0x80); //10XXXXXX
	}
    else if (ch <= 0xFFFF){
    	output[index++] = (unsigned char) ((ch >> 12) + 0xE0); // 1110XXXX
    	output[index++] = (unsigned char) ( ((ch >>6) & 63) + 0x80 );   //10XXXXXX
    	output[index++] = (unsigned char) ( (ch & 63) + 0x80 ); // 10XXXXXX
	}
    else {
    	output[index++] = (unsigned char) ( (ch >> 18) + 0xF0); // 11110XXX
    	output[index++] = (unsigned char) ( ((ch >> 12) & 63) + 0x80); // 10XXXXXX
    	output[index++] = (unsigned char) ( ((ch >>6) & 63) + 0x80);   //10XXXXXX
    	output[index++] = (unsigned char) ( (ch & 63) + 0x80); // 10XXXXXX
	}
    return index; //기록한 index 다음을 반환함. 
	
}
//int parsetable(string text);
int parsetext(string text, unsigned char* output, int index2v){
	int index2 = index2v, index, i;
	unsigned char* txt = (char*)text.p;
	for (index=0;index<=text.len;index++){
		switch(txt[index]){
			case '\\':{
				switch(txt[++index]){ // txt : \uXXXX 에서 u  
					case 't': output[index2++] = '\t'; break;
					case 'n': output[index2++] = '\n'; break;
					case 'u': index2 += u2utf8(txt+index+1,output+index2); index += 4; break;//
					default: Plain
					}
				break;
			}
			case '[':{
				switch(txt[index+1]){
					case '[':{
						index +=2;
						int bar = -1;
						int end = -1;
						
						for(i = index;(txt[i] != '\\' || txt[i+1] != 'n' ) && i<=text.len;i++){
							if(txt[i] == ']' && txt[i+1 == ']']){
								end = i;
								break;
							}
							else if(txt[i] == '|' && bar == -1) bar = i;
						}
						if(end != -1){
							if (bar != -1){
								strncpy(output+index2,"<a href=\"entry://",17);
								index2 = parsetext((string){txt+index,bar-index-1},output,index2+17);
								strncpy(output+index2,"\">",2);
								index2 = parsetext((string){txt+bar+1,end-bar-2},output,index2+2);
								strncpy(output+index2,"</a>",4);
								index2 += 4;
								index = end + 1;
							}
							else{
								
							}
							break;
						}
						//else; //default로 
					}
					default: Plain
				}
				break;
			}
			//case '~':break;
			//case '-':break;
			case '<':break;
			case '>':break;
			default: Plain
		}
	}
	return (index2);    //strspn 참고 ************************** 
}
int parse(int Nspace, string title, string text, unsigned char* opt){
	int index,index2;
	unsigned char* ttl = (char*)title.p;
	unsigned char* output = opt;
	// namespace -> title
	index2 = 0; //output index
	index =0; //input index
	int i;
	//if(Nspace != 0){
	//	output[index2++] = Nspace+48;
	//	output[index2++] = ':';
	//}
	while(index<=title.len) parsetitle(ttl,index,index2)
	output[index2++] ='\n';
	index2 = parsetext(text,opt,index2);
	strncpy(opt+index2,"\n</>\n",5);
	//printf("%c%c%c\n",opt[1],opt[2],opt[3]);
	return index2+5; // 작성한 위치 다음을 반환한다. 
}
