#include "dialogtreedata.h"
#include "ui_dialogtreedata.h"

DialogTreeData::DialogTreeData(QWidget *parent) :
    QDialog(parent)
//  ,settings("NRNU MEPhI","ScientificResearchWork")
  ,flagSaveOpenDefault(false)
  ,countScreens(1)//как будет по дефолту????????????? добавить дефолт....
  ,countCapillaries(1)//как будет по дефолту????????????? добавить дефолт....
  ,grWasCreated(false)
  ,ui(new Ui::DialogTreeData)
{

    ui->setupUi(this);
    testfft = new testFFT();
    testsource = new testSource();
    connect(ui->toolButtonAddCapillary,SIGNAL(clicked()),this,SLOT(slotAddCap()));
    connect(ui->toolButtonAddScreen,SIGNAL(clicked()),this,SLOT(slotAddScreen()));
    connect(ui->toolButtonRemoveCapillary,SIGNAL(clicked()),this,SLOT(slotRemoveCap()));
    connect(ui->toolButtonRemoveScreen,SIGNAL(clicked()),this,SLOT(slotRemoveScreen()));
    connect(ui->toolButton_Default,SIGNAL(clicked()),this,SLOT(slotOpenDefaultSettings()));
    connect(ui->toolButton_Ok,SIGNAL(clicked()),this,SLOT(slotOk()));
    connect(ui->toolButton_Open,SIGNAL(clicked()),this,SLOT(slotOpen()));
    connect(ui->toolButton_Save,SIGNAL(clicked()),this,SLOT(slotSave()));

    QObject::connect(ui->toolButton_testFFT,SIGNAL(clicked()),this,SLOT(slotDrawTestFFT()));
    QObject::connect(ui->toolButton_testSource,SIGNAL(clicked()),this,SLOT(slotDrawTestSourceM()));
    QObject::connect(ui->toolButton_testSourceTime,SIGNAL(clicked()),this,SLOT(slotDrawTestSourceT()));


    connect(ui->treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(setFalseFlagSaveOpenDefault()));

    this->slotPrepareForTest();

    connect(ui->treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(slotPrepareForTest()));

    ui->treeWidget->expandItem(ui->treeWidget->topLevelItem(0));
    ui->treeWidget->expandItem(ui->treeWidget->topLevelItem(1));
    ui->treeWidget->expandItem(ui->treeWidget->topLevelItem(2));
    ui->treeWidget->expandItem(ui->treeWidget->topLevelItem(3));
//    ui->treeWidget->expandAll();
//    connect(ui->toolButton_testFFT,SIGNAL(clicked()),this,SLOT(slotDrawTestFFT()));


//    connect(ui->pushButtonAddScreen,SIGNAL(clicked()),this,SLOT(slotAddScreen()));//????????????????????удалить
//    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(slotSetListItem()));//удалить
//    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(slotSetTempListItem()));//удалить
}

DialogTreeData::~DialogTreeData()
{
    delete testfft;
    delete testsource;
    if(grWasCreated)
        delete gr;
    delete ui;
}

void DialogTreeData::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/*void DialogTreeData::loadFileInConstruktor()
{
    defaultFileName = "default.xml";
    file1.setFileName(defaultFileName);
    QFileInfo tempFileInfo(file1);
//    qDebug() << file1Name << tempFileInfo.size();
    openXMLToTree(/*file1Name*);
//    qDebug() << params;
    setWidget();
            if (params.size()>0)
            {
                saveSettings();
                QDir::setCurrent(tempFileInfo.absolutePath());
                flagSaveOpenDefault = true;
                QMessageBox::information(0,QObject::trUtf8("Information"),QObject::trUtf8("Operation has successfully completed!"));
            }
            else
            {
                if(flag == 0)
                {
                    QMessageBox::warning(0,
                                         QObject::trUtf8("Warning"),
                                         QObject::trUtf8("File has not open!"),
                                         QMessageBox::Ok,
                                         QMessageBox::Ok
                                         );
                    flag = 1;
                }
            }
//            connectDomDoc(file1Name);
//            createModel();
//            setupView();
        }
    }
}*/

