#include <stddef.h>
#include <stdint.h>

#include "include/areion.h"

#if defined(__GNUC__) || defined(__clang__)
#define ALWAYS_INLINE __attribute__((always_inline)) static inline
#else
#define ALWAYS_INLINE static inline
#endif

enum { AREION256_ROUNDS = 10, AREION512_ROUNDS = 15 };

static const uint16_t round_constants[AREION512_ROUNDS][8] = {
    { 0x1d20, 0x2107, 0xe416, 0x41c6, 0x7270, 0xc811, 0x65ba, 0x4dca },
    { 0x8539, 0x012e, 0xba6a, 0x2730, 0x6cff, 0x844e, 0x1464, 0x4e01 },
    { 0x1494, 0x9575, 0x79ad, 0x0fc8, 0x4897, 0x91fb, 0x13b0, 0xa314 },
    { 0xd978, 0x8f24, 0x7498, 0x17b9, 0x6d82, 0x55fd, 0x1085, 0x39bf },
    { 0xba9f, 0x3684, 0x0653, 0xf7ce, 0x1f2d, 0x6017, 0xc334, 0x3fe6 },
    { 0x5bf1, 0x7a5a, 0xe1fe, 0x7f83, 0xd6ba, 0xc337, 0xb72f, 0xd370 },
    { 0xa96d, 0x5a13, 0xcecb, 0xeb2d, 0xc722, 0x5693, 0x8219, 0x1b7d },
    { 0x8653, 0x0f06, 0x3586, 0x48cd, 0x7c20, 0x65be, 0x65d2, 0x4638 },
    { 0xbc56, 0x6929, 0xbb2a, 0x6f6b, 0x6395, 0xabf6, 0x351e, 0x16d0 },
    { 0x6929, 0xb3ce, 0x81a1, 0x94d7, 0x7396, 0x2571, 0x4b8c, 0xe493 },
    { 0xa4e3, 0x26c5, 0x5e3d, 0xf561, 0x9808, 0xa082, 0x0d10, 0x31d2 },
    { 0x8db0, 0xe568, 0x2b5c, 0x3e76, 0xbfe7, 0x01a1, 0x8585, 0x6540 },
    { 0x5a98, 0x80b2, 0x892d, 0x4dec, 0xf70a, 0xe5e9, 0x77a3, 0x10ff },
    { 0x509b, 0x9fd4, 0xe5ba, 0x9a55, 0xf20a, 0x6ae5, 0x709a, 0x6876 },
    { 0x2445, 0xddd0, 0x1c3d, 0xa2a3, 0x647c, 0x8281, 0x985d, 0x88a4 },
};

