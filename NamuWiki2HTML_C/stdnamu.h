#define parsetitle(text,index,index2) { \
	if(text[index] == '\\'){ \
		switch(text[++index]){ \
		case 't': output[index2] = '\t'; break; \
		case 'u': index2 += u2utf8(text+index+1,output+index2) - 1; index += 4; break; \
		default: output[index2] = text[index]; break;\
		}\
	}\
	else output[index2] = text[index]; index2++; index++; \
}
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
	return -1;
}

#define UntilNewLine(i) for(i = index;(txt[i] != '\\' || txt[i+1] != 'n' ) && i<=text.len;i++)
#define TextProp(boolean,start,end) { \
					if (boolean)index2+=sprintf(output+index2,end); \
					else index2+=sprintf(output+index2,start); \
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

int parsetext(string text, unsigned char* output, int index2v,short *ParagraphIndex){
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
						if (strike1 || strike2)index2+=sprintf(output+index2,"</s>");
						if (italic)index2+=sprintf(output+index2,"</em>");
						if (bold)index2+=sprintf(output+index2,"</b>");
						if (note)index2+=sprintf(output+index2,"</acronym>");
						if (ubar)index2+=sprintf(output+index2,"</u>");
						index2+=sprintf(output+index2,"<br>");
						index++; 
						if(txt[index] == '='){
							short paragraph = 0,paragraph2 = 0;
							while(txt[index++] == '=')paragraph++;
							UntilNewLine(i);
							int nextline = i;
							while(txt[--i] != '=');
							while(txt[i--] == '=')paragraph2++;
							if(paragraph == paragraph2){ //문단 처리. 
							ParagraphIndex[paragraph-1]++;
							index2 += sprintf(output+index2,"<h%d>",paragraph2);
							while(paragraph<=10)ParagraphIndex[paragraph++] = 0;
							paragraph = 0;
							//printf("%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.\r",ParagraphIndex[0],ParagraphIndex[1],ParagraphIndex[2],ParagraphIndex[3],ParagraphIndex[4],ParagraphIndex[5],ParagraphIndex[6],ParagraphIndex[7],ParagraphIndex[8],ParagraphIndex[9]);
							while(ParagraphIndex[paragraph] == 0)paragraph++;
							while(paragraph<12 && ParagraphIndex[paragraph] != 0 )index2 += sprintf(output+index2,"%d.",ParagraphIndex[paragraph++]); //0 is false
							index2 = parsetext((string){txt+index,i-index},output,index2,ParagraphIndex);
							index2 += sprintf(output+index2,"</h%d>",paragraph2);
							index = nextline;
							//printf("%c%c%c\n",txt[index],txt[index+1],txt[index+2]);
							}
							else index -= paragraph;
							
						}
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
								index2+=sprintf(output+index2,"<a href=\"entry://");
								index2 = parsetext((string){txt+index,bar-index-1},output,index2,ParagraphIndex); //0부터니까 -1 
								index2+=sprintf(output+index2,"\">");
								index2 = parsetext((string){txt+bar+1,end-bar-2},output,index2,ParagraphIndex);
								index2+=sprintf(output+index2,"</a>");
							}
							else{
								index2+=sprintf(output+index2,"<a href=\"entry://");
								//i = index2;
								index2 =  parsetext((string){txt+index,end-index-1},output,index2,ParagraphIndex); //0부터니까 -1 
								index2+=sprintf(output+index2,"\">");
								/*
								<a href=\"entry://"bla">bla</a>
								                   ^    ^
								                   i    index2
												    
								*/
								index2 =  parsetext((string){txt+index,end-index-1},output,index2,ParagraphIndex);
								//memcpy(output+index2,output+i,index2-i-4);
								//index2 += index2-i-4;
								index2+=sprintf(output+index2,"</a>");
							}
							index = end + 2;
							break;
						}
						else {Plain} 
					}
					case '*':{
						if (!note){
							index2+=sprintf(output+index2,"<acronym title=\"#\">");
							note = true;
							index += 2;
						}
						else {Plain}
						break;
					}
					case 'a':{
						if(strncmp(&txt[index+1],"anchor",6) == 0){
							index += 7;
							if(txt[index] == ' ')index++;
							index++; //(*<-여기 
							index2 += sprintf(output+index2,"<a name=\"");
							while(txt[index] != ')')parsetitle(txt,index,index2)
							index2 += sprintf(output+index2,"\"></a>");
							i = index;
							while(txt[index] != ']' && (index - i) < 5)index++;
							index++;
						}
						else output[index2++] = txt[index++];
						break;
					}
					case 'i':{
						if(strncmp(&txt[index+1],"include",7) == 0){
							index += 8;
							if(txt[index] == ' ')index++;
							index++; //(*<-여기 
							index2 += sprintf(output+index2,"<div w3-include-html=\"test.html"); //entry:// 
							//index2 += sprintf(output+index2,"<link rel=\"import\" href=\"");
							//while(txt[index] != ')' && txt[index] != ',')parsetitle(txt,index,index2)
							index2 += sprintf(output+index2,"\"></div>");
							//index2 += sprintf(output+index2,"\" />");
							while(txt[index] != ']')index++;
							index++;
						}
						else output[index2++] = txt[index++];
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
					index2+=sprintf(output+index2,"</acronym>");
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
							index +=6;
							while(strncmp(&txt[index],"}}}",3) != 0 && index<text.len)parsetitle(txt,index,index2)
							index += 3;
						}
						else{
							index++;
							index2+=sprintf(output+index2,"<font color=\"");
							while(txt[index] != ' ') output[index2++] = txt[index++];
							index2+=sprintf(output+index2,"\">");
							color = true;
						}
					}
					else if(txt[index] == '+'){
						index2+=sprintf(output+index2,"<font size=\"");
						output[index2++] = txt[index++];
						index2+=sprintf(output+index2,"\">");
						size = true;
					}
					else {
						while(strncmp(&txt[index],"}}}",3) != 0 && index<text.len)parsetitle(txt,index,index2)
						index += 3;
					}
				}
				else {Plain}
				break;
			}
			case '}':{
				if(txt[index+1] == '}' && txt[index+2] == '}'){
					index += 3;
					if(color) {
						index2+=sprintf(output+index2,"</font>");
						color = false;
					}
					else if(size) {
						index2+=sprintf(output+index2,"</font>");
						size = false;
					}
				}
				else {Plain}
				break;
			}
			case '<':index2+=sprintf(output+index2,"&lt;"); index++; break;
			case '>':index2+=sprintf(output+index2,"&gt;"); index++; break;
			case '&':index2+=sprintf(output+index2,"&quot;"); index++; break;
			case '|':{
				if(txt[index+1] == '|'){
					int cols=0, next, start;
					int count = 0;
					for(start=index;txt[start]=='|'&&txt[start+1]=='|';start+=2)cols++;
					if(0 < (next = nexttable(text,start)) ){
						index2 += sprintf(output+index2,"<table>");
						bool line = false;
						while(true){
							//parse prop
							if(!line){
								index2 += sprintf(output+index2,"<tr>");
								line = true;
							}
							index2 += sprintf(output+index2,"<td colspan=\"%d\">",cols);
							index2 = parsetext((string){txt+start,next-start-1},output,index2,ParagraphIndex);
							index2 += sprintf(output+index2,"</td>");
							checknext:;
							index = next;
							cols=0;
							for(start=index;txt[start]=='|'&&txt[start+1]=='|';start+=2)cols++;
							next = nexttable(text,start);
							if(next == 0){
								index2 += sprintf(output+index2,"</tr>");
								while(txt[start]!='\\'&&txt[start+1]!='n'){
									start++;
									if(text.len <= start) goto end;
								}
								start+=2;
								if(txt[start] == '|' && txt[start+1] == '|') {
									line=false;
									next = start;
									goto checknext;
								}
								else {
									index += 2; 
									break;
								}
								
							}
							else if(next == -1){
								index2 += sprintf(output+index2,"</tr>");
								index = start; 
								break;
							}
						}
						end:;
						index2 += sprintf(output+index2,"</table>");
					}
					else {
						index2 += sprintf(output+index2,"||");
						index = start;
					}
				}
				Plain
			}
			default: Plain
		}
	}
	return (index2);    //strspn 참고 ************************** 
}

