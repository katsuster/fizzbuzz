#define _GNU_SOURCE

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/uio.h>

//512KB
#define CHUNKSIZE    (4096 * 64)

char buf2[2][CHUNKSIZE + 4096] __attribute__((aligned(4096)));
uint32_t rp __attribute__((aligned(8)));
uint32_t wp __attribute__((aligned(8)));
int f __attribute__((aligned(8)));

#define wrap(wp)    ((wp) & (CHUNKSIZE - 1))

static void vwrite(int fd, void *buf, size_t count)
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

static char *get_p(void)
{
	return &buf2[f][wrap(wp)];
}

static char *get_p_r(void)
{
	return &buf2[f][wrap(rp)];
}

static void rb_wrap(uint32_t wp_before)
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

struct dec {
	uint64_t h;
	uint64_t l;
	int ke;
	uint32_t next_ke;
};

//8 digits, offset 0xf6 (ff: 9, fe: 8, ..., f6: 0)
#define D_ZERO    0xf6f6f6f6f6f6f6f6ULL
#define D_MAX     0xffffffffffffffffULL
//convert from digits to characters (ex. 0xf6 - 0xc6 = '0')
#define D_TOCHR   0xc6c6c6c6c6c6c6c6ULL

static void inc_c(struct dec *d)
{
	d->l++;

	if (d->l == 0) {
		d->l = D_ZERO;
		d->h++;

		int ctz = __builtin_ctzll(d->h) & ~0x7;
		uint64_t mask = (1ULL << ctz) - 1;
		d->h |= mask & D_ZERO;
	} else {
		int ctz = __builtin_ctzll(d->l) & ~0x7;
		uint64_t mask = (1ULL << ctz) - 1;
		d->l |= mask & D_ZERO;
	}
}

static void to_num(struct dec *d, uint64_t *ph, uint64_t *pl)
{
	uint64_t h, l;

	h = d->h - D_TOCHR;
	l = d->l - D_TOCHR;

	switch (d->ke) {
	case 9:
	case 10:
		h <<= (16 - d->ke) * 8;
		h = htobe64(h);
		*ph = h;
		l = htobe64(l);
		*pl = l;
		break;
	case 0:
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
		*pl = l;
		break;
	}
}

static int out_fixnum(char *buf, struct dec *d, uint64_t h, uint64_t l)
{
	uint64_t *b = (void *)buf;

	switch (d->ke) {
	case 9:
	case 10:
		*b = h;
		b = (void *)(buf + d->ke - 8);
		*b = l;
		break;
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
		*b = l;
		break;
	}

	return d->ke;
}

static int out_two(char *buf, const char *str)
{
	const uint16_t *s = (const void *)str;
	uint16_t *b = (void *)buf;

	*b = *s;

	return 2;
}

static int out_1fizz(char *buf)
{
	const char *str = "1\nFizz\n ";
	const uint64_t *s = (const void *)str;
	uint64_t *b = (void *)buf;

	*b = *s;

	return 7;
}

static int out_2fizz(char *buf)
{
	const char *str = "2\nFizz\n ";
	const uint64_t *s = (const void *)str;
	uint64_t *b = (void *)buf;

	*b = *s;

	return 7;
}

static int out_6fizz(char *buf)
{
	const char *str = "6\nFizz\n ";
	const uint64_t *s = (const void *)str;
	uint64_t *b = (void *)buf;

	*b = *s;

	return 7;
}

static int out_7fizz(char *buf)
{
	const char *str = "7\nFizz\n ";
	const uint64_t *s = (const void *)str;
	uint64_t *b = (void *)buf;

	*b = *s;

	return 7;
}

static int out_4fb(char *buf)
{
	const char *str = "4\nFizzBu";
	const char *str2 = "zz\n";
	const uint64_t *s = (const void *)str;
	const uint32_t *s2 = (const void *)str2;
	uint64_t *b = (void *)buf;
	uint32_t *b2 = (void *)buf + 8;

	*b = *s;
	*b2 = *s2;

	return 11;
}

static int out_9fb(char *buf)
{
	const char *str = "9\nFizzBu";
	const char *str2 = "zz\n";
	const uint64_t *s = (const void *)str;
	const uint32_t *s2 = (const void *)str2;
	uint64_t *b = (void *)buf;
	uint32_t *b2 = (void *)buf + 8;

	*b = *s;
	*b2 = *s2;

	return 11;
}

