#include "gost3410_12.h"

gost3410_12::gost3410_12()
{
    gmp_randinit_default(state);
    gmp_randseed_ui (state,time(nullptr) );
    mpz_init(q);
    mpz_init(m);
    mpz_init(d);
}
gost3410_12::~gost3410_12()
{
    mpz_clear(q);
    mpz_clear(m);
    mpz_clear(d);
    gmp_randclear(state);
}




void gost3410_12::generate_signature_keys()
{
    mpz_t temp_d;
    mpz_init(temp_d);

    Point temp_Q;
    mpz_urandomm(temp_d, state, q);
    Scalar_Multiplication(temp_Q,this->P,temp_d);
    mpz_set(this->d, temp_d);
    mpz_set(this->Q.x, temp_Q.x);
    mpz_set(this->Q.y, temp_Q.y);
}

void gost3410_12::set_default_Elleptic_Curve()
{
    mpz_set_str(this->EC.p,"3623986102229003635907788753683874306021320925534678605086546150450856166624002482588482022271496854025090823603058735163734263822371964987228582907372403",10);
    mpz_set_str(this->EC.a,"7",10);
    mpz_set_str(this->EC.b,"1518655069210828534508950034714043154928747527740206436194018823352809982443793732829756914785974674866041605397883677596626326413990136959047435811826396",10);

    mpz_set_str(this->q,"3623986102229003635907788753683874306021320925534678605086546150450856166623969164898305032863068499961404079437936585455865192212970734808812618120619743",10);
    mpz_set_str(this->m,"3623986102229003635907788753683874306021320925534678605086546150450856166623969164898305032863068499961404079437936585455865192212970734808812618120619743",10);
    mpz_set_str(this->P.x, "1928356944067022849399309401243137598997786635459507974357075491307766592685835441065557681003184874819658004903212332884252335830250729527632383493573274",10);
    mpz_set_str(this->P.y, "2288728693371972859970012155529478416353562327329506180314497425931102860301572814141997072271708807066593850650334152381857347798885864807605098724013854",10);

}

void gost3410_12::Point_Doubling(Point &R,const Point &P)
{
    mpz_t slope, temp;
    mpz_init(temp);
    mpz_init(slope);

    if(mpz_cmp_ui(P.y, 0) != 0) {
        mpz_mul_ui(temp, P.y, 2);
        mpz_invert(temp, temp, EC.p);
        mpz_mul(slope, P.x, P.x);
        mpz_mul_ui(slope, slope, 3);
        mpz_add(slope, slope, EC.a);
        mpz_mul(slope, slope, temp);
        mpz_mod(slope, slope, EC.p);
        mpz_mul(R.x, slope, slope);
        mpz_sub(R.x, R.x, P.x);
        mpz_sub(R.x, R.x, P.x);
        mpz_mod(R.x, R.x, EC.p);
        mpz_sub(temp, P.x, R.x);
        mpz_mul(R.y, slope, temp);
        mpz_sub(R.y, R.y, P.y);
        mpz_mod(R.y, R.y, EC.p);
    } else {
        mpz_set_ui(R.x, 0);
        mpz_set_ui(R.y, 0);
    }
    mpz_clear(temp);
    mpz_clear(slope);
}


void gost3410_12::Point_Addition(Point &R, const Point &P,const Point &Q)
{
    /*mpz_mod(P.x, P.x, EC.p);
    mpz_mod(P.y, P.y, EC.p);
    mpz_mod(Q.x, Q.x, EC.p);
    mpz_mod(Q.y, Q.y, EC.p);*/

    if(mpz_cmp_ui(P.x, 0) == 0 && mpz_cmp_ui(P.y, 0) == 0) {
        mpz_set(R.x, Q.x);
        mpz_set(R.y, Q.y);
        return;
    }

    if(mpz_cmp_ui(Q.x, 0) == 0 && mpz_cmp_ui(Q.y, 0) == 0) {
        mpz_set(R.x, P.x);
        mpz_set(R.y, P.y);
        return;
    }

    mpz_t temp;
    mpz_init(temp);

    if(mpz_cmp_ui(Q.y, 0) != 0) {
        mpz_sub(temp, EC.p, Q.y);
        mpz_mod(temp, temp, EC.p);
    } else
        mpz_set_ui(temp, 0);

    if(mpz_cmp(P.y, temp) == 0 && mpz_cmp(P.x, Q.x) == 0) {
        mpz_set_ui(R.x, 0);
        mpz_set_ui(R.y, 0);
        mpz_clear(temp);
        return;
    }

    if(mpz_cmp(P.x, Q.x) == 0 && mpz_cmp(P.y, Q.y) == 0)	{
        Point_Doubling(R, P);

        mpz_clear(temp);
        return;
    } else {
        mpz_t slope;
        mpz_init_set_ui(slope, 0);

        mpz_sub(temp, P.x, Q.x);
        mpz_mod(temp, temp, EC.p);
        mpz_invert(temp, temp, EC.p);
        mpz_sub(slope, P.y, Q.y);
        mpz_mul(slope, slope, temp);
        mpz_mod(slope, slope, EC.p);
        mpz_mul(R.x, slope, slope);
        mpz_sub(R.x, R.x, P.x);
        mpz_sub(R.x, R.x, Q.x);
        mpz_mod(R.x, R.x, EC.p);
        mpz_sub(temp, P.x, R.x);
        mpz_mul(R.y, slope, temp);
        mpz_sub(R.y, R.y, P.y);
        mpz_mod(R.y, R.y, EC.p);

        mpz_clear(temp);
        mpz_clear(slope);
        return;
    }
}

