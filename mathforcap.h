#ifndef MATHFORCAP_H
#define MATHFORCAP_H

#include <QObject>
#include <QtCore/QDebug>
#include <QSettings>
#include <complex>
#include <cmath>
#include <mgl2/qt.h>
#include <mgl2/eval.h>
#include <randomforcap.h>
#include <QMutex>

class MathForCap : public QObject
{
    Q_OBJECT
private:
    double accuracy; //точность вычислений... примерная +-3 порядка, как показала практика. В Методе Ньютона используем модифицированную функцию, в этом может быть проблема с неточностью
    QMutex m_mutex;
public:
    double RSPECTRUM,r_0SPECTRUM,LSPECTRUM;

    double getAccuracy() {return accuracy;}
    bool rayGoesUp; //или считаем со стороны экрана, или со стороны источника
//    static std::complex<double> imaginaryUnit; //мнимая единица
//    static int numOfVar; //кол-во переменных 3 -> x,y,z
//    static int numOfEq; //кол-во переменных 3 -> x,y,z
//    static char diffCharXi[6]; //массив букв, по которым дифференцируем

    explicit MathForCap(QObject *parent = 0);

    static void Unit_Lower_Triangular_Solve(double *L, double B[], double x[], int n);
    static int Upper_Triangular_Solve(double *U, double B[], double x[], int n);
    static int Doolittle_LU_Decomposition_with_Pivoting(double *A, int pivot[], int n);
    static int Doolittle_LU_with_Pivoting_Solve(double *A, double B[], int pivot[],double x[], int n);
    static int Doolittle_LU_Decomposition(double *A, int n);
    static int Doolittle_LU_Solve(double *LU, double B[], double x[], int n);

    double rectangle_integrate(mglExpr & funRe, mglExpr & funIm, double tau
                               ,/* double border_, double border__, */long int & n
                               , double & delay);// DELET ME!!!!

    double FunTor(double x, double y, double z);
    double FunTorDX(double x, double y, double z);
    double FunTorDY(double x, double y, double z);
    double FunTorDZ(double x, double y, double z);

    inline int Sign(double Val1,double Val2) const;
    int NewtonMetodPath(QByteArray &FUN, const double *prevPoint,
                                    double *rPoint,
                                    double * directingVectorLine);

    int NewtonMetodForEquationAndLine(const QByteArray &INITIALFUN, const QByteArray &FUN,
                                       const double *prevPoint, double *rPoint,
                                      double *directingVectorLine); //здесь хорошее уравнение, модифицированное
                                       //rPoint - текущее приближение. DirectingVectorLine - направляющий вектор прямой.
                                       //prevPoint - какая-то точка, принадлежащая прямой
                                       //если не удалось сделать LU и solve возвращает -1
                                       //если зациклился, то -2
                                       //если всё ок, то 0 и записывает в rPoint нужную нам точку!;)

signals:
    
public slots:

};

static const std::complex<double> imaginaryUnit = std::complex<double> (0.0,1.0);
static const int numOfVar = 3;
static const int numOfEq = 3;
static const char diffCharXi[] = {'x','y','z','u','v','w'};

static const int DEBUGCOUNTIMG = 147;//количество изображений...!!!!
static const int DEBUGINCEPTIONIMG = 100;
static const char * DEBUGNAMEFOLDER = "/home/don_vanchos/Documents/!CONF27MAY/";
//используется в calcAndDraw, а так же... где?


static const int DEBUGintSpectrumTorOrCap = 0; //0 - циллиндр, 1 - тор.
static const double DEBUGdouleInitLength = 20000; //в мкм


static const bool flagDEBUGParallel = false; //ЛУЧШЕ НЕ ИСПОЛЬЗОВАТЬ!!! флаг для сoherence.. включением многопоточный sendRay!!! НЕ РАБОТАЕТ ДОЛЖНЫМ ОБРАЗОМ!!! ТОЛЬКО ОДИН ПОТОК!
static const bool flagDEBUGSearchAllPoints = false; // флаг для сoherence.. ОСТОРОЖНО!!! переключение на метод поиска всех точек!!
//!!!!!для использования метода счёт всех точек пересечения(для сложной геометрии) - не работает!!!
static const bool flagDEBUGf0ONNSPECTRUM = false;//это для учёта или не учёта уравнения поверхности записанной в явном виде!! //было в mathforcap
//<--- это отладочное, надо быть очень осторожным, вносит изменения - добавляет возможность пользоваться явно прописанной функцией тора
static const bool flagDEBUGForSpectrumOFFNonReflRaes = false;// вносит изменения - убирает глубоко лучи, прошедшие без отражения в capillary
static const bool flagDEBUGtestRandRay = false; //!!! включает проверку лучей на отрезке от предыдущей точки до настоящей... - не работает

static const bool flagTestDrawAnimation = false; //включает создание анимации для конференции

static const bool flagDebugDrawTitle = false; //включает рисовалку для удобства для конференции - длину в титл ИИИ установку параметров, размером изображений... scale

static const bool flagDEBUGKkolvomaxrays = true; //включается легенду - максимальное количество лучей

static QString elapsCap(long i)
{
//    double h,m,s;
    long di = i/1000;
//    h = (di/3600);
//    m = (di%3600)/60;
//    s = (di%3600)%60;
    return (QString::number(di/3600) + ":"
            + QString::number((di%3600)/60) + ":"
            + QString::number((di%3600)%60));
}

//static const double accuracy = 1e-14;
//char MathForCap::diffCharXi[1] = 'y';
//char MathForCap::diffCharXi[2] = 'z';
//char MathForCap::diffCharXi[3] = 'u';
//char MathForCap::diffCharXi[4] = 'v';
//char MathForCap::diffCharXi[5] = 'w';

#endif // MATHFORCAP_H
