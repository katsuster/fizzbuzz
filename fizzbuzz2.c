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

char tab2[4 * 10000];

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
		tab2[i * 4 + 0] = '0' + (i / 1000) % 10;
		tab2[i * 4 + 1] = '0' + (i / 100) % 10;
		tab2[i * 4 + 2] = '0' + (i / 10) % 10;
		tab2[i * 4 + 3] = '0' + i % 10;
	}

	return 0;
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

struct dec {
	unsigned long long h;
	unsigned long long l;
	int ke;
};

//8 digits, offset 0xf6 (ff: 9, fe: 8, ..., f6: 0)
#define D_ZERO    0xf6f6f6f6f6f6f6f6ULL
#define D_ONE     0xf6f6f6f6f6f6f6f7ULL
#define D_MAX     0xffffffffffffffffULL
//convert from digits to characters (ex. 0xf6 - 0xc6 = '0')
#define D_TOCHR   0xc6c6c6c6c6c6c6c6ULL

void inc_c(struct dec *d)
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

void inc_nc(struct dec *d)
{
	d->l++;
}

int out_num(char *buf, struct dec *d)
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

int main(int argc, char *argv[])
{
	struct dec d = {D_ZERO, D_ZERO, 1};
	unsigned int next_ke = 10, i = 1, j = 0;

	fcntl(1, F_SETPIPE_SZ, BUFSIZE / 2);

	gentbl();

	j = 10;
	for (i = 1; i < 0xffffffd2;) {
		unsigned int wp_before = wp;
		char *p = &buf[wrap(wp)];
		char *p_s = p;
		int r;

		inc_nc(&d); r = out_num(p, &d); p += r;
		inc_nc(&d); r = out_num(p, &d); p += r;
		inc_nc(&d); r = out_fizz(p);    p += r;
		inc_nc(&d); r = out_num(p, &d); p += r;
		inc_nc(&d); r = out_bandf(p);   p += r;
		inc_nc(&d);
		inc_nc(&d); r = out_num(p, &d); p += r;
		inc_nc(&d); r = out_num(p, &d); p += r;
		inc_nc(&d); r = out_fandb(p);   p += r;
		inc_c(&d);

		if (next_ke == j) {
			d.ke++;
			next_ke *= 10;
		}

		inc_nc(&d); r = out_num(p, &d); p += r;
		inc_nc(&d); r = out_fizz(p);    p += r;
		inc_nc(&d); r = out_num(p, &d); p += r;
		inc_nc(&d); r = out_num(p, &d); p += r;
		inc_nc(&d); r = out_fb(p);      p += r;
		inc_nc(&d); r = out_num(p, &d); p += r;
		inc_nc(&d); r = out_num(p, &d); p += r;
		inc_nc(&d); r = out_fizz(p);    p += r;
		inc_nc(&d); r = out_num(p, &d); p += r;
		inc_c(&d);  r = out_bandf(p);   p += r;

		inc_nc(&d);
		inc_nc(&d); r = out_num(p, &d); p += r;
		inc_nc(&d); r = out_num(p, &d); p += r;
		inc_nc(&d); r = out_fandb(p);   p += r;
		inc_nc(&d);
		inc_nc(&d); r = out_num(p, &d); p += r;
		inc_nc(&d); r = out_fizz(p);    p += r;
		inc_nc(&d); r = out_num(p, &d); p += r;
		inc_nc(&d); r = out_num(p, &d); p += r;
		inc_c(&d);  r = out_fb(p);      p += r;

		wp += p - p_s;

		if (wrap(wp) < wrap(wp_before)) {
			memcpy(&buf[0], &buf[BUFSIZE], wrap(wp));
		}
		if (wp - rp >= CHUNKSIZE) {
			vwrite(1, &buf[wrap(rp)], CHUNKSIZE);
			rp += CHUNKSIZE;
		}

		j += 30;
		i += 30;
	}

	for (; i < 0xffffffff;) {
		i = fizzbuzz(i);
	}

	vwrite(1, &buf[wrap(rp)], wp - rp);

	return 0;
}
