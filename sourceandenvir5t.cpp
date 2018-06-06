#include "sourceandenvir5t.h"

SourceAndEnvir5t::SourceAndEnvir5t(QObject *parent) :
    QObject(parent)
{
    qDebug() << "---> The source are being created...";

    QSettings settings;
    mathforcap = new MathForCap(this);

    lengthUnitMultPower = settings.value("CurrentSettings/lengthUnitMult",404).toInt();
    lengthUnitMult = QString("1e"+settings.value("CurrentSettings/lengthUnitMult",0).toString()).toDouble();
    speedOfLightForCap = 299792458.0 * (1e-9) / lengthUnitMult; // 10^8*10^-9=10^-1 m = 10^5 mkm

    x_0_ = settings.value("CurrentSettings/x_0_",404.404).toDouble();
    y_0_ = settings.value("CurrentSettings/y_0_",404.404).toDouble();
    z_0_ = settings.value("CurrentSettings/z_0_",404.404).toDouble();
    x_0__ = settings.value("CurrentSettings/x_0__",404.404).toDouble();
    y_0__ = settings.value("CurrentSettings/y_0__",404.404).toDouble();
    z_0__ = settings.value("CurrentSettings/z_0__",404.404).toDouble();
    fSource = settings.value("CurrentSettings/fSource","404").toByteArray();
    phi_0_ = settings.value("CurrentSettings/phi_0_",404.404).toDouble();
    theta_0_ = settings.value("CurrentSettings/theta_0_",404.404).toDouble();
    phi_0__ = settings.value("CurrentSettings/phi_0__",404.404).toDouble();
    theta_0__ = settings.value("CurrentSettings/theta_0__",404.404).toDouble();
    tau = settings.value("CurrentSettings/tau",404.404).toDouble();
    FRe = settings.value("CurrentSettings/FRe","404").toByteArray();
    FIm = settings.value("CurrentSettings/FIm","404").toByteArray();
    averageFrequency = settings.value("CurrentSettings/averageFrequancy",404.404).toDouble();

    wavelength = speedOfLightForCap/averageFrequency;
    wavelength2 = std::pow(speedOfLightForCap/averageFrequency,2);

    fAmpl = settings.value("CurrentSettings/fAmpl","404").toByteArray();
    fPhase = settings.value("CurrentSettings/fPhase","404").toByteArray();
    fScopePhase = settings.value("CurrentSettings/fScopePhase","404").toByteArray();

//    nonmonoch7yPercent = settings.value("CurrentSettings/nonmonoch7yPercent",404.404).toDouble();
//    initialAmplitude = settings.value("CurrentSettings/initialAmplitude",404.404).toDouble();
    mglFSource = new mglExpr(fSource.data());
    mglFRe = new mglExpr(FRe.data());
    mglFIm = new mglExpr(FIm.data());
    mglFAmpl = new mglExpr(fAmpl.data());
    mglFPhase = new mglExpr(fPhase.data());
    mglFScopePhase = new mglExpr(fScopePhase.data());

    epsilonEnvir = std::complex<double>(settings.value("CurrentSettings/epsilonEnvirRe",404.404).toDouble()
                                        ,settings.value("CurrentSettings/epsilonEnvirIm",404.404).toDouble());
    this->COUTDEBUG();
    qDebug() << "<--- The source were created!";
    qDebug();
}

void SourceAndEnvir5t::COUTDEBUG()
{
    qDebug() << "lengthUnitMultPower" << lengthUnitMultPower;
    qDebug() << "lengthUnitMult" << lengthUnitMult;
    qDebug() << "speedOfLightForCap" << speedOfLightForCap;

    qDebug() << "x_0_, y_0_, z_0_" << x_0_ << y_0_ << z_0_;
    qDebug() << "x_0__, y_0__, z_0__" << x_0__ << y_0__ << z_0__;
    qDebug() << "fSource" << fSource;
    qDebug() << "angles_:" << phi_0_ << theta_0_;
    qDebug() << "angles__:" << phi_0__ << theta_0__;
    qDebug() << "TAU" << tau;
    qDebug() << "FRE, FIM:" << FRe << FIm;
    qDebug() << "averageFrequency" << averageFrequency;

    qDebug() << "fAmpl, fPhase, fScopePhase:" << fAmpl << fPhase << fScopePhase;
//    nonmonoch7yPercent = settings.value("CurrentSettings/nonmonoch7yPercent",404.404).toDouble();
    qDebug() << "epsilonEnvir" << epsilonEnvir.real() << epsilonEnvir.imag();

}

SourceAndEnvir5t::~SourceAndEnvir5t()
{
    delete mglFSource;
    delete mglFRe;
    delete mglFIm;
    delete mglFAmpl;
    delete mglFPhase;
    delete mglFScopePhase;
}

