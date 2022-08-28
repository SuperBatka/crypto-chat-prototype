#include "rsa.h"
#include <time.h>
#include <QDebug>


Rsa::Rsa()

{
    init();
    moduleByteCount = 0;

}

Rsa::~Rsa()
{
    mpz_clear(pPrime_m);
    mpz_clear(qPrime_m);
    mpz_clear(nModule_m);
    mpz_clear(closeExp_m);
    mpz_clear(openExp_m);
    mpz_clear(result_m);
}

bool Rsa::isPrime(const mpz_t candidate)
{
        if(mpz_even_p (candidate))
        {
            return false;
        }

        if(mpz_cmp_ui (candidate, 2)<1)
        {
            return false;
        }
        mpz_class num(candidate);
        mpz_class num_m(candidate);
        mpz_class random;
        num_m = num_m - 1;
        num = num -1;

        mpz_t pow_result;
        mpz_t sqr_result;

        mpz_init(pow_result);
        mpz_init(sqr_result);

        uint64_t s = 0;

        while ( num % 2 == 0) {
            num = num / 2;
            s++;
        }

        gmp_randclass r1 (gmp_randinit_default);
        r1.seed(time(nullptr));

        for(int i = 0; i < 40; i++)
        {
            random = r1.get_z_range(num_m - 3);
            random = random + 2;
            mpz_powm(pow_result, random.get_mpz_t(), num.get_mpz_t(), candidate);

            mpz_class pow_res(pow_result);

            if((pow_res ==  1) || (pow_res == num_m) ) {
                continue;
            }

            for(uint64_t j = 0; j < (s); j++) {
                mpz_powm_ui(sqr_result, pow_res.get_mpz_t(), 2, candidate);

                mpz_class sqr_res(sqr_result);

                if(sqr_res == 1)  {
                    mpz_clear(pow_result);
                    mpz_clear(sqr_result);
                    return false;
                }

                if(sqr_res == num_m) {
                    break;
                }
                pow_res = sqr_res;
                if (j  == (s-1)) {
                    mpz_clear(pow_result);
                    mpz_clear(sqr_result);
                    return false;
                }
            }


        }
        mpz_clear(pow_result);
        mpz_clear(sqr_result);
        return true;


    }

bool Rsa::setPrimeNumbers(const char *pBuffer, const char *qBuffer)
{
   mpz_set_str(pPrime_m, pBuffer,10);
   if (mpz_cmp_ui(pPrime_m,29)<0)
   {
       return false;
   }
   if(!isPrime(pPrime_m))
   {
       return false;
   }
   mpz_set_str(qPrime_m, qBuffer,10);
   if (mpz_cmp_ui(qPrime_m,29)<0)
   {
       return false;
   }
   if(!isPrime(qPrime_m))
   {
       return false;
   }
   mpz_mul(nModule_m, pPrime_m, qPrime_m);
   calcOpenExp();
   calcClosedExp();

   //moduleByteCount = mpz_size(nModule_m) * sizeof(mp_limb_t);
   moduleByteCount = (mpz_sizeinbase(nModule_m, 2) + 7) / 8;
   return true;
};

void Rsa::setParametrs(const char *openBuffer, const char *closeBuffer, const char *moduleBuffer)
{
    mpz_set_str(openExp_m, openBuffer,10);
    mpz_set_str(closeExp_m, closeBuffer,10);
    mpz_set_str(nModule_m, moduleBuffer,10);
    moduleByteCount = (mpz_sizeinbase(nModule_m, 2) + 7) / 8;
}

void Rsa::calcOpenExp()
{

    mpz_class fi;
    mpz_class p(pPrime_m);
    mpz_class q(qPrime_m);
    mpz_class random;

    fi = (p-1)*(q-1);

    gmp_randclass r1 (gmp_randinit_default);
    do
    {
        random = r1.get_z_range(mpz_class(fi - 1));
        random = random + 1;
    } while(gcd(fi, random)!= 1);
    mpz_set(openExp_m, random.get_mpz_t());
}

void Rsa::calcClosedExp()
{
    mpz_class fi;
    mpz_class p(pPrime_m);
    mpz_class q(qPrime_m);
    fi = (p-1)*(q-1);
    if(!mpz_invert (closeExp_m, openExp_m, fi.get_mpz_t()))
        throw std::invalid_argument("Обратные элемент не существует");
}