void DialogTreeData::createFileXML()
{

//    while (true)
//    {
        QFileDialog fd;
        fd.setAcceptMode(QFileDialog::AcceptSave);
        fd.setDefaultSuffix("xml");
        fd.setWindowTitle(QObject::trUtf8("New XML file with input data"));
//        fd.setFileMode(QFileDialog::AnyFile);
        fd.setNameFilter(QObject::trUtf8("XML files (*.xml)"));
        if (fd.exec() == QDialog::Accepted)
        {
            if (fd.selectedFiles().count() > 0)//проверить КОЛ_ВО ФАЙЛОВ ДЛЯ ВЫЗОВА!!!!!!!!!!!!!
            {
                file1Name = fd.selectedFiles().at(0);

                file1.setFileName(file1Name);
                QFileInfo tempFileInfo(file1);
                if(tempFileInfo.baseName() == "defaultData")
                {
                    file1Name.remove("ault");
                    qDebug() <<file1Name;

                    file1.setFileName(file1Name);
                }
/*                if(file1.exists())
//                {
//                    int n = QMessageBox::warning(0,
//                                           QObject::trUtf8("Warning"),
//                                           QObject::trUtf8("<b>Do you want to rewrite this file?</b>"),

//                                           QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
//                                           QMessageBox::Yes
//                                           );
//                    if (n == QMessageBox::No)
//                    {
//                        continue;
//                    }
//                    if (n == QMessageBox::Yes)
//                    {
//                    }
//                    if (n == QMessageBox::Cancel)
//                    {
//                        break;
//                    }
//                }
*/
                setWindowTitle(QObject::trUtf8("Project - ") + tempFileInfo.fileName());
                saveTreeToXML(/*file1Name*/);

                if (!params.isEmpty())
                {

                    saveSettings();

//                    qDebug() << tempFileInfo.absolutePath();
                    QDir::setCurrent(tempFileInfo.absolutePath());
                    flagSaveOpenDefault = true;
                    qDebug() << "####################################";
                    qDebug() << "####################################";
                    qDebug() << params;
                    qDebug() << "####################################";
                    qDebug() << "####################################";
                    QMessageBox::information(0,QObject::trUtf8("Information"),QObject::trUtf8("Operation has successfully completed!"));
                }
                else
                {
                    QMessageBox::warning(0,
                                         QObject::trUtf8("Warning"),
                                         QObject::trUtf8("File has not open!"),
                                         QMessageBox::Ok,
                                         QMessageBox::Ok
                                         );
                }
//                break;
            }
        }
//        else
//        {
//            break;
//        }
//    }
}

void DialogTreeData::saveTreeToXML(/*const QString &name*/)
{
    countParams = 1; //--------------------------------------------------!!!!!!!!!!сбрасываем счётчик!!!!!!!!!!!
    params.clear(); //--------------------------------------------------!!!обнуляем файл параметров!!!!!!!!!!!!!!!!!!
    doc.clear();
    QDomElement domElementTop; QDomAttr domAttrTop; QDomText domTextTop;
    QDomElement domElementChTop,domElementChTop2; QDomAttr domAttrChTop; QDomText domTextChTop;
    QDomElement domElement; QDomText domText;
    QDomElement domElement2;
    QDomElement domElementTitle = doc.createElement("Project");
    for (int k=0; k < ui->treeWidget->topLevelItemCount(); ++k)
    {
        domElementTop = doc.createElement(ui->treeWidget->topLevelItem(k)->text(0));
        domAttrTop = doc.createAttribute("itemCount");
        domAttrTop.setValue(QString::number(ui->treeWidget->topLevelItem(k)->childCount(),'f',0));
        domElementTop.setAttributeNode(domAttrTop);
//        domTextTop = doc.createTextNode(ui->treeWidget->topLevelItem(k)->text(0));
//        domElementTop.appendChild(domTextTop);
        for(int j=0; j < ui->treeWidget->topLevelItem(k)->childCount(); ++j)
        {
            //            if(ui->treeWidget->topLevelItem(k)->child(j)->childCount() != 0)
            //            {
            domElementChTop = doc.createElement("field");
            domElementChTop2 = doc.createElement("name");
            //                domElementChTop = doc.createElement(ui->treeWidget->topLevelItem(k)->child(j)->text(0).replace(" ","_"));
            domAttrChTop = doc.createAttribute("itemCount");
            domAttrChTop.setValue(QString::number(ui->treeWidget->topLevelItem(k)->child(j)->childCount(),'f',0));
            domElementChTop.setAttributeNode(domAttrChTop);
            domTextChTop = doc.createTextNode(ui->treeWidget->topLevelItem(k)->child(j)->text(0));
            domElementChTop2.appendChild(domTextChTop);
            domElementChTop.appendChild(domElementChTop2);
            for(int i=0; i < ui->treeWidget->topLevelItem(k)->child(j)->childCount(); ++i)
            {
                domElement = doc.createElement("value"+QString::number(countParams));
                domText = doc.createTextNode(ui->treeWidget->topLevelItem(k)->child(j)->child(i)->text(0));
                domElement.appendChild(domText);
                domElementChTop.appendChild(domElement);
                params.push_back(ui->treeWidget->topLevelItem(k)->child(j)->child(i)->text(0));
                countParams++;
            }

            //            domElementChTop.appendChild(domElementChTop3);
            domElementTop.appendChild(domElementChTop);


            //            }
            //            else
            //            {
            //                domElementChTop = doc.createElement("value"+QString::number(j+1,'f',0));
            //                domText = doc.createTextNode(ui->treeWidget->topLevelItem(k)->child(j)->text(0));
            //                domElementChTop.appendChild(domText);
            //                domElementTop.appendChild(domElementChTop);
            //            }
        }


        domElementTitle.appendChild(domElementTop);


    }
    domElement2 = doc.createElement("countScreens");
    domText = doc.createTextNode(QString::number(countScreens));
    domElement2.appendChild(domText);
    domElementTitle.appendChild(domElement2);
    domElement2 = doc.createElement("countCapillaries");
    domText = doc.createTextNode(QString::number(countCapillaries));
    domElement2.appendChild(domText);
    domElementTitle.appendChild(domElement2);
    doc.appendChild(domElementTitle);


    if(file1.open(QIODevice::WriteOnly))
    {
        QTextStream(&file1) << doc.toString();
        file1.close();
    }
}

