#ifndef COHERENCE_H
#define COHERENCE_H

#include <QtConcurrent>
#include <QtCore>
#include <QObject>
#include <QVector>
#include <QList>
#include <QFile>
#include <QDataStream>
#include <QIODevice>
#include <QList>
#include <QTreeWidget>

#include <mgl2/qt.h>
#include <mgl2/qmathgl.h>

#include "capillary.h"
#include "screen.h"
#include "sourceandenvir5t.h"
#include "dialogsetrangesetc.h"
//#include <functional>
//#include <algorithm>
//#include <memory>
//#include <tr1/functional>

#include <QTime>

#include <QProgressDialog>

//#include <QThreadPool>

class Coherence : public QObject, public mglDraw
{
    Q_OBJECT
private:
    int kolvoProc;  //количество потоков для распрараллеливания
    int kolvoProcDAQ; //то же самое для sendRay
    QByteArray nameProj;

    int flagNumCap;                                     //999 номер капилляра, которым захватился луч
    QVector<double> pointRsou;                          //999
    QVector<double> pointsX, pointsY, pointsZ;          //888 контейнеры с точками отражения, первая точка - положение точечного источника
    QVector<double> pointsXscr, pointsYscr, pointsZscr; //999 точка прихода одного луча для всех экранов
    double directingVectorLine[numOfVar];               //999 направляющий вектор прямой
    QVector<double> anglesGlacing;                      //888 углы отражения только на капилляре
    QVector<double> anglesScrPhi,anglesScrTheta;        //999 углы в точке прихода луча на экране для всех экранов
    QVector<double> lengthOfRay;                        //888 длины путей лучей для всех экранов


    QMutex m_mutex; //первый инт - номер эксперимента:
    QMap< long int, QList< QVector<double> > > mapResultRay; //замещаем то, что выше на это... постепенно переходя к многопоточному методу!!!!!!!

    // 999 - с новым лучём обновляются сами     -> в перспективе перейти под управление
    // 888 - обновляем руками в функции перед добавлением
    // 777 - обновляем руками в основной функции в Coherence
    // ***** - параметр для мультилучевого прохождения как минимум(не надо обновлять с каждым лучом)
    // *** - как *****, только используется и для одного луча(не надо обновлять с каждым лучом)

//    double accuracy;            //*** математическая точность
    long int countRays;         //*****
    int countPointsIntegr;      //*****

    int countCapillaries;               //***
    int countScreens;                   //***
    QVector< Capillary *> capillaryVec; //*** вектор капилляров
    QVector< Screen *> screenVec;       //*** вектор экранов
    SourceAndEnvir5t *sourceEnvir;      //*** объект - источник
    MathForCap *mathforcap;             //*** объект - для точности и направления полёта луча(может быть упразднить в будущем...
    //    RandomForCap randomForCap;        // не нужно - объект класса рандом(там статические методы)
    DialogSetRangesETC *dialogranges;
    double xrang_,xrang__,yrang_,yrang__,zrang_,zrang__,rxrotate,ryrotate,rzrotate;//для рисовалки

    //* следующие параметры исключительно для каждого отдельного экрана... и будет сохраняться в файл... и удаляться перед вычислением следующего экрана*//
    //* рисовалка у же будет обрабатывать эти данный отдельно... сначала всё будет подсчитано и сохранено... потом извлечено из файла и нарисовано *//

    QVector< QVector< QVector< double > > > totalPhase; //для каждого экрана, в каждой точке на экране, начальные фазы по порядку
    QVector< QVector< QVector< double > > > totalAmpl; //для каждого экрана, в каждой точке на экране, вектор амплитуд по порядку
    QVector< QVector< QVector< double > > > totalLength;            //групп длин лучей для каждого экрана из списка... в каждой точке на экране таких групп много...
    QVector< QVector< QVector<int> > > totalNumOfRefl;
    int numOfRefl;

    QVector< QVector< QVector< QVector<double> > > > totalAngles;   //здесь для каждой длины луча - вектор скользящих углов.
    QVector< QVector< QVector< QVector<double> > > > totalPointRSou; //для каждого экрана из списка для каждой точки на экране - для каждого луча - точка на источнике
    QVector< QVector< QVector< QVector<double> > > > totalPointRScr; //для каждого экрана из списка для каждой точки на экране - для каждого луча - точка на экране
    QVector< QVector< QVector< int > > > totalNumOfCap;                          //для каждого экрана из списка для каждой точки на экране - для каждого луча - номер капилляра прохождения
    QVector< QVector<double> > totalMaxTheta;                   //здесь максимальный угол отклонения по всем лучам в точке на экране
//    QVector< QVector<double> > totalMinTheta;                   //минимальный угол отклонения по всем лучам в точке не экране - постепенно избавляюсь от этой зависимости
    QVector< QVector<double> > totalAverTheta;                  //средний угол отклонения по всем лучам в ячейке на экране
    QVector< QVector< QVector<double> > > totalScrTheta;        //все углы...все все..