#define SBOX_BODY(T)                                                                               \
    do {                                                                                           \
        const T s0 = u[1] ^ u[4];                                                                  \
        const T s1 = u[5] ^ u[7];                                                                  \
        const T s2 = u[3] ^ s0;                                                                    \
        const T s3 = u[0] ^ u[2];                                                                  \
        const T q0 = s1 ^ s2;                                                                      \
        const T s4 = u[0] ^ u[6];                                                                  \
        const T s5 = u[2] ^ u[6];                                                                  \
        const T s6 = u[3] ^ s1;                                                                    \
        const T s7 = u[5] ^ s3;                                                                    \
        const T q1 = s1 ^ s5;                                                                      \
        const T q2 = u[2] ^ q0;                                                                    \
        const T q3 = s4 ^ s2;                                                                      \
        const T q4 = s3 ^ q0;                                                                      \
        const T s8 = u[4] ^ s3;                                                                    \
        const T q5 = s6 ^ s8;                                                                      \
        const T q6 = u[2] ^ u[3];                                                                  \
        const T q7 = u[6] ^ s2;                                                                    \
        const T s9 = u[6] ^ s0;                                                                    \
        const T q8 = s3 ^ s9;                                                                      \
        const T q9 = s4 ^ s6;                                                                      \
        const T q10 = s0 ^ s5;                                                                     \
        const T q12 = u[7] ^ s2;                                                                   \
        const T q13 = u[1] ^ s7;                                                                   \
        const T q14 = u[7] ^ s3;                                                                   \
        const T q15 = s2 ^ s7;                                                                     \
        const T q16 = u[1] ^ s1;                                                                   \
        const T q17 = u[1] ^ u[7];                                                                 \
        const T q11 = u[5];                                                                        \
        const T t20 = q6 & q12;                                                                    \
        const T t21 = q3 & q14;                                                                    \
        const T t22 = q1 & q16;                                                                    \
        const T t23 = q2 & q17;                                                                    \
        const T x0 = ((q3 | q14) ^ (q0 & q7)) ^ (t20 ^ t22);                                       \
        const T x1 = ((q4 | q13) ^ (q10 & q11)) ^ (t21 ^ t20);                                     \
        const T x2 = ((q2 | q17) ^ (q5 & q9)) ^ (t21 ^ t22);                                       \
        const T x3 = ((q8 | q15) ^ t23) ^ (t21 ^ (q4 & q13));                                      \
        const T a = x1 & ~x3;                                                                      \
        const T b = x0 & ~x3;                                                                      \
        const T c = x3 & ~x1;                                                                      \
        const T d = x2 & ~x1;                                                                      \
        const T e = x0 ^ a;                                                                        \
        const T y0 = x3 ^ (x2 & ~e);                                                               \
        const T f = x1 ^ b;                                                                        \
        const T y1 = c ^ (x2 & f);                                                                 \
        const T g = x2 ^ c;                                                                        \
        const T y2 = x1 ^ (x0 & ~g);                                                               \
        const T h = x3 ^ d;                                                                        \
        const T y3 = a ^ (x0 & h);                                                                 \
        const T y02 = y2 ^ y0;                                                                     \
        const T y13 = y3 ^ y1;                                                                     \
        const T y23 = y3 ^ y2;                                                                     \
        const T y01 = y1 ^ y0;                                                                     \
        const T y00 = y02 ^ y13;                                                                   \
        const T a0 = y01 & q11;                                                                    \
        const T a1 = y0 & q12;                                                                     \
        const T a2 = y1 & q0;                                                                      \
        const T a3 = y23 & q17;                                                                    \
        const T a4 = y2 & q5;                                                                      \
        const T a5 = y3 & q15;                                                                     \
        const T a6 = y13 & q14;                                                                    \
        const T a7 = y00 & q16;                                                                    \
        const T a8 = y02 & q13;                                                                    \
        const T a9 = y01 & q7;                                                                     \
        const T a10 = y0 & q10;                                                                    \
        const T a11 = y1 & q6;                                                                     \
        const T a12 = y23 & q2;                                                                    \
        const T a13 = y2 & q9;                                                                     \
        const T a14 = y3 & q8;                                                                     \
        const T a15 = y13 & q3;                                                                    \
        const T a16 = y00 & q1;                                                                    \
        const T a17 = y02 & q4;                                                                    \
        const T r0 = a1 ^ a5;                                                                      \
        const T r1 = a9 ^ a15;                                                                     \
        const T r2 = a4 ^ r0;                                                                      \
        const T r3 = a2 ^ a10;                                                                     \
        const T r4 = a11 ^ a17;                                                                    \
        const T r5 = a8 ^ r1;                                                                      \
        const T r6 = a0 ^ a16;                                                                     \
        const T r7 = a7 ^ a13;                                                                     \
        const T r8 = a11 ^ a14;                                                                    \
        const T r9 = r3 ^ r4;                                                                      \
        const T r10 = r5 ^ r6;                                                                     \
        const T r11 = r2 ^ r9;                                                                     \
        const T r12 = a3 ^ r0;                                                                     \
        const T r13 = r7 ^ r8;                                                                     \
        const T r14 = r12 ^ r13;                                                                   \
        u[0] = r10 ^ r14;                                                                          \
        const T r15 = a6 ^ a10;                                                                    \
        const T r16 = r15 ^ r2;                                                                    \
        u[1] = ~(r10 ^ r16);                                                                       \
        u[2] = ~(a2 ^ r2);                                                                         \
        const T r17 = a12 ^ a13;                                                                   \
        const T r18 = a15 ^ r17;                                                                   \
        u[3] = r18 ^ r11;                                                                          \
        const T r19 = a1 ^ a14;                                                                    \
        const T r20 = a17 ^ r3;                                                                    \
        const T r21 = r7 ^ r19;                                                                    \
        const T r22 = r5 ^ r20;                                                                    \
        u[4] = r21 ^ r22;                                                                          \
        const T r23 = a9 ^ a12;                                                                    \
        u[5] = r8 ^ r23;                                                                           \
        u[6] = ~(r1 ^ r4);                                                                         \
        u[7] = ~(a16 ^ r11);                                                                       \
    } while (0)

ALWAYS_INLINE void sbox32(uint32_t u[8]) { SBOX_BODY(uint32_t); }

ALWAYS_INLINE void sbox64(uint64_t u[8]) { SBOX_BODY(uint64_t); }

