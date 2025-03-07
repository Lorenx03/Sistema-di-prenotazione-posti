#include "userInput.h"

void read_int(int *n){
	char buffer[1024];
	bool success = false;

	do{
		if(!fgets(buffer,1024,stdin)){
			fprintf(stderr, "Errore");
			exit(EXIT_FAILURE);
		}

		errno = 0;
		char *endptr;

		*n = strtol(buffer,&endptr,10);

		if(errno == ERANGE){
			success = false;
		}else if(endptr == buffer){
			success = false;
		}else if(*endptr && *endptr != '\n'){
			success = false;
		}else{
			success = true;
		}
	}while(!success);
}


void read_str(char *str){
	char buffer[1024];
	if(fgets(buffer,1024,stdin)){
		buffer[strcspn(buffer,"\n")] = 0;
		strcpy(str,buffer);
	}else{
		fprintf(stderr, "error");
		exit(EXIT_FAILURE);
	}
}


void waitForKey() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}