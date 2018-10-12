#ifndef SCREEN_H
#define SCREEN_H

#include <QObject>
#include "randomforcap.h"
#include "mathforcap.h"

class Screen : public QObject
{
    Q_OBJECT
private:
    int scrNum;
//    double border;//например граница 2, значит экран по y и х [-2;2] ????????????????/избавляемся.
//    int meshScreen;//=nx=ny кол-во детекторов на экране. ???????????????????????/избавляемся
    double zScreen;//координата z экрана - плоскости, параллельной X0Y
    QByteArray fScreen;//"z-200.0"
    double edgeOfScreenSquare;//сторона квадрата экрана
    long int meshScreenNorm;//задаётся пользователем
    MathForCap * mathforcap;//для рассчёта NewtonMetod))

    long int meshScreenX;//кол-во детекторов по оси X (пока равно по оси Y)
    long int meshScreenY;//кол-во детекторов по оси Y (пока равно по оси X)
    long int meshScreenXY;//полное кол-во детекторов...

public:
    QVector<double> pointsXscreen, pointsYscreen;//для самопроверки индексации -> положения детектора
    double xShift,yShift;
    QByteArray getFScreen() { return fScreen; }
    long int isMeshScreenNorm() {return meshScreenNorm;}
    long int isMeshScreenXY() {return meshScreenXY;}
    long int isMeshScreenX() {return meshScreenX;}
    long int isMeshScreenY() {return meshScreenY;}
    double isEdgeOfScreenSquare() {return edgeOfScreenSquare;} //не используется
    explicit Screen(int scrNumber, QObject *parent = 0);
    void getRandomX_0Y_0Z_0(double &randX_0, double &randY_0, double &randZ_0);
    void getRandomPhi_0Theta_0(double &randPhi_0, double &randTheta_0);
    long int getIndexOfLastPoint(const double &x, const double &y);
    int CalculationOfPassageOfTheBeam(double *directingVectorLine, double *prevPoint,
                                      double &pointXscr, double &pointYscr, double &pointZscr,
                                      double &angleScrPhi, double &angleScrTheta);
    //возвращает два угла и точку на экране... больше ничего и не нужно

signals:
    
public slots:
    
};

#endif // SCREEN_H