    QVector< QVector< QVector< double > > > totalAverCoefOfRefl;//здесь для каждой длины луча - суммарный коэффициенты отражения для средней частоты
    //<---- нафиг надо??? используется только в старом методе, который подлежит удалению //оставим только для старого метода, который постепенно выродится
    QVector< QVector< QVector< QVector<double> > > > totalPointsX;//здесь для каждой длины луча - вектор X координат
    QVector< QVector< QVector< QVector<double> > > > totalPointsY;//здесь для каждой длины луча - вектор Y координат
    QVector< QVector< QVector< QVector<double> > > > totalPointsZ;//здесь для каждой длины луча - вектор Z координат
    //<---- совершенно не понятно зачем я их храню, они нигде, абсолютно не используются. Только занимают память //оставим только для старого метода, который постепенно выродится
    //<---- в обнулении и ресайзе пока можно оставить, но потом всё равно надо будет подчистить

    QVector<double> znc, zcRe, zcIm; //плотность распределения фотонов на эране, а так же интенсивность.
    QVector< QVector<double> > zzzRe, zzzIm, zzzNORM; //для каждого эксперимента - своя картинка когерентности.
    QVector<double> zzzTotal, zzzNORMTotal; //усреднённые картинки когерентности

    QVector<double> xgeneral,ygeneral,zgeneral,zgeneralIm; //суть первого метода вычисления когерентности

    double pointR_1_X, pointR_1_Y; //точка для которой вычисляется взимная степень когерентности
    long pointIndex, sizeForPointIndex, numberForPointIndex; //номер области с этой точкой, кол-во лучей в этой области, номер луча, который будем использовать

    // не интенсивность, а поток излучения на самом деле:
    QVector< QVector< QVector< QVector<double> > > > spectrumScreen;//для каждого r_0, для каждой точки i на экране, для каждой nu_i вектор значения спектра(для каждой точки).
    QVector< QVector<double> > spectrumTotalR_0Nu;//для каждого r_0 для каждой Nu значение
    QVector< QVector<double> > spectrumIntensScreen;//для каждого r_0, для каждой точки i на экране значение интенсивности
    QVector<double> spectrumSumIntensScreen;//для каждого r_0, суммарная интенсивность на экране
    QVector<double> spectrumSumInputIntensWithoutNonRefl;//для каждого r_0, суммарная входная интенсивность без просвета
    QVector<double> spectrumSumInputIntensWithNonRefl;//с пройденной без отражения
    int countR_0; QVector<double> tempLengthCap, tempRCap, tempr0Cap;

    //обнуление для пряников впереди написал?????????????????????
    //темповые данные:
    QVector< double > absV;         //777 модуль в квадрате спектра - бпф от нашей волновой функции
    QVector< double > nu_i,nu_i_imput;         //777 контейнер частот, соответствующие и absV
//    QVector< double > tempR_nu_i;   //888 !!! этот создаётся в методе, для указываемого рука луча после DAQ, контейнер коэф. отражения, уже собранных по всем углам для каждого конкретного луча. Контейнер потому что разбит на монохроматические составляющие
    QVector< QVector<double> > tempZcIm, tempZnc, tempZcRe;
    QVector< QVector<double> > tempzzzRe, tempzzzIm, tempzzzNORM;
//    QVector< QVector < double > > tempzzzReNorm, tempzzzImNorm;
//    QVector< QVector < double > > tempIncoh, tempGeneral, tempGeneralIm;//то же самое, что zc znc только по всем частотам
//    QVector< QVector < double > > tempBasis;
//    QVector< QVector<double> > tempZc, tempZnc;//здесь хранится для каждой точки на экране картинка когерентности для определённой частоты

    //-------------------------- темп для многопоточности
    QVector<double> nu2PiC;//здесь хранится всё, что под косинусом, исключая разности хода туда же, что и предыдущее
    //----------------------------------------------------

    void sendRay();//прогоняет один луч и создаёт:
    //    double pointsXsou,pointsYsou,pointsZsou;            //999
    //    QVector<double> pointsX, pointsY, pointsZ;          //888 контейнеры с точками отражения, первая точка - положение точечного источника
    //    QVector<double> pointsXscr, pointsYscr, pointsZscr; //999 точка прихода одного луча для всех экранов
    //    double directingVectorLine[numOfVar];   //999 направляющий вектор прямой
    //    QVector<double> anglesGlacing;                      //888 углы отражения только на капилляре
    //    QVector<double> anglesScrPhi,anglesScrTheta;        //999 углы в точке прихода луча на экране для всех экранов
    //    QVector<double> lengthOfRay;                        //888 длины путей лучей для всех экранов
    void setLengthOfRay();//считает длину луча, включено в sendRay();
    void setTotal();//включено DAQ, записывает углы и длины и средние для средней частоты для каждого луча коэф. отражения в общие итоговые массивы и делает прочее total...
    double setAverCoefRefl();//считает для setTotal, только когда есть anglesGlacing
    void setTempCoefReflNuI(QVector<double> &anglesGlacingR, QVector<double> &tempR);//вычисление R для каждой частоты для луча anglesGlacing
    void clearTotal();//включено в DAQ, обнуляет все эти вещички...

public:
    bool flagWithSetRanges;//флаг для рисования с перестановкой масштаба или без перестановки
    bool flagWithAnimation;//флаг для рисования с анимацией или без
    int flagDraw; //флаг для рисовалки - по умолчанию 0 - не рисует ничего.

