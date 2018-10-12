#ifndef TESTFFT_H
#define TESTFFT_H

//#include <QWidget>
//#include <QObject>
#include <QDebug>
#include "fft.h"
#include <mgl2/qt.h>

class testFFT : /*public QWidget,*/ public mglDraw
{
//    Q_OBJECT
public:
    explicit testFFT(/*QWidget *parent = 0*/);
    int Draw(mglGraph *gr);

    double AverNu;
    double tau;
    int n;
    double tauF;
    QByteArray FRE;
    QByteArray FIM;

signals:
    
public slots:
    
};

#endif // TESTFFT_H
