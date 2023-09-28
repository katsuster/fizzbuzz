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

static void inc_nc(struct dec *d)
{
	d->l++;
}

static int out_num(char *buf, struct dec *d)
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

static int out_fizz(char *buf)
{
	const char *str = "Fizz\n   ";
	const unsigned long long *s = (const void *)str;
	unsigned long long *b = (void *)buf;

	*b = *s;

	return 5;
}

static int out_fb(char *buf)
{
	const char *str = "FizzBuzz";
	const unsigned long long *s = (const void *)str;
	unsigned long long *b = (void *)buf;

	*b = *s;
	buf[8] = '\n';

	return 9;
}

static int out_fandb(char *buf)
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

static int out_bandf(char *buf)
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

const char last[] =
"4294967281\n4294967282\n"
"Fizz\n4294967284\nBuzz\n"
"Fizz\n4294967287\n4294967288\n"
"Fizz\nBuzz\n4294967291\n"
"Fizz\n4294967293\n4294967294\n";

int main(int argc, char *argv[])
{
	struct dec d = {D_ZERO, D_ZERO, 1, 10};
	unsigned int i = 1, j = 0;

	fcntl(1, F_SETPIPE_SZ, CHUNKSIZE);

	j = 10;
	for (i = 1; i < 0xfffffff0;) {
		fizzbuzz30(&d, j);

		j += 30;
		i += 30;
	}

	{
		unsigned int wp_before = wp;
		char *p = get_p();

		memmove(p, last, sizeof(last));
		wp += sizeof(last);

		rb_wrap(wp_before);
	}

	vwrite(1, get_p_r(), wp - rp);

	return 0;
}