void gost3410_12::Scalar_Multiplication(Point &R, const Point &P,const mpz_t m)
{
    Point Q, T;
    mpz_init(Q.x); mpz_init(Q.y);
    mpz_init(T.x); mpz_init(T.y);
    long no_of_bits, loop;

    no_of_bits = mpz_sizeinbase(m, 2);
    mpz_set_ui(R.x, 0);
    mpz_set_ui(R.y, 0);
    if(mpz_cmp_ui(m, 0) == 0)
        return;

    mpz_set(Q.x, P.x);
    mpz_set(Q.y, P.y);
    if(mpz_tstbit(m, 0) == 1){
        mpz_set(R.x, P.x);
        mpz_set(R.y, P.y);
    }

    for(loop = 1; loop < no_of_bits; loop++) {
        mpz_set_ui(T.x, 0);
        mpz_set_ui(T.y, 0);
        Point_Doubling(T, Q);

        mpz_set(Q.x, T.x);
        mpz_set(Q.y, T.y);
        mpz_set(T.x, R.x);
        mpz_set(T.y, R.y);
        if(mpz_tstbit(m, loop))
            Point_Addition(R, T, Q);
    }

    /*mpz_clear(Q.x); mpz_clear(Q.y);
    mpz_clear(T.x); mpz_clear(T.y);*/
}

char* gost3410_12::sign(const char *hash)
{
    mpz_t a;
    mpz_t e;
    mpz_t k;
    mpz_t r;
    mpz_t s;
    mpz_t temp1;
    mpz_t temp2;
    mpz_init(a);
    mpz_init(e);
    mpz_init(k);
    mpz_init(r);
    mpz_init(s);
    mpz_init(temp1);
    mpz_init(temp2);
    mpz_init(a);

    mpz_import(a, 64, 1, sizeof(char), 0, 0, hash);

    mpz_mod(e,a,q);

    if(mpz_cmp_ui(e,0) == 0)
    {
        mpz_set_ui(e,1);
    }

    do
    {
        mpz_urandomm(k, state, q);
        Scalar_Multiplication(R,P,k);
        mpz_mod(r,R.x,q);
        if(mpz_cmp_ui(r,0) == 0)
        {
            continue;
        }
        mpz_mul(temp1,r,d);
        mpz_mul(temp2,k,e);
        mpz_add(s,temp1,temp2);
        mpz_mod(s,s,q);
        if(mpz_cmp_ui(s,0) == 0)
        {
            continue;
        }

        break;
    } while(true);

    char *result = new char[130];
    memset(result,'\0',130);
    size_t data_byte_count = (mpz_sizeinbase(r, 2) + 7) / 8;
    size_t g = 0;
    mpz_export(result + (64 -data_byte_count), &g, 1, sizeof(char), 0, 0, r);
    mpz_export(result +64 + (64 -data_byte_count), &g, 1, sizeof(char), 0, 0, s);


    mpz_clear(a);
    mpz_clear(e);
    mpz_clear(k);
    mpz_clear(r);
    mpz_clear(s);
    mpz_clear(temp1);
    mpz_clear(temp2);

    return result;
}