static int out_3fandb(char *buf)
{
	const char *str = "3\nFizz\nB";
	const char *str2 = "uzz\n";
	const uint64_t *s = (const void *)str;
	const uint32_t *s2 = (const void *)str2;
	uint64_t *b = (void *)buf;
	uint32_t *b2 = (void *)buf + 8;

	*b = *s;
	*b2 = *s2;

	return 12;
}

static int out_8fandb(char *buf)
{
	const char *str = "8\nFizz\nB";
	const char *str2 = "uzz\n";
	const uint64_t *s = (const void *)str;
	const uint32_t *s2 = (const void *)str2;
	uint64_t *b = (void *)buf;
	uint32_t *b2 = (void *)buf + 8;

	*b = *s;
	*b2 = *s2;

	return 12;
}

static int out_4bandf(char *buf)
{
	const char *str = "4\nBuzz\nF";
	const char *str2 = "izz\n";
	const uint64_t *s = (const void *)str;
	const uint32_t *s2 = (const void *)str2;
	uint64_t *b = (void *)buf;
	uint32_t *b2 = (void *)buf + 8;

	*b = *s;
	*b2 = *s2;

	return 12;
}

static int out_9bandf(char *buf)
{
	const char *str = "9\nBuzz\nF";
	const char *str2 = "izz\n";
	const uint64_t *s = (const void *)str;
	const uint32_t *s2 = (const void *)str2;
	uint64_t *b = (void *)buf;
	uint32_t *b2 = (void *)buf + 8;

	*b = *s;
	*b2 = *s2;

	return 12;
}

static void fizzbuzz30(struct dec *d, uint32_t j)
{
	uint64_t h, l;
	uint32_t wp_before = wp;
	char *p = get_p();
	char *p_s = p;
	int r;

	to_num(d, &h, &l);
	r = out_fixnum(p, d, h, l); p += r;
	r = out_two(p, "1\n");      p += r;
	r = out_fixnum(p, d, h, l); p += r;
	r = out_2fizz(p);           p += r;
	r = out_fixnum(p, d, h, l); p += r;
	r = out_4bandf(p);          p += r;
	r = out_fixnum(p, d, h, l); p += r;
	r = out_two(p, "7\n");      p += r;
	r = out_fixnum(p, d, h, l); p += r;
	r = out_8fandb(p);          p += r;
	inc_c(d);

	if (d->next_ke == j) {
		d->ke++;
		d->next_ke *= 10;
	}

	to_num(d, &h, &l);
	r = out_fixnum(p, d, h, l); p += r;
	r = out_1fizz(p);           p += r;
	r = out_fixnum(p, d, h, l); p += r;
	r = out_two(p, "3\n");      p += r;
	r = out_fixnum(p, d, h, l); p += r;
	r = out_4fb(p);             p += r;
	r = out_fixnum(p, d, h, l); p += r;
	r = out_two(p, "6\n");      p += r;
	r = out_fixnum(p, d, h, l); p += r;
	r = out_7fizz(p);           p += r;
	r = out_fixnum(p, d, h, l); p += r;
	r = out_9bandf(p);          p += r;
	inc_c(d);

	to_num(d, &h, &l);
	r = out_fixnum(p, d, h, l); p += r;
	r = out_two(p, "2\n");      p += r;
	r = out_fixnum(p, d, h, l); p += r;
	r = out_3fandb(p);          p += r;
	r = out_fixnum(p, d, h, l); p += r;
	r = out_6fizz(p);           p += r;
	r = out_fixnum(p, d, h, l); p += r;
	r = out_two(p, "8\n");      p += r;
	r = out_fixnum(p, d, h, l); p += r;
	r = out_9fb(p);             p += r;
	inc_c(d);

	wp += p - p_s;

	rb_wrap(wp_before);
}

int main(int argc, char *argv[])
{
	struct dec d = {D_ZERO, D_ZERO, 0, 1};
	uint64_t i = 1;

	fcntl(1, F_SETPIPE_SZ, CHUNKSIZE);

	for (; i <= 0xffffffffUL/10; i += 3) {
		fizzbuzz30(&d, i);
	}

	vwrite(1, get_p_r(), wp - rp);

	return 0;
}
