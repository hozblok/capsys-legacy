#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dialogtreedata.h"
#include "coherence.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    // Method for closing the mathql window.
    int closeQMathGLWindow();
    // Method for opening the mathql window and draw graphics.
    int openQMathGLWindow(const char * title = "MathGL Graph");
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
protected:
    void changeEvent(QEvent *e);
    
private slots:

public slots:
    void slotCreateDialogTreeData();
    void slotCreateCoherence();
    void slotTestDraw();
    void slotDrawAnglesScr();
    void slotDrawCoefOfRefl();

    void slotDAQ();
    void slotIntegr();//не используется... и не надо!!!
    void slotCalcIntensity();
    void slotCalcDegrOfCoh();
    void slotCalcSpectrum();

    void slotDrawInCohIntens();
    void slotDrawReDegOfCoh();
    void slotDrawAbsDegOfCoh();
    void slotDrawTotalIntensity();
    void slotDrawIntTerm();

    void slotDrawInitialSpectrum();
    void slotDrawFinalSpectrum();
    void slotCalcAndDraw();

    void slotSetCountRaysTestDraw(int val) { coherence->countRaysTestDraw = val; }
    void slotSetCountNumOfReflections(int val) { coherence->countNumOfReflections = val; }
    void slotSetDeltaNorma(double val) { coherence->testDeltaNorma = val; }
    void slotSetNumOfExpForAverageDegOfCoh(int val) { coherence->kolvoExp = val; }

    void slotAbout() { QMessageBox::about(0, "About", "CAPSYS\n(c) Ivan Ergunov\n\nhozblok@gmail.com\n2012-2013"); }
    void slotAboutQt() { QMessageBox::aboutQt(0); }

private:
    Ui::MainWindow *ui;
    DialogTreeData *dialogtreedata;
    Coherence *coherence;
    mglQT *mathglWindow;

    bool flagDAQ;//флаг отвечающий за "посчитано ли DAQ или не посчитано"
    void tryLoad();//попытаться загрузить данные из файлов
    void DAQdone();//когда посчитано DAQ приводит кнопки и т.п. в порядок
    void COHdone();//DELETME больше не используется

    void CalcIntensDone();
    void CalcDegOfCohDone();
    void Spectrumdone();

};

#endif // MAINWINDOW_H