int parse(int Nspace, string title, string text, unsigned char* opt){
	int index,index2;
	short ParagraphIndex[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
	unsigned char* ttl = (char*)title.p;
	unsigned char* output = opt;
	// namespace -> title
	index2 = 0; //output index
	index =0; //input index
	int i;
	switch(Nspace){
		case 1:{
			output[index2++] = 0xED;
			output[index2++] = 0x8B;
			output[index2++] = 0x80;
			output[index2++] = ':'; //틀: 
			break;
		}
		case 2:{
			output[index2++] = 0xEB;
			output[index2++] = 0xB6;
			output[index2++] = 0x84;
			output[index2++] = 0xEB;
			output[index2++] = 0xA5;
			output[index2++] = 0x98;
			output[index2++] = ':'; //분류: 
			break;
		}
		case 3:{
			output[index2++] = 0xEC;
			output[index2++] = 0x9D;
			output[index2++] = 0xB4;
			output[index2++] = 0xEB;
			output[index2++] = 0xAF;
			output[index2++] = 0xB8;
			output[index2++] = 0xEC;
			output[index2++] = 0xA7;
			output[index2++] = 0x80;
			output[index2++] = ':'; //이미지:
			break;
		}
		case 6: {
			output[index2++] = 0xEB;
			output[index2++] = 0x82;
			output[index2++] = 0x98;
			output[index2++] = 0xEB;
			output[index2++] = 0xAC;
			output[index2++] = 0xB4;
			output[index2++] = 0xEC;
			output[index2++] = 0x9C;
			output[index2++] = 0x84;
			output[index2++] = 0xED;
			output[index2++] = 0x82;
			output[index2++] = 0xA4;
			output[index2++] = ':'; //나무위키: 
			break;
		}
		default: break;
	}
	while(ttl[index] == 0)index++;
	while(index<=title.len) parsetitle(ttl,index,index2)
	output[index2++] ='\n';
	unsigned char* txt = (char*)text.p;
	if(memcmp(text.p,"#redirect",9) == 0){
		index = 10; //공백까지. 
		index2 += sprintf(output+index2,"<a href=\"entry://");
		while(index<=text.len-2) parsetitle(txt,index,index2)
		index2 += sprintf(output+index2,"\">Redirect</a>");
	}
	else {
		index2 += sprintf(output+index2,"<head><link rel=\"stylesheet\" type=\"text/css\" href=\"qewin.css\" /></head>");
		index2 = parsetext(text,opt,index2,ParagraphIndex);
	}
	strncpy(opt+index2,"\n</>\n",5);
	return index2+5; // 작성한 위치 다음을 반환한다. 
}
