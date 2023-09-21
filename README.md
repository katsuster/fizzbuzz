# How to use

Please compile directly.

$ gcc -O3 simple.c -o simple
$ gcc -O3 fizzbuzz.c -o fizzbuzz


# How to check speed

For example.

$ ./fizzbuzz | pv > /dev/null


# How to test

Please use test.c.

$ gcc -O3 test.c -o test
$ ./fizzbuzz | ./test
