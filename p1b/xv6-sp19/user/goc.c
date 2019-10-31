#include "types.h"
#include "stat.h"
#include "user.h"
int main(int argc, char** argv) {
	int count;
	count = getopenedcount();
	printf(1, "Open has ben called %d times.", count);
        open("1", 1);
	count = getopenedcount();
        printf(1, "Open has ben called %d times.", count);
	open("1", 1);
	count = getopenedcount();
        printf(1, "Open has ben called %d times.", count);	
	exit();
}
