////////////////////////////////////////////////////////////////////////////////
// Main File:        my-sed.c
// This File:        my-sed.c
// Other Files:      my-uniq.c , my-cat.c , README.md
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
 * Scan a line and delete find term if found and replace with replace term then return the edited line.
 * If replace term is an empty string then the find word is just deleted from the line.
 */
char *find_replace(char *line_to_search, char* find, char* replace) {
	char *find_addr;
	find_addr = strstr(line_to_search, find);
	if (find_addr == NULL) {
		return line_to_search;
	}
	int size_of_first = strlen(line_to_search) - strlen(find_addr);
	int size_of_second = strlen(line_to_search) - size_of_first - strlen(find);
	char *second = (find_addr + strlen(find));
	char *to_return;
	if (strlen(replace) == 0) {
		to_return = malloc(sizeof(char) * (size_of_first + size_of_second));
		if (to_return == NULL) {
			exit(1);
		}
		stpncpy(to_return, line_to_search, size_of_first);
		strcat(to_return, second);
	}
	else {
		int mallocsize = size_of_first + strlen(replace) + size_of_second;
		to_return = calloc(sizeof(char) * (mallocsize), sizeof(char));
		if (to_return == NULL) {
			exit(1);
		}
		stpncpy(to_return, line_to_search, size_of_first);
		strcat(to_return, replace);
		strcat(to_return, second);
	}
	return to_return;

}

int main(int argc, char **argv) {
	//Check number of CLA
	if (argc < 3) {
		printf("my-sed: find_term replace_term [file ...]\n");
		exit(1);
	}
	//Grab inputs and start the process of find/replace
	else {
		char *find = argv[1];
		//find = calloc((sizeof(char) * strlen(argv[1])), sizeof(char));
		//stpncpy(find, argv[1], strlen(argv[1]) - 1);
		char *replace = argv[2];
		//replace = malloc(sizeof(char) * strlen(argv[2]));
		//strcpy(replace, argv[2]);
		//If CLA > 3, get input files and find/replace on each of them
		if (argc > 3) {
			for (int i = 3; i < argc; i++) {
				FILE *fp = fopen(argv[i], "r");
				//Error checking for opening the file
				if (fp == NULL) {
					printf("%s", "my-sed: cannot open file\n");
					exit(1);
				}
				//Parse line by line, call get_line
		                size_t a = 0;
                		int result;
                		char *line;
                		while ((result = getline(&line, &a, fp) != -1)) {
					if (line != NULL) {
						printf("%s", find_replace(line, find, replace));
                        		}
                		}
				fclose(fp);
			}
		}
		//Otherwise, read from stdin and find/replace that way
		else {
                	size_t a = 0;
                	int result;
                	char *line;
                	while ((result = getline(&line, &a, stdin) != -1)) {
                    		if (result < 0) {
					printf("%s", "Error: line must be a positive integer length.");
					exit(1);
				}
				else {
					printf("%s", find_replace(line, find, replace));
				}
                	}
		}
	}
	return 0;
}