bool gost3410_12::checkSign(const char *signature, const char *hash)
{
    mpz_t r;
    mpz_t s;
    mpz_t a;
    mpz_t e;
    mpz_t v;
    mpz_t z1;
    mpz_t z2;
    mpz_t Rr;

    mpz_init(a);
    mpz_init(e);
    mpz_init(v);
    mpz_init(z1);
    mpz_init(z2);
    mpz_init(Rr);
    mpz_init(r);
    mpz_init(s);
    mpz_init(a);

    mpz_import(a, 64, 1, sizeof(char), 0, 0, hash);
    mpz_import(r, 64, 1, sizeof(char), 0, 0, signature);
    mpz_import(s, 64, 1, sizeof(char), 0, 0, signature+64);

    if(mpz_cmp(r,q) >= 0)
    {
         mpz_clear(r);
         mpz_clear(s);
         mpz_clear(a);
         mpz_clear(e);
         mpz_clear(v);
         mpz_clear(z1);
         mpz_clear(z2);
         mpz_clear(Rr);
        return false;
    }

    if(mpz_cmp(s,q) >= 0)
    {
        mpz_clear(r);
        mpz_clear(s);
        mpz_clear(a);
        mpz_clear(e);
        mpz_clear(v);
        mpz_clear(z1);
        mpz_clear(z2);
        mpz_clear(Rr);
        return false;
    }



    mpz_mod(e,a,q);

    if(mpz_cmp_ui(e,0) == 0)
    {
        mpz_set_ui(e,1);
    }
    mpz_invert(v,e,q);

    mpz_mul(z1,s,v);
    mpz_mod(z1,z1,q);

    mpz_mul(z2,r,v);
    mpz_neg(z2,z2);
    mpz_mod(z2,z2,q);
    Point temp1;
    Point temp2;
    Scalar_Multiplication(temp1,P,z1);
    Scalar_Multiplication(temp2,Q,z2);
    Point_Addition(R,temp1,temp2);

    mpz_mod(Rr,R.x,q);
    if(mpz_cmp(Rr,r) != 0)
    {
        mpz_clear(r);
        mpz_clear(s);
        mpz_clear(a);
        mpz_clear(e);
        mpz_clear(v);
        mpz_clear(z1);
        mpz_clear(z2);
        mpz_clear(Rr);
        return false;
    }

    mpz_clear(r);
    mpz_clear(s);
    mpz_clear(a);
    mpz_clear(e);
    mpz_clear(v);
    mpz_clear(z1);
    mpz_clear(z2);
    mpz_clear(Rr);
    return true;
}
char* gost3410_12::get_sign_key_d()
{
    size_t byte_count = mpz_sizeinbase(d, 10);
    char *result = new char[byte_count+5];
    result = mpz_get_str(result,10,d);
    return result;

}

char* gost3410_12::get_signcheck_keyQ_x()
{
    size_t byte_count = mpz_sizeinbase(Q.x, 10);
    char *result = new char[byte_count+5];
    result = mpz_get_str(result,10,Q.x);
    return result;
}

char* gost3410_12::get_signcheck_keyQ_y()
{
    size_t byte_count = mpz_sizeinbase(Q.y, 10);
    char *result = new char[byte_count+5];
    result = mpz_get_str(result,10,Q.y);
    return result;
}

