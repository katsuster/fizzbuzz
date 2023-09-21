#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/uio.h>

//512KB
#define BUFSIZE      (CHUNKSIZE * 16)
#define CHUNKSIZE    (4096 * 8)

#define PARTS    3

char buf[BUFSIZE + 256];
unsigned int rp, wp;

#define wrap(wp)    ((wp) & (BUFSIZE - 1))

char tab2[4 * 10000];

ssize_t vwrite(int fd, void *buf, size_t count)
{
	struct iovec iov;
	ssize_t n;

	iov.iov_base = buf;
	iov.iov_len = count;

	while (iov.iov_len > 0) {
		n = vmsplice(1, &iov, 1, 0);
		iov.iov_base += n;
		iov.iov_len -= n;
	}

	return count;
}

int gentbl(void)
{
	int i;

	for (i = 0; i < 10000; i++) {
		tab2[i * 4 + 0] = '0' + (i / 1000) % 10;
		tab2[i * 4 + 1] = '0' + (i / 100) % 10;
		tab2[i * 4 + 2] = '0' + (i / 10) % 10;
		tab2[i * 4 + 3] = '0' + i % 10;
	}

	return 0;
}

void my_itoa9_1(char *buf, unsigned int i)
{
	char *buf1 = buf + 9;
	char *buf2 = buf + 19;
	char *buf3 = buf + 34;
	char *buf4 = buf + 54;
	char *buf5 = buf + 64;
	const char *str;
	unsigned long long sss;

	str = &tab2[(i % 10000) * 4];
	i /= 10000;
	sss = *((unsigned int *)str);
	sss <<= 32;

	str = &tab2[(i % 10000) * 4];
	i /= 10000;
	sss |= *((unsigned int *)str);

	*((unsigned long long *)&buf1[1]) = sss;
	*((unsigned long long *)&buf2[1]) = sss;
	*((unsigned long long *)&buf3[1]) = sss;
	*((unsigned long long *)&buf4[1]) = sss;
	*((unsigned long long *)&buf5[1]) = sss;

	buf1[8] = '1';
	buf2[8] = '2';
	buf3[8] = '4';
	buf4[8] = '7';
	buf5[8] = '8';

	str = &tab2[(i % 10000) * 4];
	buf1[0] = buf2[0] = buf3[0] = buf4[0] = buf5[0] = str[3];
}

void my_itoa9_2(char *buf, unsigned int i)
{
	char *buf1 = buf + 84;
	char *buf2 = buf + 99;
	char *buf3 = buf + 109;
	char *buf4 = buf + 128;
	char *buf5 = buf + 138;
	char *buf6 = buf + 153;
	const char *str;
	unsigned long long sss;

	str = &tab2[(i % 10000) * 4];
	i /= 10000;
	sss = *((unsigned int *)str);
	sss <<= 32;

	str = &tab2[(i % 10000) * 4];
	i /= 10000;
	sss |= *((unsigned int *)str);

	*((unsigned long long *)&buf1[1]) = sss;
	*((unsigned long long *)&buf2[1]) = sss;
	*((unsigned long long *)&buf3[1]) = sss;
	*((unsigned long long *)&buf4[1]) = sss;
	*((unsigned long long *)&buf5[1]) = sss;
	*((unsigned long long *)&buf6[1]) = sss;

	buf1[8] = '1';
	buf2[8] = '3';
	buf3[8] = '4';
	buf4[8] = '6';
	buf5[8] = '7';
	buf6[8] = '9';

	str = &tab2[(i % 10000) * 4];
	buf1[0] = buf2[0] = buf3[0] = buf4[0] = buf5[0] = buf6[0] = str[3];
}

void my_itoa9_3(char *buf, unsigned int i)
{
	char *buf1 = buf + 173;
	char *buf2 = buf + 183;
	char *buf3 = buf + 203;
	char *buf4 = buf + 218;
	char *buf5 = buf + 228;
	const char *str;
	unsigned long long sss;

	str = &tab2[(i % 10000) * 4];
	i /= 10000;
	sss = *((unsigned int *)str);
	sss <<= 32;

	str = &tab2[(i % 10000) * 4];
	i /= 10000;
	sss |= *((unsigned int *)str);

	*((unsigned long long *)&buf1[1]) = sss;
	*((unsigned long long *)&buf2[1]) = sss;
	*((unsigned long long *)&buf3[1]) = sss;
	*((unsigned long long *)&buf4[1]) = sss;
	*((unsigned long long *)&buf5[1]) = sss;

	buf1[8] = '2';
	buf2[8] = '3';
	buf3[8] = '6';
	buf4[8] = '8';
	buf5[8] = '9';

	str = &tab2[(i % 10000) * 4];
	buf1[0] = buf2[0] = buf3[0] = buf4[0] = buf5[0] = str[3];
}

