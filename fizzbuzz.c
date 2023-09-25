#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/uio.h>

//512KB
#define BUFSIZE      (CHUNKSIZE * 4)
#define CHUNKSIZE    (4096 * 32)

#define PARTS    3

char buf[BUFSIZE + 256];
unsigned int rp, wp;

#define wrap(wp)    ((wp) & (BUFSIZE - 1))

char tab[4 * 10000];
unsigned int *tab2 = (void *)tab;

ssize_t vwrite(int fd, void *buf, size_t count)
{
	struct iovec iov;
	ssize_t n;

	iov.iov_base = buf;
	iov.iov_len = count;

	while (iov.iov_len > 0) {
		n = vmsplice(1, &iov, 1, 0);
		if (n < 0) {
			perror("vmsplice");
			exit(1);
		}
		iov.iov_base += n;
		iov.iov_len -= n;
	}

	return count;
}

int gentbl(void)
{
	int i;

	for (i = 0; i < 10000; i++) {
		tab[i * 4 + 0] = '0' + (i / 1000) % 10;
		tab[i * 4 + 1] = '0' + (i / 100) % 10;
		tab[i * 4 + 2] = '0' + (i / 10) % 10;
		tab[i * 4 + 3] = '0' + i % 10;
	}

	return 0;
}

#define DEF_BUF5(OFF_A, OFF_B, OFF_C, OFF_D, OFF_E) \
	char *buf1 = buf + OFF_A; \
	char *buf2 = buf + OFF_B; \
	char *buf3 = buf + OFF_C; \
	char *buf4 = buf + OFF_D; \
	char *buf5 = buf + OFF_E

#define DEF_BUF6(OFF_A, OFF_B, OFF_C, OFF_D, OFF_E, OFF_F) \
	char *buf1 = buf + OFF_A; \
	char *buf2 = buf + OFF_B; \
	char *buf3 = buf + OFF_C; \
	char *buf4 = buf + OFF_D; \
	char *buf5 = buf + OFF_E; \
	char *buf6 = buf + OFF_F

#define SET_BUF5(OFF, VAL) \
	*((unsigned long long *)&buf1[OFF]) = VAL; \
	*((unsigned long long *)&buf2[OFF]) = VAL; \
	*((unsigned long long *)&buf3[OFF]) = VAL; \
	*((unsigned long long *)&buf4[OFF]) = VAL; \
	*((unsigned long long *)&buf5[OFF]) = VAL

#define SET_BUF6(OFF, VAL) \
	*((unsigned long long *)&buf1[OFF]) = VAL; \
	*((unsigned long long *)&buf2[OFF]) = VAL; \
	*((unsigned long long *)&buf3[OFF]) = VAL; \
	*((unsigned long long *)&buf4[OFF]) = VAL; \
	*((unsigned long long *)&buf5[OFF]) = VAL; \
	*((unsigned long long *)&buf6[OFF]) = VAL

void inline my_itoa8_1(char *buf, unsigned int i, unsigned int j)
{
	DEF_BUF5(0, 9, 23, 42, 51);
	unsigned long long sss;

	sss = tab2[i];
	sss = ((sss << 32) | tab2[j]) >> 8;

	*((unsigned long long *)&buf1[0]) = sss | (0x31ULL << 56);
	*((unsigned long long *)&buf2[0]) = sss | (0x32ULL << 56);
	*((unsigned long long *)&buf3[0]) = sss | (0x34ULL << 56);
	*((unsigned long long *)&buf4[0]) = sss | (0x37ULL << 56);
	*((unsigned long long *)&buf5[0]) = sss | (0x38ULL << 56);
}

void inline my_itoa8_2(char *buf, unsigned int i, unsigned int j)
{
	DEF_BUF6(70, 84, 93, 111, 120, 134);
	unsigned long long sss;

	sss = tab2[i];
	sss = ((sss << 32) | tab2[j]) >> 8;

	*((unsigned long long *)&buf1[0]) = sss | (0x31ULL << 56);
	*((unsigned long long *)&buf2[0]) = sss | (0x33ULL << 56);
	*((unsigned long long *)&buf3[0]) = sss | (0x34ULL << 56);
	*((unsigned long long *)&buf4[0]) = sss | (0x36ULL << 56);
	*((unsigned long long *)&buf5[0]) = sss | (0x37ULL << 56);
	*((unsigned long long *)&buf6[0]) = sss | (0x39ULL << 56);
}

