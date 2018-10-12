#include "screen.h"

Screen::Screen( int scrNumber, QObject *parent) :
    QObject(parent)
{
    scrNum = scrNumber;
    qDebug() << "---> Screen" + QString::number(scrNum-1)+" are being created...";

    mathforcap = new MathForCap(this);
    QSettings settings;
    fScreen = settings.value("CurrentSettings/fScreen_"+QString::number(scrNum-1),"404").toByteArray();
    mglExpr mglFScreen(fScreen.data());
    zScreen = std::fabs(mglFScreen.Eval(0.,0.,0.));

    xShift = settings.value("CurrentSettings/xShift_"+QString::number(scrNum-1),404.404).toDouble();
    yShift = settings.value("CurrentSettings/yShift_"+QString::number(scrNum-1),404.404).toDouble();

    edgeOfScreenSquare =  settings.value("CurrentSettings/edgeOfScreenSquare_"+QString::number(scrNum-1),404.404).toDouble();
    meshScreenNorm = settings.value("CurrentSettings/meshScreenNorm_"+QString::number(scrNum-1),404).toInt();
    qDebug() << "zScreen << fScreen << edgeOfScreenSquare << meshScreenNorm" << zScreen << fScreen << edgeOfScreenSquare << meshScreenNorm;

    meshScreenX = (long int)(edgeOfScreenSquare*meshScreenNorm) + 1;
    meshScreenY = (long int)(edgeOfScreenSquare*meshScreenNorm) + 1;
    meshScreenXY = meshScreenX*meshScreenY;

    pointsXscreen.clear();
    pointsYscreen.clear();
    for(long int i = 0; i < meshScreenX; ++i)
    {
        pointsXscreen.push_back(-edgeOfScreenSquare/2.0+xShift+i/(double)meshScreenNorm);
        pointsYscreen.push_back(-edgeOfScreenSquare/2.0+yShift+i/(double)meshScreenNorm);
    }

//    qDebug() << "pointsXandpointsYscreen" << pointsXandpointsYscreen;
    qDebug() << "<--- Screen" +QString::number(scrNum-1) +" were created!..." << fScreen;
    qDebug();
}

void Screen::getRandomX_0Y_0Z_0(double &randX_0, double &randY_0, double &randZ_0)
{
//    do
//    {
        randX_0 = RandomForCap::GetRandomIncInc(-edgeOfScreenSquare/2.0+xShift,edgeOfScreenSquare/2.0+xShift);
        randY_0 = RandomForCap::GetRandomIncInc(-edgeOfScreenSquare/2.0+yShift,edgeOfScreenSquare/2.0+yShift);
        randZ_0 = zScreen;
//    }
//    while (fSourceXYZ.Calc(randX_0,randY_0,randZ_0) > fSourceXYZ_1.toDouble());
}

void Screen::getRandomPhi_0Theta_0(double &randPhi_0, double &randTheta_0)
{
    randPhi_0 = RandomForCap::GetRandomIncInc(-M_PI,M_PI);
    randTheta_0 = RandomForCap::GetRandomIncInc(-M_PI_2,-4e-13);
//    qDebug() << "  tempRandPhi_0 tempRandTheta_0:" << tempRandPhi_0*180/M_PI << tempRandTheta_0*180/M_PI;
}

long int Screen::getIndexOfLastPoint(const double &x,const double &y)
{
    return(
                (long int)( ((x-xShift) + edgeOfScreenSquare/2.0)*meshScreenNorm )
                + meshScreenX*(long int)( ((y-yShift) + edgeOfScreenSquare/2.0)*meshScreenNorm )
                );
}

int Screen::CalculationOfPassageOfTheBeam(double *directingVectorLine, double *prevPoint,
                                          double &pointXscr, double &pointYscr, double &pointZscr,
                                          double &angleScrPhi, double &angleScrTheta)//луч вылетел за переделы капилляра в сторону экрана ->
{
    double rPoint[numOfVar];
    rPoint[0] = 0.01; rPoint[1] = -0.01; rPoint[2] = (zScreen-2.);
    if( mathforcap->NewtonMetodForEquationAndLine(fScreen,fScreen,prevPoint,rPoint,directingVectorLine) < 0 )
    {
        qDebug() << "### ERROR. There is no point on the screen!";
        return -1;
    }
    if ( (std::fabs(rPoint[0]-xShift) > (edgeOfScreenSquare/2.0))
         ||(std::fabs(rPoint[1]-yShift) > (edgeOfScreenSquare/2.0))
        )
    {
//        qDebug() << "                                   @@@ The ray flew outsiden of the screen!";
//        qDebug() << "rPoint" << rPoint[0] << rPoint[1] <<rPoint[2];
        return 1;
    }
    pointXscr = rPoint[0];
    pointYscr = rPoint[1];
    pointZscr = rPoint[2];
    //присвоим углы, только нормально...
    mglExpr fsc(fScreen.data());
    double nvecF0[numOfVar];
    for(int l=0; l<3; ++l)
        nvecF0[l] = fsc.Diff(diffCharXi[l],rPoint);
    double tempNormP = std::sqrt(directingVectorLine[0]*directingVectorLine[0] + directingVectorLine[1]*directingVectorLine[1] + directingVectorLine[2]*directingVectorLine[2]);
    double tempNormN = std::sqrt(nvecF0[0]*nvecF0[0] + nvecF0[1]*nvecF0[1] + nvecF0[2]*nvecF0[2]);
    angleScrTheta = std::acos(
                (nvecF0[0]*directingVectorLine[0] + nvecF0[1]*directingVectorLine[1] + nvecF0[2]*directingVectorLine[2])
                /
                (tempNormN*tempNormP)
                );

    //прошлый метод:
//    double tempNorm = std::sqrt(directingVectorLine[0]*directingVectorLine[0] + directingVectorLine[1]*directingVectorLine[1] + directingVectorLine[2]*directingVectorLine[2]);
//    for (int l=0; l<3 ; ++l)
//        directingVectorLine[i] = directingVectorLine[i]/tempNorm;
    double temp1 = std::sqrt(directingVectorLine[0]*directingVectorLine[0] + directingVectorLine[1]*directingVectorLine[1]);
//    angleScrTheta = std::asin(temp1);
    angleScrPhi = std::asin(directingVectorLine[2]/temp1);

    return 0;
}
