#ifndef GOST_H
#define GOST_H
#include <stdint.h>

struct gost_context {
    uint32_t key[8];
};


#define rot11(x) ( (x << 11) | (x >> (32-11)) )


#define GET_32BIT_MSB_FIRST(cp) \
    (((unsigned long)(unsigned char)(cp)[3]) | \
    ((unsigned long)(unsigned char)(cp)[2] << 8) | \
    ((unsigned long)(unsigned char)(cp)[1] << 16) | \
    ((unsigned long)(unsigned char)(cp)[0] << 24))

#define PUT_32BIT_MSB_FIRST(cp, value) do { \
    (cp)[3] = (value); \
    (cp)[2] = (value) >> 8; \
    (cp)[1] = (value) >> 16; \
    (cp)[0] = (value) >> 24; } while (0)


static inline void gost_key_setup(char *buf, struct gost_context *sched)
{
    for (int i = 0; i < 4; i++) {
        sched->key[i*2] = GET_32BIT_MSB_FIRST(buf + i);
        sched->key[i*2+1] = GET_32BIT_MSB_FIRST(buf + 4 + i);
    }
}

static const uint32_t Sboxes[8][16] = {
    {0xb0000000, 0xa0000000, 0x40000000, 0xe0000000, 0x30000000, 0x90000000, 0xc0000000, 0x60000000, 0x20000000, 0x0, 0xd0000000, 0x10000000, 0x80000000, 0x70000000, 0xf0000000, 0x50000000},
    {0x8000000, 0x4000000, 0xa000000, 0xd000000, 0x5000000, 0x6000000, 0x2000000, 0x0, 0xc000000, 0x7000000, 0xe000000, 0xf000000, 0x9000000, 0xb000000, 0x1000000, 0x3000000},
    {0x700000, 0x600000, 0x300000, 0x200000, 0x500000, 0x400000, 0xd00000, 0x900000, 0x800000, 0xf00000, 0xe00000, 0x0, 0xb00000, 0xa00000, 0xc00000, 0x100000},
    {0x10000, 0x40000, 0x0, 0x30000, 0xd0000, 0xb0000, 0x20000, 0xf0000, 0x80000, 0xa0000, 0xc0000, 0x90000, 0xe0000, 0x50000, 0x60000, 0x70000},
    {0x2000, 0x7000, 0xf000, 0xa000, 0x5000, 0xc000, 0xb000, 0x4000, 0x1000, 0x0, 0xd000, 0x9000, 0xe000, 0x6000, 0x3000, 0x8000},
    {0x700, 0x0, 0x400, 0x200, 0x500, 0xa00, 0x800, 0x300, 0xe00, 0xd00, 0xf00, 0xc00, 0xb00, 0x100, 0x600, 0x900},
    {0x70, 0x60, 0x40, 0xa0, 0xf0, 0xd0, 0xe0, 0x20, 0x0, 0x80, 0x30, 0xb0, 0x90, 0x10, 0xc0, 0x50},
    {0xc, 0x8, 0x2, 0x1, 0xa, 0x5, 0xb, 0xd, 0x3, 0xf, 0x6, 0x7, 0x9, 0x0, 0x4, 0xe}
};

#define f_func_gost(R, key) (\
    sboxBits = R + key, \
    Sboxes[0] [(sboxBits >> 28) & 0xF] | \
    Sboxes[1] [(sboxBits >> 24) & 0xF] | \
    Sboxes[2] [(sboxBits >> 20) & 0xF] | \
    Sboxes[3] [(sboxBits >> 16) & 0xF] | \
    Sboxes[4] [(sboxBits >> 12) & 0xF] | \
    Sboxes[5] [(sboxBits >>  8) & 0xF] | \
    Sboxes[6] [(sboxBits >>  4) & 0xF] | \
    Sboxes[7] [(sboxBits      ) & 0xF]  )

static void gostEncryptBlock(uint32_t *output, uint32_t L, uint32_t R,struct gost_context *sched)
{
    uint32_t swap, sboxBits;

    uint32_t tempRight;

    for(int i = 0; i < 24; i ++)
    {
            tempRight = R;
            tempRight = f_func_gost(tempRight, sched->key[i%8]);
            tempRight = rot11(tempRight);
            tempRight ^= L;
            L = R;
            R = tempRight;
    }

    for(int i = 7; i >=0; i--)
    {
            tempRight = R;
            tempRight = f_func_gost(tempRight, sched->key[i]);
            tempRight = rot11(tempRight);
            tempRight ^= L;
            L = R;
            R = tempRight;
    }

    swap = L;
    L = R;
    R = swap;

    output[0] = L;
    output[1] = R;

}


static void gostDecryptBlock(uint32_t *output, uint32_t L, uint32_t R,struct gost_context *sched)
{
    uint32_t swap, sboxBits;

    uint32_t tempRight;

    for(int i = 0; i < 8; i ++)
    {
            tempRight = R;
            tempRight = f_func_gost(tempRight, sched->key[i]);
            tempRight = rot11(tempRight);
            tempRight ^= L;
            L = R;
            R = tempRight;
    }

    for(int i = 8; i < 32; i++)
    {
            tempRight = R;
            tempRight = f_func_gost(tempRight, sched->key[7 - (i % 8)]);
            tempRight = rot11(tempRight);
            tempRight ^= L;
            L = R;
            R = tempRight;
    }

    swap = L;
    L = R;
    R = swap;

    output[0] = L;
    output[1] = R;

}

#endif // GOST_H