void my_itoa10_1(char *buf, unsigned int i)
{
	char *buf1 = buf + 9;
	char *buf2 = buf + 20;
	char *buf3 = buf + 36;
	char *buf4 = buf + 57;
	char *buf5 = buf + 68;
	const char *str;
	unsigned long long sss;

	str = &tab2[(i % 10000) * 4];
	i /= 10000;
	sss = *((unsigned int *)str);
	sss <<= 32;

	str = &tab2[(i % 10000) * 4];
	i /= 10000;
	sss |= *((unsigned int *)str);

	*((unsigned long long *)&buf1[2]) = sss;
	*((unsigned long long *)&buf2[2]) = sss;
	*((unsigned long long *)&buf3[2]) = sss;
	*((unsigned long long *)&buf4[2]) = sss;
	*((unsigned long long *)&buf5[2]) = sss;

	buf1[9] = '1';
	buf2[9] = '2';
	buf3[9] = '4';
	buf4[9] = '7';
	buf5[9] = '8';

	str = &tab2[(i % 10000) * 4];
	buf1[0] = buf2[0] = buf3[0] = buf4[0] = buf5[0] = str[2];
	buf1[1] = buf2[1] = buf3[1] = buf4[1] = buf5[1] = str[3];
}

void my_itoa10_2(char *buf, unsigned int i)
{
	char *buf1 = buf + 89;
	char *buf2 = buf + 105;
	char *buf3 = buf + 116;
	char *buf4 = buf + 136;
	char *buf5 = buf + 147;
	char *buf6 = buf + 163;
	const char *str;
	unsigned long long sss;

	str = &tab2[(i % 10000) * 4];
	i /= 10000;
	sss = *((unsigned int *)str);
	sss <<= 32;

	str = &tab2[(i % 10000) * 4];
	i /= 10000;
	sss |= *((unsigned int *)str);

	*((unsigned long long *)&buf1[2]) = sss;
	*((unsigned long long *)&buf2[2]) = sss;
	*((unsigned long long *)&buf3[2]) = sss;
	*((unsigned long long *)&buf4[2]) = sss;
	*((unsigned long long *)&buf5[2]) = sss;
	*((unsigned long long *)&buf6[2]) = sss;

	buf1[9] = '1';
	buf2[9] = '3';
	buf3[9] = '4';
	buf4[9] = '6';
	buf5[9] = '7';
	buf6[9] = '9';

	str = &tab2[(i % 10000) * 4];
	buf1[0] = buf2[0] = buf3[0] = buf4[0] = buf5[0] = buf6[0] = str[2];
	buf1[1] = buf2[1] = buf3[1] = buf4[1] = buf5[1] = buf6[1] = str[3];
}

void my_itoa10_3(char *buf, unsigned int i)
{
	char *buf1 = buf + 184;
	char *buf2 = buf + 195;
	char *buf3 = buf + 216;
	char *buf4 = buf + 232;
	char *buf5 = buf + 243;
	const char *str;
	unsigned long long sss;

	str = &tab2[(i % 10000) * 4];
	i /= 10000;
	sss = *((unsigned int *)str);
	sss <<= 32;

	str = &tab2[(i % 10000) * 4];
	i /= 10000;
	sss |= *((unsigned int *)str);

	*((unsigned long long *)&buf1[2]) = sss;
	*((unsigned long long *)&buf2[2]) = sss;
	*((unsigned long long *)&buf3[2]) = sss;
	*((unsigned long long *)&buf4[2]) = sss;
	*((unsigned long long *)&buf5[2]) = sss;

	buf1[9] = '2';
	buf2[9] = '3';
	buf3[9] = '6';
	buf4[9] = '8';
	buf5[9] = '9';

	str = &tab2[(i % 10000) * 4];
	buf1[0] = buf2[0] = buf3[0] = buf4[0] = buf5[0] = str[2];
	buf1[1] = buf2[1] = buf3[1] = buf4[1] = buf5[1] = str[3];
}

