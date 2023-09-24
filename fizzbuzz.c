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

void inline my_itoa9_1(char *buf, unsigned int i)
{
	char *buf1 = buf + 0;
	char *buf2 = buf + 10;
	char *buf3 = buf + 25;
	char *buf4 = buf + 45;
	char *buf5 = buf + 55;
	const char *str;
	unsigned long long sss;

	str = &tab2[(i % 10000) * 4];
	i /= 10000;
	sss = *((unsigned int *)str);
	sss <<= 32;

	str = &tab2[i * 4];
	sss |= *((unsigned int *)str);

	*((unsigned long long *)&buf1[0]) = sss;
	*((unsigned long long *)&buf2[0]) = sss;
	*((unsigned long long *)&buf3[0]) = sss;
	*((unsigned long long *)&buf4[0]) = sss;
	*((unsigned long long *)&buf5[0]) = sss;
}

void inline my_itoa9_2(char *buf, unsigned int i)
{
	char *buf1 = buf + 75;
	char *buf2 = buf + 90;
	char *buf3 = buf + 100;
	char *buf4 = buf + 119;
	char *buf5 = buf + 129;
	char *buf6 = buf + 144;
	const char *str;
	unsigned long long sss;

	str = &tab2[(i % 10000) * 4];
	i /= 10000;
	sss = *((unsigned int *)str);
	sss <<= 32;

	str = &tab2[i * 4];
	sss |= *((unsigned int *)str);

	*((unsigned long long *)&buf1[0]) = sss;
	*((unsigned long long *)&buf2[0]) = sss;
	*((unsigned long long *)&buf3[0]) = sss;
	*((unsigned long long *)&buf4[0]) = sss;
	*((unsigned long long *)&buf5[0]) = sss;
	*((unsigned long long *)&buf6[0]) = sss;
}

void inline my_itoa9_3(char *buf, unsigned int i)
{
	char *buf1 = buf + 164;
	char *buf2 = buf + 174;
	char *buf3 = buf + 194;
	char *buf4 = buf + 209;
	char *buf5 = buf + 219;
	const char *str;
	unsigned long long sss;

	str = &tab2[(i % 10000) * 4];
	i /= 10000;
	sss = *((unsigned int *)str);
	sss <<= 32;

	str = &tab2[i * 4];
	sss |= *((unsigned int *)str);

	*((unsigned long long *)&buf1[0]) = sss;
	*((unsigned long long *)&buf2[0]) = sss;
	*((unsigned long long *)&buf3[0]) = sss;
	*((unsigned long long *)&buf4[0]) = sss;
	*((unsigned long long *)&buf5[0]) = sss;
}

void inline my_itoa10_1(char *buf, unsigned int i, unsigned int j)
{
	char *buf1 = buf + 0;
	char *buf2 = buf + 11;
	char *buf3 = buf + 27;
	char *buf4 = buf + 48;
	char *buf5 = buf + 59;
	const char *str;
	unsigned long long sss;

	str = &tab2[(i % 10000) * 4];
	i /= 10000;
	sss = *((unsigned int *)str);
	sss <<= 32;

	str = &tab2[i * 4];
	sss |= *((unsigned int *)str);

	*((unsigned long long *)&buf1[1]) = sss;
	*((unsigned long long *)&buf2[1]) = sss;
	*((unsigned long long *)&buf3[1]) = sss;
	*((unsigned long long *)&buf4[1]) = sss;
	*((unsigned long long *)&buf5[1]) = sss;

	buf1[0] = j + '0';
	buf2[0] = j + '0';
	buf3[0] = j + '0';
	buf4[0] = j + '0';
	buf5[0] = j + '0';
}

void inline my_itoa10_2(char *buf, unsigned int i, unsigned int j)
{
	char *buf1 = buf + 80;
	char *buf2 = buf + 96;
	char *buf3 = buf + 107;
	char *buf4 = buf + 127;
	char *buf5 = buf + 138;
	char *buf6 = buf + 154;
	const char *str;
	unsigned long long sss;

	str = &tab2[(i % 10000) * 4];
	i /= 10000;
	sss = *((unsigned int *)str);
	sss <<= 32;

	str = &tab2[i * 4];
	sss |= *((unsigned int *)str);

	*((unsigned long long *)&buf1[1]) = sss;
	*((unsigned long long *)&buf2[1]) = sss;
	*((unsigned long long *)&buf3[1]) = sss;
	*((unsigned long long *)&buf4[1]) = sss;
	*((unsigned long long *)&buf5[1]) = sss;
	*((unsigned long long *)&buf6[1]) = sss;

	buf1[0] = j + '0';
	buf2[0] = j + '0';
	buf3[0] = j + '0';
	buf4[0] = j + '0';
	buf5[0] = j + '0';
	buf6[0] = j + '0';
}

