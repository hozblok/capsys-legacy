#include "capillary.h"

Capillary::Capillary(int capNumber, QObject *parent) :
    QObject(parent)
  //ОСТОРОЖНО!!!

{
    capNum = capNumber;
    mathforcap = new MathForCap(this);
    qDebug();
    qDebug() << QObject::trUtf8("---> Capillary:");
    QSettings settings;
    f0 = settings.value("CurrentSettings/f0_"+QString::number(capNum-1),"404").toByteArray();
    mglf0 = new mglExpr(f0.data());

    fBottomOfCap = settings.value("CurrentSettings/fBottomOfCap_"+QString::number(capNum-1),"404").toByteArray();
    mglFBottomOfCap = new mglExpr(fBottomOfCap.data());
//    zBottomOfCap = std::fabs(mglFBottomOfCap->Eval(0,0,0));

    fEndOfCap = settings.value("CurrentSettings/fEndOfCap_"+QString::number(capNum-1),"404").toByteArray();
    mglFEndOfCap = new mglExpr(fEndOfCap.data());
//    zEndOfCap = std::fabs(mglFEndOfCap->Eval(0,0,0));

    plasmaFrequency = settings.value("CurrentSettings/plasmaFrequency_"+QString::number(capNum-1),404.404).toDouble();
    epsilonCap = std::complex<double>(
                0.,
                settings.value("CurrentSettings/epsilonCapIm_"+QString::number(capNum-1),404.404).toDouble()
                );
    qDebug() << "f0 << fBottomOfCap << fEndOfCap << plasmaFrequency << epsilonCap:"
             <<  f0 << fBottomOfCap << fEndOfCap << plasmaFrequency << epsilonCap.imag();
    qDebug() << QObject::trUtf8("<--- Capillary N." +QByteArray::number(capNum)+ " was created!");
    qDebug();
}

//Capillary::~Capillary()
//{
//    delete mglf0;
//    mglf0 = 0;
//    delete mglFBottomOfCap;
//    mglFBottomOfCap = 0;
//    delete mglFEndOfCap;
//    mglFEndOfCap = 0;
//}

double Capillary::getCoefOfRefl(double theta, double nu)
{
    std::complex<double> tempComplSqrt = std::sqrt(
                std::complex<double>((1.0 - std::pow(plasmaFrequency/nu,2)) , epsilonCap.imag()) - std::pow(std::cos(theta) ,2)
                );

  //  qDebug() << tempComplSqrt.real() << tempComplSqrt.imag();
    double tempsin = std::sin(theta);
//    qDebug() << "nu:" << nu << "plasmaFrequency/nu:" << plasmaFrequency/nu << "angle theta:" << theta;
//    qDebug() << "nu*4.135667517e-6:" << (nu*4.135667517e-6) << "sqrt(coef):" << std::abs((tempsin - tempComplSqrt)/(tempsin + tempComplSqrt));
    return (std::pow(std::abs( (tempsin - tempComplSqrt)/(tempsin + tempComplSqrt) ),2));
}

void Capillary::getFirstApproximation(double *r)
{
    if (mathforcap->rayGoesUp == 1)
    {
        r[0] = 0.1;
        r[1] = 0.1;
        r[2] = mglFBottomOfCap->Eval(1e-2,1e-5,0.);
//        r[2] = 1e5;
    }
    else
    {
        r[0] = 0.01;
        r[1] = 0.01;
        r[2] = mglFEndOfCap->Eval(0.1,0.1,1.0);
    }
//   начальное приближение руками!!?????????Устанавливаем начальное приближение 0,0,0 выбирать нельзя!!!!!!!!!!!!!!
}

// подумать и вытащить нахождение первой точки за пределы общего цикла????????????????????????????
//int Capillary::CalculationFirstPoint()
//{

//}

bool Capillary::tryCaptureSou(double * rSou, double * directVec)
{
    double rPoint[numOfVar];

    *rPoint = (*directVec)/8;
    *(rPoint+1) = (*(directVec+1))/8;
    *(rPoint+2) = (*(directVec+2))/8;
//    qDebug() << "13";
    if( mathforcap->NewtonMetodForEquationAndLine(fBottomOfCap,fBottomOfCap,rSou,rPoint,directVec) != 0 )
    {
        qDebug() << "### ERROR. There is no point on the fBottomOfCap!";
        return false;
    }

    if( mglf0->Eval(rPoint[0],rPoint[1],rPoint[2]) < 0)
    {
        *rSou = *rPoint;
        *(rSou+1) = *(rPoint+1);
        *(rSou+2) = *(rPoint+2);
        return true;
    }
    else
        return false;
}

bool Capillary::tryCaptureScr(double * rScr, double & phi, double & theta)
{
    double rPoint[numOfVar], directingVectorLine[numOfVar];
    rPoint[0] = 0.01; rPoint[1] = -0.01; rPoint[2] = 0.1;
    directingVectorLine[0] = std::cos(phi)*std::sin(theta);
    directingVectorLine[1] = std::sin(phi)*std::sin(theta);
    directingVectorLine[2] = std::cos(theta);
    if( mathforcap->NewtonMetodForEquationAndLine(fEndOfCap,fEndOfCap,rScr,rPoint,directingVectorLine) != 0 )
    {
        qDebug() << "### ERROR. There is no point on the fEndOfCap!";
        return false;
    }
    if( mglf0->Eval(rPoint[0],rPoint[1],rPoint[2]) < 0)
        return true;
    else
        return false;
}

