#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/uio.h>

//512KB
#define CHUNKSIZE    (4096 * 64)

char buf2[2][CHUNKSIZE + 4096] __attribute__((aligned(4096)));
unsigned int rp __attribute__((aligned(8)));
unsigned int wp __attribute__((aligned(8)));
int f __attribute__((aligned(8)));

#define wrap(wp)    ((wp) & (CHUNKSIZE - 1))

char tab[4 * 10000];
unsigned int *tab2 = (void *)tab;

static inline void vwrite(int fd, void *buf, size_t count)
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
}

static inline char *get_p(void)
{
	return &buf2[f][wrap(wp)];
}

static inline char *get_p_r(void)
{
	return &buf2[f][wrap(rp)];
}

static inline void rb_wrap(unsigned int wp_before)
{
	if (wp - rp >= CHUNKSIZE) {
		vwrite(1, &buf2[f][wrap(rp)], CHUNKSIZE);
		rp += CHUNKSIZE;
		f = !f;
	}
	if (wrap(wp) < wrap(wp_before)) {
		memcpy(&buf2[f][0], &buf2[!f][CHUNKSIZE], wrap(wp));
	}
}

static inline void append(const char *str, size_t len)
{
	unsigned int wp_before = wp;
	char *p = get_p();

	memcpy(p, str, len);
	wp += len;

	rb_wrap(wp_before);
}

static int gentbl(void)
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

#define CALC_P(KE, N, OFF)    (((KE) + 1) * (N) + (OFF))

#define P_00(KE)    CALC_P((KE),  0,  0)
#define P_01(KE)    CALC_P((KE),  1,  0)
#define P_02(KE)    CALC_P((KE),  2,  5)
#define P_03(KE)    CALC_P((KE),  3, 15)
#define P_04(KE)    CALC_P((KE),  4, 15)
#define P_05(KE)    CALC_P((KE),  5, 25)
#define P_06(KE)    CALC_P((KE),  6, 30)
#define P_07(KE)    CALC_P((KE),  7, 30)
#define P_08(KE)    CALC_P((KE),  8, 39)
#define P_09(KE)    CALC_P((KE),  9, 39)
#define P_10(KE)    CALC_P((KE), 10, 44)
#define P_11(KE)    CALC_P((KE), 11, 54)
#define P_12(KE)    CALC_P((KE), 12, 54)
#define P_13(KE)    CALC_P((KE), 13, 64)
#define P_14(KE)    CALC_P((KE), 14, 69)
#define P_15(KE)    CALC_P((KE), 15, 69)

static void my_itoa8_1(char *buf, unsigned int i, unsigned int j)
{
	DEF_BUF5(P_00(8), P_01(8), P_02(8), P_03(8), P_04(8));
	unsigned long long sss;

	sss = tab2[i];
	sss = ((sss << 32) | tab2[j]) >> 8;

	*((unsigned long long *)&buf1[0]) = sss | (0x31ULL << 56);
	*((unsigned long long *)&buf2[0]) = sss | (0x32ULL << 56);
	*((unsigned long long *)&buf3[0]) = sss | (0x34ULL << 56);
	*((unsigned long long *)&buf4[0]) = sss | (0x37ULL << 56);
	*((unsigned long long *)&buf5[0]) = sss | (0x38ULL << 56);
}