void inline my_itoa10_3(char *buf, unsigned int i, unsigned int j)
{
	char *buf1 = buf + 175;
	char *buf2 = buf + 186;
	char *buf3 = buf + 207;
	char *buf4 = buf + 223;
	char *buf5 = buf + 234;
	const char *str;
	unsigned long long sss;

	str = &tab2[(i % 10000) * 4];
	i /= 10000;
	sss = *((unsigned int *)str);
	sss <<= 32;

	str = &tab2[i * 4];
	sss |= *((unsigned int *)str);

	*((unsigned long long *)&buf1[1]) = sss;
	*((unsigned long long *)&buf2[1]) = sss;
	*((unsigned long long *)&buf3[1]) = sss;
	*((unsigned long long *)&buf4[1]) = sss;
	*((unsigned long long *)&buf5[1]) = sss;

	buf1[0] = j + '0';
	buf2[0] = j + '0';
	buf3[0] = j + '0';
	buf4[0] = j + '0';
	buf5[0] = j + '0';
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

	buf[pos++] = '\n';

	return pos;
}

int my_itoa_n(char *buf, unsigned int i)
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

int out_fizz(char *buf)
{
	const char *str = "Fizz\n   ";
	const unsigned long long *s = (const void *)str;
	unsigned long long *b = (void *)buf;

	*b = *s;

	return 5;
}

int out_buzz(char *buf)
{
	const char *str = "Buzz\n   ";
	const unsigned long long *s = (const void *)str;
	unsigned long long *b = (void *)buf;

	*b = *s;

	return 5;
}

int out_fb(char *buf)
{
	const char *str = "FizzBuzz";
	const unsigned long long *s = (const void *)str;
	unsigned long long *b = (void *)buf;

	*b = *s;
	buf[8] = '\n';

	return 9;
}

int out_fandb(char *buf)
{
	const char *str = "Fizz\nBuz";
	const char *str2 = "z\n";
	const unsigned long long *s = (const void *)str;
	const unsigned short *s2 = (const void *)str2;
	unsigned long long *b = (void *)buf;
	unsigned short *b2 = (void *)buf + 8;

	*b = *s;
	*b2 = *s2;

	return 10;
}

int out_bandf(char *buf)
{
	const char *str = "Buzz\nFiz";
	const char *str2 = "z\n";
	const unsigned long long *s = (const void *)str;
	const unsigned short *s2 = (const void *)str2;
	unsigned long long *b = (void *)buf;
	unsigned short *b2 = (void *)buf + 8;

	*b = *s;
	*b2 = *s2;

	return 10;
}

