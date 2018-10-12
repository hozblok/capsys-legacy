#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    flagDAQ(false),
    coherence(0),
    mathglWindow(0)
{
    ui->setupUi(this);
    dialogtreedata = new DialogTreeData(this);

    QObject::connect(dialogtreedata,SIGNAL(rejected()),this,SLOT(slotCreateCoherence()));

    QObject::connect(ui->action_Quit, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(ui->toolButton_setData, SIGNAL(clicked()), this, SLOT(slotCreateDialogTreeData()));
    QObject::connect(ui->spinBox_testSys, SIGNAL(valueChanged(int)), this, SLOT(slotSetCountRaysTestDraw(int)));
    QObject::connect(ui->toolButton_testSys, SIGNAL(clicked()), this, SLOT(slotTestDraw()));
    QObject::connect(ui->toolButton_drawCoefOfRefl, SIGNAL(clicked()), this, SLOT(slotDrawCoefOfRefl()));
    QObject::connect(ui->spinBox_numOfReflections, SIGNAL(valueChanged(int)), this, SLOT(slotSetCountNumOfReflections(int)));
    QObject::connect(ui->doubleSpinBox_deltaNorma, SIGNAL(valueChanged(double)), this, SLOT(slotSetDeltaNorma(double)));
    QObject::connect(ui->toolButton_calcTheRays, SIGNAL(clicked()), this, SLOT(slotDAQ()));
    QObject::connect(ui->toolButton_drawAnglesScreen, SIGNAL(clicked()), this, SLOT(slotDrawAnglesScr()));
    QObject::connect(ui->toolButton_drawReDegOfCoh, SIGNAL(clicked()), this, SLOT(slotDrawReDegOfCoh()));
    QObject::connect(ui->toolButton_drawAbsDegOfCoh, SIGNAL(clicked()),this, SLOT(slotDrawAbsDegOfCoh()));
    QObject::connect(ui->toolButton_drawInCoher, SIGNAL(clicked()), this, SLOT(slotDrawInCohIntens()));
    QObject::connect(ui->toolButton_drawTotal, SIGNAL(clicked()),this,SLOT(slotDrawTotalIntensity()));
    QObject::connect(ui->toolButton_drawIntTerm, SIGNAL(clicked()), this, SLOT(slotDrawIntTerm()));
    QObject::connect(ui->toolButton_calcIntens, SIGNAL(clicked()), this, SLOT(slotCalcIntensity()));
    QObject::connect(ui->toolButton_calcCoher_2, SIGNAL(clicked()), this, SLOT(slotCalcDegrOfCoh()));
//    QObject::connect(ui->toolButton_calcCoher, SIGNAL(clicked()), this, SLOT(slotIntegr()));

    QObject::connect(ui->toolButton_drawInitialSpectrum, SIGNAL(clicked()), this, SLOT(slotDrawInitialSpectrum()));
    QObject::connect(ui->toolButton_calcSpectrum, SIGNAL(clicked()),this, SLOT(slotCalcSpectrum()));
    QObject::connect(ui->toolButton_drawFinalSpectrum, SIGNAL(clicked()), this, SLOT(slotDrawFinalSpectrum()));

    QObject::connect(ui->toolButton_calcAndDraw, SIGNAL(clicked()),this, SLOT(slotCalcAndDraw()));

    QObject::connect(ui->action_About, SIGNAL(triggered()), this, SLOT(slotAbout()));
    QObject::connect(ui->action_About_Qt, SIGNAL(triggered()), this, SLOT(slotAboutQt()));

}

MainWindow::~MainWindow()
{
    delete mathglWindow;
    mathglWindow = 0;
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::slotCreateDialogTreeData()
{
    dialogtreedata->setFalseFlagSaveOpenDefault();
    dialogtreedata->exec();
    //далее здесь вызывается сигнал dialogtreedata rejected() - см. выше... и переход к void MainWindow::slotCreateCoherence()
}

void MainWindow::slotCreateCoherence()
{
    if (dialogtreedata->getFlagSaveOpenDefault() == true)
    {
        QFile file1;
        file1.setFileName(dialogtreedata->file1Name);
        QFileInfo tempFileInfo(file1);
        if(coherence != 0)
        {
            delete coherence;
            coherence = 0;
        }
        coherence = new Coherence(tempFileInfo.completeBaseName().toLocal8Bit(), this);

        ui->toolButton_calcTheRays->setEnabled(true);
        QIcon icon_;
        icon_.addFile(QString::fromUtf8(":/images/Warning"), QSize(), QIcon::Normal, QIcon::Off);
        ui->toolButton_calcTheRays->setIcon(icon_);

        ui->spinBox_testSys->setEnabled(true);
        this->slotSetCountRaysTestDraw(ui->spinBox_testSys->value());
        ui->toolButton_testSys->setEnabled(true);

        ui->spinBox_numOfExp->setEnabled(true);
        ui->spinBox_numOfReflections->setEnabled(true);
        this->slotSetCountNumOfReflections(ui->spinBox_numOfReflections->value());
        ui->doubleSpinBox_deltaNorma->setEnabled(true);
        this->slotSetDeltaNorma(ui->doubleSpinBox_deltaNorma->value());
        ui->toolButton_drawCoefOfRefl->setEnabled(true);

//        if(coherence->flagWithConvolution == 1)
//        {
//            ui->toolButton_calcCoher->setEnabled(false);
            ui->toolButton_calcCoher_2->setEnabled(true);
//        }
//        else
//        {
//            ui->toolButton_calcCoher->setEnabled(true);
//            ui->toolButton_calcCoher_2->setEnabled(false);
//        }
//        ui->toolButton_calcCoher->setIcon(icon_);
        ui->toolButton_calcIntens->setEnabled(true);
        ui->toolButton_calcIntens->setIcon(icon_);
        ui->toolButton_calcCoher_2->setIcon(icon_);

        ui->toolButton_drawInCoher->setEnabled(false);
        ui->toolButton_drawReDegOfCoh->setEnabled(false);
        ui->toolButton_drawAbsDegOfCoh->setEnabled(false);
        ui->toolButton_drawTotal->setEnabled(false);
        ui->toolButton_drawIntTerm->setEnabled(false);

        ui->toolButton_drawFinalSpectrum->setEnabled(false);
//        ui->toolButton_drawInitialSpectrum->setEnabled(false);
        ui->toolButton_calcSpectrum->setEnabled(true);

        flagDAQ = false;

        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/Accepted"), QSize(), QIcon::Normal, QIcon::Off);
        ui->toolButton_setData->setIcon(icon);

        if(coherence->tryLoadTotal())
        {
            int qmb = QMessageBox::question(0,
                                            QObject::trUtf8("Question"),
                                            QObject::trUtf8("<b>The program has found save data.\nDo you want to open these data?</b>"),

                                            QMessageBox::Yes | QMessageBox::No,
                                            QMessageBox::Yes
                                            );
            if (qmb == QMessageBox::No)
            {
            }
            if (qmb == QMessageBox::Yes)
            {
                this->tryLoad();
            }
            if (qmb == QMessageBox::Cancel)
            {
//                break;
            }
            //        QMessageBox msgBox;
            //        msgBox.setIcon(QMessageBox::Question);
            //        msgBox.setText("The document has been modified.");
            //        msgBox.setInformativeText("Do you want to save your changes?");
            //        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            //        msgBox.setDefaultButton(QMessageBox::Save);
            //         int ret = msgBox.exec();
        }
        else
        {

        }
    }
}

void MainWindow::tryLoad()
{
    if(coherence->LoadTotal() == 0)
    {
        this->DAQdone();
        if(coherence->LoadCoherence() == 0)
        {
            if(coherence->flagWithConvolution == 0)
            {
                this->COHdone();
            }
            else
            {
                this->CalcIntensDone();
            }
        }
        if(coherence->LoadDegOfCoh() == 0)
        {
            this->CalcDegOfCohDone();
        }
        if(coherence->LoadSpectrumExp() == 0)
        {
            this->Spectrumdone();
        }
    }
}

int MainWindow::closeQMathGLWindow()
{
    if (mathglWindow != 0) {
        // Hide main window.
        this->lower();
        // Update and Show mathql widget.
        // Unfortunately, mathgl does not give us pointer to QMainWindow (or QMathGL) inside mglQT.
        mathglWindow->Update();
        QMessageBox::StandardButton reply;
            reply = QMessageBox::question(0, "Close MathGL widget", "Are you sure you want to close the window with 2d/3d plot?",
                QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            // Delete unwaned widget.
            delete mathglWindow;
            mathglWindow = 0;
        } else {
            return -1;
        }
    }
    return 0;
}

int MainWindow::openQMathGLWindow(const char * title)
{
    if (mathglWindow != 0) {
        return -1;
    }
    // Show new mathgl window.
    mathglWindow = new mglQT(coherence, title);
    return mathglWindow->Run();
}

void MainWindow::slotTestDraw()
{
    if (this->closeQMathGLWindow()) {
        return;
    }
    coherence->flagDraw = 0;
    this->openQMathGLWindow();
}

void MainWindow::slotDrawAnglesScr()
{
    if (this->closeQMathGLWindow()) {
        return;
    }
    if(flagDAQ == false)
    {
        this->slotDAQ();
    }
    coherence->flagDraw = 3;
    coherence->flagWithSetRanges = ui->checkBox_withSetRanges->isChecked();
    this->openQMathGLWindow();
}

void MainWindow::slotDrawCoefOfRefl()
{
    if (this->closeQMathGLWindow()) {
        return;
    }
    coherence->flagDraw = 4;
    coherence->flagWithAnimation = ui->checkBox_withAnimation->isChecked();
    this->openQMathGLWindow();
}

void MainWindow::DAQdone()
{
    flagDAQ = true;
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/Accepted"), QSize(), QIcon::Normal, QIcon::Off);
    ui->toolButton_calcTheRays->setIcon(icon);
    ui->toolButton_drawAnglesScreen->setEnabled(true);
}

void MainWindow::slotDAQ()
{
//    ui->centralWidget->setEnabled(false);
    this->setEnabled(false);
    if(coherence->DAQ() == 0)
    {
        coherence->SaveTotal();
        this->DAQdone();
    }
    this->setEnabled(true);
    QIcon icon_;
    icon_.addFile(QString::fromUtf8(":/images/Warning"), QSize(), QIcon::Normal, QIcon::Off);
    ui->toolButton_calcIntens->setIcon(icon_);
    ui->toolButton_calcCoher_2->setIcon(icon_);
//    ui->centralWidget->setEnabled(true);
}

void MainWindow::COHdone()//DELETEME
{
    ui->toolButton_drawReDegOfCoh->setEnabled(true);
    ui->toolButton_drawAbsDegOfCoh->setEnabled(true);
    ui->toolButton_drawInCoher->setEnabled(true);
    ui->toolButton_drawTotal->setEnabled(true);
    ui->toolButton_drawIntTerm->setEnabled(true);
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/Accepted"), QSize(), QIcon::Normal, QIcon::Off);
//    ui->toolButton_calcCoher->setIcon(icon);
}

void MainWindow::CalcIntensDone()
{
    ui->toolButton_drawInCoher->setEnabled(true);
    ui->toolButton_drawTotal->setEnabled(true);
//    ui->toolButton_drawIntTerm->setEnabled(true);
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/Accepted"), QSize(), QIcon::Normal, QIcon::Off);
    ui->toolButton_calcIntens->setIcon(icon);
}

void MainWindow::CalcDegOfCohDone()
{
    ui->toolButton_drawReDegOfCoh->setEnabled(true);//УБРАТЬ!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ui->toolButton_drawAbsDegOfCoh->setEnabled(true);

//    ui->toolButton_drawTotal->setEnabled(true);

    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/Accepted"), QSize(), QIcon::Normal, QIcon::Off);
    ui->toolButton_calcCoher_2->setIcon(icon);
}

void MainWindow::Spectrumdone()
{
    ui->toolButton_drawInitialSpectrum->setEnabled(true);
    ui->toolButton_drawFinalSpectrum->setEnabled(true);
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/Accepted"), QSize(), QIcon::Normal, QIcon::Off);
    ui->toolButton_calcSpectrum->setIcon(icon);
}

void MainWindow::slotIntegr()//старый и уже не нужный никому метод...
{
    if(flagDAQ == 0)
    {
        this->slotDAQ();
    }
    coherence->mainIntegrationINCOHAmpl();
    coherence->mainIntegrationCOHAmpl();
    coherence->SaveIntens();
    this->COHdone();
}

void MainWindow::slotCalcIntensity()
{
    if(!flagDAQ)
    {
        this->slotDAQ();
    }
//    coherence->mainIntegrationConvolution();
    if(flagDAQ)
    {
        this->setEnabled(false);
        if (coherence->mainIntegrationConvolutionMNOGOPOTOCHNIJ() == 0)
        {
            coherence->SaveIntens();
//            coherence->SaveDegOfCoh();
            this->CalcIntensDone();
        }
        this->setEnabled(true);
    }
}

void MainWindow::slotCalcDegrOfCoh()
{
    if(!flagDAQ)
    {
        this->slotDAQ();
    }
//    coherence->mainIntegrationConvolution();
    if(flagDAQ)//зачем?
    {
        this->setEnabled(false);
        coherence->kolvoExp = ui->spinBox_numOfExp->value();
        if (coherence->mainIntegrationConvolutionMNOGOPOTOCHNIJDegOfCoh_NEW() == 0)
        {
            coherence->setAverageDegOfCoh();
            coherence->SaveDegOfCoh();
            this->CalcDegOfCohDone();
        }
        this->setEnabled(true);
    }
}

void MainWindow::slotCalcSpectrum()
{
    if(!flagDAQ)
    {
        this->slotDAQ();
    }

    this->setEnabled(false);
    if (coherence->mainSpectrumExp() == 0)//здесь весь основной счёт
    {
        this->Spectrumdone();//просто включаем кнопки
        this->slotDrawFinalSpectrum();//просто рисовалка
    }
    this->setEnabled(true);
}

void MainWindow::slotDrawInCohIntens()
{
    if (this->closeQMathGLWindow()) {
        return;
    }
    coherence->flagDraw = 1;
    coherence->flagWithSetRanges = ui->checkBox_withSetRanges->isChecked();
    this->openQMathGLWindow();
}

void MainWindow::slotDrawReDegOfCoh()
{
    if (this->closeQMathGLWindow()) {
        return;
    }
    coherence->flagDraw = 2;
    coherence->flagWithSetRanges = ui->checkBox_withSetRanges->isChecked();
    this->openQMathGLWindow();
}

void MainWindow::slotDrawAbsDegOfCoh()
{
    if (this->closeQMathGLWindow()) {
        return;
    }
    coherence->flagDraw = 9;
    coherence->flagWithSetRanges = ui->checkBox_withSetRanges->isChecked();
    this->openQMathGLWindow();
}

void MainWindow::slotDrawTotalIntensity()
{
    if (this->closeQMathGLWindow()) {
        return;
    }
    coherence->flagDraw = 5;
    coherence->flagWithSetRanges = ui->checkBox_withSetRanges->isChecked();
    this->openQMathGLWindow();
}

void MainWindow::slotDrawIntTerm()
{
    if (this->closeQMathGLWindow()) {
        return;
    }
    coherence->flagDraw = 8;
    coherence->flagWithSetRanges = ui->checkBox_withSetRanges->isChecked();
    this->openQMathGLWindow();
}

void MainWindow::slotDrawInitialSpectrum()
{
    if (this->closeQMathGLWindow()) {
        return;
    }
    coherence->flagDraw = 70;
    this->openQMathGLWindow();
}

void MainWindow::slotDrawFinalSpectrum()
{
    if (this->closeQMathGLWindow()) {
        return;
    }
    coherence->flagDraw = 71;
    this->openQMathGLWindow();

}

void MainWindow::slotCalcAndDraw()
{
    qDebug() << "Inception:";
    QDir::setCurrent(DEBUGNAMEFOLDER);
    for(int i = DEBUGINCEPTIONIMG; i<=DEBUGCOUNTIMG; ++i)//то же подправить в методе рисования
    {
        QByteArray a = QByteArray::number(i)+".xml";
        dialogtreedata->file1Name = a;
        dialogtreedata->openFileXML(a.data());
        this->slotCreateCoherence();

        qDebug() << "Number of experiment" << i;
        flagDAQ = 0;
        this->slotCalcDegrOfCoh();
        this->slotCalcIntensity();

        // TODO
//        coherence->flagDraw = 11;
//        if( i == DEBUGINCEPTIONIMG )
//        {
//            mglwindowtest = new mglQT(coherence,"MathGL Graph");
//        }
//        else
//        {
//            qDebug() << "update";
//            mglwindowtest->Update();
//        }

//        mglwindowtest->DelFrame();
//        mglwindowtest->Update();
//        mglwindowtest->~mglGraph();
    }
}