#undef SBOX_BODY

#define C32(x) ((uint32_t)UINT64_C(x))

#define SWAPMOVE(T, a, b, mask, n)                                                                 \
    do {                                                                                           \
        const T swapmove_tmp = ((b) ^ ((a) >> (n))) & (mask);                                      \
        (b) ^= swapmove_tmp;                                                                       \
        (a) ^= swapmove_tmp << (n);                                                                \
    } while (0)

#define DEFINE_TRANSPOSE(name, T, C)                                                               \
    static inline void name(T q[8])                                                                \
    {                                                                                              \
        SWAPMOVE(T, q[0], q[1], C(0x5555555555555555), 1);                                         \
        SWAPMOVE(T, q[2], q[3], C(0x5555555555555555), 1);                                         \
        SWAPMOVE(T, q[4], q[5], C(0x5555555555555555), 1);                                         \
        SWAPMOVE(T, q[6], q[7], C(0x5555555555555555), 1);                                         \
        SWAPMOVE(T, q[0], q[2], C(0x3333333333333333), 2);                                         \
        SWAPMOVE(T, q[1], q[3], C(0x3333333333333333), 2);                                         \
        SWAPMOVE(T, q[4], q[6], C(0x3333333333333333), 2);                                         \
        SWAPMOVE(T, q[5], q[7], C(0x3333333333333333), 2);                                         \
        SWAPMOVE(T, q[0], q[4], C(0x0f0f0f0f0f0f0f0f), 4);                                         \
        SWAPMOVE(T, q[1], q[5], C(0x0f0f0f0f0f0f0f0f), 4);                                         \
        SWAPMOVE(T, q[2], q[6], C(0x0f0f0f0f0f0f0f0f), 4);                                         \
        SWAPMOVE(T, q[3], q[7], C(0x0f0f0f0f0f0f0f0f), 4);                                         \
    }

DEFINE_TRANSPOSE(transpose32, uint32_t, C32)
DEFINE_TRANSPOSE(transpose64, uint64_t, UINT64_C)
#undef DEFINE_TRANSPOSE
#undef SWAPMOVE