void fizzbuzz30(unsigned int i)
{
	unsigned int wp_before = wp;
	char *p = &buf[wrap(wp)];
	char *p_s = p;
	int r;

	        r = my_itoa(p, i); p += r;
	i += 1; r = my_itoa(p, i); p += r;
	        r = out_fizz(p);   p += r;
	i += 2; r = my_itoa(p, i); p += r;
	        r = out_bandf(p);  p += r;
	i += 3; r = my_itoa(p, i); p += r;
	i += 1; r = my_itoa(p, i); p += r;
	        r = out_fandb(p);  p += r;

	i += 3; r = my_itoa(p, i); p += r;
	        r = out_fizz(p);   p += r;
	i += 2; r = my_itoa(p, i); p += r;
	i += 1; r = my_itoa(p, i); p += r;
	        r = out_fb(p);     p += r;
	i += 2; r = my_itoa(p, i); p += r;
	i += 1; r = my_itoa(p, i); p += r;
	        r = out_fizz(p);   p += r;
	i += 2; r = my_itoa(p, i); p += r;
	        r = out_bandf(p);  p += r;

	i += 3; r = my_itoa(p, i); p += r;
	i += 1; r = my_itoa(p, i); p += r;
	        r = out_fandb(p);  p += r;
	i += 3; r = my_itoa(p, i); p += r;
	        r = out_fizz(p);   p += r;
	i += 2; r = my_itoa(p, i); p += r;
	i += 1; r = my_itoa(p, i); p += r;
	        r = out_fb(p);     p += r;

	wp += p - p_s;

	if (wrap(wp) < wrap(wp_before)) {
		memcpy(&buf[0], &buf[BUFSIZE], wrap(wp));
	}
	if (wp - rp >= CHUNKSIZE) {
		vwrite(1, &buf[wrap(rp)], CHUNKSIZE);
		rp += CHUNKSIZE;
	}
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
		r = my_itoa_n(p, i);
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

const char tmp9[] =
"........1\n........2\n"
"Fizz\n........4\nBuzz\n"
"Fizz\n........7\n........8\n"
"Fizz\nBuzz\n........1\n"
"Fizz\n........3\n........4\n"
"FizzBuzz\n........6\n........7\n"
"Fizz\n........9\nBuzz\n"
"Fizz\n........2\n........3\n"
"Fizz\nBuzz\n........6\n"
"Fizz\n........8\n........9\n"
"FizzBuzz\n";

const char tmp10[] =
".........1\n.........2\n"
"Fizz\n.........4\nBuzz\n"
"Fizz\n.........7\n.........8\n"
"Fizz\nBuzz\n.........1\n"
"Fizz\n.........3\n.........4\n"
"FizzBuzz\n.........6\n.........7\n"
"Fizz\n.........9\nBuzz\n"
"Fizz\n.........2\n.........3\n"
"Fizz\nBuzz\n.........6\n"
"Fizz\n.........8\n.........9\n"
"FizzBuzz\n";

int main(int argc, char *argv[])
{
	unsigned int i, t;

	fcntl(1, F_SETPIPE_SZ, BUFSIZE / 2);

	gentbl();

	for (i = 1; i < 100000020UL; i += 30) {
		fizzbuzz30(i);
	}

	t = i / 10;
	for (; i < 999999990UL; i += 30, t += 3) {
		unsigned int wp_before = wp;
		char *p = &buf[wrap(wp)];

		memcpy(p, tmp9, sizeof(tmp9));
		my_itoa9_1(p, t);
		my_itoa9_2(p, t + 1);
		my_itoa9_3(p, t + 2);

		wp += 119 * 2;

		if (wrap(wp) < wrap(wp_before)) {
			memcpy(&buf[0], &buf[BUFSIZE], wrap(wp));
		}
		if (wp - rp >= CHUNKSIZE) {
			vwrite(1, &buf[wrap(rp)], CHUNKSIZE);
			rp += CHUNKSIZE;
		}
	}

	fizzbuzz30(i);
	i += 30;

	t = i / 10;
	for (; i < 4294967250UL; i += 30, t += 3) {
		unsigned int u, v;
		unsigned int wp_before = wp;
		char *p = &buf[wrap(wp)];

		memcpy(p, tmp10, sizeof(tmp10));
		u = t % 100000000;
		v = t / 100000000;
		my_itoa10_1(p, u, v);
		switch (u) {
		case 99999999:
			my_itoa10_2(p, 0, v + 1);
			my_itoa10_3(p, 1, v + 1);
			break;
		case 99999998:
			my_itoa10_2(p, u + 1, v);
			my_itoa10_3(p, 0, v + 1);
			break;
		default:
			my_itoa10_2(p, u + 1, v);
			my_itoa10_3(p, u + 2, v);
			break;
		}

		wp += 127 * 2;

		if (wrap(wp) < wrap(wp_before)) {
			memcpy(&buf[0], &buf[BUFSIZE], wrap(wp));
		}
		if (wp - rp >= CHUNKSIZE) {
			vwrite(1, &buf[wrap(rp)], CHUNKSIZE);
			rp += CHUNKSIZE;
		}
	}

	for (; i < 0xffffffff;) {
		i = fizzbuzz(i);
	}

	vwrite(1, &buf[wrap(rp)], wp - rp);

	return 0;
}
