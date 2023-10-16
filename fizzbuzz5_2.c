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
static int f __attribute__((aligned(8)));

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

static __m128i inc_c(__m128i num)
{
	__m128i aaa;

	num = _mm_add_epi64(num, _mm_set_epi64x(0, 1));

	aaa = _mm_cmpeq_epi64(num, _mm_setzero_si128());
	aaa = _mm_slli_si128(aaa, 8);
	num = _mm_sub_epi64(num, aaa);

	num = _mm_max_epu8(num, _mm_set1_epi8(0xf6));

	return num;
}

static __m128i to_num(__m128i num, int k)
{
	__m128i aaa, bbb;

	aaa = _mm_sub_epi64(num, _mm_set1_epi8(0xc6));
	bbb = _mm_set_epi64x(0x0001020304050607ULL, 0x08090a0b0c0d0e0fULL);
	aaa = _mm_shuffle_epi8(aaa, bbb);
	aaa = _mm_shuffle_epi8(aaa, mask_shuffle[16 - k]);

	return aaa;
}

static int out_fixnum(char *buf, __m128i v, int k)
{
	_mm_storeu_si128((__m128i *)buf, v);

	return k;
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
	//"1\nFizz\n " = 0x200a7a7a69460a31
	*((uint64_t *)buf) = 0x200a7a7a69460a31;
	return 7;
}

static int out_2fizz(char *buf)
{
	//"2\nFizz\n " = 0x200a7a7a69460a32
	*((uint64_t *)buf) = 0x200a7a7a69460a32;
	return 7;
}

static int out_6fizz(char *buf)
{
	//"6\nFizz\n " = 0x200a7a7a69460a36
	*((uint64_t *)buf) = 0x200a7a7a69460a36;
	return 7;
}

static int out_7fizz(char *buf)
{
	//"7\nFizz\n " = 0x200a7a7a69460a37
	*((uint64_t *)buf) = 0x200a7a7a69460a37;
	return 7;
}

#if 1

static int out_4fb(char *buf)
{
	//"4\nFizzBu" = 0x75427a7a69460a34
	//"zz\n" = 0x2e0a7a7a
	*((uint64_t *)buf) = 0x75427a7a69460a34;
	*((uint32_t *)(buf + 8)) = 0x2e0a7a7a;
	return 11;
}

static int out_9fb(char *buf)
{
	//"9\nFizzBu" = 0x75427a7a69460a39
	//"zz\n" = 0x2e0a7a7a
	*((uint64_t *)buf) = 0x75427a7a69460a39;
	*((uint32_t *)(buf + 8)) = 0x2e0a7a7a;
	return 11;
}

static int out_3fandb(char *buf)
{
	//"3\nFizz\nB" = 0x420a7a7a69460a33
	//"uzz\n" = 0x0a7a7a75
	*((uint64_t *)buf) = 0x420a7a7a69460a33;
	*((uint32_t *)(buf + 8)) = 0x0a7a7a75;
	return 12;
}

static int out_8fandb(char *buf)
{
	//"8\nFizz\nB" = 0x420a7a7a69460a38
	//"uzz\n" = 0x0a7a7a75
	*((uint64_t *)buf) = 0x420a7a7a69460a38;
	*((uint32_t *)(buf + 8)) = 0x0a7a7a75;
	return 12;
}

static int out_4bandf(char *buf)
{
	//"4\nBuzz\nF" = 0x460a7a7a75420a34
	//"izz\n" = 0x0a7a7a69
	*((uint64_t *)buf) = 0x460a7a7a75420a34;
	*((uint32_t *)(buf + 8)) = 0x0a7a7a69;
	return 12;
}

static int out_9bandf(char *buf)
{
	//"9\nBuzz\nF" = 0x460a7a7a75420a39
	//"izz\n" = 0x0a7a7a69
	*((uint64_t *)buf) = 0x460a7a7a75420a39;
	*((uint32_t *)(buf + 8)) = 0x0a7a7a69;
	return 12;
}

#else

static int out_4fb(char *buf)
{
	//"4\nFizzBu" = 0x75427a7a69460a34
	//"zz\n" = 0x2e0a7a7a
	__m128i t = _mm_set_epi64x(0x2e0a7a7a, 0x75427a7a69460a34ULL);
	_mm_storeu_si128((__m128i *)buf, t);

	return 11;
}

static int out_9fb(char *buf)
{
	//"9\nFizzBu" = 0x75427a7a69460a39
	//"zz\n" = 0x2e0a7a7a
	__m128i t = _mm_set_epi64x(0x2e0a7a7a, 0x75427a7a69460a39ULL);
	_mm_storeu_si128((__m128i *)buf, t);

	return 11;
}

