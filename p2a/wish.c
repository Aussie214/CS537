#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
 * Struct to hold prev commands
 */
typedef struct node {
        char *line;
        int num_of_nodes;
        struct node *next;
} node_t;

void error() {
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
}

char *sep = " \n\t";

void pipe_control(char *line, char** paths, int p_counter) {
	char *command_one = NULL;
	char *command_two = NULL;
	char **args_one = malloc(sizeof(char*) * 20);
	char **args_two = malloc(sizeof(char*) * 20);
	int pipe_char = 0;
	int arg_num = 1;
	int arg_one_counter = 0;
	int arg_two_counter = 0;
	
	char *new_line = malloc(sizeof(char) * strlen(line) + 1);
	strcpy(new_line, line);

	char *token = strtok(new_line, sep);
	command_one = malloc(sizeof(char) * strlen(token) + 1);
	strcpy(command_one, token);
	//Loop 
	while (token != NULL) {
		//printf("%d", e);
	//	printf("%s", token);
		//Checks for valid pipe
		if (pipe_char == 0 && strcmp(token, "|") == 0) {
			pipe_char = 1;
			arg_num = 2;
		}
		//Checks for multiple pipes
		else if (pipe_char == 1 && strcmp(token, "|") == 0) {
			error();
			return;
		}
		//Checks for redirection
		else if (strcmp(token, ">") == 0) {
			error();
			return;
		}
		//Add to first args
		if (arg_num == 1) {
			char *temp_arg = malloc(sizeof(char) * strlen(token) + 1);
			strcpy(temp_arg, token);
			printf(".%s.", token);
			args_one[arg_one_counter] = temp_arg;
			arg_one_counter++;	
		}
		//Add to second args
		if (arg_num == 2 && strcmp(token, "|") != 0) {
			char *temp_arg_two = malloc(sizeof(char) * strlen(token) + 1);
			strcpy(temp_arg_two, token);
			printf(".%s.", token);
			args_one[arg_two_counter] = temp_arg_two;
			arg_two_counter++;
		}
		//e++;
		token = strtok(NULL, sep);
	}
	//Sets last elements to NULL
	//args_one[18] = NULL;
	//args_two[18] = NULL;
	//Checks that there is a second command
	if (pipe_char == 1 && command_two == NULL) {
		error();
		return;
	}

	//Testing
	//printf("%s\n", command_one);
	for (int r = 0; args_one[r] != NULL; r++) {
		printf("[%s]", args_one[r]);
	}
	//printf("[%s]", args_one[1]);
	//exit(0);
	//printf("%s\n", command_two);
	//for (int d = 0; args_two[d] != NULL; d++) {
	//	printf("[%s]", args_two[d]);
	//}
	//fflush(NULL);
	//exit(0);





	//Now its time to execute the pipe
	int file_d[2];
	pipe(file_d);
	pid_t child_1 = fork();
	if (child_1 == 0) {
		close(file_d[0]);
		dup2(file_d[1], 1);
		//Execute command

		//
		for (int b = 0; b < p_counter; b++) {
                        //printf("%d", p_counter);
                        //printf("%d", b        
                        //exit(0);
                        char *new_path;
                        int length_pat = strlen(paths[b]);
                        int size_of_appended;
                        char *appended;
                        if (paths[b][length_pat - 1] == '/') {
                          size_of_appended = 1;
                          appended = malloc(sizeof(char) * size_of_appended);
                          appended[0] = '\0';
                        }
                        else {
                          size_of_appended = 2;
                          appended = malloc(sizeof(char) * size_of_appended);
                          strcpy(appended, "/");
                        }
                        //exit(0);
                        char *total = malloc(sizeof(char) * strlen(command_one) + strlen(appended) + 1);
                        strcpy(total, appended);
                        strcat(total, command_one);
                        //printf("|%s|", total);
                        //Gets path and appends system call
                        char *path = malloc(sizeof(char) * strlen(paths[b]) + 1);
                        //size_t length_copy = strlen(paths[b]);                             
                        strcpy(path, paths[b]);
                        new_path = malloc(sizeof(char) * strlen(path) + strlen(total) + 1);
                        strcpy(new_path, path);
                        strcat(new_path, total);
                        free(appended);
                        free(total);
                        free(path);
                        //printf("|%s|\n", new_path);
                        //PATHS ARE CORRECTLY MADE ABOVE THIS

                        //Actual System call code is below
                        int success = access(new_path, X_OK);
			
			if (success == 0) {
				//Makes new arg array with command
				char **new_args;
                                new_args = malloc(sizeof(char*) * arg_one_counter + 1);
                                char *exec = malloc(sizeof(char) * strlen(command_one) + 1);
                                strcpy(exec, command_one);
                                new_args[0] = exec;
                                for (int r = 0; r < arg_one_counter; r++) {
                                        char *arg_line = malloc(sizeof(char) * strlen(args_one[r]) + 1);
                                        strcpy(arg_line, args_one[r]);
                                        new_args[r + 1] = arg_line;
                                        //printf("'%s'", new_args[r]);
                                }
				int test = execv(new_path, new_args);
				if (test == -1) {
					error();
				}
			}
		}		
		//
	}
	else {
		wait(NULL);
		pid_t child_2 = fork();
		if (child_2 == 0) {
			close(file_d[1]);
			dup2(file_d[0], 0);
			//Execute the second command

			//
			for (int b = 0; b < p_counter; b++) {
                        	//printf("%d", p_counter);
                        	//printf("%d", b        
                        	//exit(0);
                        	char *new_path;
                        	int length_pat = strlen(paths[b]);
                        	int size_of_appended;
                       		char *appended;
                        	if (paths[b][length_pat - 1] == '/') {
                        	  size_of_appended = 1;
                        	  appended = malloc(sizeof(char) * size_of_appended);
                        	  appended[0] = '\0';
                        	}
                        	else {
                        	  size_of_appended = 2;
                        	  appended = malloc(sizeof(char) * size_of_appended);
                        	  strcpy(appended, "/");
                        	}
                        	//exit(0);
                        	char *total = malloc(sizeof(char) * strlen(command_two) + strlen(appended) + 1);
                        	strcpy(total, appended);
                        	strcat(total, command_two);
                        	//printf("|%s|", total);
                        	//Gets path and appends system call
                        	char *path = malloc(sizeof(char) * strlen(paths[b]) + 1);
                        	//size_t length_copy = strlen(paths[b]);                             
                        	strcpy(path, paths[b]);
                        	new_path = malloc(sizeof(char) * strlen(path) + strlen(total) + 1);
                        	strcpy(new_path, path);
                        	strcat(new_path, total);
                        	free(appended);
                        	free(total);
                        	free(path);
                        	//printf("|%s|\n", new_path);
                        	//PATHS ARE CORRECTLY MADE ABOVE THIS
	
	                        //Actual System call code is below
	                        int success = access(new_path, X_OK);
	
	                        if (success == 0) {
	                                //Makes new arg array with command
	                                char **new_args;
	                                new_args = malloc(sizeof(char*) * arg_two_counter + 1);
	                                char *exec = malloc(sizeof(char) * strlen(command_two) + 1);
	                                strcpy(exec, command_two);
	                                new_args[0] = exec;
	                                for (int r = 0; r < arg_one_counter; r++) {
	                                        char *arg_line = malloc(sizeof(char) * strlen(args_two[r]) + 1);
	                                        strcpy(arg_line, args_two[r]);
	                                        new_args[r + 1] = arg_line;
	                                        //printf("'%s'", new_args[r]);
	                                }
	                                int test = execv(new_path, new_args);
	                                if (test == -1) {
	                                        error();
	                                }
	                        }
			}
			//
		}
		else {
			wait(NULL);
		}
	}	
}


