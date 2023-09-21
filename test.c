#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_expected(char *buf, size_t bufsz, unsigned int i)
{
	if (i % 3 == 0 && i % 5 == 0) {
		snprintf(buf, bufsz, "FizzBuzz\n");
	} else if (i % 3 == 0) {
		snprintf(buf, bufsz, "Fizz\n");
	} else if (i % 5 == 0) {
		snprintf(buf, bufsz, "Buzz\n");
	} else {
		snprintf(buf, bufsz, "%u\n", i);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	char *inbufp = NULL;
	size_t insz = 0;

	for (unsigned int i = 1; i < 0xffffffff; i++) {
		char expected[64];

		get_expected(expected, sizeof(expected), i);
		getline(&inbufp, &insz, stdin);

		if ((i & 0xffffff) == 0) {
			printf("\r%u      ", i);
			fflush(stdout);
		}
		if (strcmp(expected, inbufp) != 0) {
			printf("\n");
			printf("Not matched in %u\n", i);
			printf("  expected: %s\n", expected);
			printf("  input   : %s\n", inbufp);
			return 1;
		}
	}

	printf("\nOK\n");

	return 0;
}
