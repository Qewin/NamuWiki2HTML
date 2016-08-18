#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MUL 10
#define CORE 4

#define parsetitle(a,b,c) { if(a[b] == '\\'){ switch(a[++b]){ case 't': output[c] = '\t'; break; case 'u': c += u2utf8(a+b+1,output+c) - 1; b += 4; break; default: output[c] = a[b]; break;}} else output[c] = a[b]; c++; b++; }
#define Plain output[index2++] = txt[index++]; break;
#define _FILE_OFFSET_BITS  64
//4GB ���� Ǯ�� ���� �ʿ��Դ�. 
typedef struct typicalstring{
	unsigned char *p; //string ��ü. 
	int len; //����.(0���� ������.) 
}string;
typedef struct pointerstring{
	unsigned char **p; //string ��ü. 
	int len; //����.(0���� ������.) 
}pstring;
typedef struct combinediostring{
	unsigned char *Cdoc[CORE]; //�ڱ� ������ ��ȣ�� ��������� ��! 
	unsigned char **Ip; // �Է� ������ 
	int Ilen; //����.(0���� ������.) 
	int Olen[CORE]; 
}cstring;
typedef enum {false, true} bool;

short u2utf8(unsigned char *u, unsigned char* output){
	unsigned int i , ch = 0;
	for(i=0;i<=3;i++) {
		if ((unsigned int)u[i] <= 57) ch += ((int)u[i] - 48) << ((3-i)*4); // �ϳ��� ��ġ�� 
		else ch += ((unsigned int)u[i] - 97 + 10) << ((3-i)*4); //10���� �����̶� 10 ����. �׸��� �ҹ��� ����. 
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
    return index; //����� index ������ ��ȯ��. 
	
}
int nexttable(string text, int index){
	int i;
	unsigned char *txt = text.p;
	for(i=index;;i++){
		if(txt[i] == '|' && txt[i+1] == '|')return i;
		if(txt[i] == '\\' && txt[i+1] == 'n')return 0;
		if(txt[i] != ' ')break;
	}
	for(i++;i<=text.len;i++){
		if(txt[i] == '|' && txt[i+1] == '|')return i;
		if(txt[i] == '\\' && txt[i+1] == 'n'){
			if(txt[i+2] == '=')return -1;
		}
	}
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
					TextProp(boolean,start,end) \
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
				switch(txt[++index]){ // txt : \uXXXX ���� u  
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
								index2 = parsetext((string){txt+index,bar-index-1},output,index2); //0���ʹϱ� -1 
								append("\">");
								index2 = parsetext((string){txt+bar+1,end-bar-2},output,index2);
								append("</a>");
							}
							else{
								append("<a href=\"entry://");
								//i = index2;
								index2 =  parsetext((string){txt+index,end-index-1},output,index2); //0���ʹϱ� -1 
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
			case '|':{
				if(txt[index+1] == '|'){
					int cols=0, next;
					for(i=index;txt[i]=='|'&&txt[i+1]=='|';i+=2)cols++;
					if(0 < (next = nexttable(text,i)) ){
						index2 += sprintf(output+index2,"<table>");
						bool line = false;
						while(true){
							//parse prop
							if(!line){
								index2 += sprintf(output+index2,"<tr>");
								line = true;
							}
							index2 += sprintf(output+index2,"<td colspan=\"%d\">",cols);
							index2 = parsetext((string){txt+i,next-i},output,index2);
							index = next;
							index2 += sprintf(output+index2,"</td>");
							checknext:;
							cols=0;
							for(i=next;txt[i]=='|'&&txt[i+1]=='|';i+=2)cols++;
							next = nexttable(text,i);
							if(next == 0){
								index2 += sprintf(output+index2,"</tr>");
								while(txt[i]!='\\'&&txt[i+1]!='n')i++;
								i+=2;
								if(txt[i] == '|' && txt[i+1] == '|') line=false;
								else break;
								next = i;
								goto checknext;
								
							}
							else if(next == -1){
								index2 += sprintf(output+index2,"</tr>");
								break;
							}
						}
						index2 += sprintf(output+index2,"</table>");
					}
					else {
						index2 += sprintf(output+index2,"||");
						index += 2;
					}
				}
				break;
			}
			default: Plain
		}
	}
	return (index2);    //strspn ���� ************************** 
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
	unsigned char* txt = (char*)text.p;
	if(memcmp(text.p,"#redirect",9) == 0){
		index = 10; //�������. 
		index2 += sprintf(output+index2,"<a href=\"entry://");
		while(index<=text.len-2) parsetitle(txt,index,index2)
		index2 += sprintf(output+index2,"\">Redirect</a>");
	}
	else {
		index2 += sprintf(output+index2,"<head><link rel=\"style\" type=\"text/css\" href=\"qewin.css\"></head>");
		index2 = parsetext(text,opt,index2);
	}
	strncpy(opt+index2,"\n</>\n",5);
	return index2+5; // �ۼ��� ��ġ ������ ��ȯ�Ѵ�. 
}