void DialogTreeData::saveSettings()
{
    settings.setValue("CurrentSettings/x_0_",params[0]);
    settings.setValue("CurrentSettings/y_0_",params[1]);
    settings.setValue("CurrentSettings/z_0_",params[2]);
    settings.setValue("CurrentSettings/x_0__",params[3]);
    settings.setValue("CurrentSettings/y_0__",params[4]);
    settings.setValue("CurrentSettings/z_0__",params[5]);
    settings.setValue("CurrentSettings/fSource",params[6]);
    settings.setValue("CurrentSettings/phi_0_",params[7]);
    settings.setValue("CurrentSettings/theta_0_",params[8]);
    settings.setValue("CurrentSettings/phi_0__",params[9]);
    settings.setValue("CurrentSettings/theta_0__",params[10]);
    settings.setValue("CurrentSettings/averageFrequancy",params[11]);
    settings.setValue("CurrentSettings/FRe",params[12]);
    settings.setValue("CurrentSettings/FIm",params[13]);
    settings.setValue("CurrentSettings/fAmpl",params[14]);
    settings.setValue("CurrentSettings/tau",params[15]);
    settings.setValue("CurrentSettings/fPhase",params[16]);
    settings.setValue("CurrentSettings/fScopePhase",params[17]);
    for (int i = 0; i < countCapillaries; ++i)
    {
        settings.setValue("CurrentSettings/f0_"+QString::number(i),params[18+i]);
        settings.setValue("CurrentSettings/fBottomOfCap_"+QString::number(i),params[18+countCapillaries+i]);
        settings.setValue("CurrentSettings/fEndOfCap_"+QString::number(i),params[18+2*countCapillaries+i]);
        settings.setValue("CurrentSettings/plasmaFrequency_"+QString::number(i),params[18+3*countCapillaries+i]);
        settings.setValue("CurrentSettings/epsilonCapIm_"+QString::number(i),params[18+4*countCapillaries+i]);
    }
    for (int i = 0; i < countScreens; ++i)
    {
        settings.setValue("CurrentSettings/fScreen_"+QString::number(i),params[18 + 5*countCapillaries + i]);
        settings.setValue("CurrentSettings/edgeOfScreenSquare_"+QString::number(i),params[18 + 5*countCapillaries + countScreens + i]);
        settings.setValue("CurrentSettings/meshScreenNorm_"+QString::number(i),params[18 + 5*countCapillaries + 2*countScreens + i]);
        settings.setValue("CurrentSettings/xShift_"+QString::number(i),QString::number(0.0)); //???????
        settings.setValue("CurrentSettings/yShift_"+QString::number(i),QString::number(0.0)); //???????
    }
    settings.setValue("CurrentSettings/accurancy",params[18 + 5*countCapillaries + 3*countScreens]);
    settings.setValue("CurrentSettings/epsilonEnvirRe",params[18 + 5*countCapillaries + 3*countScreens + 1]);
    settings.setValue("CurrentSettings/epsilonEnvirIm",params[18 + 5*countCapillaries + 3*countScreens + 2]);
    settings.setValue("CurrentSettings/countRays",params[18 + 5*countCapillaries + 3*countScreens + 3]);
    settings.setValue("CurrentSettings/countPointsIntegr",params[18 + 5*countCapillaries + 3*countScreens + 4]);
    settings.setValue("CurrentSettings/rayGoesUp",params[18 + 5*countCapillaries + 3*countScreens + 5]);
//    settings.setValue("CurrentSettings/withConvolution",params[18 + 5*countCapillaries + 3*countScreens + 6]);
    settings.setValue("CurrentSettings/pointDegOfCohX",params[18 + 5*countCapillaries + 3*countScreens + 6]);
    settings.setValue("CurrentSettings/pointDegOfCohY",params[18 + 5*countCapillaries + 3*countScreens + 7]);
    settings.setValue("CurrentSettings/withNonReflRadiation",params[18 + 5*countCapillaries + 3*countScreens + 8]);
    settings.setValue("CurrentSettings/lengthUnitMult",params[18 + 5*countCapillaries + 3*countScreens + 9]);

    settings.setValue("CurrentSettings/countScreens",ui->spinBoxCountScreens->value());
    settings.setValue("CurrentSettings/countCapillaries",ui->spinBoxCountCapillaries->value());

}

