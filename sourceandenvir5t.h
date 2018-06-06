#ifndef SOURCEANDENVIR5T_H
#define SOURCEANDENVIR5T_H

#include <QObject>
#include "randomforcap.h"
#include "mathforcap.h"
//#include <mgl2/qt.h>
#include "fft.h"

class SourceAndEnvir5t : public QObject
{
    Q_OBJECT
private:
    double x_0_;
    double x_0__;//границы для выбора рандома
    double y_0_;
    double y_0__;
    double z_0_;
    double z_0__;
    QByteArray fSource;
    double phi_0_,phi_0__;//нижняя и верхняя граница рандома phi_0
    double theta_0_,theta_0__;//нижняя и верхняя граница рандома theta_0
    double speedOfLightForCap;
    double wavelength,wavelength2;
    double tau;
    QByteArray FRe,FIm;
    QByteArray fAmpl,fPhase,fScopePhase;

    int lengthUnitMultPower;
    double lengthUnitMult;

    mglExpr *mglFSource,*mglFRe,*mglFIm, *mglFAmpl, *mglFPhase, *mglFScopePhase;
    double averageFrequency;
//    double nonmonoch7yPercent;// бывшее double periodOfTime;
//    double initialAmplitude;
    std::complex<double> epsilonEnvir;//узнать как это влияет на распространение рентгена.
    MathForCap *mathforcap;

    void COUTDEBUG();

public:
    double getTau() {return tau;}
    QByteArray getFRe() {return FRe;}
    QByteArray getFIm() {return FIm;}
    double getZ_0_() {return z_0_;}
    double getZ_0__() {return z_0__;}
    double getLengthUnitMult() {return lengthUnitMult;}
    int getLengthUnitMultPower() {return lengthUnitMultPower;}
    double getAverageFrequency() {return averageFrequency;}
    double getSpeedOfLightForCap() {return speedOfLightForCap;}
    double getWaveLength() const {return wavelength;}
    double getWaveLength2() const {return (wavelength2);}

    explicit SourceAndEnvir5t(QObject *parent = 0);
    virtual ~SourceAndEnvir5t();
    void getRandomX_0Y_0Z_0(double &randX_0, double &randY_0, double &randZ_0);
    void getRandomX_0Y_0Z_0(double * randR_0);
    void getRandomPhi_0Theta_0(double &randPhi_0, double &randTheta_0);
    void getRandomPhi_0Theta_0(double *directVec);
    int CalculationOfPassageOfTheBeam(double *directingVectorLine, double *prevPoint,
                                              double &pointXsou, double &pointYsou, double &pointZsou);
    double integrationAmpl(int &n, double &delay);
    int prepareConvolution(int &n, QVector<double> &absV, QVector<double> &nu_i); //тут или создаётся спектр
    //, или делается преобразование фурье.... или просто задаётся монохромат.

    double getPhase(QVector<double> &rSou) const;
    double getAmpl(QVector<double> &rSou) const;
signals:
    
public slots:
    void slotSetZ_0_(double tz_0_) { z_0_ = tz_0_; }
    void slotSetZ_0__(double tz_0__) { z_0__ = tz_0__; }
    void slotSetX_0_(double tx_0_) { x_0_ = tx_0_; }
    void slotSetX_0__(double tx_0__) { x_0__ = tx_0__; }
    void slotSetY_0_(double ty_0_) { y_0_ = ty_0_; }
    void slotSetY_0__(double ty_0__) { y_0__ = ty_0__; }
    void slotSetTheta_0_(double t_0_) { theta_0_ = t_0_;}
    void slotSetTheta_0__(double t_0__) { theta_0__ = t_0__;}
};

#endif // SOURCEANDENVIR5T_H
