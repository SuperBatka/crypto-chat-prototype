#ifndef RSA_H
#define RSA_H
#include <random>
#include <gmp.h>
#include <gmpxx.h>


class Rsa
{

public:
    Rsa();
    ~Rsa();

    bool setPrimeNumbers(const char *pBuffer, const char *qBuffer);
    void setParametrs(const char *openBuffer, const char *closeBuffer, const char *moduleBuffer);
    void generatePrimeNumbers(int digits_count);
    void setBufferSize(size_t bufferSize);
    size_t getModuleByteCount();
    char* encrypt(const char *buffer, const size_t len, size_t *result_size,bool last_block);
    char* decrypt(const char *buffer, const size_t len, size_t *result_size);
    char* getStrModule(size_t *size);
    char* getStrQPrime(size_t *size);
    char* getStrPPrime(size_t *size);
    char* getStrClosedExp(size_t *size);
    char* getStrOpenExp(size_t *size);


    void getClosedExp(mpz_t dst);
    void getOpenExp(mpz_t dst);
    void getModule(mpz_t dst);
    void setClosedExp(const mpz_t src);
    void setOpenExp(const mpz_t src);
    void setModule(const mpz_t src);


    void clear();


private:
    mpz_t pPrime_m;
    mpz_t qPrime_m;
    mpz_t nModule_m;
    mpz_t closeExp_m;
    mpz_t openExp_m;
    mpz_t result_m;
    gmp_randstate_t state;
    size_t moduleByteCount;


    bool isPrime(const mpz_t candidate);
    void calculateModule();
    void calcOpenExp();
    void calcClosedExp();
    void generateNumber(mpz_t generated_num, const int digits_count);
    void init();
    char* getStrParam(size_t *size, mpz_t param);
    char *result;





};

#endif // RSA_H