void DialogTreeData::openFileXML(QByteArray fName)
{
    if(fName == "")
    {
        bool flag = 0;//флаг для того, чтобы при ошибке не выскакивал варнинг бесконечное число раз
        QFileDialog fd;
        fd.setDefaultSuffix("xml");
        fd.setWindowTitle(QObject::trUtf8("Open input data"));
        fd.setFileMode(QFileDialog::AnyFile);
        fd.setNameFilter(QObject::trUtf8("XML files (*.xml)"));
        if (fd.exec() == QDialog::Accepted) {
            if (fd.selectedFiles().count() > 0) {
                file1Name = fd.selectedFiles().at(0);
                file1.setFileName(file1Name);
                QFileInfo tempFileInfo(file1);
                qDebug() << file1Name << tempFileInfo.size();
                setWindowTitle(QObject::trUtf8("Project - ") + tempFileInfo.fileName());
                openXMLToTree(/*file1Name*/);
                qDebug() << params;
                setWidget();
                if (params.size()>0)
                {
                    saveSettings();
                    QDir::setCurrent(tempFileInfo.absolutePath());
                    flagSaveOpenDefault = true;
                    QMessageBox::information(0,QObject::trUtf8("Information"),QObject::trUtf8("Operation has successfully completed!"));
                }
                else
                {
                    if(flag == 0)
                    {
                        QMessageBox::warning(0,
                                             QObject::trUtf8("Warning"),
                                             QObject::trUtf8("File has not open!"),
                                             QMessageBox::Ok,
                                             QMessageBox::Ok
                                             );
                        flag = 1;
                    }
                }
                //            connectDomDoc(file1Name);
                //            createModel();
                //            setupView();
            }
        }
    }
    else
    {
        file1.setFileName(fName.data());
//        QFileInfo tempFileInfo(file1);
        //                qDebug() << file1Name << tempFileInfo.size();
        openXMLToTree(/*file1Name*/);
        qDebug() << params;
        if (params.size()>0)
        {
            saveSettings();
            flagSaveOpenDefault = true;
            //                    QDir::setCurrent(tempFileInfo.absolutePath());
            //                    flagSaveOpenDefault = true;
            //                    QMessageBox::information(0,QObject::trUtf8("Information"),QObject::trUtf8("Operation has successfully completed!"));
        }
    }
}

void DialogTreeData::openXMLToTree(/*const QString &name*/)
{
    QDomDocument domDoc;
    if(file1.open(QIODevice::ReadOnly))
    {
        if(domDoc.setContent(&file1))
        {
            countParams = 1; //--------------------------------------------------!!!!!!!!!!сбрасываем счётчик!!!!!!!!!!!->проверка дальше
            params.clear();
            QDomElement domElement = domDoc.documentElement();
            traverseNode(domElement);
        }
        file1.close();
    }

/* //это другой способ перебрать элементы XML
    if(file1.open(QIODevice::ReadOnly))
    {
        QXmlStreamReader sr(&file1);
        do{
            sr.readNext();
            qDebug() << sr.tokenString() << sr.name() << sr.text() << sr.prefix();
        } while (!sr.atEnd());
        if(sr.hasError())
        {
            qDebug() << "Error" << sr.errorString();
            sr.readNext();
        }
//        file1.reset();
//        qDebug() << file1.readAll();
        file1.close();
    }*/
}

