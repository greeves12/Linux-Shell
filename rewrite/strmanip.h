/**
strmanip.h

string manipulation library
created to minimize reliance on c std library
*/

#include <stdlib.h>
#include <unistd.h>


#ifndef STRMANIP_H
#define STRMANIP_H


typedef char* string;

#define STDIN  0
#define STDOUT 1
#define STDERR 2

/**  
    mstrcpy()
    
    copies the contents of src[] into dst[] up to size bytes.
    dst will always return null terminated.
    src is assumed to be null terminated. 
    
    parameters:
    src - the string to be copied
    dst - the destination array for src to be copied into
    size - the max number of bytes to be copied
    return: 1 if copy was successful, 0 if max size was reached/surpassed.
*/  
int mstrcpy(char src[], char dst[], int size);


/**  
    mstrcmp()
    
    Compares str1 and str2.
    Both strings are assumed to be null-terminated. 
    
    parameters:
    str1 - a string to be matched against str2
    str2 - a string to be matched against str1
   
    return: 1 if strings are identical, 0 if strings non-identical.
*/
int mstrcmp(char str1[], char str2[]);


/**  
    tokenize()
    
    breaks str into words ("tokens") seaparated by whitespace.
    ignores duplicate, leading, and trailing spaces.
    str is assumed to be null terminated. 
    
    parameters:
    str - the string to be split into tokens -- will likely be altered upon return.
    
    return: number of tokens found.
    argv - an array of char pointers to be returned as a set of dynamically allocated arrays
           each containing a token extracted from str.
*/
int tokenize(char str[], string argv[]);

/**
     pipeCheck()
     
     searches for a pipe character.

     parameters:
     str - the string to be searched

     return: -1 if pipe was not found, if pipe was found return
             will be the index of the pipe.
 */
int pipeCheck(char str[]);


/**
   strcmp_p()
   
   string compare-partial. checks if str2 is a subset of str1.
   example:
   str1: "racecar"
   str2: "ace"
   return value: 1

   parameters:
   str1 - the (larger) string that str2 will be compared to
   str2 - the (smaller) string that inquires if it exists in str1

   return:
   -1 if str2 is not a subset of str1,
   if str2 is a subset of str1, function returns the index of str1 that marks the start of str2.
*/
int strcmp_p(char str1[], char str2[]);

/**
   mstrlen()

   finds the length of a null terminated string (not including the null character).
   count starts at zero. i.e. "" will return -1, "foo" will return 2, etc.

   parameters:
   str - the string to be measured.

   return:
   the length of the string not including the null terminator. returns length of printable characters.
*/
int mstrlen(char str[]);


#endif /* STRMANIP_H */