#define DEFINE_SHIFTROWS(name, T, C)                                                               \
    static inline T name##_ror4(T x)                                                               \
    {                                                                                              \
        return ((x >> 4) & C(0x0fff0fff0fff0fff)) | ((x << 12) & C(0xf000f000f000f000));           \
    }                                                                                              \
    static inline T name##_ror8(T x)                                                               \
    {                                                                                              \
        return ((x >> 8) & C(0x00ff00ff00ff00ff)) | ((x << 8) & C(0xff00ff00ff00ff00));            \
    }                                                                                              \
    static inline T name##_ror12(T x)                                                              \
    {                                                                                              \
        return ((x >> 12) & C(0x000f000f000f000f)) | ((x << 4) & C(0xfff0fff0fff0fff0));           \
    }                                                                                              \
    static inline T name(T x)                                                                      \
    {                                                                                              \
        return (x & C(0x1111111111111111)) | (name##_ror4(x) & C(0x2222222222222222))              \
            | (name##_ror8(x) & C(0x4444444444444444))                                             \
            | (name##_ror12(x) & C(0x8888888888888888));                                           \
    }

DEFINE_SHIFTROWS(shiftrows32, uint32_t, C32)
DEFINE_SHIFTROWS(shiftrows64, uint64_t, UINT64_C)
#undef DEFINE_SHIFTROWS

#define DEFINE_MIXCOLUMNS(name, T, C)                                                              \
    static inline T name##_ror1(T x)                                                               \
    {                                                                                              \
        return ((x >> 1) & C(0x7777777777777777)) | ((x << 3) & C(0x8888888888888888));            \
    }                                                                                              \
    static inline T name##_ror2(T x)                                                               \
    {                                                                                              \
        return ((x >> 2) & C(0x3333333333333333)) | ((x << 2) & C(0xcccccccccccccccc));            \
    }                                                                                              \
    ALWAYS_INLINE void name(T u[8])                                                                \
    {                                                                                              \
        const T r0 = name##_ror1(u[0]);                                                            \
        const T d0 = u[0] ^ r0;                                                                    \
        const T s0 = r0 ^ name##_ror2(d0);                                                         \
        const T r1 = name##_ror1(u[1]);                                                            \
        const T d1 = u[1] ^ r1;                                                                    \
        const T s1 = r1 ^ name##_ror2(d1);                                                         \
        const T r2 = name##_ror1(u[2]);                                                            \
        const T d2 = u[2] ^ r2;                                                                    \
        const T s2 = r2 ^ name##_ror2(d2);                                                         \
        const T r3 = name##_ror1(u[3]);                                                            \
        const T d3 = u[3] ^ r3;                                                                    \
        const T s3 = r3 ^ name##_ror2(d3);                                                         \
        const T r4 = name##_ror1(u[4]);                                                            \
        const T d4 = u[4] ^ r4;                                                                    \
        const T s4 = r4 ^ name##_ror2(d4);                                                         \
        const T r5 = name##_ror1(u[5]);                                                            \
        const T d5 = u[5] ^ r5;                                                                    \
        const T s5 = r5 ^ name##_ror2(d5);                                                         \
        const T r6 = name##_ror1(u[6]);                                                            \
        const T d6 = u[6] ^ r6;                                                                    \
        const T s6 = r6 ^ name##_ror2(d6);                                                         \
        const T r7 = name##_ror1(u[7]);                                                            \
        const T d7 = u[7] ^ r7;                                                                    \
        const T s7 = r7 ^ name##_ror2(d7);                                                         \
        u[0] = d1 ^ s0;                                                                            \
        u[1] = d2 ^ s1;                                                                            \
        u[2] = d3 ^ s2;                                                                            \
        u[3] = d4 ^ d0 ^ s3;                                                                       \
        u[4] = d5 ^ d0 ^ s4;                                                                       \
        u[5] = d6 ^ s5;                                                                            \
        u[6] = d7 ^ d0 ^ s6;                                                                       \
        u[7] = d0 ^ s7;                                                                            \
    }

DEFINE_MIXCOLUMNS(mixcolumns32, uint32_t, C32)
DEFINE_MIXCOLUMNS(mixcolumns64, uint64_t, UINT64_C)
#undef DEFINE_MIXCOLUMNS
#undef C32

ALWAYS_INLINE void subbytes_shiftrows32(uint32_t q[8])
{
    size_t k;

    sbox32(q);
    for (k = 0; k < 8; k++) {
        q[k] = shiftrows32(q[k]);
    }
}

ALWAYS_INLINE void subbytes_shiftrows64(uint64_t q[8])
{
    size_t k;

    sbox64(q);
    for (k = 0; k < 8; k++) {
        q[k] = shiftrows64(q[k]);
    }
}

static void pack256(uint32_t q[8], const uint8_t state[32])
{
    uint32_t tmp;
    size_t i;

    for (i = 0; i < 8; i++) {
        q[i] = (uint32_t)state[i] | ((uint32_t)state[8 + i] << 8) | ((uint32_t)state[16 + i] << 16)
            | ((uint32_t)state[24 + i] << 24);
    }
    transpose32(q);
    for (i = 0; i < 4; i++) {
        tmp = q[i];
        q[i] = q[7 - i];
        q[7 - i] = tmp;
    }
}

static void unpack256(uint8_t state[32], uint32_t q[8])
{
    uint32_t tmp;
    size_t i;

    for (i = 0; i < 4; i++) {
        tmp = q[i];
        q[i] = q[7 - i];
        q[7 - i] = tmp;
    }
    transpose32(q);
    for (i = 0; i < 8; i++) {
        state[i] = (uint8_t)q[i];
        state[8 + i] = (uint8_t)(q[i] >> 8);
        state[16 + i] = (uint8_t)(q[i] >> 16);
        state[24 + i] = (uint8_t)(q[i] >> 24);
    }
}

static void pack512(uint64_t q[8], const uint8_t state[64])
{
    uint64_t tmp;
    size_t i;

    for (i = 0; i < 8; i++) {
        q[i] = (uint64_t)state[i] | ((uint64_t)state[8 + i] << 8) | ((uint64_t)state[16 + i] << 16)
            | ((uint64_t)state[24 + i] << 24) | ((uint64_t)state[32 + i] << 32)
            | ((uint64_t)state[40 + i] << 40) | ((uint64_t)state[48 + i] << 48)
            | ((uint64_t)state[56 + i] << 56);
    }
    transpose64(q);
    for (i = 0; i < 4; i++) {
        tmp = q[i];
        q[i] = q[7 - i];
        q[7 - i] = tmp;
    }
}

static void unpack512(uint8_t state[64], uint64_t q[8])
{
    uint64_t tmp;
    size_t i;

    for (i = 0; i < 4; i++) {
        tmp = q[i];
        q[i] = q[7 - i];
        q[7 - i] = tmp;
    }
    transpose64(q);
    for (i = 0; i < 8; i++) {
        state[i] = (uint8_t)q[i];
        state[8 + i] = (uint8_t)(q[i] >> 8);
        state[16 + i] = (uint8_t)(q[i] >> 16);
        state[24 + i] = (uint8_t)(q[i] >> 24);
        state[32 + i] = (uint8_t)(q[i] >> 32);
        state[40 + i] = (uint8_t)(q[i] >> 40);
        state[48 + i] = (uint8_t)(q[i] >> 48);
        state[56 + i] = (uint8_t)(q[i] >> 56);
    }
}

static inline uint32_t rotl_lanes32(uint32_t x) { return (x << 16) | (x >> 16); }

static inline uint64_t rotl_lanes64(uint64_t x) { return (x << 16) | (x >> 48); }

void areion256_permute(uint8_t state[AREION256_STATEBYTES])
{
    static const uint32_t lane_masks[2] = { UINT32_C(0x0000ffff), UINT32_C(0xffff0000) };
    uint32_t q[8];
    uint32_t work[8];
    size_t round;
    size_t k;

    pack256(q, state);
    for (round = 0; round < AREION256_ROUNDS; round++) {
        const size_t active = round & 1;
        const size_t target = active ^ 1;
        const uint32_t active_mask = lane_masks[active];
        const uint32_t target_mask = lane_masks[target];

        for (k = 0; k < 8; k++) {
            work[k] = q[k] & active_mask;
        }
        subbytes_shiftrows32(work);
        for (k = 0; k < 8; k++) {
            q[k] = (q[k] & target_mask) | (work[k] & active_mask);
        }
        mixcolumns32(work);
        for (k = 0; k < 8; k++) {
            work[k] = (rotl_lanes32(work[k]) & target_mask)
                ^ ((uint32_t)round_constants[round][k] << (16 * target));
        }
        subbytes_shiftrows32(work);
        mixcolumns32(work);
        for (k = 0; k < 8; k++) {
            q[k] ^= work[k] & target_mask;
        }
    }
    unpack256(state, q);
}

void areion512_permute(uint8_t state[AREION512_STATEBYTES])
{
    static const uint64_t lane_masks[4] = {
        UINT64_C(0x000000000000ffff),
        UINT64_C(0x00000000ffff0000),
        UINT64_C(0x0000ffff00000000),
        UINT64_C(0xffff000000000000),
    };
    uint64_t q[8];
    uint64_t pending[8];
    uint64_t work[8];
    uint64_t full[8];
    size_t round;
    size_t k;

    pack512(q, state);

    for (k = 0; k < 8; k++) {
        work[k] = q[k] & (lane_masks[0] | lane_masks[2]);
    }
    subbytes_shiftrows64(work);
    for (k = 0; k < 8; k++) {
        full[k] = work[k];
    }
    mixcolumns64(full);
    for (k = 0; k < 8; k++) {
        q[k] = (q[k] & ~lane_masks[0]) | (work[k] & lane_masks[0]);
        q[k] ^= rotl_lanes64(full[k] & (lane_masks[0] | lane_masks[2]));
        pending[k] = (work[k] & lane_masks[2]) ^ ((uint64_t)round_constants[0][k] << 32);
    }

    for (round = 0; round < AREION512_ROUNDS - 1; round++) {
        const size_t next = round + 1;
        const size_t a = next & 3;
        const size_t b = (next + 1) & 3;
        const size_t c = (next + 2) & 3;
        const uint64_t ma = lane_masks[a];
        const uint64_t mb = lane_masks[b];
        const uint64_t mc = lane_masks[c];

        for (k = 0; k < 8; k++) {
            work[k] = pending[k] | (q[k] & (ma | mc));
        }
        subbytes_shiftrows64(work);
        for (k = 0; k < 8; k++) {
            full[k] = work[k];
        }
        mixcolumns64(full);
        for (k = 0; k < 8; k++) {
            q[k] = (q[k] & ~(ma | mb)) | (full[k] & mb) | (work[k] & ma);
            q[k] ^= rotl_lanes64(full[k] & (ma | mc));
            pending[k] = (work[k] & mc) ^ ((uint64_t)round_constants[next][k] << (16 * c));
        }
    }

    subbytes_shiftrows64(pending);
    mixcolumns64(pending);
    for (k = 0; k < 8; k++) {
        q[k] = (q[k] & ~lane_masks[0]) | (pending[k] & lane_masks[0]);
    }

    for (k = 0; k < 8; k++) {
        q[k] = rotl_lanes64(q[k]);
    }
    unpack512(state, q);
}