void DialogTreeData::traverseNode(const QDomNode& node)
{
    QDomNode node1 = node.firstChild();
    while(!node1.isNull())
    {
        if(node1.isElement())
        {
            QDomElement domElement = node1.toElement();
            if(!domElement.isNull())
            {
                if(domElement.tagName() == ("value"+QString::number(countParams,'f',0)))
                {
                    params.push_back(domElement.text());
                    countParams++;
                }
                if(domElement.tagName() == ("countScreens"))
                {
                    countScreensTempFile = domElement.text().toInt();
                    qDebug() << "!Number of screens:" << countScreensTempFile;
                }
                if(domElement.tagName() == ("countCapillaries"))
                {
                    countCapillariesTempFile = domElement.text().toInt();
                    qDebug() << "!Number of capillaries:" << countCapillariesTempFile;
                }
/*Отладочная проверка
                if(domElement.tagName() == "value4") {
                    qDebug() << "Attr: "
                             << node.toElement().attribute("number","");
                    qDebug() << "TagName: " << domElement.tagName()
                             << "\tText: " << domElement.text();
                }
                else {
                    qDebug() << "TagName: " << domElement.tagName()
                             << "\tText: " << domElement.text();
                }*/
            }

        }
        traverseNode(node1);
        node1 = node1.nextSibling();
    }

}

void DialogTreeData::setWidget()
{
    int tempCount = 0;
    qDebug() << ui->spinBoxCountScreens->value() << countScreensTempFile;
    while(ui->spinBoxCountScreens->value() < countScreensTempFile)
        this->slotAddScreen();
    while(ui->spinBoxCountScreens->value() > countScreensTempFile)
        if(this->slotRemoveScreen() != 0)
            break;
    while(ui->spinBoxCountCapillaries->value() < countCapillariesTempFile)
        this->slotAddCap();
    while(ui->spinBoxCountCapillaries->value() > countCapillariesTempFile)
        if(this->slotRemoveCap() != 0)
            break;
//    qDebug() << "**************" << (countScreens)*3 << (countCapillaries)*5 << 20 + (countCapillaries)*5 + (countScreens)*3 << "!!!!=="<<countParams;
    for (int i=0; i < ui->treeWidget->topLevelItemCount(); ++i)
    for (int j=0; j < ui->treeWidget->topLevelItem(i)->childCount(); ++j)
    for (int k=0; k < ui->treeWidget->topLevelItem(i)->child(j)->childCount(); ++k)
    {
        ui->treeWidget->topLevelItem(i)->child(j)->child(k)->setText(0,params[tempCount]);
        ++tempCount;
    }
//    }
}

/*
void DialogTreeData::openXML()
{
    if(file1.open(QIODevice::ReadOnly)) {
        if(domDoc1.setContent(&file1)) {
            QDomElement domElement = domDoc1.documentElement();
            traverseNode(domElement);
        }
        file1.close();
    }
}

void DialogTreeData::slotSaveXML()
{
    if(file1.open(QIODevice::ReadOnly)) {
        if(domDoc1.setContent(&file1)) {
            QDomElement domElement = domDoc1.documentElement();
            traverseNode(domElement);
        }
        file1.close();
    }
}*/

void DialogTreeData::slotAddScreen()
{
//    QTreeWidgetItem * topItem22 = ui->treeWidget->takeTopLevelItem(22);
//    QTreeWidgetItem * item22 = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("z-200.0")));
//    item22->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
//   // item->setFlags(Qt::ItemIsEditable);
//    topItem22->addChild(item22);
//    ui->treeWidget->insertTopLevelItem(22, topItem22);

//    QTreeWidgetItem * topItem23 = ui->treeWidget->takeTopLevelItem(23);
//    QTreeWidgetItem * item23 = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("200.0")));
//    item23->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
//   // item->setFlags(Qt::ItemIsEditable);
//    topItem23->addChild(item23);
//    ui->treeWidget->insertTopLevelItem(23, topItem23);

//    ui->spinBoxCountScreens->setValue(++countScreens);

//    for(int i=0; i<ui->treeWidget->topLevelItem(2)->childCount(); ++i)
    QTreeWidgetItem * item0 = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("")));
    item0->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
    ui->treeWidget->topLevelItem(2)->child(0)->addChild(item0);
    QTreeWidgetItem * item1 = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("")));
    item1->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
    ui->treeWidget->topLevelItem(2)->child(1)->addChild(item1);
    QTreeWidgetItem * item2 = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("")));
    item2->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
    ui->treeWidget->topLevelItem(2)->child(2)->addChild(item2);
    ui->spinBoxCountScreens->setValue(++countScreens);
    flagSaveOpenDefault = false;
}

