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

/*
char** split(char *str, char *delim) {
    int num_tokens = 0;
    char *s = strdup(str);
    if (!s) { // Check for allocation failure
        return NULL;
    }

    // Count the tokens
    char *token = strtok(s, delim);
    while (token != NULL) {
        num_tokens++;
        token = strtok(NULL, delim);
    }

    // Allocate the array of pointers with an additional slot for the NULL terminator
    char **array = (char **)malloc((num_tokens + 1) * sizeof(char *));
    if (!array) { // Check for allocation failure
        free(s);
        return NULL;
    }

    // Split the string into tokens and allocate space for each token
    strcpy(s, str); // Reset s to the original string content
    int i = 0;
    token = strtok(s, delim);
    while (token != NULL && i < num_tokens) {
        array[i] = strdup(token); // Allocate and copy each token
        if (!array[i]) { // Check for allocation failure
            // Free previously allocated memory
            while (i > 0) {
                free(array[--i]);
            }
            free(array);
            free(s);
            return NULL;
        }
        i++;
        token = strtok(NULL, delim);
    }
    array[num_tokens] = NULL; // Null-terminate the array

    free(s); // Free the duplicated string
    return array;
}
*/
