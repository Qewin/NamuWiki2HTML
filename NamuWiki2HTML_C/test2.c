#include <stdio.h>

#include <windows.h>
short u2utf8(unsigned char *u){
	unsigned int i , ch = 0;
	for(i=0;i<=3;i++) {
		if ((unsigned int)u[i] <= 57) ch += ((int)u[i] - 48) << ((3-i)*4); // �ϳ��� ��ġ�� 
		else ch += ((unsigned int)u[i] - 97 + 10) << ((3-i)*4); //10���� �����̶� 10 ����. 
	}
	printf("(%d)",ch);
	short index = 0;
    if (ch <= 0x7F) printf("%02X," ,ch );
    else if (ch <= 0x7FF){
    	printf("%02X," ,((ch >> 6) + 0xC0) ); //110XXXXX
    	printf("%02X," ,((ch & 63) + 0x80) ); //10XXXXXX
	}
    else if (ch <= 0xFFFF){
    	printf("%02X," ,((ch >> 12) + 0xE0) ); // 1110XXXX
    	printf("%02X," ,( ((ch >>6) & 63) + 0x80 ) );   //10XXXXXX
    	printf("%02X," ,( (ch & 63) + 0x80 ) ); // 10XXXXXX
	}
    else {
    	printf("%02X," ,( (ch >> 18) + 0xF0) ); // 11110XXX
    	printf("%02X," ,( ((ch >> 12) & 63) + 0x80) ); // 10XXXXXX
    	printf("%02X," ,( ((ch >>6) & 63) + 0x80 ) );   //10XXXXXX
    	printf("%02X," ,( (ch & 63) + 0x80 ) ); // 10XXXXXX
	}
    return 3;
	
}

int main(){
	system("chcp 65001");
	printf("\\u �Է��� ������!(1000�� ����)");
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