int DialogTreeData::slotRemoveScreen()
{
    qDebug() << countScreens;
    if(countScreens > 1)
    {
        for(int i=0; i < ui->treeWidget->topLevelItem(2)->childCount(); ++i)
            ui->treeWidget->topLevelItem(2)->child(i)->removeChild(ui->treeWidget->topLevelItem(2)->child(i)->child(countScreens-1));
        ui->spinBoxCountScreens->setValue(--countScreens);
        flagSaveOpenDefault = false;
    }
    else
    {
        QMessageBox::information(0,QObject::trUtf8("Information"),QObject::trUtf8("One screen should be set!"));
        return -1;
    }
    return 0;
}

void DialogTreeData::slotAddCap()
{
    QTreeWidgetItem * item0 = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("")));
    item0->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
    ui->treeWidget->topLevelItem(1)->child(0)->addChild(item0);
    QTreeWidgetItem * item1 = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("")));
    item1->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
    ui->treeWidget->topLevelItem(1)->child(1)->addChild(item1);
    QTreeWidgetItem * item2 = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("")));
    item2->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
    ui->treeWidget->topLevelItem(1)->child(2)->addChild(item2);
    QTreeWidgetItem * item3 = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("")));
    item3->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
    ui->treeWidget->topLevelItem(1)->child(3)->addChild(item3);
    QTreeWidgetItem * item4 = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("")));
    item4->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
    ui->treeWidget->topLevelItem(1)->child(4)->addChild(item4);
    ui->spinBoxCountCapillaries->setValue(++countCapillaries);
    flagSaveOpenDefault = false;

}

int DialogTreeData::slotRemoveCap()
{
    if(countCapillaries > 1)
    {
        for(int i=0; i < ui->treeWidget->topLevelItem(1)->childCount(); ++i)
            ui->treeWidget->topLevelItem(1)->child(i)->removeChild(ui->treeWidget->topLevelItem(1)->child(i)->child(countCapillaries-1));
        ui->spinBoxCountCapillaries->setValue(--countCapillaries);
        flagSaveOpenDefault = false;
    }
    else
    {
        QMessageBox::information(0,QObject::trUtf8("Information"),QObject::trUtf8("One capillary should be set!"));
        return -1;
    }
    return 0;
}

//void DialogTreeData::slotSetListItem()
//{
//    listItemTreeModel.clear();
//    templistItemTreeModel.clear();
//    for (int i=0;i<numOfGivenParam;++i)
//    {
//        listItemTreeModel.push_back(ui->treeWidget->topLevelItem(i));
//        templistItemTreeModel.push_back(ui->treeWidget->topLevelItem(i));
////        QVariant as = listItemTreeModel[i]->child(0)->data(0,Qt::DisplayRole);
////        qDebug() << "!!!!" << as <<as.toDouble();
//    }
//}

//void DialogTreeData::slotSetTempListItem()
//{
//    //????????????????????????Дописать
//}

void DialogTreeData::slotOpen()
{
    openFileXML();
}

void DialogTreeData::slotOpen(QByteArray fName)
{
    openFileXML(fName);
}

void DialogTreeData::slotSave()
{
    createFileXML();
}

void DialogTreeData::slotOpenDefaultSettings()
{
//    QDir::setCurrent(":/data/");
    file1Name = ":/data/data/defaultData.xml";
    file1.setFileName(file1Name);
    openXMLToTree(/*file1Name*/);
    setWidget();
    if (params.size()>0)
    {
        saveSettings();
        flagSaveOpenDefault = true;
        QMessageBox::information(0,QObject::trUtf8("Information"),QObject::trUtf8("Operation has successfully completed!"));
    }
    else
    {
        QMessageBox::warning(0,
                             QObject::trUtf8("Warning"),
                             QObject::trUtf8("File has not open!"),
                             QMessageBox::Ok,
                             QMessageBox::Ok
                             );
    }
}

