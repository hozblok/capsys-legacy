#ifndef TESTSOURCE_H
#define TESTSOURCE_H

//#include <QObject>
#include <QDebug>
#include <QObject>
#include "randomforcap.h"
#include <mgl2/qt.h>

class testSource : public mglDraw//, public QObject
{
//    Q_OBJECT
public:
    explicit testSource(/*QObject *parent = 0*/);

    int Draw(mglGraph *gr);

    int flagDraw;
    int numOfPointsForTest;
    double x_0_;
    double x_0__;//границы для выбора рандома
    double y_0_;
    double y_0__;
    double z_0_;
    double z_0__;
    QByteArray fSource;
    QByteArray fAmpl;
    QByteArray fPhase;
    QByteArray fScopePhase;

    int lengthUnitMultPower;

    RandomForCap rand;

signals:
    
public slots:
    
};

#endif // TESTSOURCE_H
