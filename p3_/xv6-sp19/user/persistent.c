#include "types.h"
#include "stat.h"
#include "user.h"

void
test_failed()
{
	printf(1, "TEST FAILED\n");
	exit();
}

void
test_passed()
{
 printf(1, "TEST PASSED\n");
 exit();
}

int
main(int argc, char *argv[])
{	
	char *ptr;
	int i;
	char arr[6] = "CS537";
	
	int pid = fork();
	if (pid < 0) {
   printf(1, "0");
		test_failed();
	}	
	else if (pid == 0) {
   //printf(1, "CHILD");
		ptr = shmget(2);
		if (ptr == NULL) {
      printf(1, "1");
			test_failed();
		}
		
		for (i = 0; i < 5; i++) {
       printf(1, "[%c]", arr[i]);
			 *(ptr+i) = arr[i];
        printf(1, "%c", *(ptr + i));
		}

		exit();
	}
	else {
   //printf(1, "PARENT");
		wait();
		
		ptr = shmget(2);
		if (ptr == NULL) {
   printf(1, "2");
			test_failed();
		}
   for (i = 0; i < 5; i++) {	
     printf(1, "{%c}", *(ptr + i));
   }
   for (i = 0; i < 5; i++) {	
     printf(1, "|%c|", arr[i]);
   }
	
		for (i = 0; i < 5; i++) {		
			if (*(ptr+i) != arr[i]) {
      printf(1, "%d", (3 + i));
				test_failed();
			}
		}
	}
	
	test_passed();
	exit();
}