void work_on_line(char *line, char **paths, char *path_counter, node_t* head) {
	if (strlen(line) == 0) {
		return;
	}
	
	//Path counter variable
	int p_counter = atoi(path_counter);
	//printf("%d", p_counter);

	//Count number of args
        int num_of_args = 0;
	
	char *something = malloc(sizeof(char) * strlen(line) + 1);
    	strcpy(something, line);
    
    	char *token;
    
    	token = strtok(something, sep);
    	while (token != NULL) {
        	num_of_args++;
        	token = strtok(NULL, sep);
    	}
    	if (num_of_args != 0) {
		--num_of_args;
	}

         //Vars for redirection
         int re_char = 0;
         char *re_file = NULL;

         int file_char;

         char **args;
         char *command;
         char *next;
         int counter = 0;
         
         args = malloc(sizeof(char*) * (num_of_args + 2));
         command = strtok(line, sep);
         //printf("-%s-\n", command);
	 //Check for bad command
	 if (command == NULL) {
		return;
	 }
	 //exit(0);
         next = strtok(NULL, sep);
         while (next != NULL) {
                //Check for piping
		if (strcmp(next, "|") == 0) {
			pipe_control(line, paths, p_counter);
			return;
		}
		//Check to set re_char
                if (re_char == 0 && strcmp(next, ">") == 0) {
                        re_char = 1;
                }
                //Check duplicate re_char symbols
                else if (re_char == 1 && strcmp(next, ">") == 0) {
                        error();
                        return;
                }
                //Check to grab file name
                else if (re_char == 1 && re_file == NULL) {
                        re_file = malloc(sizeof(char) * strlen(next) + 1);
                        strcpy(re_file, next);
                }
                //Check for multiple redirect files
                else if (re_char == 1 && re_file != NULL) {
                        error();
                        return;
                }       
                 args[counter] = malloc(sizeof(char) * strlen(next) + 1);
                 strcpy(args[counter], next);
                 //printf("|%s|", args[counter]);
                 counter++;
                 next = strtok(NULL, sep);
         }
         //args[counter] = malloc(sizeof(char) * 10);
         //free(args[num_of_args]);
         args[num_of_args] = NULL; 
         //After getting args and redirect info, if there is no redirection file, throw error
         if (re_char == 1 && re_file == NULL) {
                error();
                return;
         }
         //If there is redirection, and there is a file, set those args as null
         if (re_char == 1 && re_file != NULL) {
                args[num_of_args - 1] = NULL;
                args[num_of_args - 2] = NULL;
		//Tries to get args right lol
		num_of_args = num_of_args - 2;
         } 
         if (re_char == 1 && re_file == NULL) {
                error();
                return;
         }

	 //Check for exit call
	 if (strcmp(command, "exit") == 0) {
                 exit(0);
         }

	 //Check for path call
	 else if (strcmp(command, "path") == 0) {
                free(paths);
                paths = malloc(sizeof(char*) * num_of_args);
                for (int i = 0; i < num_of_args; i++) {
                        paths[i] = malloc(sizeof(char) * strlen(args[i]));
                        strcpy(paths[i], args[i]);
                        p_counter++;
                }
		char c = p_counter +'0';
		path_counter[0] = c;
         }

	 //Check for cd call
	 else if (strcmp(command, "cd") == 0) {
                if (num_of_args == 1) {
                        //printf("|%s|\n", command);
			char *path = malloc(sizeof(char) * strlen(args[0]));
                        strcpy(path, args[0]);
                        int result = chdir(path);
                        if (result != 0) {
                                error();
                        }
                }
                else {
                        error();
                }
         }

	 //Check for history call
	 else if (strcmp(command, "history") == 0) {
		 
		 if (head->num_of_nodes == 0) {
			return;
		 }	
		 
		 
		 int VALID = 1;
		 int value = 0;
		 //printf("%d", num_of_args);
		 if (num_of_args == 0 ) {
			VALID = 0;
		 }
		 if (num_of_args > 1) {
			error();
			return;
		 }
		 if (VALID) {
			double d;
			char* in = malloc(sizeof(char) * strlen(args[0]) + 1);
			strcpy(in, args[0]);
			
			int num_or_not = 0;
		 	for (int p = 0; in[p] != '\0'; p++) {
				if (isdigit(in[p]) == 0) {
					if (isalpha(in[p]) != 0) {
             					num_or_not = 1; 
            				}
        			} 
			}

			if (num_or_not) {
			       error();
			       return;	
			}
			
			sscanf(in, "%lf", &d);
			//printf("%f", d);
			value = ceilf(d);
			//printf("%d", value);
			if (value < 0 || value == 0) {
				return;
			}

		 }
		 //After getting int value, make sure process worked, and range is correct
		 if (value != 0 && value < (head->num_of_nodes)) {
			int marker = head->num_of_nodes;
                        node_t* tracker = head;
			
			while (tracker->next != NULL) {
                        	if (marker <= value) {
					printf("%s\n", tracker->line);
				}
				marker--;
				tracker = tracker->next;	
                        }
		        if (tracker->line != NULL) {
				printf("%s\n", tracker->line);
			}	
		 }
		 //Print every line of history
		 else {
		 	node_t* temps = head;
		 	while (temps->next != NULL) {
                 		printf("%s\n", temps->line);
                 		temps = temps->next;
                 	}
			if (temps->line != NULL) {
                                printf("%s\n", temps->line);
                        }
		 }
	 }

	 //Check for all other unix calls
	 else {
		//printf("%s", command);
		//exit(0);
		//printf("%s", paths[0]);
		int unknown = 1;
		//printf("%d", unknown);
		//printf("|%d|", p_counter);
                //printf("|%s|\n", command);
		//printf("%d", p_counter);
		//printf("|%d|", p_counter);
		//exit(0);
		int e = -1;
                for (int b = 0; b < p_counter; b++) {
                        e++;
			//printf("%d", p_counter);
			//printf("%d", b	
			//exit(0);
			char *new_path;
			int length_pat = strlen(paths[b]);
                        int size_of_appended;
                        char *appended;
			if (paths[b][length_pat - 1] == '/') {
                          size_of_appended = 1; 
                          appended = malloc(sizeof(char) * size_of_appended);
                          appended[0] = '\0';
                        }
                        else {
                          size_of_appended = 2;
                          appended = malloc(sizeof(char) * size_of_appended);
                          strcpy(appended, "/");
                        }
			//exit(0);
                        char *total = malloc(sizeof(char) * strlen(command) + strlen(appended) + 1);
                        strcpy(total, appended);
                        strcat(total, command);
                        //printf("|%s|", total);
                        //Gets path and appends system call
                        char *path = malloc(sizeof(char) * strlen(paths[b]) + 1);
                        //size_t length_copy = strlen(paths[b]);                             
                        strcpy(path, paths[b]);
                        new_path = malloc(sizeof(char) * strlen(path) + strlen(total) + 1);
                        strcpy(new_path, path);
                        strcat(new_path, total);
                        free(appended);
                        free(total);
                        free(path);
                        //printf("|%s|\n", new_path);
                        //PATHS ARE CORRECTLY MADE ABOVE THIS
			
			//Actual System call code is below
			int success = access(new_path, X_OK);
			//printf("|%s|\n", new_path);
			//printf("%s", command);
			//if (e > 0) {
			//	exit(0);
			//}
			if (success == 0) {
                                //printf("?");
				//exit(0);
				unknown = 0;
				int proc = fork();
				
				//Start
				if (proc < 0) {
					//printf("%s", "12");
					error();
					//printf("either this oen");
				}
				else if (proc == 0) {
					//printf("?2");
					char **new_args;
                                        new_args = malloc(sizeof(char*) * num_of_args + 1);
                                        char *exec = malloc(sizeof(char) * strlen(command) + 1);
                                        strcpy(exec, command);
                                        new_args[0] = exec;
                                        for (int r = 0; r < num_of_args; r++) {
                                                char *arg_line = malloc(sizeof(char) * strlen(args[r]) + 1);
                                                strcpy(arg_line, args[r]);
                                                new_args[r + 1] = arg_line;
                                                //printf("'%s'", new_args[r]);
						//fflush(NULL);
						//printf("|%s|", args[r]);
                                         }
					//printf("?");
					
					 //printf("'%s'", new_args[num_of_args]);
					 //fflush(NULL);

                                         /*
                                         *
                                         */
					if (re_char) {
                				file_char = open(re_file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                				if (file_char != -1) {
                				        dup2(file_char, 1);
                				        dup2(file_char, 2);
                				        close(file_char);
                				}
                				else {
                        				error();
                				}
         				}
//printf("HELLO A");
/*
					int i = 0;
				        while ( new_args[i] != NULL ) {

						printf("-%s-", new_args[i]);
i++;
fflush(NULL);



if ( i == 100 ) {
//exit(1);
}
					//}
}*/
                                        //printf("%d", num_of_args);
					//fflush(NULL);
					int test = execv(new_path, new_args);
					//exit(0);
					if (test == -1) {
                                                //printf("%s", new_path);
						//pid_t proccc = getpid();
						//printf("[%d]", proccc);						
						//printf("%s", "11");
						//printf("Error 0");
						//error();
                                                return;
                                        }
                                        //break;				
				}
				else {
					//printf("or this one");
					//pid_t pro = getpid();
					//printf("{%d}", pro);
					wait(NULL);
					//return;
					break;
				}
				//End
		
                        }
		if (e > 0) {
                        return;
                }
		//printf("%d", p_counter);
		//printf("'%d'", b);
		//printf("IM HERE");
		//exit(0);
                }
		if (unknown) {
			//printf("GONNA BE AN ERROR SOON");
			//exit(0);
			//printf("%s", "1221");
			error();
		}
		//printf("NOW HERE");
        }
}

/*
 * Adds a new line of command input to the history struct
 */
node_t* add_line(node_t* head, char *line) {
        //printf("|%s|", line);
       	node_t* temp = head;
        if (head->line == NULL) {
                //strcpy(head->line, line);
                head->line = malloc(sizeof(char) * strlen(line) + 1);
	        strcpy(head->line, line);	
		head->num_of_nodes = 1;
        }
        else {
                while (temp->next != NULL) {
                        temp = temp->next; 
                }
                int size = head->num_of_nodes + 1;
		node_t* to_add = malloc(sizeof(node_t));
                //strcpy(to_add->line, line);
		to_add->line = malloc(sizeof(char) * strlen(line) + 1);
	        strcpy(to_add->line, line);	
                to_add->next = NULL;
                to_add->num_of_nodes = size;
                head->num_of_nodes = size;
                temp->next = to_add;
        }
        return head;
}

int main(int argc, char **argv) {
        
	if (argc > 2) {
		error();
		exit(1);
	}

	//Create vars needed for path control
        char *DEFAULT_PATH = "/bin";
        char **paths = malloc(sizeof(char*));
        paths[0] = DEFAULT_PATH;
        int p_counter = 1;
	char *path_counter = malloc(sizeof(char) + 1);
	char c = p_counter + '0';
        path_counter[0] = c;
        path_counter[1] = '\0';
	
	//Struct for command history
        node_t* head = malloc(sizeof(node_t));
        if (head == NULL) {
                error();
                exit(1);
        }
        head->line = NULL;
        head->next = NULL;
        head->num_of_nodes = 0;
	
	//Read from STDIN
	if (argc == 1) {
                char *line;
                size_t n = 0;
                //Main Loop of bash
                while (1) {
                        printf("%s", "wish> ");
                        fflush(NULL);
			//Input is valid
                        int result = getline(&line, &n, stdin);
			if (result != -1) {
				//printf("%d", path_counter[0]);
				
				//Checking for empty line
        			size_t len = strlen(line);
        			if (len > 0 && line[len - 1] == '\n') {
        			        line[--len] = '\0';
        			}
				if (strlen(line) != 0) {
					add_line(head, line);
				}
				work_on_line(line, paths, path_counter, head);
                        }
                }
        }
        //Read from FILE
        else if (argc == 2) {
                FILE *fp = fopen(argv[1], "r");
                if (fp == NULL) {
                        error();
			exit(1);
                }
                //File is opened
                char *line;
                size_t n = 0;
                //Main loop to read from the file
                while (1) {
                        int result = getline(&line, &n, fp);
                        //Input is valid
                        if (result != -1) {
				size_t len = strlen(line);
                                if (len > 0 && line[len - 1] == '\n') {
                                        line[--len] = '\0';
                                }
                                if (strlen(line) != 0) {
                                        add_line(head, line);
                                }
                                work_on_line(line, paths, path_counter, head);
                        }
                }
        }
        else {
                error();
        }
        return 0;
}
