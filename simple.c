#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	for (unsigned int i = 1; i < 0xffffffff; i++) {
		if (i % 3 == 0 && i % 5 == 0) {
			printf("FizzBuzz\n");
		} else if (i % 3 == 0) {
			printf("Fizz\n");
		} else if (i % 5 == 0) {
			printf("Buzz\n");
		} else {
			printf("%u\n", i);
		}
	}

	return 0;
}