void inline my_itoa8_3(char *buf, unsigned int i, unsigned int j)
{
	DEF_BUF5(153, 162, 181, 195, 204);
	unsigned long long sss;

	sss = tab2[i];
	sss = ((sss << 32) | tab2[j]) >> 8;

	*((unsigned long long *)&buf1[0]) = sss | (0x32ULL << 56);
	*((unsigned long long *)&buf2[0]) = sss | (0x33ULL << 56);
	*((unsigned long long *)&buf3[0]) = sss | (0x36ULL << 56);
	*((unsigned long long *)&buf4[0]) = sss | (0x38ULL << 56);
	*((unsigned long long *)&buf5[0]) = sss | (0x39ULL << 56);
}

void inline my_itoa9_1(char *buf, unsigned int i, unsigned int j)
{
	DEF_BUF5(0, 10, 25, 45, 55);
	unsigned long long sss;

	sss = tab2[i];
	sss = (sss << 32) | tab2[j];
	SET_BUF5(0, sss);
}

void inline my_itoa9_2(char *buf, unsigned int i, unsigned int j)
{
	DEF_BUF6(75, 90, 100, 119, 129, 144);
	unsigned long long sss;

	sss = tab2[i];
	sss = (sss << 32) | tab2[j];
	SET_BUF6(0, sss);
}

void inline my_itoa9_3(char *buf, unsigned int i, unsigned int j)
{
	DEF_BUF5(164, 174, 194, 209, 219);
	unsigned long long sss;

	sss = tab2[i];
	sss = (sss << 32) | tab2[j];
	SET_BUF5(0, sss);
}

void inline my_itoa10_1(char *buf, unsigned int i, unsigned int j, unsigned int k)
{
	DEF_BUF5(0, 11, 27, 48, 59);
	unsigned long long sss;

	sss = tab2[i];
	sss = (sss << 32) | tab2[j];
	SET_BUF5(1, sss);

	buf1[0] = buf2[0] = buf3[0] = buf4[0] = buf5[0] = k + '0';
}

void inline my_itoa10_2(char *buf, unsigned int i, unsigned int j, unsigned int k)
{
	DEF_BUF6(80, 96, 107, 127, 138, 154);
	unsigned long long sss;

	sss = tab2[i];
	sss = (sss << 32) | tab2[j];
	SET_BUF6(1, sss);

	buf1[0] = buf2[0] = buf3[0] = buf4[0] = buf5[0] = buf6[0] = k + '0';
}