int Capillary::CalculationOfPassageOfTheBeam(
        double *directingVectorLine,
        QVector<double> &pointsX, QVector<double> &pointsY, QVector<double> &pointsZ,
        QVector<double> &anglesGlacing, bool useOnlyFirstPointRef)
{


    long int i = 0;
    long int ii = 0; //counter
    QByteArray f0Modified = ""; //уравнение функции без лишнего члена

    double normalVectorOfF0[numOfVar]; //нормальный вектор поверхности
    if(!flagDEBUGf0ONNSPECTRUM)
    {
        for (i = 0; i < numOfVar; ++i)
            normalVectorOfF0[i] = mglf0->Diff(diffCharXi[i],pointsX[0],pointsY[0],pointsZ[0]);  //normalVector==={l_f0;m_f0;n_f0}
    }
    else
    {
        normalVectorOfF0[0] = mathforcap->FunTorDX(pointsX[0],pointsY[0],pointsZ[0]);
        normalVectorOfF0[1] = mathforcap->FunTorDY(pointsX[0],pointsY[0],pointsZ[0]);
        normalVectorOfF0[2] = mathforcap->FunTorDZ(pointsX[0],pointsY[0],pointsZ[0]);
    }
    //    double pointPrevRef[numOfVar];    //отражённая предыдущая точка - полезно для понимания, но лишнее для алгоритма
    double prevPoint[numOfVar];             //точка, что принадлежит прямой, которая используется в NM
    double rPoint[numOfVar];                //основная рабочая точка //r[0]===x,r[1]===y,r[2]===z
    prevPoint[0] = pointsX[0]; prevPoint[1] = pointsY[0]; prevPoint[2] = pointsZ[0];
//    qDebug() << "CalculationOfPassageOfTheBeam 0:";
    rPoint[0] = pointsX[0] - directingVectorLine[0];
    rPoint[1] = pointsY[0] - directingVectorLine[1];
    rPoint[2] = pointsZ[0] - directingVectorLine[2];
//    qDebug() << "COP rPoint:" << rPoint[0] << rPoint[1] << rPoint[2];
//    qDebug() << "COP normalvector" << normalVectorOfF0[0] << normalVectorOfF0[1] << normalVectorOfF0[2];
//    rPoint[0] = pointsX[0] + directingVectorLine[0]; rPoint[1] = pointsY[0] + directingVectorLine[1]; rPoint[2] = pointsZ[0] + directingVectorLine[2];
//    getFirstApproximation(rPoint);          //начальное приближение руками!!?????????Устанавливаем начальное приближение 0,0,0 выбирать нельзя!!!!!!!!!!!!!!

    //flagDEBUGtestRandRay !!!!!!!!!!!!!!!!!!!!!!!!!!
//          double tempnorm = 0, templength = 0;
//          QVector<double> p;
//          p.resize(numOfVar);
      //flagDEBUGtestRandRay


/// начало поиска точек отражения
    for(ii = 0; ii < 10000; ++ii)//ДОПИСАТЬ УСЛОВИЕ!!!!????????????????
    {
        if (ii != 0)
            f0Modified = "(" + f0 + ")/(z-(" + QByteArray::number(prevPoint[2],'f',16) + "))"; //модифицируем уравнение, убираем лишний корень
        else
            f0Modified = f0;

//        qDebug() << "ONE ii:" << ii << rPoint[0] << rPoint[1] << rPoint[2] << mglf0->Eval(rPoint[0],rPoint[1],rPoint[2])
//                    <<
//                       std::fabs(directingVectorLine[1]*(rPoint[0]-prevPoint[0]) - directingVectorLine[0]*(rPoint[1]-prevPoint[1]))+
//                       std::fabs(directingVectorLine[2]*(rPoint[1]-prevPoint[1]) - directingVectorLine[1]*(rPoint[2]-prevPoint[2]));

/*        while(true)
          {*/
        if (mathforcap->NewtonMetodForEquationAndLine(f0,f0Modified,prevPoint,rPoint,directingVectorLine) < 0)
        {
                return -2;
        }
//        qDebug() << "TWO ii:" << ii << rPoint[0] << rPoint[1] << QString::number(rPoint[2],'g',15) << mglf0->Eval(rPoint[0],rPoint[1],rPoint[2])
//                    <<
//                       std::fabs(directingVectorLine[1]*(rPoint[0]-prevPoint[0]) - directingVectorLine[0]*(rPoint[1]-prevPoint[1]))+
//                       std::fabs(directingVectorLine[2]*(rPoint[1]-prevPoint[1]) - directingVectorLine[1]*(rPoint[2]-prevPoint[2]));
    /*        if(mglFEndOfCap->Eval(rPoint[0],rPoint[1],rPoint[2])>0)
            {
                qDebug() << "14Apr2: rPoint:" << rPoint[0] << rPoint[1] << rPoint[2]
                         <<"PrevPoint:" << prevPoint[0] << prevPoint[1] << prevPoint[2];
                double norm =0.;
                for(int iii=0;iii<numOfVar;++iii)
                    norm += std::pow(directingVectorLine[iii],2);
                norm = std::sqrt(norm)/1e-6;
                qDebug() << "14Apr2:" << "new point: " <<rPoint[0]-directingVectorLine[0]/norm
                         << rPoint[1]-directingVectorLine[1]/norm
                         << rPoint[2]-directingVectorLine[2]/norm;
                qDebug() << "14Apr2:" << "new prevpoint: " <<prevPoint[0] + directingVectorLine[0]/norm
                         << prevPoint[1] + directingVectorLine[1]/norm
                         << prevPoint[2] + directingVectorLine[2]/norm;
                qDebug() << "14Apr2: Eval"
                            << mglf0->Eval(rPoint[0]-directingVectorLine[0]/norm
                                           , rPoint[1]-directingVectorLine[1]/norm
                                           , rPoint[2]-directingVectorLine[2]/norm)
                << mglf0->Eval(prevPoint[0]+directingVectorLine[0]/norm
                               , prevPoint[1]+directingVectorLine[1]/norm
                               , prevPoint[2]+directingVectorLine[2]/norm);
            }  //проверка на нет ли точек пересечения внутри луча.
*/
         /*   if(flagDEBUGtestRandRay&&(ii!=0))
            {
//                int countRandTestRay = 100;
                for(int kk = 0; kk < numOfVar; ++kk)
                {
                    tempnorm += std::pow(directingVectorLine[kk],2);
                    templength += std::pow((rPoint[kk]-prevPoint[kk]),2);
                }
                for(int i = 0; i < 100; ++i)//количество попыток проверить на нормальность луч
                {
                    double tt = std::sqrt(tempnorm)/RandomForCap::GetRandomExcExc(0.,std::sqrt(templength));
                    for(int kk = 0; kk < numOfVar; ++kk)
                    {
                        p[kk]=directingVectorLine[kk]/tt;
                    }

//                    if(mglf0->Eval(prevPoint[0],prevPoint[1],prevPoint[2]) < 0)
//                    {
//                        double newprevpoint[numOfVar];
//                        newprevpoint[0] = 0.1; newprevpoint[1]=0.1; newprevpoint[2]=0.1;
//                        if(mathforcap->NewtonMetodForEquationAndLine(fBottomOfCap,fBottomOfCap,prevPoint,newprevpoint,directingVectorLine) < 0)
//                        {
//                            qDebug() << "###Error prevPoint: bottom...";
//                            break;
//                        }
//                        for(int kk=0;kk<numOfVar;++kk)
//                            prevPoint[kk]=newprevpoint[kk];
//                    }
                    qDebug() <<"KKJHKJHKJHKJHKHJKH"<<prevPoint[0] << prevPoint[1] << prevPoint[2] << mglf0->Eval(prevPoint[0],prevPoint[1],prevPoint[2]);
                    qDebug() << p[0] << p[1] << p[2] << "!!!" << tt << mglf0->Eval(p[0]+prevPoint[0],p[1]+prevPoint[1],p[2]+prevPoint[2]);
                    if( mglf0->Eval(p[0]+prevPoint[0],p[1]+prevPoint[1],p[2]+prevPoint[2]) <= -mathforcap->getAccuracy() )
                    {
                        for(int kk = 0; kk < 1; ++kk)//количество попыток наказать луч
                        {
                            qDebug() << "&&& Try to punish ray.";
                            tt = std::sqrt(tempnorm)/RandomForCap::GetRandomExcExc(-5*std::sqrt(templength),5*std::sqrt(templength));
                            for(int iii=0; iii<3; ++iii)
                                rPoint[iii] = prevPoint[iii]+directingVectorLine[iii]/tt;
                            QByteArray tf0Modified = "(" + f0 + ")/(z-(" + QByteArray::number(rPoint[2],'f',15) + "))";
                            if(mathforcap->NewtonMetodForEquationAndLine(f0,tf0Modified,prevPoint,rPoint,directingVectorLine) < 0)
                            {
                                qDebug() << "### Error. Unable to punish ray.";
                                return -2;
                            }
                            if(mglFBottomOfCap->Eval(rPoint[0],rPoint[1],rPoint[2]) < 0.)
                            {
                                tf0Modified +="/(x-("+ QByteArray::number(rPoint[0],'f',15) + "))";
                                if(mathforcap->NewtonMetodForEquationAndLine(f0,tf0Modified,prevPoint,rPoint,directingVectorLine) < 0)
                                {
                                    qDebug() << "### Error 2. Unable to punish ray.";
                                    return -2;
                                }
                            }
                            for (int iii = 0; iii < numOfVar; ++iii)
                                normalVectorOfF0[iii] = mglf0->Diff(diffCharXi[iii],rPoint[0],rPoint[1],rPoint[2]);
                            double testAngleMult = normalVectorOfF0[0]*(rPoint[0]-prevPoint[0])
                                    + normalVectorOfF0[1]*(rPoint[1]-prevPoint[1])
                                    + normalVectorOfF0[2]*(rPoint[2]-prevPoint[2]);
                            if(testAngleMult > 0)
                            {
                                qDebug() << "Suссessfully punished ray!" << rPoint[0] << rPoint[1] << rPoint[2];
                                break;
                            }
                            else
                            {
                                tf0Modified +="/(x-("+ QByteArray::number(rPoint[0],'f',15) + "))";
                                if(mathforcap->NewtonMetodForEquationAndLine(f0,tf0Modified,prevPoint,rPoint,directingVectorLine) < 0)
                                {
                                    qDebug() << "### Error 3. Unable to punish ray.";
                                    return -2;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                break;
            }
        }*/
//        qDebug() << "TWO" << rPoint[0] << rPoint[1] << rPoint[2];
        if ( ii==0 ) //первая точка
        {
//            qDebug() << "FIRST POINT:" << "rPoint:" << rPoint << ",pointsZ[0]:" << pointsZ[0];
        //если луч не в ту сторону полетел

            if ( ( (mathforcap->rayGoesUp == 1)
                 && (mglFBottomOfCap->Eval(rPoint[0],rPoint[1],rPoint[2]) < 0.) )
                    || ( (mathforcap->rayGoesUp == 0)
                        && (mglFEndOfCap->Eval(rPoint[0],rPoint[1],rPoint[2]) > 0.))
                    ) //луч не в ту сторону полетел)
            {
//                qDebug() << "luch ne v tu storonu poletel";
                QByteArray tf0Modified = "(" + f0 + ")/(z-(" + QByteArray::number(rPoint[2],'f',16) + "))";
//                qDebug() << "rPoint[2] < pointsZ[0]:" << "tf0Modified:" << tf0Modified.data();
                getFirstApproximation(rPoint);
//                prevPoint[0] = pointsX[0]; prevPoint[1] = pointsY[0]; prevPoint[2] = pointsZ[0];
                if (int temp = mathforcap->NewtonMetodForEquationAndLine(f0,tf0Modified,prevPoint,rPoint,directingVectorLine) < 0)
                {
                    qDebug() << "### ERROR in first point mode" << temp;
                    return -3;
                }
            }


//            qDebug() << "Удаляем точки, а напр. вектор:" << directingVectorLine[0] << directingVectorLine[1] << directingVectorLine[2];
            pointsX.clear(); pointsY.clear(); pointsZ.clear(); anglesGlacing.clear();

            //если нам нужна только первая точка отражения НЕ ОТЛАЖЕНО!?????????????
            if (useOnlyFirstPointRef == 1)
            {
                pointsX.push_back(rPoint[0]); pointsY.push_back(rPoint[1]); pointsZ.push_back(rPoint[2]);
                directingVectorLine[0] = pointsX[ii+1] - pointsX[ii]; //
                directingVectorLine[1] = pointsY[ii+1] - pointsY[ii]; //напр. вектор такой же как прошлый, только более подходящей для дальнейшего длины
                directingVectorLine[2] = pointsZ[ii+1] - pointsZ[ii]; //ll_ray;mm_ray;nn_ray;
                double tempNormDirVec = std::sqrt(directingVectorLine[0]*directingVectorLine[0]
                                                  +directingVectorLine[1]*directingVectorLine[1]
                                                  +directingVectorLine[2]*directingVectorLine[2]);
                if(!flagDEBUGf0ONNSPECTRUM)
                {
                    for (i = 0; i < numOfVar; ++i)
                        normalVectorOfF0[i] = mglf0->Diff(diffCharXi[i],pointsX[ii+1],pointsY[ii+1],pointsZ[ii+1]);  //normalVector==={l_f0;m_f0;n_f0}
                }
                else
                {
                    normalVectorOfF0[0] = mathforcap->FunTorDX(pointsX[ii+1],pointsY[ii+1],pointsZ[ii+1]);
                    normalVectorOfF0[1] = mathforcap->FunTorDY(pointsX[ii+1],pointsY[ii+1],pointsZ[ii+1]);
                    normalVectorOfF0[2] = mathforcap->FunTorDZ(pointsX[ii+1],pointsY[ii+1],pointsZ[ii+1]);
                }
                double tempNormalVectorOfF0 = std::sqrt(normalVectorOfF0[0]*normalVectorOfF0[0]+normalVectorOfF0[1]*normalVectorOfF0[1]+normalVectorOfF0[2]*normalVectorOfF0[2]);
                for (i = 0; i < numOfVar; ++i) // как нормально контролировать внешность или внутренность нормали???????????????
                    normalVectorOfF0[i] = - normalVectorOfF0[i]/tempNormalVectorOfF0;  //нормированный normalVector==={l_f0;m_f0;n_f0}
    //            qDebug() << "n" << -lf << -mf << -nf << "l" << ll << ml << nl << "angle" << (M_PI_2 - std::acos((-lf*ll-mf*ml-nf*nl)/tempNormDirVec))*180/M_PI;
                anglesGlacing.push_back(std::asin((-normalVectorOfF0[0]*directingVectorLine[0]-normalVectorOfF0[1]*directingVectorLine[1]-normalVectorOfF0[2]*directingVectorLine[2])/tempNormDirVec));

                return 1;
            }
        }//конец специального блока для первой точки пересечения

        if (mathforcap->rayGoesUp == 1)
        {
//            if(rPoint[2] <= zBottomOfCap) //луч не захватился капилляром, или вылетел из каппиляра обратно
            if( mglFBottomOfCap->Eval(rPoint[0],rPoint[1],rPoint[2]) <= 0. )
            {
                qDebug() << "                                   @@@ Ray isn't captured"
                 << "rPoint" << rPoint[0] << rPoint[1] << rPoint[2];
                return -1;
            }

//            if(rPoint[2] >= zEndOfCap) //луч вылетел за пределы капилляра - ему место на экране
            if( mglFEndOfCap->Eval(rPoint[0],rPoint[1],rPoint[2]) >= 0. )
            {
                if((ii==0)&&(flagDEBUGForSpectrumOFFNonReflRaes))
                {
                    return -8;
                }
//                qDebug() << "THREE" << "Luchu mesto na ekrane: mglFEndOfCap->Eval(rPoint[0],rPoint[1],rPoint[2])"
//                            << mglFEndOfCap->Eval(rPoint[0],rPoint[1],rPoint[2])
//                            << "rPoint: " << rPoint[0] << rPoint[1] << rPoint[2];
                return 2;
            }

        }
        else//rayGoesUp == 0
        {
//            if(rPoint[2] >= zEndOfCap) //луч не захватился капилляром, или вылетел из каппиляра обратно
            if( mglFEndOfCap->Eval(rPoint[0],rPoint[1],rPoint[2]) > 0. )
            {
                qDebug() << "                                   @@@ Ray isn't captured";
                return -1;
            }
//            if(rPoint[2] <= zBottomOfCap) //луч вылетел за переделы капилляра - ищем соответствующую ему точку источника
            if( mglFBottomOfCap->Eval(rPoint[0],rPoint[1],rPoint[2]) < 0. )
            {
                return 3;
            }
        }
        pointsX.push_back(rPoint[0]); pointsY.push_back(rPoint[1]); pointsZ.push_back(rPoint[2]);
        prevPoint[0] = rPoint[0]; prevPoint[1] = rPoint[1]; prevPoint[2] = rPoint[2];
//        if(ii !=0 )
//        {
//            directingVectorLine[0]=pointsX[ii]-pointsX[ii-1];
//            directingVectorLine[1]=pointsY[ii]-pointsY[ii-1]; //напр. вектор такой же как прошлый, только более подходящей для дальнейшего длины
//            directingVectorLine[2]=pointsZ[ii]-pointsZ[ii-1];
//        }//в целом не нужно, пережитки старого алгоритма
        if(!flagDEBUGf0ONNSPECTRUM)
        {
            for (i = 0; i < numOfVar; ++i)
                normalVectorOfF0[i] = mglf0->Diff(diffCharXi[i],pointsX[ii],pointsY[ii],pointsZ[ii]);
        }
        else
        {
            normalVectorOfF0[0] = mathforcap->FunTorDX(pointsX[ii],pointsY[ii],pointsZ[ii]);
            normalVectorOfF0[1] = mathforcap->FunTorDY(pointsX[ii],pointsY[ii],pointsZ[ii]);
            normalVectorOfF0[2] = mathforcap->FunTorDZ(pointsX[ii],pointsY[ii],pointsZ[ii]);
        }

        double tempNormalVectorOfF0 = std::sqrt(std::pow(normalVectorOfF0[0],2) +
                                                std::pow(normalVectorOfF0[1],2) +
                                                std::pow(normalVectorOfF0[2],2) );

        for (i = 0; i < numOfVar; ++i)
        {
            normalVectorOfF0[i] /= tempNormalVectorOfF0;  //нормированный вектор внутренней нормали normalVector==={l_f0;m_f0;n_f0}
        }
        double tempProjection = normalVectorOfF0[0]*directingVectorLine[0] //проекция прошлого направл. вектора на нормаль
                              + normalVectorOfF0[1]*directingVectorLine[1]
                              + normalVectorOfF0[2]*directingVectorLine[2];
        double tempNormDirVec = std::sqrt(directingVectorLine[0]*directingVectorLine[0] +
                                          directingVectorLine[1]*directingVectorLine[1] +
                                          directingVectorLine[2]*directingVectorLine[2]);
        anglesGlacing.push_back(std::asin(tempProjection/tempNormDirVec));
//        qDebug() << "noremalVector" <<  normalVectorOfF0[0] << normalVectorOfF0[1] << normalVectorOfF0[2]
//                 << "directVector" << directingVectorLine[0] << directingVectorLine[1] << directingVectorLine[2];
//        qDebug() << "angle theta :" << std::asin(tempProjection/tempNormDirVec)
//                 << "rPoint :" << rPoint[0] << rPoint[1] << rPoint[2];

        directingVectorLine[0]=(-2*std::fabs(tempProjection)*normalVectorOfF0[0] + directingVectorLine[0]); //новый направляющий вектор прямой)
        directingVectorLine[1]=(-2*std::fabs(tempProjection)*normalVectorOfF0[1] + directingVectorLine[1]); //новые = для новой прямой
        directingVectorLine[2]=(-2*std::fabs(tempProjection)*normalVectorOfF0[2] + directingVectorLine[2]); //для старой прямой значение потеряно и не нужно


//        qDebug() << "14apr new directingVectorLine:" <<directingVectorLine[0] << directingVectorLine[1] << directingVectorLine[2];
//        qDebug() << "14apr normalVectorLine:" <<normalVectorOfF0[0] << normalVectorOfF0[1] << normalVectorOfF0[2];
//        qDebug() << "14apr angleglancing:" << std::asin(tempProjection/tempNormDirVec);

        if(std::asin(tempProjection/tempNormDirVec) < 0)
        {
            qDebug() << "           ### ERROR ANGLEGLACING <0!";
        }
        //qDebug() << directingVectorLine[0] << directingVectorLine[1] << directingVectorLine[2];
//        pointPrevRef[0] = directingVectorLine[0] + pointsX[ii+1];//отражённая предыдущая точка - полезно для понимания, но лишнее для алгоритма
//        pointPrevRef[1] = directingVectorLine[1] + pointsY[ii+1];
//        pointPrevRef[2] = directingVectorLine[2] + pointsZ[ii+1];
        rPoint[0] = directingVectorLine[0] + pointsX.at(ii); //pointPrevRef[0]
        rPoint[1] = directingVectorLine[1] + pointsY.at(ii); //pointPrevRef[1]
        rPoint[2] = directingVectorLine[2] + pointsZ.at(ii); //pointPrevRef[2] --новое начальное приближение
//        qDebug() << "New rPoint:" << rPoint[0] << rPoint[1] << rPoint[2];
        //Здесь придётся выбирать приближение на новом луче, т.к. изменение вектор очень маленькое, а нам нужна точность изменения вектора аж до -30 степени!!!
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }
    /// все точки отражения посчитаны

    qDebug() << "There are too many points!";
    return -2;
}

