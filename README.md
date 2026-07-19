# Bitsliced AREION

Portable, constant-time C implementations of the [AREION-256 and AREION-512](https://eprint.iacr.org/2023/794) permutations.

They target CPUs without AES instructions and use no lookup tables or data-dependent branches.

The AES S-box is the Maximov&Ekdahl circuit also used by the bitsliced AEGIS and HiAE implementations.

The external byte state is packed once, remains bitsliced for the complete permutation, and is unpacked only at the end.

AREION-512 uses a rolling three-lane pipeline.

After its initial layer, every S-box pass finishes the dependent `F3` branch of one round while starting the two independent branches of the next.

This schedule needs 16 bitsliced S-box passes for all 15 rounds, rather than 30 passes in a direct round-by-round implementation.

AREION-256 has an unavoidable round-to-round dependency and uses two S-box passes per round.

## API

```c
#include <areion.h>

void areion256_permute(uint8_t state[32]);
void areion512_permute(uint8_t state[64]);
```

Both functions transform their state in place.

## Build and test

```sh
make
make test
make benchmark
```

Or with Zig:

```sh
zig build -Drelease
zig build test
```