void SourceAndEnvir5t::getRandomX_0Y_0Z_0(double &randX_0, double &randY_0, double &randZ_0)
{
    do
    {
        randX_0 = RandomForCap::GetRandomIncInc(x_0_,x_0__);
        randY_0 = RandomForCap::GetRandomIncInc(y_0_,y_0__);
        randZ_0 = RandomForCap::GetRandomIncInc(z_0_,z_0__);
        if (randX_0 == 0.0)
            randX_0 = 1e-15;
        if (randY_0 == 0.0)
            randY_0 = 1e-15;
    }
    while (mglFSource->Eval(randX_0,randY_0,randZ_0) > 0);
}

void SourceAndEnvir5t::getRandomX_0Y_0Z_0(double *randR_0)
{
    do
    {
        *randR_0 = RandomForCap::GetRandomIncInc(x_0_,x_0__);
        *(randR_0+1) = RandomForCap::GetRandomIncInc(y_0_,y_0__);
        *(randR_0+2) = RandomForCap::GetRandomIncInc(z_0_,z_0__);
        if (*randR_0 == 0.0)
        {
            *randR_0 = 1e-15;
        }
        if (*(randR_0+1) == 0.0)
        {
            *(randR_0+1) = 1e-15;
        }
    }
    while (mglFSource->Eval(randR_0[0],randR_0[1],randR_0[2]) > 0);
}

void SourceAndEnvir5t::getRandomPhi_0Theta_0(double &randPhi_0, double &randTheta_0)
{
    randPhi_0 = RandomForCap::GetRandomIncInc(phi_0_,phi_0__);
    randTheta_0 = RandomForCap::GetRandomIncInc(theta_0_,theta_0__);
//    randPhi_0 = 1e-13;
//    randTheta_0 = 1e-13;
//    qDebug() << "  tempRandPhi_0 tempRandTheta_0:" << QString::number(randPhi_0,'f',16) << QString::number(randTheta_0,'f',16);
}

void SourceAndEnvir5t::getRandomPhi_0Theta_0(double *directVec)
{
    double randPhi_0 = RandomForCap::GetRandomIncInc(phi_0_,phi_0__);
    double randTheta_0 = RandomForCap::GetRandomIncInc(theta_0_,theta_0__);
    *directVec = std::cos(randPhi_0)*std::sin(randTheta_0);
    *(directVec + 1) = std::sin(randPhi_0)*std::sin(randTheta_0);
    *(directVec + 2) = std::cos(randTheta_0);
}

int SourceAndEnvir5t::CalculationOfPassageOfTheBeam(double *directingVectorLine, double * prevPoint,
                                          double &pointXsou, double &pointYsou, double &pointZsou)//луч вылетел за переделы капилляра в сторону экрана ->
{
    double rPoint[numOfVar];
    rPoint[0] = 0.01; rPoint[1] = -0.01; rPoint[2] = z_0__+1.0;
    if( mathforcap->NewtonMetodForEquationAndLine(fSource,fSource,prevPoint,rPoint,directingVectorLine) < 0 )
    {
        qDebug() << "### ERROR. There is no point on the source!";
        return -1;
    }
//    mglFormula tempF(fSourceXYZ_0);
//    if ( tempF.Calc(rPoint[0],rPoint[1],rPoint[2]) > )
//    {
//        qDebug() << "@@@ The ray flew outsiden of the source!";
//        return 1;
//    }
    pointXsou = rPoint[0];
    pointYsou = rPoint[1];
    pointZsou = rPoint[2];
//    double tempNorm = std::sqrt(directingVectorLine[0]*directingVectorLine[0] + directingVectorLine[1]*directingVectorLine[1] + directingVectorLine[2]*directingVectorLine[2]);
//    for (int i=0; i<3 ; ++i)
//    {
//        directingVectorLine[i] = directingVectorLine[i]/tempNorm;
//    }
//    double temp1 = std::sqrt(directingVectorLine[0]*directingVectorLine[0] + directingVectorLine[1]*directingVectorLine[1]);
//    angleScrTheta = std::asin(temp1);
//    angleScrPhi = std::asin(directingVectorLine[2]/temp1);

    return 0;
}

double SourceAndEnvir5t::integrationAmpl(int & n, double & delay)
{
    std::complex<double> result = std::complex<double> (0.0,0.0);
//    double h = (border__ - border_)/n; //Шаг сетки
    double h = (tau)/n; //Шаг сетки
    double border_ = -tau/2.0;
    double argument = 0.0;

    for(int i = 0; i < n; ++i)
    {
        argument = border_ + h*(i + 0.5);
        if (std::fabs(argument+delay) <= tau/2.0)
        {
            result += std::complex<double> (mglFRe->Eval(argument+delay/speedOfLightForCap),mglFIm->Eval(argument+(delay/speedOfLightForCap)))
                    * std::complex<double> (mglFRe->Eval(argument),(-1)*mglFIm->Eval(argument)); //Вычисляем в средней точке и добавляем в сумму
        }
        //       qDebug() << "!" << result.real() << result.imag();
    }
//    result *= h;//мы же усредняем, нам не обязательно на это домножать...

    return result.real();
}