void inline my_itoa10_3(char *buf, unsigned int i, unsigned int j, unsigned int k)
{
	DEF_BUF5(175, 186, 207, 223, 234);
	unsigned long long sss;

	sss = tab2[i];
	sss = (sss << 32) | tab2[j];
	SET_BUF5(1, sss);

	buf1[0] = buf2[0] = buf3[0] = buf4[0] = buf5[0] = k + '0';
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

		str = &tab[part[partp] * 4];
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

		str = &tab[part[partp] * 4];
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

		str = &tab[part[partp] * 4];
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

		str = &tab[part[partp] * 4];
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

const char tmp8[] =
".......1\n.......2\n"
"Fizz\n.......4\nBuzz\n"
"Fizz\n.......7\n.......8\n"
"Fizz\nBuzz\n.......1\n"
"Fizz\n.......3\n.......4\n"
"FizzBuzz\n.......6\n.......7\n"
"Fizz\n.......9\nBuzz\n"
"Fizz\n.......2\n.......3\n"
"Fizz\nBuzz\n.......6\n"
"Fizz\n.......8\n.......9\n"
"FizzBuzz\n";

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

void inline do_8(unsigned int t)
{
	unsigned int w, x;
	unsigned int wp_before = wp;
	char *p = &buf[wrap(wp)];

	memcpy(p, tmp8, sizeof(tmp8));
	w = t % 10000;
	x = t / 10000;

	my_itoa8_1(p, w, x);

	switch (w) {
	case 9999:
		my_itoa8_2(p, 0, x + 1);
		my_itoa8_3(p, 1, x + 1);
		break;
	case 9998:
		my_itoa8_2(p, 9999, x);
		my_itoa8_3(p,    0, x + 1);
		break;
	default:
		my_itoa8_2(p, w + 1, x);
		my_itoa8_3(p, w + 2, x);
		break;
	}

	wp += 111 * 2;

	if (wrap(wp) < wrap(wp_before)) {
		memcpy(&buf[0], &buf[BUFSIZE], wrap(wp));
	}
	if (wp - rp >= CHUNKSIZE) {
		vwrite(1, &buf[wrap(rp)], CHUNKSIZE);
		rp += CHUNKSIZE;
	}
}

void inline do_9(unsigned int t)
{
	unsigned int w, x;
	unsigned int wp_before = wp;
	char *p = &buf[wrap(wp)];

	memcpy(p, tmp9, sizeof(tmp9));
	w = t % 10000;
	x = t / 10000;

	my_itoa9_1(p, w, x);

	switch (w) {
	case 9999:
		my_itoa9_2(p, 0, x + 1);
		my_itoa9_3(p, 1, x + 1);
		break;
	case 9998:
		my_itoa9_2(p, 9999, x);
		my_itoa9_3(p,    0, x + 1);
		break;
	default:
		my_itoa9_2(p, w + 1, x);
		my_itoa9_3(p, w + 2, x);
		break;
	}

	wp += 119 * 2;

	if (wrap(wp) < wrap(wp_before)) {
		memcpy(&buf[0], &buf[BUFSIZE], wrap(wp));
	}
	if (wp - rp >= CHUNKSIZE) {
		vwrite(1, &buf[wrap(rp)], CHUNKSIZE);
		rp += CHUNKSIZE;
	}
}

void do_10_gen(unsigned int t)
{
	unsigned int u, v, w, x;
	unsigned int wp_before = wp;
	char *p = &buf[wrap(wp)];

	memcpy(p, tmp10, sizeof(tmp10));
	u = t % 100000000;
	v = t / 100000000;
	w = u % 10000;
	x = u / 10000;
	my_itoa10_1(p, w, x, v);
	switch (u) {
	case 99999999:
		my_itoa10_2(p, 0, 0, v + 1);
		my_itoa10_3(p, 1, 0, v + 1);
		break;
	case 99999998:
		my_itoa10_2(p, (u + 1) % 10000, (u + 1) / 10000, v);
		my_itoa10_3(p, 0, 0, v + 1);
		break;
	default:
		my_itoa10_2(p, (u + 1) % 10000, (u + 1) / 10000, v);
		my_itoa10_3(p, (u + 2) % 10000, (u + 2) / 10000, v);
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

void inline do_10(unsigned int t, unsigned int v)
{
	unsigned int w, x;
	unsigned int wp_before = wp;
	char *p = &buf[wrap(wp)];

	memcpy(p, tmp10, sizeof(tmp10));

	w = t % 10000;
	x = t / 10000;

	my_itoa10_1(p, w, x, v);

	switch (w) {
	case 9999:
		my_itoa10_2(p, 0, x + 1, v);
		my_itoa10_3(p, 1, x + 1, v);
		break;
	case 9998:
		my_itoa10_2(p, 9999, x, v);
		my_itoa10_3(p,    0, x + 1, v);
		break;
	default:
		my_itoa10_2(p, w + 1, x, v);
		my_itoa10_3(p, w + 2, x, v);
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

int main(int argc, char *argv[])
{
	unsigned int i, t, v;

	fcntl(1, F_SETPIPE_SZ, BUFSIZE / 2);

	gentbl();

	for (i = 1; i < 10000020UL; i += 30) {
		fizzbuzz30(i);
	}

	t = i / 10;
	for (; i < 99999990UL; i += 30, t += 3) {
		do_8(t);
	}

	fizzbuzz30(i);
	i += 30;
	t += 3;

	for (; i < 999999990UL; i += 30, t += 3) {
		do_9(t);
	}

	fizzbuzz30(i);
	i += 30;
	t += 3;

	t -= 100000000UL;
	v = 1;
	for (; i < 1999999980UL; i += 30, t += 3) {
		do_10(t, v);
	}

	do_10_gen(t + 100000000UL);
	i += 30;
	t += 3;

	t -= 100000000UL;
	v = 2;
	for (; i < 3000000000UL; i += 30, t += 3) {
		do_10(t, v);
	}

	t -= 100000000UL;
	v = 3;
	for (; i < 3999999990UL; i += 30, t += 3) {
		do_10(t, v);
	}

	do_10_gen(t + 300000000UL);
	i += 30;
	t += 3;

	t -= 100000000UL;
	v = 4;
	for (; i < 4294967250UL; i += 30, t += 3) {
		do_10(t, v);
	}

	for (; i < 0xffffffff;) {
		i = fizzbuzz(i);
	}

	vwrite(1, &buf[wrap(rp)], wp - rp);

	return 0;
}
