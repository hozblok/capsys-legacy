#ifndef CAPILLARY_H
#define CAPILLARY_H

#include <QObject>

#include "randomforcap.h"
#include "mathforcap.h"

class Capillary : public QObject
{
    Q_OBJECT
private:
    int capNum;//=1,2,3,4,5,6,....
    QByteArray f0;//"x^2+y^2-1"
//    double zBottomOfCap; //начало капилляра по оси Z - избавлись от этой штуки, перешли к поверхности
    QByteArray fBottomOfCap;
//    double zEndOfCap; //конец капилляра по оси Z - избавились от этой штуки, перешли к поверхности
    QByteArray fEndOfCap;
    double plasmaFrequency;
    std::complex<double> epsilonCap; //диэл. проницаемость капилляра
//    double density; //????????????->plasmaFrequency
    mglExpr *mglf0, *mglFBottomOfCap, *mglFEndOfCap;// mglfScreen;
    MathForCap *mathforcap;

    void getFirstApproximation(double *r);

public:
    void setF0(QByteArray t) { delete mglf0; mglf0 = new mglExpr(t.data()); f0=t;}
    void setFEndOfCap(QByteArray t) {delete mglFEndOfCap; mglFEndOfCap = new mglExpr(t.data()); fEndOfCap = t;}

    QByteArray getF0() { return f0; }
    QByteArray getFBottomOfCap() { return fBottomOfCap; }
    QByteArray getFEndOfCap() { return fEndOfCap; }
    double getPlasmaFrequency() { return plasmaFrequency; }
    std::complex<double> getEpsilonCap() { return epsilonCap; }
    explicit Capillary( int capNumber, QObject *parent = 0);
    double getCoefOfRefl(double theta, double nu);
    bool tryCaptureSou(double *rSou, double *directVec);
    bool tryCaptureScr(double *rScr, double & phi, double & theta);
    int CalculationOfPassageOfTheBeam(
            double *directingVectorLine,
            QVector<double> &pointsX, QVector<double> &pointsY, QVector<double> &pointsZ, QVector<double> &anglesGlacing,
            bool useOnlyFirstPointRef);
                  //-3 - зациклился метод после попыток выправить луч в нужную сторону(вероятность такого события очень очень мала, почти невозможно);
                  //-2 - зациклился метод ньютона, скорее всего нет точек пересечения, или введены нули где не надо,но возможно и выставлена слишком мощная accurancy(лучше ставить не меньше 3e-13);
                  //-1 - луч выскочил или не захватился;
                  //0 - всё хорошо;
                  //1 - всё хорошо в режиме одного отражения; rayGoesUp = 0 пока не отлажен и не нужен
                  //2 - всё хорошо, ловим луч на экране
                  //3 - всё хорошо, ловим луч на источнике
                  //-8 - режим анти неотражённое для метода-спектра
    int CalculationOfPassageOfTheBeamALL(double *directingVectorLine,
            QVector<double> &pointsX, QVector<double> &pointsY, QVector<double> &pointsZ, QVector<double> &anglesGlacing);//НЕ РАБОТАЕТ!!!!
    //отличается от предыдущего только тем, что в процессе находит все до одной точки пересечения и уже из них выбирает подходящую
                  //-3 - зациклился метод после попыток выправить луч в нужную сторону(вероятность такого события очень очень мала, почти невозможно);
                  //-2 - зациклился метод ньютона, скорее всего нет точек пересечения, или введены нули где не надо,но возможно и выставлена слишком мощная accurancy(лучше ставить не меньше 3e-13);
                  //-1 - луч выскочил или не захватился;
                  //0 - всё хорошо;
                  //1 - всё хорошо в режиме одного отражения; rayGoesUp = 0 пока не отлажен и не нужен
                  //2 - всё хорошо, ловим луч на экране
                  //3 - всё хорошо, ловим луч на источнике
                  //-8 - режим анти неотражённое для метода-спектра
signals:
    
public slots:

};

#endif // CAPILLARY_H
