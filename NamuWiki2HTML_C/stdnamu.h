#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MUL 10
#define CORE 4

#define parsetitle(a,b,c) { if(a[b] == '\\'){ switch(a[++b]){ case 't': output[c] = '\t'; break; case 'u': c += u2utf8(a+b+1,output+c) - 1; b += 4; break; default: output[c] = a[b]; break;}} else output[c] = a[b]; c++; b++; }
#define Plain output[index2++] = txt[index++]; break;
#define _FILE_OFFSET_BITS  64
//4GB 제한 풀기 위해 필요함당. 
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
typedef enum {false, true} bool;

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
#define append(a) memcpy(output+index2,a,sizeof(a)-1); \
                  index2 += sizeof(a)-1
#define UntilNewLine(i) for(i = index;(txt[i] != '\\' || txt[i+1] != 'n' ) && i<=text.len;i++)
#define TextProp(boolean,start,end) { \
					if (boolean){append(end);} \
					else {append(start);} \
					boolean = !boolean; \
					}
#define TextDeco(boolean,start,end,char) { \
				if (txt[++index] == char){ \
					TextProp(boolean,"start","end") \
					index++; \
				} \
				else output[index2++] = char; \
				break; \
			}
int parsetext(string text, unsigned char* output, int index2v){
	int index2 = index2v, index = 0, i;
	bool strike1 = false, strike2 = false, italic = false, bold = false;
	bool sub = false, sup = false, ubar = false;
	bool color = false, size = false, note = false;
	unsigned char* txt = (char*)text.p;
	while(index<=text.len){
		switch(txt[index]){
			case '\\':{
				switch(txt[++index]){ // txt : \uXXXX 에서 u  
					case 'u': {
						index--;
						do{
							index2 += u2utf8(txt+index+2,output+index2);
							index += 6;
							if(txt[index] == ' ') output[index2++] = txt[index++];
						}while(txt[index] == '\\' && txt[index+1] == 'u');
						break;
						}
					case 'n': {
						if (strike1 || strike2){append("</s>");}
						if (italic){append("</em>");}
						if (bold){append("</b>");}
						if (note){append("</acronym>");}
						if (ubar){append("</u>");}
						append("<br>");
						index++; 
						break;
					}
					case 't': output[index2++] = '\t'; index++; break;
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
						
						UntilNewLine(i){
							if(txt[i] == ']' && txt[i+1] == ']'){
								end = i;
								break;
							}
							else if(txt[i] == '|' && bar == -1) bar = i;
						}
						if(end != -1){
							if (bar != -1){
								append("<a href=\"entry://");
								index2 = parsetext((string){txt+index,bar-index-1},output,index2); //0부터니까 -1 
								append("\">");
								index2 = parsetext((string){txt+bar+1,end-bar-2},output,index2);
								append("</a>");
							}
							else{
								append("<a href=\"entry://");
								//i = index2;
								index2 =  parsetext((string){txt+index,end-index-1},output,index2); //0부터니까 -1 
								append("\">");
								/*
								<a href=\"entry://"bla">bla</a>
								                   ^    ^
								                   i    index2
												    
								*/
								index2 =  parsetext((string){txt+index,end-index-1},output,index2);
								//memcpy(output+index2,output+i,index2-i-4);
								//index2 += index2-i-4;
								append("</a>");
							}
							index = end + 2;
							break;
						}
						else {Plain} 
					}
					case '*':{
						if (!note){
							append("<acronym title=\"#\">");
							note = true;
							index += 2;
						}
						else {Plain}
						break;
					}
					default: Plain
				}
				break;
			}
			case '~':TextDeco(strike1,"<s>","</s>",'~')
			case '-':TextDeco(strike2,"<s>","</s>",'-')
			case '_':TextDeco(ubar,"<u>","</u>",'_')
			case '^':TextDeco(sup,"<sup>","</sup>",'^')
			case ',':TextDeco(sub,"<sub>","</sub>",',')
			case '\'':{
				if(txt[++index] == '\''){
					if(txt[++index] == '\'') {
						index++;
						TextProp(bold,"<b>","</b>")
					}
					else TextProp(italic,"<em>","</em>")
				}
				else output[index2++] = '\'';
				break;
			}
			case ']':{
				if(note){
					append("</acronym>");
					note = !note; 
					index++;
				}
				else {Plain}
				break;
			}
			case '{':{
				if(txt[index+1] == '{' && txt[index+2] == '{'){
					index += 3;
					if(txt[index] == '#'){
						if(strncmp(&txt[index],"#!html",6) == 0){
							while(strncmp(&txt[++index],"}}}",3) != 0);
							index += 3;
						}
						else{
							index++;
							append("<font color=\"");
							while(txt[index] != ' ') output[index2++] = txt[index++];
							append("\">");
							color = true;
						}
					}
					else if(txt[index] == '+'){
						append("<font size=\"");
						output[index2++] = txt[index++];
						append("\">");
						size = true;
					}
					else {Plain}
				}
				else {Plain}
				break;
			}
			case '}':{
				if(txt[index+1] == '}' && txt[index+2] == '}'){
					index += 3;
					if(color) {
						append("</font>");
						color = false;
					}
					else if(size) {
						append("</font>");
						size = false;
					}
				}
				else {Plain}
				break;
			}
			case '<':append("&lt;"); index++; break;
			case '>':append("&gt;"); index++; break;
			case '&':append("&quot;"); index++; break;
			default: Plain
		}
	}
	return (index2);    //strspn 참고 ************************** 
}
#undef append(a)
int parse(int Nspace, string title, string text, unsigned char* opt){
	int index,index2;
	unsigned char* ttl = (char*)title.p;
	unsigned char* output = opt;
	// namespace -> title
	index2 = 0; //output index
	index =0; //input index
	int i;
	if(Nspace != 0){
		output[index2++] = Nspace+48;
		output[index2++] = ':';
	}
	while(ttl[index] == 0)index++;
	while(index<=title.len) parsetitle(ttl,index,index2)
	output[index2++] ='\n';
	index2 = parsetext(text,opt,index2);
	strncpy(opt+index2,"\n</>\n",5);
	return index2+5; // 작성한 위치 다음을 반환한다. 
}
