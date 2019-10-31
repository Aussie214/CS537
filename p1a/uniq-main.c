 //Read files in
        else {
                //Loop through files
                for (int i = 1; i < argc; i++) {
                        FILE *fp = fopen(argv[i], "r");
                        //Check file is valid and can be opened
                        if (fp == NULL) {
                                printf("%s", "my-uniq: cannot open file");
                                exit(1);
                        }
                        char *line_one;
                        ssize_t error_one;
                        size_t length_one = 0;
                        char *line_two;
                        ssize_t error_two;
                        size_t length_two;
                        char *match;
                        while ((error_one = getline(&line_one, &length_one, fp) != -1)) {
                                //if (strcmp(line_one, match) == 0) {

                                //}
                                //else {
                                        if ((error_two = getline(&line_two, &length_two, fp) != -1)) {
                                                //Checking if lines are the same
                                                if (strcmp(line_one, line_two) == 0) {
                                                        printf("%s", line_one);
                                                        match = line_two;
                                                }
                                                else {
                                                        printf("%s", line_one);
                                                        printf("%s", line_two);
                                                }
                                        }
                                //}
                        }

                }
        }