static void my_itoa8_2(char *buf, unsigned int i, unsigned int j)
{
	DEF_BUF6(P_05(8), P_06(8), P_07(8), P_08(8), P_09(8), P_10(8));
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

static void my_itoa8_3(char *buf, unsigned int i, unsigned int j)
{
	DEF_BUF5(P_11(8), P_12(8), P_13(8), P_14(8), P_15(8));
	unsigned long long sss;

	sss = tab2[i];
	sss = ((sss << 32) | tab2[j]) >> 8;

	*((unsigned long long *)&buf1[0]) = sss | (0x32ULL << 56);
	*((unsigned long long *)&buf2[0]) = sss | (0x33ULL << 56);
	*((unsigned long long *)&buf3[0]) = sss | (0x36ULL << 56);
	*((unsigned long long *)&buf4[0]) = sss | (0x38ULL << 56);
	*((unsigned long long *)&buf5[0]) = sss | (0x39ULL << 56);
}

static void my_itoa9_1(char *buf, unsigned int i, unsigned int j)
{
	DEF_BUF5(P_00(9), P_01(9), P_02(9), P_03(9), P_04(9));
	unsigned long long sss;

	sss = tab2[i];
	sss = (sss << 32) | tab2[j];
	SET_BUF5(0, sss);
}

static void my_itoa9_2(char *buf, unsigned int i, unsigned int j)
{
	DEF_BUF6(P_05(9), P_06(9), P_07(9), P_08(9), P_09(9), P_10(9));
	unsigned long long sss;

	sss = tab2[i];
	sss = (sss << 32) | tab2[j];
	SET_BUF6(0, sss);
}

static void my_itoa9_3(char *buf, unsigned int i, unsigned int j)
{
	DEF_BUF5(P_11(9), P_12(9), P_13(9), P_14(9), P_15(9));
	unsigned long long sss;

	sss = tab2[i];
	sss = (sss << 32) | tab2[j];
	SET_BUF5(0, sss);
}

static void my_itoa10_1(char *buf, unsigned int i, unsigned int j, unsigned int k)
{
	DEF_BUF5(P_00(10), P_01(10), P_02(10), P_03(10), P_04(10));
	unsigned long long sss;

	sss = tab2[i];
	sss = (sss << 32) | tab2[j];

	buf1[0] = buf2[0] = buf3[0] = buf4[0] = buf5[0] = k + '0';
	SET_BUF5(1, sss);
}

static void my_itoa10_2(char *buf, unsigned int i, unsigned int j, unsigned int k)
{
	DEF_BUF6(P_05(10), P_06(10), P_07(10), P_08(10), P_09(10), P_10(10));
	unsigned long long sss;

	sss = tab2[i];
	sss = (sss << 32) | tab2[j];

	buf1[0] = buf2[0] = buf3[0] = buf4[0] = buf5[0] = buf6[0] = k + '0';
	SET_BUF6(1, sss);
}

static void my_itoa10_3(char *buf, unsigned int i, unsigned int j, unsigned int k)
{
	DEF_BUF5(P_11(10), P_12(10), P_13(10), P_14(10), P_15(10));
	unsigned long long sss;

	sss = tab2[i];
	sss = (sss << 32) | tab2[j];

	buf1[0] = buf2[0] = buf3[0] = buf4[0] = buf5[0] = k + '0';
	SET_BUF5(1, sss);
}

struct dec {
	unsigned long long h;
	unsigned long long l;
	int ke;
	unsigned int next_ke;
};

//8 digits, offset 0xf6 (ff: 9, fe: 8, ..., f6: 0)
#define D_ZERO    0xf6f6f6f6f6f6f6f6ULL
#define D_ONE     0xf6f6f6f6f6f6f6f7ULL
#define D_MAX     0xffffffffffffffffULL
//convert from digits to characters (ex. 0xf6 - 0xc6 = '0')
#define D_TOCHR   0xc6c6c6c6c6c6c6c6ULL

static void inc_c(struct dec *d)
{
	d->l++;

	int ctz = __builtin_ctzll(d->l) & ~0x7;
	unsigned long long mask = (1ULL << ctz) - 1;
	d->l |= mask & D_ZERO;
}

static void inc_nc(struct dec *d)
{
	d->l++;
}

static int out_num(char *buf, struct dec *d)
{
	unsigned long long l;
	unsigned long long *b = (void *)buf;

	l = d->l - D_TOCHR;
	l <<= (8 - d->ke) * 8;
	l = htobe64(l);
	*b = l;

	buf[d->ke] = '\n';

	return d->ke + 1;
}

static inline int out_fizz(char *buf)
{
	memcpy(buf, "Fizz\n   ", 8);
	return 5;
}

static inline int out_fb(char *buf)
{
	memcpy(buf, "FizzBuzz\n ", 10);
	return 9;
}

static inline int out_fandb(char *buf)
{
	memcpy(buf, "Fizz\nBuzz\n", 10);
	return 10;
}

static inline int out_bandf(char *buf)
{
	memcpy(buf, "Buzz\nFizz\n", 10);
	return 10;
}

static void fizzbuzz30(struct dec *d, unsigned int j)
{
	unsigned int wp_before = wp;
	char *p = get_p();
	char *p_s = p;
	int r;

	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_fizz(p);   p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_bandf(p);  p += r;
	inc_nc(d);
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_fandb(p);  p += r;
	inc_c(d);

	if (d->next_ke == j) {
		d->ke++;
		d->next_ke *= 10;
	}

	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_fizz(p);   p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_fb(p);     p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_fizz(p);   p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_c(d);  r = out_bandf(p);  p += r;

	inc_nc(d);
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_fandb(p);  p += r;
	inc_nc(d);
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_fizz(p);   p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_c(d);  r = out_fb(p);     p += r;

	wp += p - p_s;

	rb_wrap(wp_before);
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

static inline void do_8(unsigned int t)
{
	unsigned int w, x;
	unsigned int wp_before = wp;
	char *p = get_p();

	memcpy(p, tmp8, sizeof(tmp8) - 1);
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

	rb_wrap(wp_before);
}

static inline void do_9(unsigned int t)
{
	unsigned int w, x;
	unsigned int wp_before = wp;
	char *p = get_p();

	memcpy(p, tmp9, sizeof(tmp9) - 1);
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

	rb_wrap(wp_before);
}

static inline void do_10(unsigned int t, unsigned int v)
{
	unsigned int w, x;
	unsigned int wp_before = wp;
	char *p = get_p();

	memcpy(p, tmp10, sizeof(tmp10) - 1);
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

	rb_wrap(wp_before);
}

const char tail8[] =
"99999991\n99999992\n"
"Fizz\n99999994\nBuzz\n"
"Fizz\n99999997\n99999998\n"
"Fizz\nBuzz\n100000001\n"
"Fizz\n100000003\n100000004\n"
"FizzBuzz\n100000006\n100000007\n"
"Fizz\n100000009\nBuzz\n"
"Fizz\n100000012\n100000013\n"
"Fizz\nBuzz\n100000016\n"
"Fizz\n100000018\n100000019\n"
"FizzBuzz\n";

const char tail9[] =
"999999991\n999999992\n"
"Fizz\n999999994\nBuzz\n"
"Fizz\n999999997\n999999998\n"
"Fizz\nBuzz\n1000000001\n"
"Fizz\n1000000003\n1000000004\n"
"FizzBuzz\n1000000006\n1000000007\n"
"Fizz\n1000000009\nBuzz\n"
"Fizz\n1000000012\n1000000013\n"
"Fizz\nBuzz\n1000000016\n"
"Fizz\n1000000018\n1000000019\n"
"FizzBuzz\n";

const char tail10_1[] =
"1999999981\n1999999982\n"
"Fizz\n1999999984\nBuzz\n"
"Fizz\n1999999987\n1999999988\n"
"Fizz\nBuzz\n1999999991\n"
"Fizz\n1999999993\n1999999994\n"
"FizzBuzz\n1999999996\n1999999997\n"
"Fizz\n1999999999\nBuzz\n"
"Fizz\n2000000002\n2000000003\n"
"Fizz\nBuzz\n2000000006\n"
"Fizz\n2000000008\n2000000009\n"
"FizzBuzz\n";

const char tail10_3[] =
"3999999991\n3999999992\n"
"Fizz\n3999999994\nBuzz\n"
"Fizz\n3999999997\n3999999998\n"
"Fizz\nBuzz\n4000000001\n"
"Fizz\n4000000003\n4000000004\n"
"FizzBuzz\n4000000006\n4000000007\n"
"Fizz\n4000000009\nBuzz\n"
"Fizz\n4000000012\n4000000013\n"
"Fizz\nBuzz\n4000000016\n"
"Fizz\n4000000018\n4000000019\n"
"FizzBuzz\n";

const char last[] =
"4294967281\n4294967282\n"
"Fizz\n4294967284\nBuzz\n"
"Fizz\n4294967287\n4294967288\n"
"Fizz\nBuzz\n4294967291\n"
"Fizz\n4294967293\n4294967294\n";

int main(int argc, char *argv[])
{
	struct dec d = {D_ZERO, D_ZERO, 1, 10};
	unsigned int i, t;

	fcntl(1, F_SETPIPE_SZ, CHUNKSIZE);

	gentbl();

	for (i = 1; i < 10000020UL; i += 30) {
		fizzbuzz30(&d, i + 9);
	}

	t = i / 10;
	for (; i < 99999990UL; i += 30, t += 3) {
		do_8(t);
	}

	append(tail8, sizeof(tail8) - 1);
	i += 30;
	t += 3;

	for (; i < 999999990UL; i += 30, t += 3) {
		do_9(t);
	}

	append(tail9, sizeof(tail9) - 1);
	i += 30;
	t += 3;

	t -= 100000000UL;
	for (; i < 1999999980UL; i += 30, t += 3) {
		do_10(t, 1);
	}

	append(tail10_1, sizeof(tail10_1) - 1);
	i += 30;
	t += 3;

	t -= 100000000UL;
	for (; i < 3000000000UL; i += 30, t += 3) {
		do_10(t, 2);
	}

	t -= 100000000UL;
	for (; i < 3999999990UL; i += 30, t += 3) {
		do_10(t, 3);
	}

	append(tail10_3, sizeof(tail10_3) - 1);
	i += 30;
	t += 3;

	t -= 100000000UL;
	for (; i < 4294967280UL; i += 30, t += 3) {
		do_10(t, 4);
	}

	append(last, sizeof(last) - 1);

	vwrite(1, get_p_r(), wp - rp);

	return 0;
}
