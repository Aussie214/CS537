////////////////////////////////////////////////////////////////////////////////
// Main File:        my-uniq.c
// This File:        my-uniq.c
// Other Files:      my-sed.c , my-cat.c , README.md
// Semester:         CS 537 Spring 2019
//
// Author:           Austin Wilson
// Email:            awilson34@wisc.edu
// CS Login:         austinw
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//                   N/A
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of 
//                   of any information you find.
//                   N/A 
//////////////////////////// 80 columns wide ///////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* 
 * Given a file this method scans the entire thing and removes adjacent duplicate lines.
 * If more than two duplicate lines are adjacent only one of the many will be printed.
 * Return value is void.
 */
void remove_duplicate_lines(FILE *fp) {
	//printf("%s", "Testing");
	char *prev_line = NULL;
	size_t a = 0;
	int result;
	char *curr_line;
	while ((result = getline(&curr_line, &a, fp) != -1)) {
		if (prev_line == NULL || (strcmp(prev_line, curr_line) != 0)) {
			if (curr_line != NULL) {
                                printf("%s", curr_line);
                                prev_line = strdup(curr_line);
                        }
		}
		else {
		}
	}
}

int main(int argc, char **argv) {
	//Read from stdin
	if (argc == 1) {
		char *prev_line = NULL;
        	size_t a = 0;
        	int result;
        	char *curr_line;
        	while ((result = getline(&curr_line, &a, stdin) != -1)) {
                	if (prev_line == NULL || (strcmp(prev_line, curr_line) != 0)) {
                        	if (curr_line != NULL) {
                        	        printf("%s", curr_line);
                        	        prev_line = strdup(curr_line);
                        	}
                	}
        	}
	}
	//Read files in
        else {
                //Loop through files
                for (int i = 1; i < argc; i++) {
                        FILE *fp = fopen(argv[i], "r");
                        //Check file is valid and can be opened
                        if (fp == NULL) {
                                printf("%s", "my-uniq: cannot open file\n");
                                exit(1);
                        }
			remove_duplicate_lines(fp);      	
			fclose(fp);
		}
        }

	return 0;
}
