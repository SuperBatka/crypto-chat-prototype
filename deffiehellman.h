#ifndef DEFFIEHELLMAN_H
#define DEFFIEHELLMAN_H
#include <gmp.h>
#include <time.h>

class Deffiehellman
{
public:
    Deffiehellman();
    ~Deffiehellman();
    void generatePrimeNumbers(mpz_t p_prime, mpz_t q_prime,const int digits_count);
    void generateGeneratorNumber(mpz_t gen, const mpz_t p_prime, const mpz_t q_prime);
    void generateNumber(mpz_t result, const int digits_count);
    void calculate(mpz_t result, const mpz_t gen, const mpz_t exp, const mpz_t p);

private:
    gmp_randstate_t state;
};

#endif // DEFFIEHELLMAN_H
