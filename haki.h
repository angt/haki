#ifndef HAKI_H__
#define HAKI_H__

#include <stddef.h>

#if defined(__x86_64__)

#include <x86intrin.h>
typedef __m128i h128;

#elif defined(__aarch64__)

#include <arm_neon.h>
typedef uint8x16_t h128;

#endif

struct haki {
    h128 b[6];
};

struct haki haki_init(void);
struct haki haki_init_random(void);

void haki_absorb(struct haki *, const void *, size_t);
void haki_flip(struct haki *);
void haki_squeeze(struct haki *, void *, size_t);
unsigned long long haki_squeeze_n(struct haki *, unsigned long long);

#endif
