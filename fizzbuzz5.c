#define _GNU_SOURCE

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/uio.h>

#include <immintrin.h>

//512KB
#define CHUNKSIZE    (4096 * 64)

static char buf2[2][CHUNKSIZE + 4096] __attribute__((aligned(4096)));
static uint32_t rp __attribute__((aligned(8)));
static uint32_t wp __attribute__((aligned(8)));
static int f __attribute__((aligned(8)));

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

static inline void rb_wrap(uint32_t wp_before)
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
	__m128i num;
	int ke;
	uint64_t next_ke;
};

static const __m128i mask_shuffle[] = {
	{0x0706050403020100ULL, 0x0f0e0d0c0b0a0908ULL},
	{0x0807060504030201ULL, 0xff0f0e0d0c0b0a09ULL},
	{0x0908070605040302ULL, 0xffff0f0e0d0c0b0aULL},
	{0x0a09080706050403ULL, 0xffffff0f0e0d0c0bULL},
	{0x0b0a090807060504ULL, 0xffffffff0f0e0d0cULL},
	{0x0c0b0a0908070605ULL, 0xffffffffff0f0e0dULL},
	{0x0d0c0b0a09080706ULL, 0xffffffffffff0f0eULL},
	{0x0e0d0c0b0a090807ULL, 0xffffffffffffff0fULL},
	{0x0f0e0d0c0b0a0908ULL, 0xffffffffffffffffULL},
	{0xff0f0e0d0c0b0a09ULL, 0xffffffffffffffffULL},
	{0xffff0f0e0d0c0b0aULL, 0xffffffffffffffffULL},
	{0xffffff0f0e0d0c0bULL, 0xffffffffffffffffULL},
	{0xffffffff0f0e0d0cULL, 0xffffffffffffffffULL},
	{0xffffffffff0f0e0dULL, 0xffffffffffffffffULL},
	{0xffffffffffff0f0eULL, 0xffffffffffffffffULL},
	{0xffffffffffffff0fULL, 0xffffffffffffffffULL}
};

static void inc_c(struct dec *d)
{
	__m128i aaa;

	d->num = _mm_add_epi64(d->num, _mm_set_epi64x(0, 1));

	aaa = _mm_cmpeq_epi64(d->num, _mm_setzero_si128());
	aaa = _mm_slli_si128(aaa, 8);
	d->num = _mm_sub_epi64(d->num, aaa);

	d->num = _mm_max_epu8(d->num, _mm_set1_epi8(0xf6));
}

static __m128i to_num(struct dec *d)
{
	__m128i aaa, bbb;

	aaa = _mm_sub_epi64(d->num, _mm_set1_epi8(0xc6));
	bbb = _mm_set_epi64x(0x0001020304050607ULL, 0x08090a0b0c0d0e0fULL);
	aaa = _mm_shuffle_epi8(aaa, bbb);
	aaa = _mm_shuffle_epi8(aaa, mask_shuffle[16 - d->ke]);

	return aaa;
}

static int out_fixnum(char *buf, struct dec *d, __m128i v)
{
	_mm_storeu_si128((__m128i *)buf, v);

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

static void fizzbuzz30(struct dec *d, uint64_t j)
{
	__m128i v;
	uint32_t wp_before = wp;
	char *p = get_p();
	char *p_s = p;
	int r;

	v = to_num(d);
	r = out_fixnum(p, d, v); p += r;
	r = out_two(p, "1\n");   p += r;
	r = out_fixnum(p, d, v); p += r;
	r = out_2fizz(p);        p += r;
	r = out_fixnum(p, d, v); p += r;
	r = out_4bandf(p);       p += r;
	r = out_fixnum(p, d, v); p += r;
	r = out_two(p, "7\n");   p += r;
	r = out_fixnum(p, d, v); p += r;
	r = out_8fandb(p);       p += r;
	inc_c(d);

	if (d->next_ke == j) {
		d->ke++;
		d->next_ke *= 10;
	}

	v = to_num(d);
	r = out_fixnum(p, d, v); p += r;
	r = out_1fizz(p);        p += r;
	r = out_fixnum(p, d, v); p += r;
	r = out_two(p, "3\n");   p += r;
	r = out_fixnum(p, d, v); p += r;
	r = out_4fb(p);          p += r;
	r = out_fixnum(p, d, v); p += r;
	r = out_two(p, "6\n");   p += r;
	r = out_fixnum(p, d, v); p += r;
	r = out_7fizz(p);        p += r;
	r = out_fixnum(p, d, v); p += r;
	r = out_9bandf(p);       p += r;
	inc_c(d);

	v = to_num(d);
	r = out_fixnum(p, d, v); p += r;
	r = out_two(p, "2\n");   p += r;
	r = out_fixnum(p, d, v); p += r;
	r = out_3fandb(p);       p += r;
	r = out_fixnum(p, d, v); p += r;
	r = out_6fizz(p);        p += r;
	r = out_fixnum(p, d, v); p += r;
	r = out_two(p, "8\n");   p += r;
	r = out_fixnum(p, d, v); p += r;
	r = out_9fb(p);          p += r;
	inc_c(d);

	wp += p - p_s;

	rb_wrap(wp_before);
}

int main(int argc, char *argv[])
{
	struct dec d;

	fcntl(1, F_SETPIPE_SZ, CHUNKSIZE);

	d.num = _mm_set1_epi8(0xf6);
	d.ke = 0;
	d.next_ke = 1;

	for (uint64_t i = 1; i <= 0xffffffffUL/10; i += 3) {
		fizzbuzz30(&d, i);
	}

	vwrite(1, get_p_r(), wp - rp);

	return 0;
}
