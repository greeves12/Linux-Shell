#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define READ_END 0
#define WRITE_END 1
#define APPEND 8
#define OUTREDIR 4
#define INREDIR 1
#define ERRREDIR 2

void clearBuffer(char str[]);
void print_out(char str[]);
void remove_enter(char str[], int *bgFlag, int *pipeFlag, char *redirFlag);
void remove_spaces(char str[], char *argv[]);
void redirectHandling(char *redirFlag, const char* tokens[], int tokenCount);
void doExit(char str[]);
int strcmp(const char* string1, const char* string2);
void cleanUp(char *argv[]);
void split_pipe(char str[], char newStr[], int arg);
void trim_str(char str[]);

int main(){
    char str[256];
    char pipeStr[256];
    pid_t pid, pid2;
    int status;
    char *argv[10] = {};
    int bgFlag = 0;
    int pipeFlag = 0;
    char redirFlag = 0;
    int pipefd[2];

    
    while(1){
	clearBuffer(str);
	clearBuffer(pipeStr);
	printf("mysh$ ");
	fgets(str, 256,stdin);
	doExit(str);

	remove_enter(str, &bgFlag, &pipeFlag, &redirFlag);

	if(!pipeFlag){
	    pid = fork();
	    if(pid == 0){
		trim_str(str);
		remove_spaces(str,argv);
		
		if(execvp(argv[0], argv) < 0){
		    printf("Unknown command\n");
		    _exit(1);
		}
		cleanUp(argv);
	    }
	    if(!bgFlag){
		waitpid(pid, &status,0);
	    }

	    bgFlag = 0;
	}else{	    

	    pipe(pipefd);

	    pid = fork();

	    if(pid == 0){
		split_pipe(str, pipeStr, 0);
		close(pipefd[READ_END]);
		dup2(pipefd[WRITE_END], 1);
		remove_spaces(pipeStr, argv);
		if(execvp(argv[0], argv) < 0){
		    printf("Unknown command\n");
                    _exit(1);
		}
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

		if(execvp(argv[0],argv) < 0){
		    printf("Unknown command\n");
                    _exit(1);
		}
	    }

	    close(pipefd[READ_END]);

	    if(!bgFlag){
		waitpid(pid, &status,0);
		waitpid(pid2,&status,0);
	    }

	    bgFlag = 0;
	    pipeFlag = 0;
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

void remove_enter(char str[], int *bgFlag, int *pipeFlag, char *redirFlag){
    int i = 0;
    *redirFlag = 0;
    
    while(str[i] != '\n'){
	i++;
    }

    if(str[i-1] == '&'){
	*bgFlag = 1;
	str[i-1] = '\0';
    }
    
    str[i] = '\0';

    i = 0;
    while(str[i] != '\0'){
	if(str[i] == '|'){
	    *pipeFlag = 1;
	    break;
	}
	if(str[i] == '<'){
	    *redirFlag = INREDIR;
	}
	if(str[i] == '>'){
	    if(str[i-1] == '2'){
		*redirFlag |= ERRREDIR;
	    }
	    else{
		*redirFlag |= OUTREDIR;
	    }
	    if(str[i+1] == '>'){
		*redirFlag |= APPEND;
		i++;
	    }
	}/* end of '>' if */
	i++;
    }
}
/* 0000 0000  input: 0000 0001  std err: 0000 0010  output: 0000 0100  output(append): 0000 1100 */
/* you should define these later :) */
void redirectHandling(char *redirFlag, const char* tokens[], int tokenCount){
    int iFile = -1;
    int oFile = -1;
    int eFile = -1;
    int flags = O_CREAT;
    
    if(*redirFlag & INREDIR){ /*input redirect*/
	iFile = open(*tokens, (flags | O_RDONLY));
    }

    if(*redirFlag & 14){/* output redirect */
	flags |= O_WRONLY;
	if(!(*redirFlag & ERRREDIR)){ /*no err redirect*/
	    if(*redirFlag & APPEND){
		flags |= O_APPEND;
	    }
	    oFile = open(tokens[tokenCount], flags);
	}/*end err redir check*/
	
	if(strcmp(tokens[tokenCount], "2>&1")){
	    if (*redirFlag & APPEND){
		flags |= O_APPEND;
	    }
	    oFile = open(tokens[tokenCount-1], flags);
	    eFile = oFile;
	}
	
	else if(tokens[tokenCount-1][0] == '2'){
	    if(!*redirFlag & APPEND){
		eFile = open(tokens[tokenCount], flags);
		oFile = open(tokens[tokenCount-2], flags);
	    }
	    else if(tokens[tokenCount-1][2] == '>'){
		eFile = open(tokens[tokenCount], flags | O_APPEND);
		if((tokens[tokenCount-3][1] == '>' && tokens[tokenCount-3][0] == '>')
		   || tokens[tokenCount-3][2] == '>'){
		    oFile = open(tokens[tokenCount-2], flags | O_APPEND);
		}
		else{
		    oFile = open(tokens[tokenCount - 2], flags);
		}
	    }
	    else{
		eFile = open(tokens[tokenCount], flags);
		oFile = open(tokens[tokenCount - 2], flags | O_APPEND);
	    }
	}
	else{
	    if(!*redirFlag & APPEND){
		oFile = open(tokens[tokenCount], flags);
		eFile = open(tokens[tokenCount-2], flags);
	    }
	    else if((tokens[tokenCount-1][1] == '>' && tokens[tokenCount-1][0] == '>')
		    || tokens[tokenCount-1][2] == '>'){
		
		oFile = open(tokens[tokenCount], flags | O_APPEND);
		if(tokens[tokenCount-3][2] == '>'){
		    eFile = open(tokens[tokenCount-2], flags | O_APPEND);
		}
		else{
		    eFile = open(tokens[tokenCount - 2], flags);
		}
	    }
	    else{
		oFile = open(tokens[tokenCount], flags);
		eFile = open(tokens[tokenCount - 2], flags | O_APPEND);
	    }
	}/*end of "diff file" else */
    }/* end of 14 if*/
    
    if(iFile != -1)
	dup2(iFile ,0);
    if(oFile != -1)
	dup2(oFile ,1);
    if(eFile != -1)
	dup2(eFile ,2);

    
    return;
}

void doExit(char str[]){
    
    if(strcmp(str, "exit")){
	_exit(0);
    }
}

int strcmp(const char* string1, const char* string2){
    int i = 0;
    while(string1[i] == string2[i]){
	if(string1[i] =='\0' && string2[i] == '\0')
	    return 1;
	i++;
    }
    return 0;
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
}
