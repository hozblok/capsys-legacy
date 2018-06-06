#include "randomforcap.h"

RandomForCap::RandomForCap(QObject *parent) :
    QObject(parent)
  //,mti(625)
{
//    gsl_rng_env_setup();
//    T_gslrand = gsl_rng_mt19937; r_gslrand = gsl_rng_alloc(T_gslrand);
//    gsl_rng_set(r_gslrand, time(0));
//    gslRandomMin = gsl_rng_min(r_gslrand); // = 0 в целом это значение нам не нужно
//    gslRandomMax = gsl_rng_max(r_gslrand); //максимальное число, которое может выдать ГСЧ
}

RandomForCap::~RandomForCap()
{
//    gsl_rng_free(r_gslrand);
}

void RandomForCap::init_genrand(unsigned long s)
{
    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<NNNN; mti++) {
        mt[mti] =
        (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
void RandomForCap::init_by_array(unsigned long init_key[], int key_length)
{
    int i, j, k;
    init_genrand(19650218UL);
    i=1; j=0;
    k = (NNNN>key_length ? NNNN : key_length);
    for (; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL))
          + init_key[j] + j; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=NNNN) { mt[0] = mt[NNNN-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=NNNN-1; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL))
          - i; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=NNNN) { mt[0] = mt[NNNN-1]; i=1; }
    }

    mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long RandomForCap::genrand_int32(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= NNNN) { /* generate N words at one time */
        int kk;

        if (mti == NNNN+1)   /* if init_genrand() has not been called, */
            init_genrand(5489UL); /* a default initial seed is used */

        for (kk=0;kk<NNNN-MMMM;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+MMMM] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<NNNN-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(MMMM-NNNN)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[NNNN-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[NNNN-1] = mt[MMMM-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }

    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}




double RandomForCap::GetRandomIncInc(const double & border_,const double & border__)
{
//    qDebug() << "GetRandOutput" << doubleNumber;
//    return (border_ + (border__ - border_)*(gsl_rng_get(r_gslrand) + 0.0)/gslRandomMax);
    return (border_ + ((double)genrand_int32())*((border__ - border_)/4294967295.0));
}

double RandomForCap::GetRandomIncExc(const double & border_,const double & border__)
{
//    qDebug() << "GetRandOutput" << doubleNumber;
//    return (border_ + (border__ - border_)*(gsl_rng_get(r_gslrand) + 0.0)/(gslRandomMax + 1.0));
    return (border_ + ((double)genrand_int32())*((border__ - border_)/4294967296.0));
}

double RandomForCap::GetRandomExcInc(const double & border_, const double & border__)
{
//    qDebug() << "GetRandOutput" << doubleNumber;
//    return (border_ + (border__ - border_)*(gsl_rng_get(r_gslrand) + 1.0)/(gslRandomMax + 1.0));
    return (border_ + (((double)genrand_int32()) + 0.5)*((border__ - border_)/4294967295.5));
}

double RandomForCap::GetRandomExcExc(const double & border_, const double & border__)
{
//    qDebug() << "GetRandOutput" << doubleNumber;
//    return (border_ + (border__ - border_)*(gsl_rng_get(r_gslrand) + 1.0)/(gslRandomMax + 2.0));
    return (border_ + (((double)genrand_int32()) + 0.5)*((border__ - border_)/4294967296.0));
}


