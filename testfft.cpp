#include "testfft.h"

testFFT::testFFT(/*QWidget *parent*/) :
    /*QWidget(parent),*/ mglDraw()
{

}

int testFFT::Draw(mglGraph *gr)
{
    long i;

    if(n > 0)
    {
        mglExpr vre(FRE.data());
        mglExpr vim(FIM.data());
        //    mglExpr vim("0");
        //    n=2048;

//        double dv = 1/tau; //GHz

        tauF = n/(AverNu*4); //ns
        //    qDebug() << tau << tauF;

        std::complex<double> V[n];
        std::complex<double> VinitialT[n];
        //    qDebug() << n*(tauF-tau)/(tauF)/2;
        int nF = (int)( ( n * (tauF-tau)/(tauF) ) / 2 );
        //    qDebug() << nF;
        for(i=0; i<nF; ++i)
        {
            V[i] = std::complex<double>(0., 0.);
            VinitialT[i] = std::complex<double>(0., 0.);
        }
        for(i=nF; i<n-nF; ++i)
        {
            V[i] = std::complex<double>(vre.Eval(0,0, (tauF)*i/n ), vim.Eval(0,0, (tauF)*i/n ));
            VinitialT[i] = std::complex<double>(V[i].real(), V[i].imag());
        }
        for(i=n-1-nF+1; i<n; ++i)
        {
            V[i] = std::complex<double>(0., 0.);
            VinitialT[i] = std::complex<double>(0., 0.);
        }

        fft(V, n, true);

        mglData testAbs2(n), testRe(n),testIm(n),testX(n),testInRe(n),testInIm(n),testInX(n);

        for(i = 0; i < n; ++i)
        {
            testInRe.a[i] = VinitialT[i].real();
            testInIm.a[i] = VinitialT[i].imag();
            testInX.a[i] = i*tauF/n;
        }

        double testNorm = 0.0, testNormRe = 0.0, testNormIm = 0.0;
        for(i=0; i <= n/2; ++i)
        {
            if(testNorm < std::pow(V[i].real(),2) + std::pow(V[i].imag(),2))
                testNorm = std::pow(V[i].real(),2) + std::pow(V[i].imag(),2);
            if(testNormRe < std::fabs(V[i].real()))
                testNormRe = std::fabs(V[i].real());
            if(testNormIm < std::fabs(V[i].imag()))
                testNormIm = std::fabs(V[i].imag());
        }
        if(testNorm == 0)
            testNorm = 1;
        if(testNormRe == 0)
            testNormRe = 1;
        if(testNormIm == 0)
            testNormIm = 1;
        //    double testNormIm = testIm.a[n/4];
        //    qDebug() << "testNorm" << testNorm;
        for(i = 0; i <= n/2; ++i)
        {
            //        qDebug() << std::pow(V[i].real(),2) + std::pow(V[i].imag(),2) / testNorm;
            testAbs2.a[i] = (std::pow(V[i].real(),2) + std::pow(V[i].imag(),2))/testNorm;
            testRe.a[i] = V[i].real()/testNormRe;
            //        testRe.a[i] = V[i].real()/testNorm;
            testIm.a[i] = V[i].imag()/testNormIm;
            testX.a[i] = (i)/tauF;
            //        if(i/tauF > AverNu-2*(1/tau))
            //        {
            //            static int one = i;
            //            qDebug() << one;
            //        }
            //        if(i/tauF > AverNu+2*(1/tau))
            //        {
            //            static int two = i;
            //            qDebug() << two;
            //        }
        }
        gr->SubPlot(1,2,0);
//        gr->SetRanges(tauF/2.-tauF/10.,tauF/2.+tauF/10.,-1.5,1.5);
        gr->SetRanges(0,tauF,-1.5,1.5);
        gr->Plot(testInX,testInRe,"n");
        gr->Plot(testInX,testInIm,"r");
        gr->Axis(); gr->Grid("xy","g");
        gr->Label('x',"t, ns",0);
        gr->Label('y',"",0);

        gr->SubPlot(1,2,1);
        gr->SetRanges(AverNu-2*(1/tau),AverNu+2*(1/tau),-1.5,1.5);
        gr->Plot(testX,testAbs2,"2ko");
        gr->Plot(testX,testRe,"2n .");
        gr->Plot(testX,testIm,"r .");
        gr->Axis(); gr->Grid("xy","g");
        gr->Label('x',"\\nu, GHz",0);
        gr->Label('y',"",0);
    }
    else
    {
        mglData coscos(10000);
//        mglData cocos(1000);
//        mglData testAbs2(1), testRe(1),testIm(1),testX(1);
//        testAbs2.a[0] = 1.0;
//        testRe.a[0] = 1.0;
//        testIm.a[0] = 1.0;
//        testX.a[0] = AverNu;

        gr->SubPlot(1,2,1);
        gr->SetRanges(AverNu-(AverNu/1000.),AverNu+(AverNu/1000.));
//        gr->Fill(cocos,QByteArray::number(AverNu),"");
//        gr->Plot(cocos,"b","");
        //        gr->Title("|V(\\nu )|^2");
        gr->Line(mglPoint(AverNu),mglPoint(AverNu,1),"k",1);
//        gr->Plot(testX,testRe,"2n .");
//        gr->Plot(testX,testIm,"r .");
        gr->Axis(); gr->Grid("xy","g");
        gr->Label('x',"\\nu, GHz",0);
        gr->Label('y',"",0);

        gr->SubPlot(1,2,0);
        gr->Alpha(true);
        gr->SetRanges(AverNu-(AverNu/1000.),AverNu+(AverNu/1000.),-1.5,1.5);
        gr->Fill(coscos,"cos(2*pi*"+QByteArray::number(AverNu)+"*x)","");
//        gr->FPlot("cos(2*pi*"+QByteArray::number(AverNu)+"*x)","b");
        gr->Plot(coscos,"b","");
        gr->Axis(); gr->Grid("xy","g");
        gr->Label('x',"\\nu, GHz",0);
        gr->Label('y',"",0);
    }

//    for(int i=0; i<n/2; ++i)
//    {
//        if( (i/tauF > AverNu-2*(1/tau)) || (i/tauF < AverNu+2*(1/tau)) )
//            qDebug() << V[i].real()/testNorm;
//    }
//    return gr.Run();
    return 0;
}
