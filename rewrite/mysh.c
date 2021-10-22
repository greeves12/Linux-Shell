#include <fcntl.h>
#include "strmanip.h"
#include <sys/wait.h>


//redirect signifiers
#define IN_R  1
#define OUT_R 2
#define ERR_R 4
#define APP_R 8
#define BASE_R 0

//pipefd
#define READ_END 0
#define WRITE_END 1

//global list of io redirect symbols
string const redirSymbols[9] = {
    "<",
    ">",
    "2>",
    "1>",
    "2>>",
    "1>>",
    "2>&1",
    ">>",
};

char findBgFlag(char str[], int length);
int child(char str[], int bgFlag);
int ioRedirectFC(char str[], string paths[]);
void print_output(char str[]);
void doExit(char str[]);

int main(){
    char input[256];
    string argv[10];
    int length;
    char bgFlag = 0;
    int pid;
    int childStatus;

    int tFlag;

    while(1){
	print_output("mysh$ ");
	length = read(STDIN, input, 256);
	input[length-1] = '\0';
	bgFlag = findBgFlag(input, length-2);

	doExit(input);
	if(bgFlag != -1){
	    if(pipeCheck(input) == -1){
		pid = fork();
		if(pid == 0){ // child process
		    child(input, bgFlag);
		}
		if(!bgFlag)
		    waitpid(pid, &childStatus, 0);
	    }else{
		child(input, bgFlag);
	    }
/*	    tFlag = ioRedirectFC(input,argv);
	    if(tFlag){
		for(int i = 0; i < 10 && argv[i] != NULL; i++){
		    write(STDOUT, argv[i], 15);
		    write(STDOUT, "\n", 1);
		}
	    }
	    else{
		write(STDOUT,"io redirect symbol search failed. none detected.", 48);
		write(STDOUT, "\n", 1);
		}*/
	}
    }
    return 0;
}


void print_output(char str[]){
    int size = 0;

    while(str[size] != '\0'){
	size++;
    }

    write(1, str, size);
}

void doExit(char str[]){
    if(mstrcmp(str, "exit")){
	_exit(0);
    }
}

/**
   findBgFlag()

   checks if the last character in the string is '&',
   ignoring trailing spaces.

   parameters:
   str - the string to be checked
   length - the last character of the given string before the null character

   return:
   a flag set to 1 if the last non-space character in the string is '&', 0 if it is not.
   return value will be -1 if the string recieved was empty or only consisted of spaces.
 */
char findBgFlag(char str[], int length){

    while(str[length] == ' ' && length >=0){
	length --;
    }
    if(length < 0){
	return -1;
    }
    else if (str[length] == '&'){
	str[length] = '\0';
	return 1;
    }
    else{
	return 0;
    }
}

/**
   child()

   executes the responsibilities of the child fork as listed below
   - if pipe exists create two more child processes and continue recursively
   - if no pipe exists any recursion ends (base case) and it looks for io redirect symbols
   - if no redirect symbols exist it will execute the command in str

   parameters:
   str - a string containing any valid command line command, ending with a null terminator.

   return:
   function is returning an int in case any errors are encountered. currently unused and returning 1 to mean true.
*/
int child(char str[], int bgFlag){
    int pIndex;
    int pipefd[2];
    int pid1;
    int pid2;
    int tokens;
    string tokList[10];
    int childStatus;

    pIndex = pipeCheck(str);

    if(pIndex > -1){ // pipe exists
	pipe(pipefd);
	str[pIndex] = '\0';
	
	pid1 = fork();
	
	if (pid1 == 0){ // left half of pipe
	    close(pipefd[READ_END]);
	    dup2(pipefd[WRITE_END], 1);
	    child(str, bgFlag);
	}

	close(pipefd[WRITE_END]);
	pid2 = fork();

	if (pid2 == 0){ // right half of pipe
	    
	    dup2(pipefd[READ_END], 0);
	    child(&str[pIndex+2], bgFlag);
	}
	
	close(pipefd[READ_END]);
	if(!bgFlag){
	    waitpid(pid1, &childStatus, 0);
	    waitpid(pid2, &childStatus, 0);
	}
    }
    else{ //no pipe
	
	tokens = tokenize(str, tokList);
	
	if(execvp(tokList[0], tokList) < 0){
	    print_output("Unknown command\n");
	    _exit(1);
	}
	for(int i = 0; tokList[i] != NULL && i < 10; i++){
	    free(tokList[i]);
	}
    }
    
    return 1;
}

