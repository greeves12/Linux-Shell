#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void clearBuffer(char str[]);
void print_out(char str[]);
void remove_enter(char str[], int *bgFlag);
void remove_spaces(char str[], char *argv[]);
void doExit(char str[]);
void cleanUp(char *argv[]);

int main(){
    char str[256];
    pid_t pid;
    int status;
    char *argv[10] = {};
    int bgFlag = 1;
    
    while(1){
	clearBuffer(str);
	printf("mysh$ ");
	fgets(str, 256,stdin);
	doExit(str);
	pid = fork();

	remove_enter(str, &bgFlag);
	
	if(pid == 0){
	    remove_spaces(str,argv);
	    
            if(execvp(argv[0], argv) < 0){
		printf("Unknown command\n");
		_exit(1);
	    }
	    cleanUp(argv);
	}
	if(bgFlag == 1){
	    waitpid(pid, &status,0);
	}
	bgFlag = 1;
    }

    return 0;
}

void clearBuffer(char str[]){
    int index;

    for(index = 0; index < 256; index++){
	str[index] = '\0';
    }
   
}

void cleanUp(char *arg[]){
    int index = 0;

    while(arg[index] != NULL){
	free(arg[index]);
	index++;
    }
}

void print_out(char str[]){
    int i = 0;

    while(str[i] != '\0'){
	printf("%c", str[i]);
	i++;
    }

    printf("%d",i);
}

void remove_enter(char str[], int *bgFlag){
    int i = 0;

    
    while(str[i] != '\n'){
	i++;
    }

    if(str[i-1] == '&'){
	*bgFlag = 0;
	str[i-1] = '\0';
    }
    
    str[i] = '\0';
}

void doExit(char str[]){
    int flag = 0;
    int index = 0;
    char exitStr[] = {"exit"};
    
    while(str[index] != '\0' && exitStr[index] != '\0'){
	if(str[index] != exitStr[index]){
	    flag = 1;
	    break;
	}
	index++;
    }
    
    if(flag == 0){
	_exit(0);
    }

}

void remove_spaces(char str[], char *argv[]){
    int arguments = 0;
    int index = 0;
    int argIndex = 0;
    
    while(str[index] != '\0'){
	if(str[index] == ' '){
	    arguments++;
	}
	index++;
    }

    arguments++;

    index = 0;

    for(index = 0; index < arguments; index++){
	argv[index] = malloc(200);
    }
    
    argv[arguments] = NULL;

    arguments = 0;
    index = 0;

    while(str[index] != '\0'){
	if(str[index] == ' '){
	    argv[arguments][argIndex] = '\0';
	    arguments++;
	    argIndex = 0;
	    index++;
	}
	
	argv[arguments][argIndex] = str[index];
	index++;
	argIndex++;
    }

//    print_out(argv[0]);
//    print_out(argv[1]);
    
//    printf("%d",arguments);
    
/*    argv[0] = malloc(10);
    argv[1] = malloc(10);
    
    argv[0][0] = 'h';
    argv[0][1] = '\0';
    argv[1][0] = 't';
    argv[1][1] = '\0';
    print_out(argv[0]);
    print_out(argv[1]);
    free(argv[0]);
    free(argv[1]);*/
    /*
    argv[0] = "/bin/ls";
    argv[1] = "-al";
    argv[2] = NULL;
    */


}
