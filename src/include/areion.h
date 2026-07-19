#ifndef AREION_H
#define AREION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AREION256_STATEBYTES 32
#define AREION512_STATEBYTES 64

void areion256_permute(uint8_t state[AREION256_STATEBYTES]);

void areion512_permute(uint8_t state[AREION512_STATEBYTES]);

#ifdef __cplusplus
}
#endif

#endif
