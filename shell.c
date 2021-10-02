#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void clearBuffer(char str[]);
void print_out(char str[]);
void remove_enter(char str[]);
void remove_spaces(char str[], char *argv[]);

int main(){
    char str[256];
    pid_t pid;
    int status;
    char *argv[10] = {};

    clearBuffer(str);
    
    while(1){
	printf("mysh$ ");
	fgets(str, 256,stdin);
	pid = fork();
	
	if(pid == 0){
	    remove_enter(str);
	    remove_spaces(str,argv);

	    //    execvp(argv[0], argv);
	}
	
	clearBuffer(str);

	waitpid(pid, &status,0);
    }

    return 0;
}

void clearBuffer(char str[]){
    int index;

    for(index = 0; index < 256; index++){
	str[index] = '\0';
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

void remove_enter(char str[]){
    int i = 0;

    while(str[i] != '\n'){
	i++;
    }

    str[i] = '\0';
}

void remove_spaces(char str[], char *argv[]){
    int arguments = 0;
    int index = 0;
    
    while(str[index] != '\0'){
	if(str[index] == ' '){
	    arguments++;
	}
	index++;
    }

    printf("%d",arguments);
    
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
