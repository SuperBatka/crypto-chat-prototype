#ifndef GOST3410_12_H
#define GOST3410_12_H
#include <gmp.h>
#include <time.h>
#include <memory.h>

class gost3410_12
{
public:
    gost3410_12();
    ~gost3410_12();
    char* sign(const char *hash);
    void set_default_Elleptic_Curve();
    void generate_signature_keys();
    bool set_Elleptic_Curve_Q_point(const char *x,const char *y);
    bool set_d_parameter(const char *d);
    bool set_Elleptic_Curve_P_point(const char *x,const char *y);
    bool set_q_parameter(const char *q);
    bool set_m_parameter(const char *m);
    bool checkSign(const char *signature, const char *hash);
    bool set_Elliptic_Curve_parameters(const char* a, const char *b, const char *p);


    char* get_sign_key_d();
    char* get_signcheck_keyQ_x();
    char* get_signcheck_keyQ_y();


private:

    struct Point {
        mpz_t x;
        mpz_t y;

        Point()
        {
            mpz_init(x);
            mpz_init(y);
        }
        ~Point()
        {
            mpz_clear(x);
            mpz_clear(y);
        }

    };

    struct Elliptic_Curve {
        mpz_t a;
        mpz_t b;
        mpz_t p;

        Elliptic_Curve()
        {
            mpz_init(a);
            mpz_init(b);
            mpz_init(p);
        }
        ~Elliptic_Curve()
        {
            mpz_clear(a);
            mpz_clear(b);
            mpz_clear(p);
        }
    };

    void Point_Doubling(Point &R,const Point &P);
    void Point_Addition(Point &R, const Point &P,const Point &Q);
    void Scalar_Multiplication(Point &R, const Point &P,const mpz_t m);
    Elliptic_Curve EC;
    Point Q;
    Point P;
    Point R;
    mpz_t q;
    mpz_t m;
    mpz_t d;
    gmp_randstate_t state;
};

#endif // GOST3410_12_H
