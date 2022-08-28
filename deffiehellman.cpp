#include "deffiehellman.h"

Deffiehellman::Deffiehellman()
{
    gmp_randinit_default(state);
    gmp_randseed_ui (state,time(nullptr) );
}

Deffiehellman::~Deffiehellman()
{
    gmp_randclear(state);
}


void Deffiehellman::generatePrimeNumbers(mpz_t p_prime, mpz_t q_prime,const int digits_count)
{


     mpz_t temp1;
     mpz_t temp2;
     mpz_t temp3;

     mpz_init(temp1);
     mpz_init(temp2);
     mpz_init(temp3);
     do {

     mpz_init_set_ui(temp1,10);


     mpz_pow_ui(temp2, temp1,digits_count);
     mpz_mul_ui(temp3,temp2,9);
     mpz_urandomm(temp1, state, temp3);
     mpz_add(temp3,temp2,temp1);
     mpz_nextprime(temp1,temp3);

     mpz_mul_ui(temp3,temp1,2);
     mpz_add_ui(temp2,temp3,1);

    } while(!mpz_probab_prime_p(temp2,40));

      mpz_set(q_prime,temp1);


    mpz_set(p_prime,temp2);
    mpz_clear(temp1);
    mpz_clear(temp2);
    mpz_clear(temp3);


}


void Deffiehellman::generateGeneratorNumber(mpz_t gen, const mpz_t p_prime, const mpz_t q_prime)
{
    mpz_t g_candidate;
    mpz_t temp;
    mpz_t lim;
    mpz_t random;


    mpz_init(g_candidate);
    mpz_init(temp);
    mpz_init(random);

    mpz_init_set(lim,p_prime);

    mpz_sub_ui(temp,lim,2);
    mpz_swap(temp,lim);

    do {
        mpz_urandomm(temp, state, lim);
        mpz_add_ui(random,temp,1);
        mpz_powm(temp,random, q_prime,p_prime);


    } while(!mpz_cmp_ui(temp,1));

     mpz_set(gen,random);



    mpz_clear(g_candidate);
    mpz_clear(temp);
    mpz_clear(lim);
    mpz_clear(random);

}

void Deffiehellman::generateNumber(mpz_t result, const int digits_count)
{
    mpz_t temp1;
    mpz_t temp2;
    mpz_t temp3;

    mpz_init(temp1);
    mpz_init(temp2);
    mpz_init(temp3);


    mpz_init_set_ui(temp1,10);


    mpz_pow_ui(temp2, temp1,digits_count);
    mpz_mul_ui(temp3,temp2,9);
    mpz_urandomm(temp1, state, temp3);
    mpz_add(temp3,temp2,temp1);

   mpz_set(result,temp3);

   mpz_clear(temp1);
   mpz_clear(temp2);
   mpz_clear(temp3);


}
void Deffiehellman::calculate(mpz_t result, const mpz_t gen, const mpz_t exp, const mpz_t p)
{
     mpz_powm(result,gen,exp,p);
}
