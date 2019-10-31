////////////////////////////////////////////////////////////////////////////////
// Main File:        my-cat.c
// This File:        my-cat.c
// Other Files:      my-sed.c , my-uniq.c , README.md
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

#include <stdio.h>
#include <stdlib.h> 

int main(int argc, char **argv) {
	//printf("%d", argc);
	for (int i = 1; i < argc; i++) {
		//Grab the next file as specified by the user in the CLA
		FILE *fp = fopen(argv[i], "r");
		//Try and open the file
		if (fp == NULL) {
			printf("my-cat: cannot open file\n");
			exit(1);
		}
		//If successful, print the contents on the file to the screen
		//We don't knowthe size of the file, so we will check the size
		// dynmacially and then allocate memory for the file. 
		else {
			//Declare mem for buffer
			char *contents = NULL;
			//Check value from fseek is alright
			if (fseek(fp, 0L, SEEK_END) == 0) {
				long buffer_size = ftell(fp);
				//Error on size of file
				if (buffer_size == -1) {
					perror("Error reading file. \n");
					exit(1);
				}
				//File size is correct
				else {
					//Make mem chunk large enough to hold contents of file
					contents = malloc(sizeof(char) * (buffer_size + 1));
					//Checking malloc call 
					if (contents == NULL) {
						perror("Memory allocation failed. \n");
						exit(1);
					}
					//Assuming everything went well, return to start of the file
					if (fseek(fp, 0L, SEEK_SET) != 0) {
						perror("Error returning to start of file.\n");
						exit(1);
					}
					//Error going to beginning of file
					else {
						fread(contents, buffer_size, 1, fp);
						printf("%s", contents);
					}
				}
			}
			//Problem traversing file
			else {
				perror("Error reading file. \n");
				exit(1);
			}
		}	
		fclose(fp);
	}
	return 0;
}
