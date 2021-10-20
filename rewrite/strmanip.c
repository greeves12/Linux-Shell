#include "strmanip.h"

int mstrcpy(char src[], char dst[], int size){
    int i = 0;
    while(src[i] != '\0' && i < size){
	dst[i] = src[i];
	i++;
    }
    if(src[i] == '\0'){
	dst[i] = src[i];
	return 1;
    }
    else{
	dst[size-1] = '\0';
	return 0;
    }
}

int mstrcmp(char str1[], char str2[]){
    for(int i = 0; str1[i] == str2[i]; i++){
	if(str1[i] == '\0')
	    return 1;
    }
    return 0;
}

int tokenize(char str[], string argv[]){
    int index = 0;
    int tokens = -1;
    int first;

    while (str[index] != '\0'){
	if(str[index] != ' '){
	    first = index;
	    tokens++;
	    while(str[index] != ' ' && str[index] != '\0')
		index++;
	    if(str[index] == ' '){
		str[index] = '\0';
		index++;
	    }
	    argv[tokens] = malloc(200);
	    mstrcpy(&str[first], argv[tokens], 200);
	    argv[tokens+1] = NULL;
	}
	else
	    index++;
    }
    
    return tokens;
}

int pipeCheck(char str[]){
    int pipe = -1;
    for(int i = 0; str[i] != '\0';i++){
	if(str[i] == '|'){
	    pipe = i;
	}
    }
    return pipe;
}

int strcmp_p(char str1[], char str2[]){
    int j = 0;
    int start;
    
    for(int i = 0; str1[i] != '\0'; i++){
	while(str1[i] == str2[j] && str1[i] != '\0'){
	    start = i;
	    j++;
	    i++;
	    if(str2[j] == '\0')
		return start;
	}
	j = 0;
    }
    return -1;
}

int mstrlen(char str[]){
    int length = -1;

    while(str[length] != '\0'){
	length++;
    }
    return length;
}