static int out_3fandb(char *buf)
{
	//"3\nFizz\nB" = 0x420a7a7a69460a33
	//"uzz\n" = 0x0a7a7a75
	__m128i t = _mm_set_epi64x(0x0a7a7a75, 0x420a7a7a69460a33ULL);
	_mm_storeu_si128((__m128i *)buf, t);

	return 12;
}

static int out_8fandb(char *buf)
{
	//"8\nFizz\nB" = 0x420a7a7a69460a38
	//"uzz\n" = 0x0a7a7a75
	__m128i t = _mm_set_epi64x(0x0a7a7a75, 0x420a7a7a69460a38ULL);
	_mm_storeu_si128((__m128i *)buf, t);

	return 12;
}

static int out_4bandf(char *buf)
{
	//"4\nBuzz\nF" = 0x460a7a7a75420a34
	//"izz\n" = 0x0a7a7a69
	__m128i t = _mm_set_epi64x(0x0a7a7a69, 0x460a7a7a75420a34ULL);
	_mm_storeu_si128((__m128i *)buf, t);

	return 12;
}

static int out_9bandf(char *buf)
{
	//"9\nBuzz\nF" = 0x460a7a7a75420a39
	//"izz\n" = 0x0a7a7a69
	__m128i t = _mm_set_epi64x(0x0a7a7a69, 0x460a7a7a75420a39ULL);
	_mm_storeu_si128((__m128i *)buf, t);

	return 12;
}

#endif

int main(int argc, char *argv[])
{
	__m128i d;
	int ke = 0;
	uint64_t next_ke = 1;

	fcntl(1, F_SETPIPE_SZ, CHUNKSIZE);

	d = _mm_set1_epi8(0xf6);

	char *p = buf2[f];
	for (uint64_t i = 1; i <= 0xffffffffUL/10; i += 3) {
		__m128i v;

		v = to_num(d, ke);
		p += out_fixnum(p, v, ke);
		p += out_two(p, "1\n");
		p += out_fixnum(p, v, ke);
		p += out_2fizz(p);
		p += out_fixnum(p, v, ke);
		p += out_4bandf(p);
		p += out_fixnum(p, v, ke);
		p += out_two(p, "7\n");
		p += out_fixnum(p, v, ke);
		p += out_8fandb(p);
		d = inc_c(d);

		if (next_ke == i) {
			ke++;
			next_ke *= 10;
		}

		v = to_num(d, ke);
		p += out_fixnum(p, v, ke);
		p += out_1fizz(p);
		p += out_fixnum(p, v, ke);
		p += out_two(p, "3\n");
		p += out_fixnum(p, v, ke);
		p += out_4fb(p);
		p += out_fixnum(p, v, ke);
		p += out_two(p, "6\n");
		p += out_fixnum(p, v, ke);
		p += out_7fizz(p);
		p += out_fixnum(p, v, ke);
		p += out_9bandf(p);
		d = inc_c(d);

		v = to_num(d, ke);
		p += out_fixnum(p, v, ke);
		p += out_two(p, "2\n");
		p += out_fixnum(p, v, ke);
		p += out_3fandb(p);
		p += out_fixnum(p, v, ke);
		p += out_6fizz(p);
		p += out_fixnum(p, v, ke);
		p += out_two(p, "8\n");
		p += out_fixnum(p, v, ke);
		p += out_9fb(p);
		d = inc_c(d);

		int n = p - buf2[f] - CHUNKSIZE;
		if (n >= 0) {
			struct iovec iov;
			ssize_t nn;

			iov.iov_base = buf2[f];
			iov.iov_len = CHUNKSIZE;

			do {
				nn = vmsplice(1, &iov, 1, 0);
				if (nn < 0) {
					perror("vmsplice");
					exit(1);
				}
				iov.iov_base += nn;
				iov.iov_len -= nn;
			} while (iov.iov_len > 0);

			f = !f;
			memcpy(buf2[f], &buf2[!f][CHUNKSIZE], n);
			p = &buf2[f][n];
		}
	}

	{
		struct iovec iov;
		ssize_t nn;

		iov.iov_base = buf2[f];
		iov.iov_len = CHUNKSIZE;

		do {
			nn = vmsplice(1, &iov, 1, 0);
			if (nn < 0) {
				perror("vmsplice");
				exit(1);
			}
			iov.iov_base += nn;
			iov.iov_len -= nn;
		} while (iov.iov_len > 0);
	}

	return 0;
}