void DialogTreeData::slotOk()
{
    if(flagSaveOpenDefault == false)
    {
        int n = QMessageBox::question(0,
                                      QObject::trUtf8("Question"),
                                      QObject::trUtf8("<b>Do you want to save this parameters?</b>"),

                                      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                      QMessageBox::Yes
                                      );
        if (n == QMessageBox::No)
        {
            this->close();
        }
        if (n == QMessageBox::Yes)
        {
            this->slotSave();
            this->slotOk();
        }
        if (n == QMessageBox::Cancel)
        {

        }
    }
    else
    {
        this->close();
    }
//    QMessageBox *pmbx = new QMessageBox(QMessageBox::Information,
//                           "MessageBox",
//                           "<b>Do you want to save this changes?</b>",
//                           QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
//                           );
//    int n = pmbx->exec();
//    delete pmbx;
//    if (n != QMessageBox::Yes)
//    {
//        continue;
//    }
}

void DialogTreeData::slotPrepareForTest()
{
//    int flagConvol = QString(ui->treeWidget->topLevelItem(3)->child(5)->child(0)->text(0)).toInt();
//    if(flagConvol == 1)
//    {
//        ui->frame_2->setEnabled(true);
//    }
//    else
//    {
//        ui->frame_2->setEnabled(false);
//    }//попробуем перейти к одному единственному методу с convolution

    QByteArray FRe = ui->treeWidget->topLevelItem(0)->child(6)->child(0)->text(0).toLocal8Bit();
    QByteArray FIm = ui->treeWidget->topLevelItem(0)->child(6)->child(1)->text(0).toLocal8Bit();

    double tau = ui->treeWidget->topLevelItem(0)->child(8)->child(0)->text(0).toDouble();
    int n = ui->treeWidget->topLevelItem(3)->child(3)->child(0)->text(0).toInt();
    double AverNu = ui->treeWidget->topLevelItem(0)->child(5)->child(0)->text(0).toDouble();
    double tauF = n/(4*AverNu);

//    if(flagConvol == 1)
//    {
        while((tauF < tau)&&(n!=0))
        {
            n = 2*n; //увеличиваем частоту Найквиста
            tauF = n/(4*AverNu);
            ui->treeWidget->topLevelItem(3)->child(3)->child(0)->setText(0,QString::number(n));
            if( n > 32768 )
            {
                ui->treeWidget->topLevelItem(3)->child(3)->child(0)->setText(0,QString::number(0));//если ноль, то спектр вообще состоит из одной точки
            }
        }
        if( n > 32768 )
        {
            ui->treeWidget->topLevelItem(3)->child(3)->child(0)->setText(0,QString::number(0));//если ноль, то спектр вообще состоит из одной точки
//            ui->frame_2->setEnabled(false);
        }
//    }

    ui->lineEditFRE->setText(FRe.data());
    ui->lineEditFIM->setText(FIm.data());
    ui->lineEditAverNu->setText(QString::number(AverNu));
    ui->lineEditTau->setText(QString::number(tau));
    ui->spinBoxN->setValue(n);
    ui->lineEditTauF->setText(QString::number(tauF));

    //отдельно, но сюда же...:
    ui->spinBox_units->setValue(ui->treeWidget->topLevelItem(3)->child(7)->child(0)->text(0).toInt());

    this->slotSetTreeLengthUnit(ui->spinBox_units->value());
}

void DialogTreeData::slotSetTreeLengthUnit(int tVal)
{
    ui->treeWidget->topLevelItem(3)->child(7)->child(0)->setText(0,QString::number(tVal));
}

void DialogTreeData::slotDrawTestFFT()
{
//    static double AverNu; //nHz
//    static double tau;
//    static int n;
//    static double tauF;
//    static double dNu; //nHz

    testfft->AverNu = ui->lineEditAverNu->text().toDouble();
    testfft->tau = ui->lineEditTau->text().toDouble();
    testfft->n = ui->spinBoxN->value();
    testfft->tauF = ui->lineEditTauF->text().toDouble();
    testfft->FRE = ui->lineEditFRE->text().toLocal8Bit();
    testfft->FIM = ui->lineEditFIM->text().toLocal8Bit();
//    qDebug() << ui->spinBoxN->value();

    gr = new mglQT(testfft,"Test");
    grWasCreated = true;
//    return gr->Run();
}