char* Rsa::encrypt(const char *buffer, const size_t len, size_t *result_size, bool last_block)
{
   *result_size = 0;
   size_t left_count = len % (moduleByteCount-1);
   size_t count = 0;
   if(left_count == 0)
   {
        count = len / (moduleByteCount-1); // количество раз, которое нужно будет считать по Модуль-1 байт
        left_count = (moduleByteCount-1);
   }
   else
   {
        count = len / (moduleByteCount-1) + 1;
        if ( len < (moduleByteCount-1))
        {
            left_count = (moduleByteCount-1);
        }
   }
   size_t size = count * moduleByteCount;
   memset(result,0,size);

   mpz_t data;
   mpz_t encrypted_data;

   mpz_init(data);
   mpz_init(encrypted_data);

   for(size_t i = 0; i < count; i++)
   {

       size_t g;

       if ( last_block && i == (count-1))
       {
           char *temp_data = new char[moduleByteCount-1];
           memset(temp_data,0,moduleByteCount-1);
           memcpy(temp_data,buffer + (moduleByteCount-1) * i,left_count);
           mpz_import(data, (moduleByteCount-1), 1, sizeof(char), 0, 0, temp_data);
           delete [] temp_data;
       } else {
           mpz_import(data, moduleByteCount-1, 1, sizeof(char), 0, 0, buffer + (moduleByteCount-1) * i);
       }
       mpz_powm(encrypted_data,data,openExp_m,nModule_m);
       size_t encrypted_data_byte_count = (mpz_sizeinbase(encrypted_data, 2) + 7) / 8;
       mpz_export(result + moduleByteCount * i + (moduleByteCount - encrypted_data_byte_count), &g, 1, sizeof(char), 0, 0, encrypted_data);
       *result_size+=moduleByteCount;
   }

   mpz_clear(data);
   mpz_clear(encrypted_data);


   return result;
}
void Rsa::setBufferSize(size_t bufferSize)
{
    if(bufferSize > moduleByteCount)
        result = new char[bufferSize * 2];
    else
        result = new char[moduleByteCount * 4];
}
char* Rsa::decrypt(const char *buffer, const size_t len, size_t *result_size)
{
   *result_size = 0;
   size_t left_count = len % moduleByteCount;
   size_t count = 0;

   if(left_count == 0)
   {
        count = len / moduleByteCount; // количество раз, которое нужно будет считать по Модуль-1 байт
        left_count = moduleByteCount;
   }
   else
   {
        count = len / moduleByteCount + 1;
        if ( len < moduleByteCount)
        {
            left_count = moduleByteCount;
        }
   }

  // char *result = new char[len];
   memset(result,0,len);

   mpz_t data;
   mpz_t encrypted_data;

   mpz_init(data);
   mpz_init(encrypted_data);

   for(size_t i = 0; i < count; i++)
   {
       size_t g = 0;
       mpz_import(encrypted_data, moduleByteCount, 1, sizeof(char), 0, 0, buffer + moduleByteCount * i);
       mpz_powm(data,encrypted_data,closeExp_m,nModule_m);
       size_t encrypted_data_byte_count = (mpz_sizeinbase(data, 2) + 7) / 8;
       if( (moduleByteCount - 1 - encrypted_data_byte_count) < moduleByteCount)
       {
          mpz_export(result + (moduleByteCount - 1) * i + (moduleByteCount - 1 - encrypted_data_byte_count), &g, 1, sizeof(char), 0, 0, data);
       } else {
          mpz_export(result + (moduleByteCount - 1) * i, &g, 1, sizeof(char), 0, 0, data);
       }
       *result_size+=(moduleByteCount - 1);
   }

   mpz_clear(data);
   mpz_clear(encrypted_data);


   return result;
}


size_t Rsa::getModuleByteCount()
{
    return moduleByteCount;
}



void Rsa::generateNumber(mpz_t generated_num,const int digits_count)
{

     mpz_t temp1;
     mpz_t temp2;
     mpz_t temp3;

     mpz_init_set_ui(temp1,10);
     mpz_init(temp2);
     mpz_init(temp3);

     mpz_pow_ui(temp2, temp1,digits_count);
     mpz_mul_ui(temp3,temp2,9);
     mpz_urandomm(temp1, state, temp3);
     mpz_add(temp3,temp2,temp1);
     mpz_nextprime(generated_num,temp3);

     mpz_clear(temp1);
     mpz_clear(temp2);
     mpz_clear(temp3);

}

void Rsa::clear()
{
    mpz_clear(pPrime_m);
    mpz_clear(qPrime_m);
    mpz_clear(nModule_m);
    mpz_clear(closeExp_m);
    mpz_clear(openExp_m);
    mpz_clear(result_m);
    gmp_randclear( state);
    init();
}


void Rsa::init()
{
    mpz_init(pPrime_m);
    mpz_init(qPrime_m);
    mpz_init(nModule_m);
    mpz_init(closeExp_m);
    mpz_init(openExp_m);
    mpz_init(result_m);
    gmp_randinit_default(state);
    gmp_randseed_ui (state,time(nullptr) );
    moduleByteCount = 0;
}

void Rsa::generatePrimeNumbers(int digits_count)
{
    generateNumber(pPrime_m, digits_count-1);
    generateNumber(qPrime_m, digits_count-1);
    mpz_mul(nModule_m, pPrime_m, qPrime_m);
    calcOpenExp();
    calcClosedExp();
    moduleByteCount = (mpz_sizeinbase(nModule_m, 2) + 7) / 8;
}


char* Rsa::getStrParam(size_t *size, mpz_t param)
{
    *size = mpz_sizeinbase(param, 10);
    char *strparam = new(std::nothrow) char[*size+1];
    *size += 1;
    mpz_get_str(strparam, 10, param);
    if (strparam == nullptr)
    {
        *size = 0;
        return nullptr;
    } else return strparam;
}

char* Rsa::getStrModule(size_t *size)
{
    return getStrParam(size, nModule_m);
}

char* Rsa::getStrQPrime(size_t *size)
{
    return getStrParam(size,qPrime_m);

}

char* Rsa::getStrPPrime(size_t *size)
{
    return getStrParam(size,pPrime_m);
}

char* Rsa::getStrClosedExp(size_t *size)
{
    return getStrParam(size, closeExp_m);
}

char* Rsa::getStrOpenExp(size_t *size)
{
    return getStrParam(size, openExp_m);
}

void Rsa::getClosedExp(mpz_t dst)
{
    mpz_set(dst,closeExp_m);
}

void Rsa::getOpenExp(mpz_t dst)
{
    mpz_set(dst,openExp_m);
}

void Rsa::getModule(mpz_t dst)
{
    mpz_set(dst,nModule_m);
}

void Rsa::setClosedExp(const mpz_t dst)
{
    mpz_set(closeExp_m,dst);
}

void Rsa::setOpenExp(const mpz_t src)
{
    mpz_set(openExp_m,src);
}

void Rsa::setModule(const mpz_t src)
{
    mpz_set(nModule_m,src);
    moduleByteCount = (mpz_sizeinbase(nModule_m, 2) + 7) / 8;
}
