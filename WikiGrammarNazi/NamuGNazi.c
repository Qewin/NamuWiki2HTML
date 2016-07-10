#define CORE 4
#define 

int IOworker(FILE *, FILE *);
int Parser(int *raw[], int *out);

int main(){
	//인자 받기 
	FILE  *input, *output;
	input = fopen("","");
	output = fopen("","");
	if (input == NULL || output == NULL) return 1;
	int rtncode = IOworker(input,output);
	input = fclose("");
	output = fclose("");
	return rtncode;
}

int IOworker(FILE *in, FILE *out){
	int out[CORE - 1]; 
	while(){
		
	}
}


int Parser(int *raw[], int *out){
	int
}
