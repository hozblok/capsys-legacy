#ifndef DIALOGTREEDATA_H
#define DIALOGTREEDATA_H


#include <QDialog>
#include <QDebug>
#include <QSettings>
#include <QTreeWidget>
#include <QList>
#include <QFileDialog>
#include <QtXml>
#include <QMessageBox>
#include "testfft.h"
#include "testsource.h"


# define M_PI_MUL2		6.28318530717958647692	/* 2pi */

namespace Ui {
class DialogTreeData;
}

class DialogTreeData : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogTreeData(QWidget *parent = 0);
    virtual ~DialogTreeData();
    QList<QString> isParams() { return params; }

    bool getFlagSaveOpenDefault() { return flagSaveOpenDefault; }
    void openFileXML(QByteArray fName = "");

    QString file1Name;//имя рабочего файла
protected:
    void changeEvent(QEvent *e);
    
private:
    QSettings settings;

    bool flagSaveOpenDefault;
    int countParams;//для подсчёта данных в файле
    QList<QString> params;//все важные входные данные по порядку
    int countScreens,countScreensTempFile;//кол-во экранов, очевидно
    int countCapillaries,countCapillariesTempFile;//кол-во капилляров, из которых состоит поликапиляр.
    Ui::DialogTreeData *ui;

    QString defaultFileName;//имя файла по умолчанию
    QString defaultFile;//сам файл по умолчанию

    QFile file1;//сам рабочий файл
    QDomDocument doc;//объект для работы с xml классами
    void createFileXML();
    void saveTreeToXML(/*const QString &name*/);
    void saveSettings();

    void openXMLToTree(/*const QString &name*/);
    void traverseNode(const QDomNode& node);
    void setWidget();

    bool grWasCreated;
    mglQT *gr;
    testFFT *testfft;
    testSource *testsource;
public slots:
    void setFalseFlagSaveOpenDefault() { flagSaveOpenDefault = false; }
    void slotSetTreeLengthUnit(int tVal);

    void slotOpen();
    void slotOpen(QByteArray fName);
    void slotSave();
    void slotOpenDefaultSettings();
    void slotOk();

    void slotAddScreen();
    int slotRemoveScreen();
    void slotAddCap();
    int slotRemoveCap();

    void slotPrepareForTest();
    void slotDrawTestFFT();
    void slotDrawTestSourceM();
    void slotDrawTestSourceT();


//    void slotSetListItem();
//    void slotSetTempListItem();
private slots:

    void on_lineEdit_textEdited(const QString &arg1);
    void on_lineEdit_2_textEdited(const QString &arg1);
    void on_lineEdit_3_textEdited(const QString &arg1);
};

#endif // DIALOGTREEDATA_H