/*bool gost3410_12::set_Elliptic_Curve_parametrs(const char *a, const char *b, const char *p)
{
    mpz_t temp_p;
    mpz_t temp_a;
    mpz_t temp_b;
    mpz_init(temp_p);
    mpz_init(temp_a);
    mpz_init(temp_b);

    mpz_set_str(temp_p,p,10);
    if(mpz_probab_prime_p(temp_p,40) == 0)
    {
        mpz_clear(temp_p);
        mpz_clear(temp_a);
        mpz_clear(temp_b);
        return false;
    }
    mpz_set_str(temp_a,a,10);
    mpz_set_str(temp_b,b,10);

    mpz_mod(temp_a, temp_a,temp_p);
    mpz_mod(temp_b, temp_b,temp_p);

    if(mpz_cmp_ui(temp_a,0)== 0 || mpz_cmp_ui(temp_b,0)== 0)
    {
        mpz_clear(temp_p);
        mpz_clear(temp_a);
        mpz_clear(temp_b);
        return false;
    }

    mpz_powm_ui(temp_a,temp_a,3,temp_p);
    mpz_powm_ui(temp_b,temp_b,2,temp_p);
    mpz_mul_ui(temp_a,temp_a,4);
    mpz_mul_ui(temp_b,temp_b,27);
    mpz_add(temp_a, temp_a, temp_b);
    mpz_mod(temp_a,temp_a, temp_p);
    if(mpz_cmp_ui(temp_a,0) == 0 || mpz_cmp_ui(temp_a,1728)== 0)
    {
        mpz_clear(temp_p);
        mpz_clear(temp_a);
        mpz_clear(temp_b);
        return false;
    }
    mpz_set_str(EC.a,a,10);
    mpz_set_str(EC.b,b,10);
    mpz_set_str(EC.p,p,10);

    mpz_clear(temp_p);
    mpz_clear(temp_a);
    mpz_clear(temp_b);
    return true;
}

bool gost3410_12::set_m_parameter(const char *m)
{
    mpz_t temp_m;
    mpz_t temp_min;
    mpz_init( temp_min);
    mpz_init(temp_m);

    mpz_pow_ui(temp_min,temp_min,508);
    mpz_set_str(temp_m,m,10);

    if(mpz_cmp(temp_min,temp_m) >= 0)
    {
        mpz_clear(temp_m);
        mpz_clear( temp_min);
        return false;
    }

    if(mpz_cmp(temp_m,EC.p) == 0)
    {
        mpz_clear(temp_m);
        return false;
    }
    mpz_set(this->m,temp_m);

    mpz_clear(temp_m);

}

bool gost3410_12::set_q_parameter(const char *q)
{
    mpz_t temp_q;
    mpz_t temp_max;
    mpz_t temp_min;
    mpz_t temp;

    mpz_init(temp_q);
    mpz_init( temp_max);
    mpz_init( temp_min);
    mpz_init( temp);

    mpz_pow_ui(temp_min,temp_min,508);
    mpz_pow_ui(temp_max,temp_max,512);
    mpz_set_str(temp_q,q,10);

    if(mpz_cmp(temp_min,temp_q) >= 0 || mpz_cmp(temp_q,temp_max) >= 0)
    {
        mpz_clear(temp_q);
        mpz_clear( temp_min);
        mpz_clear( temp_max);
        mpz_clear(temp);
        return false;
    }
    mpz_mod(temp,this->m, temp_q);

    if(mpz_cmp_ui(temp,0) != 0)
    {
        mpz_clear(temp_q);
        mpz_clear( temp_min);
        mpz_clear( temp_max);
        mpz_clear(temp);
        return false;
    }

    for( unsigned long i = 1; i < 132; i++)
    {
        mpz_powm_ui(temp,EC.p,1,temp_q);
        if(mpz_cmp_ui(temp,1) == 0)
        {
            mpz_clear(temp_q);
            mpz_clear( temp_min);
            mpz_clear( temp_max);
            mpz_clear(temp);
            return false;
        }
    }
    mpz_set(this->q,temp_q);

    mpz_clear(temp_q);
    mpz_clear( temp_min);
    mpz_clear( temp_max);
    mpz_clear(temp);
    return true;

}
bool gost3410_12::set_Elleptic_Curve_P_point(const char *x,const char *y)
{
    mpz_t temp_x;
    mpz_t temp_y;
    mpz_init(temp_x);
    mpz_init(temp_y);

    mpz_set_str(temp_x,x,10);
    mpz_set_str(temp_y,y,10);
    if(mpz_cmp_ui(temp_x, 0 ) == 0 && mpz_cmp_ui(temp_y, 0 ) == 0)
    {
        mpz_clear(temp_x);
        mpz_clear(temp_y);
        return false;
    }
    mpz_t temp;
    mpz_init(temp);
    Point p;
    mpz_set(p.x,temp_x);
    mpz_set(p.y,temp_y);
    Point r;

    Scalar_Multiplication(r,p,this->q);
    if( mpz_cmp_ui(r.x, 0) !=0 || mpz_cmp_ui(r.y, 0) !=0)
    {
        mpz_clear(temp_x);
        mpz_clear(temp_y);
        mpz_clear(temp);
        return false;
    }

    mpz_set(this->P.x, temp_x);
    mpz_set(this->P.y, temp_y);

    mpz_clear(temp_x);
    mpz_clear(temp_y);
    mpz_clear(temp);
    return true;
}*/

bool gost3410_12::set_d_parameter(const char *d)
{
    mpz_t temp_d;
    mpz_init(temp_d);
    mpz_set_str(temp_d,d,10);
    if( mpz_cmp(temp_d, this->q) >= 0)
    {
        mpz_clear(temp_d);
        return false;
    }
    mpz_set(this->d,temp_d);
    mpz_clear(temp_d);
    return true;
}

bool gost3410_12::set_Elleptic_Curve_Q_point(const char *x,const char *y)
{
    mpz_t temp_x;
    mpz_t temp_y;
    mpz_init(temp_x);
    mpz_init(temp_y);

    mpz_set_str(temp_x,x,10);
    mpz_set_str(temp_y,y,10);
    if(mpz_cmp_ui(temp_x, 0 ) == 0 && mpz_cmp_ui(temp_y, 0 ) == 0)
    {
        mpz_clear(temp_x);
        mpz_clear(temp_y);
        return false;
    }

    mpz_set(this->Q.x, temp_x);
    mpz_set(this->Q.y, temp_y);

    mpz_clear(temp_x);
    mpz_clear(temp_y);
    return true;
}
