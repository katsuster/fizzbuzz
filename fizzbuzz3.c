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
		memmove(&buf2[f][0], &buf2[!f][CHUNKSIZE], wrap(wp));
	}
}

static inline void append(const char *str, size_t len)
{
	unsigned int wp_before = wp;
	char *p = get_p();

	memmove(p, str, len);
	wp += len;

	rb_wrap(wp_before);
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

static inline void inc_c(struct dec *d)
{
	if (d->l == D_MAX) {
		d->l = D_ZERO;
		d->h++;

		int ctz = __builtin_ctzll(d->h) & ~0x7;
		unsigned long long mask = (1ULL << ctz) - 1;
		d->h |= mask & D_ZERO;
	} else {
		d->l++;

		int ctz = __builtin_ctzll(d->l) & ~0x7;
		unsigned long long mask = (1ULL << ctz) - 1;
		d->l |= mask & D_ZERO;
	}
}

static inline void inc_nc(struct dec *d)
{
	d->l++;
}

static inline int out_num(char *buf, struct dec *d)
{
	unsigned long long h, l;
	unsigned long long *b = (void *)buf;

	h = d->h - D_TOCHR;
	l = d->l - D_TOCHR;

	switch (d->ke) {
	case 9:
	case 10:
		h <<= (16 - d->ke) * 8;
		h = htobe64(h);
		*b = h;
		b = (void *)(buf + d->ke - 8);
		l = htobe64(l);
		*b = l;
		break;
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
		l <<= (8 - d->ke) * 8;
		l = htobe64(l);
		*b = l;
		break;
	}

	buf[d->ke] = '\n';

	return d->ke + 1;
}

static inline int out_fizz(char *buf)
{
	memmove(buf, "Fizz\n   ", 8);
	return 5;
}

static inline int out_fb(char *buf)
{
	memmove(buf, "FizzBuzz\n ", 10);
	return 9;
}

static inline int out_fandb(char *buf)
{
	memmove(buf, "Fizz\nBuzz\n", 10);
	return 10;
}

static inline int out_bandf(char *buf)
{
	memmove(buf, "Buzz\nFizz\n", 10);
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
	inc_nc(d); r = out_fizz(p);    p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_bandf(p);   p += r;
	inc_nc(d);
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_fandb(p);   p += r;
	inc_c(d);

	if (d->next_ke == j) {
		d->ke++;
		d->next_ke *= 10;
	}

	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_fizz(p);    p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_fb(p);      p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_fizz(p);    p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_c(d);  r = out_bandf(p);   p += r;

	inc_nc(d);
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_fandb(p);   p += r;
	inc_nc(d);
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_fizz(p);    p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_nc(d); r = out_num(p, d); p += r;
	inc_c(d);  r = out_fb(p);      p += r;

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

static inline void out_num9(char *buf, unsigned long long l)
{
	unsigned long long *b = (void *)buf;

	*b = l;
}

static inline void do_9(struct dec *d)
{
	unsigned int wp_before = wp;
	char *p = get_p();
	unsigned long long h, l;

	memmove(p, tmp9, sizeof(tmp9) - 1);

	h = d->h - D_TOCHR;
	l = d->l - D_TOCHR;

	h <<= 56;
	l >>= 8;
	l |= h;
	l = htobe64(l);

	out_num9(p +   0, l);
	out_num9(p +  10, l);
	out_num9(p +  25, l);
	out_num9(p +  45, l);
	out_num9(p +  55, l);
	d->l += 9;
	inc_c(d);

	h = d->h - D_TOCHR;
	l = d->l - D_TOCHR;

	h <<= 56;
	l >>= 8;
	l |= h;
	l = htobe64(l);

	out_num9(p +  75, l);
	out_num9(p +  90, l);
	out_num9(p + 100, l);
	out_num9(p + 119, l);
	out_num9(p + 129, l);
	out_num9(p + 144, l);
	d->l += 9;
	inc_c(d);

	h = d->h - D_TOCHR;
	l = d->l - D_TOCHR;

	h <<= 56;
	l >>= 8;
	l |= h;
	l = htobe64(l);

	out_num9(p + 164, l);
	out_num9(p + 174, l);
	out_num9(p + 194, l);
	out_num9(p + 209, l);
	out_num9(p + 219, l);
	d->l += 9;
	inc_c(d);

	wp += 119 * 2;

	rb_wrap(wp_before);
}

static inline void out_num10(char *buf, unsigned long long ll, unsigned char cc)
{
	unsigned long long *b = (void *)(buf + 1);

	*buf = cc;
	*b = ll;
}

static inline void do_10(struct dec *d)
{
	unsigned int wp_before = wp;
	char *p = get_p();
	unsigned long long h, l;
	unsigned long long l1, l2, l3;
	unsigned char c1, c2, c3;

	memmove(p, tmp10, sizeof(tmp10) - 1);

	h = d->h - D_TOCHR;
	l = d->l - D_TOCHR;
	c1 = (h & 0xff00) >> 8;
	h <<= 56;
	l >>= 8;
	l |= h;
	l1 = htobe64(l);

	d->l += 9;
	inc_c(d);

	h = d->h - D_TOCHR;
	l = d->l - D_TOCHR;
	c2 = (h & 0xff00) >> 8;
	h <<= 56;
	l >>= 8;
	l |= h;
	l2 = htobe64(l);

	d->l += 9;
	inc_c(d);

	h = d->h - D_TOCHR;
	l = d->l - D_TOCHR;
	c3 = (h & 0xff00) >> 8;
	h <<= 56;
	l >>= 8;
	l |= h;
	l3 = htobe64(l);

	out_num10(p +   0, l1, c1);
	out_num10(p +  11, l1, c1);
	out_num10(p +  27, l1, c1);
	out_num10(p +  48, l1, c1);
	out_num10(p +  59, l1, c1);

	out_num10(p +  80, l2, c2);
	out_num10(p +  96, l2, c2);
	out_num10(p + 107, l2, c2);
	out_num10(p + 127, l2, c2);
	out_num10(p + 138, l2, c2);
	out_num10(p + 154, l2, c2);

	out_num10(p + 175, l3, c3);
	out_num10(p + 186, l3, c3);
	out_num10(p + 207, l3, c3);
	out_num10(p + 223, l3, c3);
	out_num10(p + 234, l3, c3);
	d->l += 9;
	inc_c(d);

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
	unsigned int i = 1;

	fcntl(1, F_SETPIPE_SZ, CHUNKSIZE);

	for (; i <= 99999991UL; i += 30) {
		fizzbuzz30(&d, i + 9);
	}

	for (; i < 999999991UL; i += 30) {
		do_9(&d);
	}

	append(tail9, sizeof(tail9) - 1);
	i += 30;

	d.l = 0xf6f6f6f6f6f6f8f6ULL;
	d.h = 0xf6f6f6f6f6f6f7f6ULL;
	d.ke = 10;
	for (; i < 1999999980UL; i += 30) {
		do_10(&d);
	}

	append(tail10_1, sizeof(tail10_1) - 1);
	i += 30;

	d.l = 0xf6f6f6f6f6f6f7f6ULL;
	d.h = 0xf6f6f6f6f6f6f8f6ULL;
	for (; i < 3999999990UL; i += 30) {
		do_10(&d);
	}

	append(tail10_3, sizeof(tail10_3) - 1);
	i += 30;

	d.l = 0xf6f6f6f6f6f6f8f6ULL;
	d.h = 0xf6f6f6f6f6f6faf6ULL;
	for (; i < 4294967280UL; i += 30) {
		do_10(&d);
	}

	append(last, sizeof(last) - 1);

	vwrite(1, get_p_r(), wp - rp);

	return 0;
}
