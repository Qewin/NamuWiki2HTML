#include <stdio.h>
#include "stdnamu.h"
#include <windows.h>

int main(){
	system("chcp 65001");
	while(1){
		FILE *output;
		output = fopen("output.txt","wt");
		printf("\\u 입력해 보세용!(1000자 제한)");
		unsigned char conv[1000];
		short index = 0;
		unsigned char* out;
		while ((conv[index++] = getchar()) != '\n' && index < 1000);
		string outfile = parse(0,(string){&conv,index-1},(string){&conv,index-1});
		out = outfile.p;
		for (index = 0; index<= outfile.len; index++)fputc(out[index],stdout);
		for (index = 0; index<= outfile.len; index++)fputc(out[index],output);
		free(outfile.p);
		fclose(output);
	}
}

