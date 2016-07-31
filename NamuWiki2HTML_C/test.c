#include <stdio.h>

#include <windows.h>
short u2utf8(unsigned char *u){
	unsigned int i , ch = 0;
	for(i=0;i<=3;i++) {
		if ((unsigned int)u[i] <= 57) ch += ((int)u[i] - 48) << ((3-i)*4); // 하나로 합치기 
		else ch += ((unsigned int)u[i] - 65 + 10) << ((3-i)*4); //10부터 시작이라 10 더함. 
	}
	printf("(%d)",ch);
	short index = 0;
    if (ch <= 0x7F) printf("%c" ,(unsigned char) u[0] );
    else if (ch <= 0x7FF){
    	printf("%c" ,(unsigned char) ((ch >> 6) + 0xC0) ); //110XXXXX
    	printf("%c" ,(unsigned char) ((ch & ~63) + 0x80) ); //10XXXXXX
	}
    else if (ch <= 0xFFFF){
    	printf("%c" ,(unsigned char) ((ch >> 11) + 0xE0) ); // 1110XXXX
    	printf("%c" ,(unsigned char) ( ((ch >>6) & ~31) + 0xC0 ) );   //110XXXXX
    	printf("%c" ,(unsigned char) ( (ch & ~63) + 0x80 ) ); // 10XXXXXX
	}
    else ;
    return 4;
	
}

int main(){
	system("chcp 65001");
	printf("\\u 입력해 보세용!(1000자 제한)");
	unsigned char conv[1000];
	short index = 0;
	while ((conv[index++] = getchar()) != '\n' && index < 1000);
	int i, ind;
	for (i =0; i<index; i++){
		if(conv[i] != '\\' || conv[i+1] != 'u') printf("%c",conv[i]);
		else {
			i += 2;
			i += u2utf8(&conv[i]);
		}
	}
	
	
}
