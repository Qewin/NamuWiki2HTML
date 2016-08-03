#include <stdio.h>
#include <stdlib.h>

#define CORE 4
#define Csize  CORE*50*1000*1000

typedef struct typicalstring{
	unsigned char *p; //string ��ü. 
	int len; //����.(0���� ������.) 
}string;
typedef struct pointerstring{
	unsigned char **p; //string ��ü. 
	int len; //����.(0���� ������.) 
}pstring;

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
//int parsetable(string text);
//int parsetext(string text);
short parse(int Nspace, string title, string text, unsigned char* opt){
	int index,index2;
	unsigned char* txt = (char*)text.p;
	unsigned char* ttl = (char*)title.p;
	unsigned char* output = opt;
	// namespace -> title
	index2 = 0; //output index
	for(index=0;index<=title.len;index++){//input index
		if(ttl[index] == '\\'){
			switch(ttl[++index]){ // ttl : \uXXXX ���� u  
				case 't': output[index2] = '\t'; break;
				case 'u': index2 += u2utf8(ttl+index+1,output+index2) - 1; index += 4; break;//
				default: output[index2] = ttl[index]; break;
			}
		}
		else output[index2] = ttl[index];
		index2++;
	}
	for (index=0;index<=text.len;index++){
		switch(txt[index]){
			case '\\':{
				switch(txt[++index]){ // txt : \uXXXX ���� u  
					case 't': output[index2++] = '\t'; break;
					case 'n': output[index2++] = '\r'; output[index2++] = '\n'; break;
					case 'u': index2 += u2utf8(txt+index+1,output+index2); index += 4; break;//
					default: output[index2++] = txt[index]; break;
					}
				break;
			}
			//case '[':break;
			//case '~':break;
			//case '-':break;
			default: output[index2++] = txt[index]; break;
		}
	}
	//for (index=0;index<=index2;index++)printf("%c",(unsigned char)output[index]);
	//printf("\n");
	//char *ctitle = u2utf8(title);
	//char *ctext = parsetext(text);
	return index2; // �ۼ��� ��ġ ������ ��ȯ�Ѵ�. 
}