void DialogTreeData::slotDrawTestSourceM()
{
    testsource->flagDraw = 0;

    testsource->x_0_ = ui->treeWidget->topLevelItem(0)->child(0)->child(0)->text(0).toDouble();
    testsource->y_0_ = ui->treeWidget->topLevelItem(0)->child(0)->child(1)->text(0).toDouble();
    testsource->z_0_ = ui->treeWidget->topLevelItem(0)->child(0)->child(2)->text(0).toDouble();
    testsource->x_0__ = ui->treeWidget->topLevelItem(0)->child(1)->child(0)->text(0).toDouble();
    testsource->y_0__ = ui->treeWidget->topLevelItem(0)->child(1)->child(1)->text(0).toDouble();
    testsource->z_0__ = ui->treeWidget->topLevelItem(0)->child(1)->child(2)->text(0).toDouble();
    testsource->fSource = ui->treeWidget->topLevelItem(0)->child(2)->child(0)->text(0).toLocal8Bit();
    testsource->fAmpl = ui->treeWidget->topLevelItem(0)->child(7)->child(0)->text(0).toLocal8Bit();
    testsource->fPhase = ui->treeWidget->topLevelItem(0)->child(9)->child(0)->text(0).toLocal8Bit();
    testsource->fScopePhase = ui->treeWidget->topLevelItem(0)->child(10)->child(0)->text(0).toLocal8Bit();

    testsource->numOfPointsForTest = ui->spinBox_testSource->value();

    testsource->lengthUnitMultPower = ui->treeWidget->topLevelItem(3)->child(7)->child(0)->text(0).toInt();

    gr = new mglQT(testsource,"Test");
    grWasCreated = true;
}

void DialogTreeData::slotDrawTestSourceT()
{
    testsource->flagDraw = 1;

    testsource->x_0_ = ui->treeWidget->topLevelItem(0)->child(0)->child(0)->text(0).toDouble();
    testsource->y_0_ = ui->treeWidget->topLevelItem(0)->child(0)->child(1)->text(0).toDouble();
    testsource->z_0_ = ui->treeWidget->topLevelItem(0)->child(0)->child(2)->text(0).toDouble();
    testsource->x_0__ = ui->treeWidget->topLevelItem(0)->child(1)->child(0)->text(0).toDouble();
    testsource->y_0__ = ui->treeWidget->topLevelItem(0)->child(1)->child(1)->text(0).toDouble();
    testsource->z_0__ = ui->treeWidget->topLevelItem(0)->child(1)->child(2)->text(0).toDouble();
    testsource->fSource = ui->treeWidget->topLevelItem(0)->child(2)->child(0)->text(0).toLocal8Bit();
    testsource->fAmpl = ui->treeWidget->topLevelItem(0)->child(7)->child(0)->text(0).toLocal8Bit();
    testsource->fPhase = ui->treeWidget->topLevelItem(0)->child(9)->child(0)->text(0).toLocal8Bit();
    testsource->fScopePhase = ui->treeWidget->topLevelItem(0)->child(10)->child(0)->text(0).toLocal8Bit();

    testsource->numOfPointsForTest = ui->spinBox_testSource->value();

    testsource->lengthUnitMultPower = ui->treeWidget->topLevelItem(3)->child(7)->child(0)->text(0).toInt();

    gr = new mglQT(testsource,"Test");
    grWasCreated = true;
}

void DialogTreeData::on_lineEdit_textEdited(const QString &arg1)
{
    double temp = arg1.toDouble() /4.135667516e-9;
    ui->lineEdit_2->setText(QString::number(temp,'e',13));
    double temp2 = 299792458.0 * 4.135667516e-9 / arg1.toDouble();
    ui->lineEdit_3->setText(QString::number(temp2,'e',13));
}

void DialogTreeData::on_lineEdit_2_textEdited(const QString &arg1)
{
    double temp = arg1.toDouble() * 4.135667516e-9;
    ui->lineEdit->setText(QString::number(temp,'e',13));
    double temp2 = 299792458.0 / arg1.toDouble();
    ui->lineEdit_3->setText(QString::number(temp2,'e',13));
}

void DialogTreeData::on_lineEdit_3_textEdited(const QString &arg1)
{
    double temp = 299792458.0 * 4.135667516e-9 / arg1.toDouble() ;
    ui->lineEdit->setText(QString::number(temp,'e',13));
    double temp2 = 299792458.0 / arg1.toDouble() ;
    ui->lineEdit_2->setText(QString::number(temp2,'e',13));
}