    int countRaysTestDraw;//количество лучей для теста... значени по умолчанию = 0, т.к. пользователь потом выбирает сколько нужно
    int countNumOfReflections;//количество отражений для теста коэффициента отражения. по умолчанию = 0, т.к. пользователь потом выбирает сколько нужно
    int kolvoExp;//количество экспериментов для усреднения степени когерентности...
    int iterExp;//номер конкретного эксперимента

    double testDeltaNorma;//для рисовалки коэф. отражения... по умолчанию = 0, т.к. пользователь потом выбирает сколько нужно
    int flagWithConvolution; //НЕ НУЖНО!! флаг для переключения кнопок и использования соответствующего алгоритма или с конволюцией или без
    int flagWithNonReflRadiation;//учитывать ли прошедшее без отражения на картине когерентности......

    explicit Coherence(QByteArray nameProject = "", QObject *parent = 0);
    int Draw(mglGraph *gr);
    int DAQ();
    int mainIntegrationINCOHAmpl();//не нужно, не используем пока, хотя интересно покопаться... сравнить
    int mainIntegrationCOHAmpl();//туда же, что и предыдущее
    int mainIntegrationConvolution();//старый ментод, не нужен, заменили многопоточным

    void spectrumSetTor(const int &m);
    int mainSpectrumExp();

    bool tryLoadTotal() const;
    int LoadTotal(/*QByteArray sadd = ""*/);//загрузка из файла
    int LoadCoherence(/*QByteArray sadd = ""*/);
    int LoadSpectrumExp(/*QByteArray sadd = ""*/);
    int LoadDegOfCoh(/*QByteArray sadd = ""*/);

    int SaveTotal(/*QByteArray sadd = ""*/) const;//пока вызываем после mainexperiment
    int SaveIntens(/*QByteArray sadd = ""*/) const;
    int SaveSpectrumExp(/*QByteArray sadd = ""*/) const;
    int SaveDegOfCoh(/*QByteArray sadd = ""*/) const;

//    int SaveCoherence(QByteArray nameFileTotalZNC, QByteArray nameFileTotalZC, QByteArray nameFileTotalZZZ);
//    int mainIntegrationCOHConvolution();два в одном в mainIntegrationConvolution()

//   double Convolution(double &beamLength1, double &beamLength2); ????????????????????????
    inline double rectangle_integrate(double border_, double border__, int n, double beamLength1, double beamLength2);//не исопльзую нигде
    inline double rectangle_integrate(double border_, double border__, int n);//не использую нигде

    //--------------------------------------------------------------------------------------------------
    // Специльно для многопоточности:
    void setTempCoefReflNuIStaticMNOGOPOTOCHNIJ(const QVector<double> &anglesGlacingR, QVector<double> &tempR, const int &tflagNumCap);
    void threadMainIntegrConvolMNOGOPOTOCHNIJ(long &i);
    void threadPrepare(/*long & i*/);//DELETEME - в итоге этот метод не нужен...
    void threadMainIntegrConvolMNOGOPOTOCHNIJ_2(long & i); //***РАБОТАЕТ НЕ ПРАВИЛЬНО - теперь нормально работает 19.05.2013 - НЕ РАБОТАЕТ! Усреднение по области == интенсивность!
    void threadMainIntegrConvolMNOGOPOTOCHNIJ_3(long & i);
    int mainIntegrationConvolutionMNOGOPOTOCHNIJ();// ОСНОВНОЙ МЕТОД!!!!
    int mainIntegrationConvolutionMNOGOPOTOCHNIJdegOfCoh();
    int mainIntegrationConvolutionMNOGOPOTOCHNIJDegOfCoh_NEW();//соединён со ***...работает не так, как нужно.
    void setAverageDegOfCoh();


    void sendRayMNOGOPOTOCHNIJ(const long &numOfExpRay);   //возвращает номер захватываемого капилляра
    void setMapResultRay(const long &numOfExpRay);         //переносит из map в рабочие переменные, как по старому методу удобно было
    void setTotalMNOGOPOTOCHNIJ(const long &numOfExpRay); //записыват только то, что надо для дальнейшего использования, обходя старый метод
    //--------------------------------------------------------------------------------------------

signals:
    
public slots:
    void slotSetCountRaysTestDraw(int t) {countRaysTestDraw = t;}
    void slotSetRanges();
    
};
#endif // COHERENCE_H
