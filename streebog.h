#ifndef STREEBOG_H
#define STREEBOG_H
#include <stdint.h>
#include <string.h>


uint8_t* hashFinal(const uint8_t *M, size_t size);
void gn(uint8_t* h,const uint8_t* m, const uint8_t* N);
void E(uint8_t* res, const uint8_t* K,const uint8_t* m);
void P(uint8_t *block);
void L(uint8_t *block);
void Sum(uint8_t *res, const uint8_t *m );
void Xor64(uint8_t *res, const uint8_t *a,const uint8_t *b);

class Streebog
{   
public:
    Streebog();
    ~Streebog() {};

    void Clear();
    void hashBlock(const unsigned char *buf);
    void hashFinaleBlock(const unsigned char *buf, unsigned short int len, size_t fileSize);
    unsigned char* getHashResult();
    int GetMode();

private:
    void gn(uint8_t* h,const uint8_t* m, const uint8_t* N);
    void E(uint8_t* res, const uint8_t* K,const uint8_t* m);
    inline void S(uint8_t *block);
    inline void P(uint8_t *block);
    inline void Sum(uint8_t *res, const uint8_t *m );
    inline void Xor64(uint8_t *res, const uint8_t *a,const uint8_t *b);
    void L(uint8_t *block);
    void Init();



    uint8_t N[64];
    uint8_t Sigma[64];
    uint8_t h[64];
    uint8_t v512[64];


    uint8_t zero[64];
    uint8_t mSize[64];

};

#endif // STREEBOG_H