int my_itoa(char *buf, unsigned int i)
{
	int part[PARTS] = {0};
	int partp;
	size_t pos = 0;

	for (partp = PARTS - 1; partp >= 0; partp--) {
		part[partp] = i % 10000;

		i /= 10000;
		if (i == 0) {
			break;
		}
	}

	{
		const char *str;

		str = &tab2[part[partp] * 4];
		if (part[partp] >= 1000) {
			buf[pos++] = str[0];
		}
		if (part[partp] >= 100) {
			buf[pos++] = str[1];
		}
		if (part[partp] >= 10) {
			buf[pos++] = str[2];
		}
		buf[pos++] = str[3];

		partp++;
	}

	for (; partp < PARTS; partp++) {
		const char *str;

		str = &tab2[part[partp] * 4];
		buf[pos++] = str[0];
		buf[pos++] = str[1];
		buf[pos++] = str[2];
		buf[pos++] = str[3];
	}

	return pos;
}

int fizzbuzz(unsigned int i)
{
	unsigned int wp_before = wp;
	char *p = &buf[wrap(wp)];

	if (i % 3 == 0) {
		*p++ = 'F';
		*p++ = 'i';
		*p++ = 'z';
		*p++ = 'z';
		wp += 4;
	}
	if (i % 5 == 0) {
		*p++ = 'B';
		*p++ = 'u';
		*p++ = 'z';
		*p++ = 'z';
		wp += 4;
	}
	if (wp_before == wp) {
		int r;
		r = my_itoa(p, i);
		p += r;
		wp += r;
	}

	*p++ = '\n';
	wp++;

	if (wrap(wp) < wrap(wp_before)) {
		memcpy(&buf[0], &buf[BUFSIZE], wrap(wp));
	}
	if (wp - rp >= CHUNKSIZE) {
		vwrite(1, &buf[wrap(rp)], CHUNKSIZE);
		rp += CHUNKSIZE;
	}

	return i + 1;
}

char tmp9[] =
"FizzBuzz\n.........\n.........\n"
"Fizz\n.........\nBuzz\n"
"Fizz\n.........\n.........\n"
"Fizz\nBuzz\n.........\n"
"Fizz\n.........\n.........\n"
"FizzBuzz\n.........\n.........\n"
"Fizz\n.........\nBuzz\n"
"Fizz\n.........\n.........\n"
"Fizz\nBuzz\n.........\n"
"Fizz\n.........\n.........\n";

char tmp10[] =
"FizzBuzz\n..........\n..........\n"
"Fizz\n..........\nBuzz\n"
"Fizz\n..........\n..........\n"
"Fizz\nBuzz\n..........\n"
"Fizz\n..........\n..........\n"
"FizzBuzz\n..........\n..........\n"
"Fizz\n..........\nBuzz\n"
"Fizz\n..........\n..........\n"
"Fizz\nBuzz\n..........\n"
"Fizz\n..........\n..........\n";

int main(int argc, char *argv[])
{
	fcntl(1, F_SETPIPE_SZ, BUFSIZE / 2);

	gentbl();

	for (unsigned int i = 1; i < 0xffffffff;) {
		if (1000000020UL <= i && i <= 4294967250UL) {
			unsigned int wp_before = wp;
			char *p = &buf[wrap(wp)];

			memcpy(p, tmp10, sizeof(tmp10));
			my_itoa10_1(p, i);
			my_itoa10_2(p, i + 10);
			my_itoa10_3(p, i + 20);

			wp += 127 * 2;

			if (wrap(wp) < wrap(wp_before)) {
				memcpy(&buf[0], &buf[BUFSIZE], wrap(wp));
			}
			if (wp - rp >= CHUNKSIZE) {
				vwrite(1, &buf[wrap(rp)], CHUNKSIZE);
				rp += CHUNKSIZE;
			}

			i += 30;
		} else if (100000020UL <= i && i <= 999999960UL) {
			unsigned int wp_before = wp;
			char *p = &buf[wrap(wp)];

			memcpy(p, tmp9, sizeof(tmp9));
			my_itoa9_1(p, i);
			my_itoa9_2(p, i + 10);
			my_itoa9_3(p, i + 20);

			wp += 119 * 2;

			if (wrap(wp) < wrap(wp_before)) {
				memcpy(&buf[0], &buf[BUFSIZE], wrap(wp));
			}
			if (wp - rp >= CHUNKSIZE) {
				vwrite(1, &buf[wrap(rp)], CHUNKSIZE);
				rp += CHUNKSIZE;
			}

			i += 30;
		} else {
			i = fizzbuzz(i);
		}
	}

	vwrite(1, &buf[wrap(rp)], wp - rp);

	return 0;
}
