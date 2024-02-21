/*
 * dsh.c
 *
 *  Created on: Aug 2, 2013
 *      Author: chiu
 */
#include "dsh.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <errno.h>
#include <err.h>
#include <sys/stat.h>
#include <string.h>


// TODO: Your function definitions below (declarations in dsh.h)

/**
 * This is just an example. Delete this before 
 * submission.
 */
/*
void example(int* x) {
    *x = thisIsGlobal;
} */



int split_length(char *str, char *delim){
    char *s = strdup(str);   
    int num_tokens=0;
    int local_num =0;
    char *token;

    //allocate the array pointer of string 

    for (token = strtok(s, delim); token != NULL; token = strtok(NULL, delim)) {
        num_tokens++;
    }
    free(s);
    return num_tokens;
}




char** split(char *str, char *delim){
    char *s = strdup(str);   // copy of original string  https://www.geeksforgeeks.org/fgets-gets-c-language/
    int num_tokens=0;
    int local_num =0;
    char *token;

    //allocate the array pointer of string 

    for (token = strtok(s, delim); token != NULL; token = strtok(NULL, delim)) {
        num_tokens++;
    }
    free(s);
    char **array = (char**) malloc((num_tokens+1) * sizeof(char*));
    
    //allocate the space for the string to which the array pointer points 
    for (int i = 0; i < num_tokens+1; i++) {
    array[i] = (char*) malloc( MAXBUF* sizeof(char));
    }
    //  the splited part 
    token = strtok(str, delim);

    s = strdup(str);  
    while (token!=NULL) {
        strcpy(array[local_num], token);
        local_num=local_num+1;
        token = strtok(NULL, delim); // contiune at the last place 
    }
    
    free(s);
    array[num_tokens]='\0';
    return array;
}