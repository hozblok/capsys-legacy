#include "fft.h"

inline void complex_mul(std::complex<double> *z, const std::complex<double> *z1, const std::complex<long double> *z2)
{
    *z = std::complex<double>(
                (double)(z1->real() * z2->real() - z1->imag() * z2->imag()),
                (double)(z1->real() * z2->imag() + z1->imag() * z2->real())
            );
}

std::complex<double> *createAlpha(unsigned int Nmax)
{
    unsigned int N, step, step2;
    std::complex<double> *alpha, *alphaTemp, *alphaEnd;
    std::complex<long double> aN, *paN;

    step2 = Nmax >> 1;
    alpha = new std::complex<double>[step2];
    alphaEnd = alpha + step2;
    alpha[0] = std::complex<double>(
                1.0,
                0.0
                );

    for(N = 4, paN = tableAlpha1_N + 1, step = step2 >> 1; N <= Nmax; N += N, paN++, step2 = step, step >>= 1)
    {
        //aN = a1_N = exp(-2*pi*j/N)
        aN= *paN;
        for(alphaTemp = alpha; alphaTemp < alphaEnd; alphaTemp += step2)
            complex_mul(alphaTemp + step, alphaTemp, &aN);
    }
    return alpha;
}

void fft_step(std::complex<double> *x, unsigned int T, bool complement, const std::complex<double> *Wstore)
{
    unsigned int Nmax, I, J, N, Nd2, N2, k, Skew, Step;
    unsigned char *Ic= (unsigned char*) &I;
    unsigned char *Jc= (unsigned char*) &J;
    std::complex<double> S;

    Nmax= 1 << T;

    double cmul= complement ? -1.0 : +1.0;

    for(I = 1; I < Nmax - 1; I++)
    {
        Jc[0]= tableReverseByte[Ic[3]];
        Jc[1]= tableReverseByte[Ic[2]];
        Jc[2]= tableReverseByte[Ic[1]];
        Jc[3]= tableReverseByte[Ic[0]];
        J >>= (32 - T);
        if (I < J)
        {
            S= x[I];
            x[I]= x[J];
            x[J]= S;
        }
    }

    double *Warray;
    double Wre, Wim;
    double Tre, Tim;
    double *arr= (double*)x;
    double *arrEnd= arr + (Nmax + Nmax);

    for(N= 2, Skew= Nmax, Step= 1; N <= Nmax; N += N, Skew >>= 1, Step++)
    {
        N2= N + N;
        Nd2= (N >> 1);
        for(Warray= (double*)Wstore, k= 0; k < Nd2; k++, Warray += Skew)
        {
            Wre= *Warray;
            Wim= cmul*Warray[1];
            for(double *x1re= arr + (k + k); x1re < arrEnd; x1re+= N2)
            {
                double *x1im= x1re + 1;
                double *x2re= x1re + N;
                double *x2im= x2re + 1;
                Tre = Wre * *x2re - Wim * *x2im;
                Tim = Wre * *x2im + Wim * *x2re;
                *x2re= *x1re - Tre;
                *x2im= *x1im - Tim;
                *x1re+= Tre;
                *x1im+= Tim;
            }
        }
    }
}


/*
  x: x - array of items
  N: N - number of items in array
  complement: false - normal (direct) transformation, true - reverse transformation
*/
void fft(std::complex<double> *x, int N, bool complement)
{
    std::complex<double> *x_;
    std::complex<double> *w;
    std::complex<double> *Wstore;

    int T;
    T= (int)floor(log((double)N) / log(2.0) + 0.5);
    if (1 << T == N)
    {
        Wstore= createAlpha(N);
        fft_step(x, T, complement, Wstore);
        if (complement)
        {
            for(int i= 0; i < N; i++)
                x[i]/= N;
        }
        delete [] Wstore;
        return;
    }
    //find N', T
    int N2= N+N;
    int N_;
    long double arg;
    for(N_= 1, T= 0; N_ < N2; N_+= N_, T++)
    {
    }
    //find --2pi/N/2 = pi/N
    long double piN= M_PI_BIG / N;
    if (complement)
        piN= -piN;
    //find x_[n] = x[n]*e^--2*j*pi*n*n/N/2 = x[n]*e^j*piN*n*n
    x_= new std::complex<double>[N_];
    std::complex<long double> v;
    int n;
    for(n= 0; n < N; ++n)
    {
        arg= piN*n*n;
        v = std::complex<double>(
                    cosl(arg),
                    sinl(arg)
                    );
        complex_mul(x_ + n, x + n, &v);
    }
    for(; n < N_; ++n)
        x_[n] = std::complex<double>(0., 0.);

    //find w[n] = e^-j*2*pi*(2*N-2-n)^2/N/2= e^-j*piN*(2*N-2-n)^2
    w= new std::complex<double>[N_];
    int N22= 2*N - 2;
    for(n= 0; n < N_; ++n, --N22)
    {
        arg= -piN*N22*N22;
        w[n]= std::complex<double>(
                (double)cos(arg),
                (double)sin(arg)
                    );
    }
    //FFT1
    Wstore= createAlpha(N_);
    fft_step(x_, T, false, Wstore);
    //FFT2
    fft_step(w, T, false, Wstore);
    //svertka
    for(n= 0; n < N_; ++n)
        x_[n]*= w[n];
    //FFT3 (complement)
    fft_step(x_, T, true, Wstore);
    //find X[n] = X_[n]*e^--j*2*pi*n*n/N/2 = X_[n]*e^j*piN*n*n
    for(n= 0, N22= 2*N - 2; n < N; ++n, --N22)
    {
        arg= piN*n*n;
        v= std::complex<double>(
                    cosl(arg),
                    sinl(arg)
                    );
        v/= N_;
        if (complement)
            v/= N;
        complex_mul(x + n, x_ + N22, &v);
    }
    delete [] x_;
    delete [] w;
    delete [] Wstore;
}