/**
   ioRedirectFC()
   
   searches a string for one of the shell io redirect symbols. list of symbols consists of:
   - '<'
   - '>', '>>', '1>', '1>>'
   - '2>', '2>>', '2>&1'

   parameters:
   str - the string to be checked to see if it contains one of these symbols
   paths - an array of character pointers to be returned containing the major tokens from the original string
           that were before and after the io redirect symbols.
	   all strings in paths[] will begin with a number to indicate their contents as follows:
	   - input redirect: 1
	   - output redirect: 2
	   - std error redirect: 4
	   - std out append: 10
	   - std err append: 12
	   - none of the above (original token): 0

   return:
   0 if no redirect symbols were found. 1 if they were.


   example:
   str: "cat file.txt >> copy.txt"
   paths[0]: "0cat file.txt"
   paths[1]: "10copy.txt"

   example2:
   str: "file.txt < wc > count.txt"
   paths[0]: "1file.txt"
   paths[1]: "0wc"
   paths[2]: "2count.txt"
   
   example3:
   str: "ls -al"
   paths[0]: ""
   return value: 0
*/
int ioRedirectFC(char str[], string paths[]){
    int prevIndex = 0;
    int currIndex = 0;
    int vIndex = -1;
    int currPath = 0;
    char temp;
    int sIndex;
    
    currIndex = strcmp_p(str, redirSymbols[0]);
    if(currIndex >= 0){
	paths[0] = malloc(200);
	paths[0][0] = IN_R;
	temp = str[currIndex];
	str[currIndex] = '\0';
	mstrcpy(&str[prevIndex], &paths[0][1], 200);
	str[currIndex] = temp;
	currPath++;
	prevIndex = currIndex + 1;
    }
    write(STDOUT, &temp, 1);
    write(STDOUT, "\n", 1);
    for(int i = 0; i < 2 && vIndex < 0; i++){
	vIndex = strcmp_p(&str[prevIndex], redirSymbols[1]) - 1;
	if(vIndex >= 0){ // found a redirect symbol
	    paths[currPath] = malloc(200);
	    for(sIndex = 2; sIndex < 8 && currIndex < 0; sIndex++){
		currIndex = strcmp_p(&str[vIndex], redirSymbols[sIndex]);
	    }
	    if(sIndex == 8)
		sIndex = 1;
		currIndex = strcmp_p(&str[vIndex], redirSymbols[sIndex]);
	    if(sIndex%2 == 0){ // all the err redirect symbols have even indexes > 0
		paths[currPath][0] = ERR_R;
		if(sIndex == 4)
		    paths[currPath][0] |= APP_R;
	    }
	    else{ // output redirect symbols have odd indexes > 0
		paths[currPath][0] = OUT_R;
		if(sIndex > 3)
		    paths[currPath][0] |= APP_R;
	    }
	    temp = str[vIndex + currIndex];
	    str[vIndex+currIndex] = '\0';
	    mstrcpy(&str[prevIndex], &paths[currPath][1], 200);
	    str[vIndex + currIndex] = temp;
	    currPath++;
	    prevIndex = vIndex + currIndex + mstrlen(redirSymbols[sIndex]);
	}
    }

    if(prevIndex == 0){
	return 0;
    }
    else if(sIndex%2 == 0){
	paths[currPath] = malloc(200);
	paths[currPath][0] = ERR_R;
    }
    else{
	paths[currPath] = malloc(200);
	paths[currPath][0] = OUT_R;
    }
    mstrcpy(&str[prevIndex], &paths[currPath][1], 200);
    paths[currPath+1] = NULL;
    
    return 1;
}





/*
ls | cat | wc


main: (things that happen once per command)
 - read from keyboard
 - check for trailing & to run in background (helper function?)
 - fork


fork child activities: (will occur once per pipe half -- helper function.)
 - pipeCheck (if yes then fork immediately(?) -- create fd and dup2 first.)
 - if no pipe then check for io redirection characters in input (include in 'tokenize'? make a new function?)
 - tokenize
 - execute tokens (execve or whatever to attempt to run the user input)

*/