double getSqrtLen(QVector<double> & vec, QVector<double> &prev)
{
    double temp= 0;
    for(int i=0;i<3;++i)
    {
        temp+= std::pow((vec[i]-prev[i]),2);
    }
    return (temp);
}

double getSqrtLen(QVector<double> & vec)
{
    double temp= 0;
    foreach(double td, vec)
    {
        temp+= std::pow(td,2);
    }
    return (temp);
}

int Capillary::CalculationOfPassageOfTheBeamALL(
        double *directingVectorLine,
        QVector<double> &pointsX, QVector<double> &pointsY, QVector<double> &pointsZ,
        QVector<double> &anglesGlacing)
{
    long int i = 0, ii = 0, jj; //counter
    QByteArray f0Modified = ""; //уравнение функции без лишнего члена
    QByteArray f0array = ""; //часть на которую мы будем делить старую функцию для модификации
    double testAngleMult = 0. ;//для скалярного произведения... для проверки под каким уголом входит луч в капилляр
    QVector<double> normalVectorOfF0; //нормальный вектор поверхности
    normalVectorOfF0.resize(numOfVar);
    if(!flagDEBUGf0ONNSPECTRUM)
    {
        for (i = 0; i < numOfVar; ++i)
            normalVectorOfF0[i] = mglf0->Diff(diffCharXi[i],pointsX[0],pointsY[0],pointsZ[0]);  //normalVector==={l_f0;m_f0;n_f0}
    }
    else
    {
        normalVectorOfF0[0] = mathforcap->FunTorDX(pointsX[0],pointsY[0],pointsZ[0]);
        normalVectorOfF0[1] = mathforcap->FunTorDY(pointsX[0],pointsY[0],pointsZ[0]);
        normalVectorOfF0[2] = mathforcap->FunTorDZ(pointsX[0],pointsY[0],pointsZ[0]);
    }

    //    double pointPrevRef[numOfVar];    //отражённая предыдущая точка - полезно для понимания, но лишнее для алгоритма
    QVector<double> prevPoint;             //точка, что принадлежит прямой, которая используется в NM
    double prevPointSimple[numOfVar];
    prevPoint.resize(numOfVar);
    double rPointSimple[numOfVar];
    QVector<double> rPoint,rTotalPoint;
    rPoint.resize(numOfVar); //основная рабочая точка
    rTotalPoint.resize(numOfVar);
    rTotalPoint.fill(0.,numOfVar);
    prevPoint[0] = pointsX[0]; prevPoint[1] = pointsY[0]; prevPoint[2] = pointsZ[0];

//    rPoint[0] = pointsX[0] + directingVectorLine[0];
//    rPoint[1] = pointsY[0] + directingVectorLine[1];
//    rPoint[2] = pointsZ[0] + directingVectorLine[2];
    rPoint[0] = 0.1;
    rPoint[1] = 0.1;
    rPoint[2] = -1e-3;
    //начальное приближение руками!!?????????Устанавливаем начальное приближение 0,0,0 выбирать нельзя!!!!!!!!!!!!!!
    QVector< QVector<double> > tAllPoints;
    int countDiffZ = 3;//!!!!!!!!!!!?????????????????????????? перенести в mathForCap <-----
    int tret;

/// начало поиска точек отражения
    for(ii = 0; ii < 10000; ++ii)
    {
        f0Modified = f0;
        tAllPoints.clear();
        for(i = 0; i < countDiffZ; ++i)
        {
            f0array = "";
            prevPointSimple[0] = prevPoint[0]; prevPointSimple[1] = prevPoint[1]; prevPointSimple[2] = prevPoint[2];
            qDebug() << "CPB: rPoint :" << rPoint << "prevPoint:" << prevPoint;
            jj=0;
            do
            {
                ++jj; if(jj==9) break;
                rPointSimple[0] = RandomForCap::GetRandomIncInc(-10.,10.);
                rPointSimple[1] = RandomForCap::GetRandomIncInc(-10.,10.);
                rPointSimple[2] = RandomForCap::GetRandomIncInc(prevPoint[2],1e4);//(mglFEndOfCap->Eval(0.,-1.,0)+10.));
                //ну и как делать начальное приближение??????????
                try {
                tret = mathforcap->NewtonMetodForEquationAndLine(f0,f0Modified,prevPointSimple,rPointSimple,directingVectorLine);
                }
                catch (...) {
                }
                rPoint[0] = rPointSimple[0]; rPoint[1] = rPointSimple[1]; rPoint[2] = rPointSimple[2];//исправить NMFEAL rPoint ---> QVector<double>
                if(std::fabs(mglf0->Eval(rPoint[0],rPoint[1],rPoint[2])) > mathforcap->getAccuracy())
                {
                    qDebug() << "Stage 3 (last attemp):";
                    if( (tret= mathforcap->NewtonMetodForEquationAndLine(f0,f0,prevPointSimple,rPointSimple,directingVectorLine)) < 0)
                        qDebug() << "### Error. Stop stage 3.";
                    else
                    {
                        rPoint[0] = rPointSimple[0]; rPoint[1] = rPointSimple[1]; rPoint[2] = rPointSimple[2];//исправить NMFEAL rPoint - QVector<double>
                    }
                }
            }
            while( ((mglFBottomOfCap->Eval(rPoint[0],rPoint[1],rPoint[2]) <= 0.))
                   ||(std::fabs(rPoint[2]-prevPoint[2]) <=1e-10));
            if (tret < 0)
            {
                if (i == 0)
                {
                    qDebug() << "Points of intersection was not find!";
                    return -2;
                }
                break; //перестали искать точки, после первой неудачной попытки - обеспечивает возможностью не следить за количеством степеней в уравнении
            }
            else
            {
                bool dobro = true;
                foreach(QVector<double> vec,tAllPoints)
                {
//                    qDebug() << std::fabs(vec[2]-rPoint[2]);
                    if(std::fabs(vec[2]-rPoint[2]) <= 1e-10)
                    {
                        dobro = false;
                    }
                }
                if(dobro)
                {
                    tAllPoints.push_back(rPoint);
                    qDebug() << "CPB: " << "Point N." << i << "tAllPoints:" << tAllPoints;
                }
            }
//            if( i > 0 )
//                f0array += "*";
//            if( i == 1 )
//                f0array.insert(0,"(");
            if( i == 0 )
                f0array += "(z-(" + QByteArray::number(rPoint[2],'f',15) + "))";
            if( i == 1 )
            {
                f0array.insert(0,"(");
                f0array += "*";
                f0array += "(x-(" + QByteArray::number(rPoint[0],'f',15) + ")))";
            }
            if( i > 1 )
            {
                f0array.remove(f0array.lastIndexOf(")"),1);//?????????/ работает? вряд ли... не отлажено.
                f0array += "*";
                f0array += "(z-(" + QByteArray::number(rPoint[2],'f',15) + "))";
                f0array += ")";
            }
            f0Modified = "(" + f0 + ")/" + f0array;
            qDebug() << f0Modified;
        }
        //для проверки под каким углом луч входит в капилляр в этой точке:
        for (i = 0; i < numOfVar; ++i)
            normalVectorOfF0[i] = mglf0->Diff(diffCharXi[i],rPoint[0],rPoint[1],rPoint[2]);
        testAngleMult = normalVectorOfF0[0]*directingVectorLine[0] + normalVectorOfF0[1]*directingVectorLine[1] + normalVectorOfF0[2]*directingVectorLine[2];
        foreach(QVector<double> vec, tAllPoints)
        {
            //if(mathforcap->rayGoesUp == 1)
            //в данном методе всё для летящего от источника!!!!!!!!!!!!
//            if( mglFBottomOfCap->Eval(vec[0],vec[1],vec[2]) >= 0. )
//            {//выше сделали эту проверку, тем лучше...
                if( (getSqrtLen(vec,prevPoint) < getSqrtLen(rTotalPoint,prevPoint))
                        ||(getSqrtLen(rTotalPoint)==0.) )
                {
//                    if(testAngleMult >= 0)//пока что отключим
//                    {
                        rTotalPoint[0] = vec[0];
                        rTotalPoint[1] = vec[1];
                        rTotalPoint[2] = vec[2];
//                    }
                }
//            }
        }
        qDebug() << "rTotalPoint" << rTotalPoint;
        if(ii==0)
        {
            pointsX.clear(); pointsY.clear(); pointsZ.clear(); anglesGlacing.clear();
        }
        if(getSqrtLen(rTotalPoint)==0.)
        {
            qDebug() << "Ray isn't captured";
            return -10;//!!!!! -10 - нету таких точек пересечения, которые бы захватились в нужную область капилляром
        }
        else
        {
            //if(rPoint[2] >= zEndOfCap) //луч вылетел за пределы капилляра - ему место на экране
            if( mglFEndOfCap->Eval(rTotalPoint[0],rTotalPoint[1],rTotalPoint[2]) > 0 )
            {
                if((ii==0)&&(flagDEBUGForSpectrumOFFNonReflRaes))
                {
                    return -11;//!!!!!! - нету отражений, только точка прошедшая без отражения
                }
                rPoint[0] = rTotalPoint[0];
                rPoint[1] = rTotalPoint[1];
                rPoint[2] = rTotalPoint[2];
                return 2;//!!!!!! - попала на экран
            }
            pointsX.push_back(rTotalPoint[0]);
            pointsY.push_back(rTotalPoint[1]);
            pointsZ.push_back(rTotalPoint[2]);
            prevPoint[0] = rTotalPoint[0];
            prevPoint[1] = rTotalPoint[1];
            prevPoint[2] = rTotalPoint[2];
        }

        if(!flagDEBUGf0ONNSPECTRUM)
        {
            for (i = 0; i < numOfVar; ++i)
                normalVectorOfF0[i] = mglf0->Diff(diffCharXi[i],pointsX[ii],pointsY[ii],pointsZ[ii]);
        }
        else
        {
            normalVectorOfF0[0] = mathforcap->FunTorDX(pointsX[ii],pointsY[ii],pointsZ[ii]);
            normalVectorOfF0[1] = mathforcap->FunTorDY(pointsX[ii],pointsY[ii],pointsZ[ii]);
            normalVectorOfF0[2] = mathforcap->FunTorDZ(pointsX[ii],pointsY[ii],pointsZ[ii]);
        }

        double tempNormalVectorOfF0 = std::sqrt(std::pow(normalVectorOfF0[0],2) +
                                                std::pow(normalVectorOfF0[1],2) +
                                                std::pow(normalVectorOfF0[2],2) );

        for (i = 0; i < numOfVar; ++i)
        {
            normalVectorOfF0[i] = normalVectorOfF0[i]/tempNormalVectorOfF0;  //нормированный вектор внутренней нормали normalVector==={l_f0;m_f0;n_f0}
        }
        double tempProjection = normalVectorOfF0[0]*directingVectorLine[0] //проекция прошлого направл. вектора на нормаль
                              + normalVectorOfF0[1]*directingVectorLine[1]
                              + normalVectorOfF0[2]*directingVectorLine[2];
        double tempNormDirVec = std::sqrt(directingVectorLine[0]*directingVectorLine[0] +
                                          directingVectorLine[1]*directingVectorLine[1] +
                                          directingVectorLine[2]*directingVectorLine[2]);
        anglesGlacing.push_back(std::asin(tempProjection/tempNormDirVec));
//        qDebug() << "noremalVector" <<  normalVectorOfF0[0] << normalVectorOfF0[1] << normalVectorOfF0[2]
//                 << "directVector" << directingVectorLine[0] << directingVectorLine[1] << directingVectorLine[2];
        qDebug() << "angle theta :" << std::asin(tempProjection/tempNormDirVec)
                 << "rPoint :" << rPoint[0] << rPoint[1] << rPoint[2]
                 << "angle cos:" << (tempProjection/tempNormDirVec);
        directingVectorLine[0]=(-2*std::fabs(tempProjection)*normalVectorOfF0[0] + directingVectorLine[0]); //новый направляющий вектор прямой)
        directingVectorLine[1]=(-2*std::fabs(tempProjection)*normalVectorOfF0[1] + directingVectorLine[1]); //новые = для новой прямой
        directingVectorLine[2]=(-2*std::fabs(tempProjection)*normalVectorOfF0[2] + directingVectorLine[2]); //для старой прямой значение потеряно и не нужно
        //qDebug() << directingVectorLine[0] << directingVectorLine[1] << directingVectorLine[2];
//        pointPrevRef[0] = directingVectorLine[0] + pointsX[ii+1];//отражённая предыдущая точка - полезно для понимания, но лишнее для алгоритма
//        pointPrevRef[1] = directingVectorLine[1] + pointsY[ii+1];
//        pointPrevRef[2] = directingVectorLine[2] + pointsZ[ii+1];
        rPoint[0] = directingVectorLine[0] + pointsX[ii]; //pointPrevRef[0]
        rPoint[1] = directingVectorLine[1] + pointsY[ii]; //pointPrevRef[1]
        rPoint[2] = directingVectorLine[2] + pointsZ[ii]; //pointPrevRef[2] --новое начальное приближение
    }
    /// все точки отражения посчитаны

    qDebug() << "There are too many points!";
    return -2;
}