int SourceAndEnvir5t::prepareConvolution(int &n, QVector<double> &absV, QVector<double> &nu_i)
{
    long i;

    if(n > 0)//количество точек дискретизации - основной параметр. По сути количество точек БПФ...
        //что интересно, так это то, что зададим 0 - монохроматический свет
        //и зададим ещё -1 - это собественный спектр или его значения из файла...
    {
//        COUTDEBUG();

        //    double dv = 1/tau; //nHz
        double tauF = n/(averageFrequency*4); //Gs

        //    qDebug() << "sddddddddddddddddddd" << mglFRe->Eval(0.0);
        std::complex<double> V[n];
        std::complex<double> VinitialT[n];
        //    qDebug() << n*(tauF-tau)/(tauF)/2;
        //    qDebug() << "asdasdadsasadasdadasdasdasdasdasdads" << n << tauF << tau << averageFrequency;
        long nF = (long)( ( n * (tauF-tau)/(tauF) ) / 2 );
//        qDebug() << "nF" << nF;

        //AverNu-2*(1/tau),AverNu+2*(1/tau)

        for(i = 0; i < nF; ++i)
        {
            V[i] = std::complex<double>(0., 0.);
            VinitialT[i] = std::complex<double>(0., 0.);
        }

        for(i = nF; i < n-nF; ++i)
        {
            V[i] = std::complex<double>(
                        mglFRe->Eval(0.,0.,(tauF)*i/n),
                        mglFIm->Eval(0.,0.,(tauF)*i/n)
                        );
            //        qDebug() << V[i].real() << mglFIm->Eval(0.0);
            VinitialT[i] = std::complex<double>(
                        V[i].real(),
                        V[i].imag()
                        );
        }
        for(i = n-1-nF+1; i < n; ++i)
        {
            V[i] = std::complex<double>(0., 0.);
            VinitialT[i] = std::complex<double>(0., 0.);
        }

        fft(V, n, true);

        //    long double testNorm = -1.0;
        //    for(int i = 0; i <= n/2; ++i)
        //    {
        //        if(testNorm<std::pow(V[i].real(),2) + std::pow(V[i].imag(),2))
        //            testNorm = std::pow(V[i].real(),2) + std::pow(V[i].imag(),2);
        //    }

        long a,b;
        bool flagA = false, flagB = false;
        double testNorm = 0.0;
        for(i = 0; i <= n/2; i++)
        {
            //        qDebug() << V[i].real();
            if ( testNorm < std::pow(V[i].real(),2) + std::pow(V[i].imag(),2) )
            {
                testNorm = std::pow(V[i].real(),2) + std::pow(V[i].imag(),2);
            }
            if( (i/tauF) >= (averageFrequency - 2*(1/tau)))
            {
                if(!flagA)
                {
                    a = i;
                    flagA = true;
                }
            }
            if( (i/tauF) >= (averageFrequency + 2*(1/tau)) )
            {
                if(!flagB)
                {
                    b = i;
                    flagB = true;
                }
            }
        }
//            qDebug() << "V" << V;
//        qDebug() << "n" << n << "tau" << tau << "tauF= n/(averageFrequency*4):" << tauF;
//        qDebug() << "testNorm" << testNorm;
//        for(i = 0; i < n/2; i++)
//        {
//            if(i == a)
//                qDebug() << "begin";
//            qDebug() << "   " << (std::pow(V[i].real(),2) + std::pow(V[i].imag(),2))/testNorm;
//            if(i == b)
//                qDebug() << "end";
//        }
//        qDebug() << "#########" << "clear:" << absV;
        for(int i=a; i<=b; i++)
        {
            absV.push_back( (std::pow(V[i].real(),2) + std::pow(V[i].imag(),2)) / testNorm );
            nu_i.push_back( i/tauF );
        }
//        qDebug() << absV;
        //    qDebug() << "absV" << absV;
        //    qDebug() << "nu_i" << nu_i;
        //    qDebug() << "+++++KOL_VO TOCHEK:::" << nu_i.size();
        //    qDebug() << "FREFIM" << FRe << FIm;
    }
    else if ( n = 0 )
    {
        absV.push_back(1);
        nu_i.push_back(averageFrequency);
    }
    return 0;
}

double SourceAndEnvir5t::getPhase(QVector<double> &rSou) const
{
    if( mglFScopePhase->Eval(rSou.at(0),rSou.at(1),rSou.at(2)) < 0 )
        return ( mglFPhase->Eval(rSou.at(0),rSou.at(1),rSou.at(2)) );
    else
        return (0.);
}

double SourceAndEnvir5t::getAmpl(QVector<double> &rSou) const
{
    return ( mglFAmpl->Eval(rSou.at(0),rSou.at(1),rSou.at(2)) );
}
