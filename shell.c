#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define READ_END 0
#define WRITE_END 1

void clearBuffer(char str[]);
void print_out(char str[]);
void remove_enter(char str[], int *bgFlag, int *pipeFlag);
void remove_spaces(char str[], char *argv[]);
void doExit(char str[]);
void cleanUp(char *argv[]);
void split_pipe(char str[], char newStr[], int arg);
void trim_str(char str[]);

int main(){
    char str[256];
    char pipeStr[256];
    pid_t pid, pid2;
    int status;
    char *argv[10] = {};
    int bgFlag = 1;
    int pipeFlag = 1;
    int pipefd[2];

    
    while(1){
	clearBuffer(str);
	clearBuffer(pipeStr);
	printf("mysh$ ");
	fgets(str, 256,stdin);
	doExit(str);

	remove_enter(str, &bgFlag, &pipeFlag);

	if(pipeFlag == 1){
	    pid = fork();
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
	}else{	    

	    pipe(pipefd);

	    pid = fork();

	    if(pid == 0){
		split_pipe(str, pipeStr, 0);
		close(pipefd[READ_END]);
		dup2(pipefd[WRITE_END], 1);
		remove_spaces(pipeStr, argv);
		execvp(argv[0], argv);	
	    }

	    cleanUp(argv);
	    clearBuffer(pipeStr);
	    close(pipefd[WRITE_END]);

	    pid2 = fork();

	    if(pid2 == 0){
		split_pipe(str, pipeStr, 1);
		close(pipefd[WRITE_END]);
		dup2(pipefd[READ_END],0);
		remove_spaces(pipeStr,argv);
		execvp(argv[0],argv);
	    }

	    close(pipefd[READ_END]);

	    if(bgFlag == 1){
		waitpid(pid, &status,0);
		waitpid(pid2,&status,0);
	    }

	    bgFlag = 1;
	    pipeFlag = 1;
	}
    }

    return 0;
}

void split_pipe(char str[], char newStr[], int arg){
    int i = 0;
    int x = 0;
    
    if(arg == 0){
	while(str[i] != '|'){
	    newStr[i] = str[i];
	    i++;
	}
	trim_str(newStr);
    }else if(arg == 1){
	while(str[i] != '|'){
	    i++;
	}
	i++;
	while(str[i] != '\0'){
	    newStr[x] = str[i];
	    i++;
	    x++;
	}
	trim_str(newStr);
    }
}

void trim_str(char str[]){
    int index = 0;
    int x;
    int i;
    int length = 0;
    
    while(str[index+1] != '\0'){
	index++;
    }

    while(str[index] == ' '){
	str[index] = '\0';
	index--;
    }

    index = 0;

    while(str[index] == ' '){
	index++;
    }

    while(str[length] != '\0'){
	length++;
    }
    
    for(x = 0; x < index; x++){
	
	for(i = 0; i < length; i++){
	    str[i] = str[i+1];
	}
	length--;
    }
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

void remove_enter(char str[], int *bgFlag, int *pipeFlag){
    int i = 0;

    while(str[i] != '\n'){
	i++;
    }

    if(str[i-1] == '&'){
	*bgFlag = 0;
	str[i-1] = '\0';
    }
    
    str[i] = '\0';

    i = 0;
    while(str[i] != '\0'){
	if(str[i] == '|'){
	    *pipeFlag = 0;
	    break;
	}
	i++;
    }
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
