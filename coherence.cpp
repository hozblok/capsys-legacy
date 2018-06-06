#include "coherence.h"

Coherence::Coherence(QByteArray nameProject, QObject *parent) : QObject(parent), mglDraw()
  , kolvoProc(7)
  , kolvoProcDAQ(1)//не удалось создать многопоточность
  , flagDraw(-1)
  , countRaysTestDraw(0)
  , countNumOfReflections(0)
  , testDeltaNorma(0)
  , sizeForPointIndex(-1)
  , numberForPointIndex(-1)
  , kolvoExp(1)
  , iterExp(0)
{
    qDebug() << QObject::trUtf8("Start CAPSYS...");
    qDebug() << "Name of project:" << nameProject;
    nameProj = nameProject;

    QSettings settings;
    countScreens = settings.value("CurrentSettings/countScreens",404).toInt();
    countCapillaries = settings.value("CurrentSettings/countCapillaries",404).toInt();
    flagWithConvolution = settings.value("CurrentSettings/withConvolution",404).toInt();
    flagWithNonReflRadiation = settings.value("CurrentSettings/withNonReflRadiation",404).toInt();
    countRays = settings.value("CurrentSettings/countRays",404).toLongLong();
    countPointsIntegr = settings.value("CurrentSettings/countPointsIntegr",404).toInt();

    screenVec.clear();
    capillaryVec.clear();
    for (int i = 0; i < countScreens; ++i)
        screenVec.push_back(new Screen(i+1,this)); //одно из самых важных, создаются экраны
    for (int i = 0; i < countCapillaries; ++i)
        capillaryVec.push_back(new Capillary(i+1,this)); //одно из самых важных, создаются капилляры
    sourceEnvir = new SourceAndEnvir5t(this); //одно из самых важных, создаётся источник и среда вокруг капилляра

    //!!!!!!!!!!!!!!!!!!
    pointR_1_X = settings.value("CurrentSettings/pointDegOfCohX",404.404).toDouble();
    pointR_1_Y = settings.value("CurrentSettings/pointDegOfCohY",404.404).toDouble();
    pointIndex = screenVec.at(0)->getIndexOfLastPoint(pointR_1_X,pointR_1_Y);
    //!!!!!!!!!!!!!!!!!!

    mathforcap = new MathForCap(this);
    dialogranges = new DialogSetRangesETC(0);

    pointRsou.clear();  pointRsou.resize(numOfVar);

    pointsX.clear(); pointsY.clear(); pointsZ.clear();          //точки пересечения для одного луча с капилляром
    anglesGlacing.clear();                                      //скользящие углы для точек одного луча только на капилляре.
    pointsXscr.clear(); pointsYscr.clear(); pointsZscr.clear(); //точки попадания на экран для одного луча
    anglesScrPhi.clear(); anglesScrTheta.clear();               //углы на экране для одного луча
    lengthOfRay.clear();                                        //длина этого одного луча
    //  оно же, только изменение размеров под кол-во экранов:
    pointsXscr.resize(countScreens); pointsYscr.resize(countScreens); pointsZscr.resize(countScreens);
    anglesScrPhi.resize(countScreens); anglesScrTheta.resize(countScreens);
    lengthOfRay.resize(countScreens);

    this->clearTotal();

    connect(dialogranges,SIGNAL(rejected()),this,SLOT(slotSetRanges()));

    qDebug() << "CAPSYS: Model is created!";
    qDebug();
//    qDebug() << "!!!!!!!!!!!!!zdes' nado vyvesti!!!";
}

int Coherence::Draw(mglGraph *gr)
{
    // 1. здесь используется flagNumCap. С какого перепуга? Пусть использует старый метод? //animation
    // 2. анимацию надо переписать
    long int i,j,k;
    const char *colors[] = {"{e7}","{n7}","{g7}","{y7}","{q7}","{c7}","{m7}"};
    gr->SetDefScheme("Nn{c4}{e9}qrR");
//    gr->SetSize(1024,768);
    gr->SetFontSizePT(8);

    QByteArray lengthUnit = "";
    if(sourceEnvir->getLengthUnitMultPower() == -6)
    {
        lengthUnit = "\\mu m";
    }
    else if(sourceEnvir->getLengthUnitMultPower() == -5)
    {
        lengthUnit = "10\\cdot\\mu m";
    }
    else if(sourceEnvir->getLengthUnitMultPower() == -4)
    {
        lengthUnit = "0.1 mm";
    }
    else if(sourceEnvir->getLengthUnitMultPower() == -3)
    {
        lengthUnit = "mm";
    }
    else if(sourceEnvir->getLengthUnitMultPower() == -2)
    {
        lengthUnit = "sm";
    }
    else if(sourceEnvir->getLengthUnitMultPower() == -1)
    {
        lengthUnit = "10^-1\\cdotm";
    }
    else if(sourceEnvir->getLengthUnitMultPower() == 0)
    {
        lengthUnit = "m";
    }
    else if(sourceEnvir->getLengthUnitMultPower() == 1)
    {
        lengthUnit = "10\\cdotm";
    }
    else if(sourceEnvir->getLengthUnitMultPower() == 2)
    {
        lengthUnit = "10^2\\cdotm";
    }
    else if(sourceEnvir->getLengthUnitMultPower() == 3)
    {
        lengthUnit = "km";
    }

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
    if (flagDraw == 0)//testDraw
    {
        int maxCountPoints = 0;

        if(countRaysTestDraw > 0)
        {
            if(!flagDEBUGParallel)
                this->sendRay();
            else
            {
                QFuture<void> future = QtConcurrent::run(this, &Coherence::sendRayMNOGOPOTOCHNIJ, 0);
                future.waitForFinished();
                this->setMapResultRay(0);
            }
            dialogranges->setParams(//-screenVec[0]->isEdgeOfScreenSquare()/2.0,screenVec[0]->isEdgeOfScreenSquare()/2.0,
                                //-screenVec[0]->isEdgeOfScreenSquare()/2.0,screenVec[0]->isEdgeOfScreenSquare()/2.0,
                                    screenVec[0]->pointsXscreen.first(),screenVec[0]->pointsXscreen.last(),
                                    screenVec[0]->pointsYscreen.first(),screenVec[0]->pointsYscreen.last(),
                                    pointRsou[2]-1,pointsZscr[0]+1);
            dialogranges->exec();
        }
        else
        {
            mglExpr a(screenVec[0]->getFScreen().data());
            dialogranges->setParams(//-screenVec[0]->isEdgeOfScreenSquare()/2.0,screenVec[0]->isEdgeOfScreenSquare()/2.0,
                                //-screenVec[0]->isEdgeOfScreenSquare()/2.0,screenVec[0]->isEdgeOfScreenSquare()/2.0,
                                    screenVec[0]->pointsXscreen.first(),screenVec[0]->pointsXscreen.last(),
                                    screenVec[0]->pointsYscreen.first(),screenVec[0]->pointsYscreen.last(),
                                    sourceEnvir->getZ_0_()-1,std::fabs(a.Eval(0.0,0.0,0.0))+1);
            dialogranges->exec();
        }

        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        QVector< QVector<double> > dataX, dataY, dataZ;
        QVector< double > tempX, tempY, tempZ;

        for(i = 0; i < countRaysTestDraw; ++i)
        {
            if(!flagDEBUGParallel)
                this->sendRay();
            else
            {
                QFuture<void> future = QtConcurrent::run(this, &Coherence::sendRayMNOGOPOTOCHNIJ, 0);
                future.waitForFinished();
                this->setMapResultRay(0);
            }
            int countPoints = pointsX.size();
            if(maxCountPoints < countPoints)
            {
                maxCountPoints = countPoints;
            }
            //            qDebug() << "       Ray passed through the capillary, number of reflections:" << countPoints;
            //            qDebug() << anglesGlacing;
            //            if(!anglesGlacing.empty())
            //            for( int ll=0; ll < 1/*anglesGlacing.size()*/; ++ll)
            //                qDebug() << capillaryVec[flagNumCap]->getCoefOfRefl(anglesGlacing[ll],sourceEnvir->getAverageFrequency());
            tempX.clear(); tempY.clear(); tempZ.clear();
            tempX.push_back(pointRsou.value(0));
            tempY.push_back(pointRsou.value(1));
            tempZ.push_back(pointRsou.value(2));
            for (j= 0; j < countPoints; ++j)
            {
                tempX.push_back(pointsX[j]);
                tempY.push_back(pointsY[j]);
                tempZ.push_back(pointsZ[j]);
            }
            tempX.push_back(pointsXscr[0]);
            tempY.push_back(pointsYscr[0]);
            tempZ.push_back(pointsZscr[0]);
            dataX.push_back(tempX);
            dataY.push_back(tempY);
            dataZ.push_back(tempZ);
        }

        if(flagTestDrawAnimation)
        {
            gr->SetFontSizePT(7);

            mglData grtemp(100,100,100);


            //        gr->SetQuality(1);
//            gr->ResetFrames();
            char str[32];
            gr->StartGIF("animationTestDraw.gif",1500);
            for(j = 0; j < 37; ++j)
            {
//                for(int ii = 0; ii < 181; ++ii)
//                    for(int jj = 0; jj < 4; ++jj)
//                    {
                        gr->NewFrame();
//                        gr->Title(("Coeficient of reflection:\ncolor - val. after "+QByteArray::number((int)j+1)+" refl.;\nposition - point of first refl."),"",5)
                        gr->Rotate(60,j*10);
                        gr->Alpha(true);

                        gr->SetRanges(xrang_,xrang__,yrang_,yrang__,zrang_,zrang__);
                        gr->SetRange('c',0,1);
                        QByteArray botbot = "<0";
                        QByteArray endend = ">0";

                        for(i = 0; i < countCapillaries; ++i)
                        {
                            gr->CutOff(("("+capillaryVec[i]->getFBottomOfCap()+botbot+") | (" + capillaryVec[i]->getFEndOfCap()+endend+")").data());
                            gr->Fill(grtemp,capillaryVec[i]->getF0().data(),"");
                            //      qDebug() << "AAAAAAAAAAAAAAAAAA" << f0_0 << f0_1;
                            //            gr->Surf3(0.0,grtemp,colors[i],"");
                            try {
                                gr->Surf3(0.0,grtemp,colors[i],"");
                                gr->CutOff("");
                            } catch (...) {
                                gr->Surf3(0.0,grtemp,"i","");
                                gr->CutOff("");
                            }

                            //            gr->Fill(grtemp,capillaryVec[i]->getFBottomOfCap().data(),"");
                            //            gr->Surf3(0.0,grtemp,"W");
                            //            gr->Fill(grtemp,capillaryVec[i]->getFEndOfCap().data(),"");
                            //            gr->Surf3(0.0,grtemp,"W");
                        }
                        gr->Alpha(false);
                        for(i = 0; i < countRaysTestDraw; ++i)
                        {
                            mglData x,y,z;
                            x.Set(dataX.at(i).toStdVector());
                            y.Set(dataY.at(i).toStdVector());
                            z.Set(dataZ.at(i).toStdVector());
                            gr->Plot(x,y,z,"2i");
                        }

                        gr->AddLegend(QByteArray("max. number of refl.: "+QByteArray::number(maxCountPoints)).constData(),"");
                        gr->Label('x',"x, "+lengthUnit,0);
                        gr->Label('y',"y, "+lengthUnit,0);
                        gr->Label('z',"z, "+lengthUnit,0);
                        gr->Axis("xyz"); gr->Box();  gr->Grid(); gr->Legend();
                        gr->EndFrame();
            }
            gr->CloseGIF();
            return gr->GetNumFrame();
        }
        else
        {
            mglExpr asdg(screenVec[0]->getFScreen().data());
            if(flagDebugDrawTitle)
            {
                gr->Title(("l = "+QByteArray::number(std::fabs(asdg.Eval(0.))/1000)+" ").data(),"",5);
            }
            gr->SetFontSizePT(7);
            gr->Rotate(rxrotate,rzrotate,ryrotate);
            //        gr->Rotate(60,40);
            gr->Alpha(true);
            gr->SetRanges(xrang_,xrang__,yrang_,yrang__,zrang_,zrang__);

            mglData grtemp(100,100,100);
            for(i = 0; i < countRaysTestDraw; ++i)
            {
                if(!flagDEBUGParallel)
                    this->sendRay();
                else
                {
                    QFuture<void> future = QtConcurrent::run(this, &Coherence::sendRayMNOGOPOTOCHNIJ, 0);
                    future.waitForFinished();
                    this->setMapResultRay(0);
                }
                int countPoints = pointsX.size();
                if(maxCountPoints < countPoints)
                {
                    maxCountPoints = countPoints;
                }
                //            qDebug() << "       Ray passed through the capillary, number of reflections:" << countPoints;

                //            qDebug() << anglesGlacing;
                //            if(!anglesGlacing.empty())
                //            for( int ll=0; ll < 1/*anglesGlacing.size()*/; ++ll)
                //                qDebug() << capillaryVec[flagNumCap]->getCoefOfRefl(anglesGlacing[ll],sourceEnvir->getAverageFrequency());

                mglData x(countPoints+2), y(countPoints+2), z(countPoints+2);
                x.a[0] = pointRsou[0];
                y.a[0] = pointRsou[1];
                z.a[0] = pointRsou[2];
                for (j= 0; j < countPoints; ++j)
                {
                    x.a[j+1] = pointsX[j];
                    y.a[j+1] = pointsY[j];
                    z.a[j+1] = pointsZ[j];
                }
                x.a[countPoints+1] = pointsXscr[0];
                y.a[countPoints+1] = pointsYscr[0];
                z.a[countPoints+1] = pointsZscr[0];
                gr->Plot(x,y,z,"i");
            }

            QByteArray botbot = "<0";
            QByteArray endend = ">0";

            for(i = 0; i < countCapillaries; ++i)
            {
                gr->CutOff(("("+capillaryVec[i]->getFBottomOfCap()+botbot+") | (" + capillaryVec[i]->getFEndOfCap()+endend+")").data());
                gr->Fill(grtemp,capillaryVec[i]->getF0().data(),"");
                //            gr->Surf3(0.0,grtemp,colors[i],"");
                try {
                    gr->Surf3(0.0,grtemp,colors[i],"");
                    gr->CutOff("");
                } catch (...) {
                    gr->Surf3(0.0,grtemp,"i","");
                    gr->CutOff("");
                }
                gr->CutOff("");
                //            gr->Fill(grtemp,capillaryVec[i]->getFBottomOfCap().data(),"");
                //            gr->Surf3(0.0,grtemp,"W");
                //            gr->Fill(grtemp,capillaryVec[i]->getFEndOfCap().data(),"");
                //            gr->Surf3(0.0,grtemp,"W");
            }

            for(i=0; i < countScreens; ++i)
            {
                //            qDebug() << "QQQQQQQQQQQQQQQQQQ" << screenVec[i]->getFScreen().data();
                gr->Fill(grtemp,screenVec[i]->getFScreen().data(),"");
                gr->Surf3(0.0,grtemp,"c","");
            }
            //        gr->FSurf(QByteArray::number(0.0,'g',15).data(),"W","");
            //        gr->FSurf(QByteArray::number(200.0,'g',15).data(),"W","");
            //        gr->FSurf(QByteArray::number(400.0,'g',15).data(),"c","");

            //        gr->Puts(mglPoint(0.1,0.1,zrang__+2.0),(QByteArray("max number of refl.: ")+QByteArray::number(maxCountPoints)));
            //        gr->Label('x',"x, m*"+QByteArray::number(sourceEnvir->getLengthUnitMult(),'e',0),1);
            //        gr->Label('y',"y, m*"+QByteArray::number(sourceEnvir->getLengthUnitMult(),'e',0),1);
            //        gr->Label('z',"z, m*"+QByteArray::number(sourceEnvir->getLengthUnitMult(),'e',0),1);
            if(flagDEBUGKkolvomaxrays)
            {
                gr->AddLegend(QByteArray("max. number of refl.: "+QByteArray::number(maxCountPoints)).constData(),"");
            }
            gr->Label('x',"x, "+lengthUnit,1);
            gr->Label('y',"y, "+lengthUnit,1);
            gr->Label('z',"z, "+lengthUnit,1);
            gr->Box(); gr->Axis(); gr->Grid();
            if(flagDEBUGKkolvomaxrays)
            {
                gr->Legend();
            }
            gr->WritePNG(QByteArray("00-testGeom-"+nameProj+".png").data());
        }

        return 0;
    }
    else if (flagDraw == 1)//Density distribution of the photons znc
    {
        long i,j;
        long n = screenVec[0]->isMeshScreenX();
        mglData a(n,n),x(n),y(n);

        double tempMinX, tempMinY, tempMinA;
        double tempMaxX, tempMaxY, tempMaxA;
        tempMaxA = znc.value(0);
        tempMinA = znc.value(0);
        for (i = 0; i < screenVec[0]->isMeshScreenXY();++i)
        {
            if (znc.value(i) > tempMaxA)
                tempMaxA = znc.value(i);
            if (znc.value(i) < tempMinA)
                tempMinA = znc.value(i);
        }
        tempMinX = screenVec[0]->pointsXscreen.value(0);
        tempMinY = screenVec[0]->pointsYscreen.value(0);
        tempMaxX = screenVec[0]->pointsXscreen.value(0);
        tempMaxY = screenVec[0]->pointsYscreen.value(0);
        for (j = 0; j < n; ++j)
        {
            if(screenVec[0]->pointsXscreen.value(j) < tempMinX)
                tempMinX = screenVec[0]->pointsXscreen.value(j);
            if(screenVec[0]->pointsYscreen.value(j) < tempMinY)
                tempMinY = screenVec[0]->pointsYscreen.value(j);
            if(screenVec[0]->pointsXscreen.value(j) > tempMaxX)
                tempMaxX = screenVec[0]->pointsXscreen.value(j);
            if(screenVec[0]->pointsYscreen.value(j) > tempMaxY)
                tempMaxY = screenVec[0]->pointsYscreen.value(j);
        }

        for (j = 0; j < n; ++j)
        {
            for(i=0;i<n;i++)
            {
                a.a[i+n*j] = znc[i+n*j];
            }
            x.a[j] = screenVec[0]->pointsXscreen[j];
            y.a[j] = screenVec[0]->pointsYscreen[j];
        }

//        a.Norm();
//        gr->Title("Density distribution of the photons");
        if(flagWithSetRanges)
        {
            dialogranges->setParams(tempMinX,tempMaxX,
                                    tempMinY,tempMaxY,
                                    0,tempMaxA);
            dialogranges->exec();
            gr->Rotate(rxrotate,rzrotate,ryrotate);
            gr->SetRange('x',xrang_,xrang__);
            gr->SetRange('y',yrang_,yrang__);
//            gr->SetRange('x',-2,2);
//            gr->SetRange('y',-2,2);
            gr->SetRange('z',zrang_-1e-13,zrang__+1e-13);
            gr->SetRange('c',zrang_-1e-13,zrang__+1e-13);
//            gr->SetRanges(x,y,a);

        }
        else
        {
            gr->Rotate(0,0);
//            gr->SetRange('x',-2,2);
//            gr->SetRange('y',-2,2);
//            gr->SetRange('z',0,1001);
//            gr->SetRange('c',0,1001);
            gr->SetRanges(x,y,a);
        }
        //gr->Alpha(true);
//        gr->Plot(x,y,z,"@");
        gr->Surf(x,y,a,"Nn{c4}{e9}qrR");
        gr->Label('x',"x, "+lengthUnit,1);
        gr->Label('y',"y, "+lengthUnit,1);
//        gr->Label('z',"",1); // (kg/Gs^3)
//        gr->Colorbar(">Nn{c4}{e9}qrR");
        gr->Colorbar();
        gr->Box(); gr->Axis(); gr->Grid();
        gr->WritePNG(QByteArray("01-densDistrOfPhot-"+nameProj+".png").data());
        return 0;
    }
    else if (flagDraw == 2)//новый усредн. метод степени когерентности
    {

//        dialogranges->setParams(-screenVec[0]->isEdgeOfScreenSquare()/2.0,screenVec[0]->isEdgeOfScreenSquare()/2.0,
//                                -screenVec[0]->isEdgeOfScreenSquare()/2.0,screenVec[0]->isEdgeOfScreenSquare()/2.0,
//                                0.0,tempMaxNC);
//        dialogranges->exec();
//        gr->SetRanges(xrang_,xrang__,yrang_,yrang__,zrang_,zrang__);

        long int i, j, n = screenVec[0]->isMeshScreenX();

        mglData a(n,n),x(n),y(n);
        for(j = 0; j<n; j++)
        {
            for(i = 0; i<n; i++)
            {
//                if(znc[i+n*j] != 0)
//                    a.a[i+n*j] = zcRe[i+n*j]/znc[i+n*j];
//                qDebug() << "" << i << j << zzz[i+n*j]/znc[i+n*j];
//                if(std::sqrt(std::pow(zcRe[i+n*j],2) + std::pow(zcIm[i+n*j],2)) != 0)
//                if(zzzNORM[0][i+n*j]!=0)
                a.a[i+n*j] = zzzTotal[i+n*j] / zzzNORMTotal[i+n*j];
//                            /std::sqrt(std::pow(zcRe[i+n*j],2) + std::pow(zcIm[i+n*j],2));
//                else
//                    a.a[i+n*j] = 0.;
            }
            x.a[j] = screenVec.at(0)->pointsXscreen.at(j);
            y.a[j] = screenVec.at(0)->pointsYscreen.at(j);
        }

//        gr->Plot(x,y,z,"@");
//        gr->Title("Real part of\nthe degree of coherence");
//        a.Norm(-1,1);
        if(flagWithSetRanges)
        {
            dialogranges->setParams(x.Minimal(),x.Maximal(),
                                    y.Minimal(),y.Maximal(),
                                    a.Minimal()-1e-13,a.Maximal()+1e-13);
            dialogranges->exec();
            gr->SetRanges(xrang_,xrang__,yrang_,yrang__,zrang_,zrang__);
            gr->Rotate(rxrotate,rzrotate,ryrotate);
        }
        else
        {
            gr->Rotate(0,0);
            gr->SetRanges(x,y,a);
        }

        //gr->Alpha(true);
        gr->Surf(x,y,a,"{e9}qrRk");

        gr->Label('x',"x, " + lengthUnit, 1);
        gr->Label('y',"y, " + lengthUnit, 1);
//        gr->Label('z',"Re[\\gamma (\\Delta ,\\tau)]",1); // (kg/Gs^3)
        gr->Colorbar(">{e9}qrRk");
        gr->Box(); gr->Axis(); gr->Grid();
        gr->WritePNG(QByteArray("03-absDegOfCoh-" + nameProj + ".png").data());

        return 0;
    }
    else if (flagDraw == 9)//absDegrOfCoh
    {
        //        dialogranges->setParams(-screenVec[0]->isEdgeOfScreenSquare()/2.0,screenVec[0]->isEdgeOfScreenSquare()/2.0,
        //                                -screenVec[0]->isEdgeOfScreenSquare()/2.0,screenVec[0]->isEdgeOfScreenSquare()/2.0,
        //                                0.0,tempMaxNC);
        //        dialogranges->exec();
        //        gr->SetRanges(xrang_,xrang__,yrang_,yrang__,zrang_,zrang__);

                long n = screenVec.at(0)->isMeshScreenX();
                long m = zgeneral.size();

                mglData a(n,n),x(n),y(n), zz(m), xx(m), yy(m);

                for(i=0;i<m;++i)
                {
                    xx.a[i] = xgeneral[i];
                    yy.a[i] = ygeneral[i];
//                    xx.a[i] = screenVec.at(0)->pointsXscreen.at(i);
//                    yy.a[i] = screenVec.at(0)->pointsYscreen.at(i);
                    zz.a[i] = std::sqrt( std::pow(zgeneral.at(i),2) + std::pow(zgeneralIm.at(i),2) );
                }

//                zz.Norm();

//                mglData xxx=gr->Hist(xx,zz), yyy=gr->Hist(yy,zz);
//                xxx.Norm(0,1); yyy.Norm(0,1);

//                pointIndex = screenVec[0]->getIndexOfLastPoint(pointR_1_X,pointR_1_Y);
//                double asdf = std::sqrt(
//                            std::pow(DOfCohRjRj_Real[pointIndex],2) + std::pow(DOfCohRjRj_Imag[pointIndex],2)
//                            );
//                for (j=0;j<n;j++)
//                {
//                    for(i=0;i<n;i++)
//                    {
////                        if(znc[i+n*j] != 0)
//                        double asd = std::sqrt(
//                                    std::pow(DOfCohRjRj_Real[i+n*j],2) + std::pow(DOfCohRjRj_Imag[i+n*j],2)
//                                    );
////                        qDebug() << asd;
////                        if(asd!=0)
////                            a.a[i+n*j] = std::sqrt(
////                                        std::pow(zgeneral[i+n*j]/asdf,2) + std::pow(zgeneralIm[i+n*j]/asdf,2)
////                                        );//znc[i+n*j];
////                        a.a[i+n*j] = std::sqrt(
////                                    std::pow(zgeneral[i+n*j],2)
////                                    + std::pow(zgeneralIm[i+n*j],2)
////                                )/(2*asdf);
//                        a.a[i+n*j] = std::sqrt(std::pow(zgeneral[i+n*j],2) + std::pow(zgeneralIm[i+n*j],2));
//                    }
//                    x.a[j] = screenVec[0]->pointsXscreen[j];
//                    y.a[j] = screenVec[0]->pointsYscreen[j];
//                }
        //        a.Norm();
        //        gr->Plot(x,y,z,"@");
        //        gr->Title("Real part of\nthe degree of coherence");
        //        a.Norm(-1,1);
                if(flagWithSetRanges)
                {
//                    dialogranges->setParams(x.Minimal(),x.Maximal(),
//                                            y.Minimal(),y.Maximal(),
//                                            -1.,1.);
                    dialogranges->setParams(xx.Minimal(),xx.Maximal(),
                                            yy.Minimal(),yy.Maximal(),
                                            -1.,1.);
                    dialogranges->exec();
                    gr->SetRanges(xrang_,xrang__,yrang_,yrang__,zrang_,zrang__);
                    gr->Rotate(rxrotate,rzrotate,ryrotate);
                }
                else
                {
                    gr->Rotate(0,0);
//                    gr->SetRanges(x,y,a);
                   gr->SetRanges(xx,yy,zz);
                }
//                gr->Alpha(true);
//                gr->Surf(x,y,a,"Nn{c4}{e9}qrR");
//                gr->MultiPlot(3,3,3,2,2,"");   gr->SetRanges(xx,yy,zz);
//                gr->Box();  gr->Dots(xx,yy,zz);
//                gr->MultiPlot(3,3,0,2,1,"");   gr->SetRanges(-1,1,0,1);
//                gr->Box();  gr->Bars(xxx);
//                gr->MultiPlot(3,3,5,1,2,"");   gr->SetRanges(0,1,-1,1);
//                gr->Box();  gr->Barh(yyy);

                zz.Smooth();

                gr->Label('x',"x, "+lengthUnit,1);
                gr->Label('y',"y, "+lengthUnit,1);
//                gr->Label('z',"Re[\\gamma (\\Delta ,\\tau)]",1); // (kg/Gs^3)
//                gr->Colorbar(">Nn{c4}{e9}qrR");
                gr->Colorbar(">{e9}qrRk");

                gr->Box(); gr->Axis(); gr->Grid("xyzt","N");

                gr->Dots(xx,yy,zz,"{e9}qrRk");

                gr->WritePNG(QByteArray("03-absDegOfCoh_prev-"+nameProj+".png").data());
                return 0;
    }
    else if (flagDraw == 3)//anglesscr
    {
//        qDebug() << totalAverTheta[0];
        long int i,j;
        long n = screenVec[0]->isMeshScreenX();

        mglData a(n,n), max(n,n);
        mglData x(n),y(n);
        for (i=0;i<n;++i)
        {
            x.a[i]=screenVec[0]->pointsXscreen[i];
            y.a[i]=screenVec[0]->pointsYscreen[i];
        }
        for (j= 0; j< n; ++j)
        {
            for(i= 0; i< n; ++i)
            {
                a.a[i+n*j] = totalAverTheta[0][i+n*j];
                max.a[i+n*j] = totalMaxTheta[0][i+n*j];
            }
        }
        if(flagWithSetRanges)
        {
            dialogranges->setParams(x.Minimal(),x.Maximal(),
                                    y.Minimal(),y.Maximal(),
                                    a.Minimal()-1e-10,a.Maximal()+1e-10);
            dialogranges->exec();
            gr->Rotate(rxrotate,rzrotate);
            gr->SetRanges(xrang_,xrang__,yrang_,yrang__,zrang_,zrang__);
        }
        else
        {
            gr->Rotate(0,0);
            gr->SetRanges(x,y,max);
        }

//        gr->Alpha(true);

        gr->Surf(x,y,a,"Nn{c4}{e9}qrR");
//        gr->Surf(x,y,max,"2k");
        gr->Label('x',"x, "+lengthUnit,1);
        gr->Label('y',"y, "+lengthUnit,1);
//        gr->Label('z',"Zenit angles \\theta_{max+min}, rad",1);
        gr->Colorbar("<Nn{c4}{e9}qrR");
//        gr->Colorbar("<cbuBUk");
        gr->Box(); gr->Axis("xyz"); gr->Grid();
//        gr->EndFrame();

//        gr->NewFrame();
//        gr->Alpha(true);
//        if(flagWithSetRanges)
//        {
//            gr->Rotate(rxrotate,rzrotate);
//            gr->SetRanges(xrang_,xrang__,yrang_,yrang__,zrang_,zrang__);
//        }
//        else
//        {
//            gr->Rotate(60,40);
//            gr->SetRanges(x,y,b);
//        }
////        gr->Surf(x,y,a,"PRqrqy");
//        gr->Surf(x,y,b,"cbuBUk");
//        gr->Label('x',"x, m*"+QByteArray::number(sourceEnvir->getLengthUnitMult(),'e',0),1);
//        gr->Label('y',"y, m*"+QByteArray::number(sourceEnvir->getLengthUnitMult(),'e',0),1);
//        gr->Label('z',"Zenit angles \\theta_{min}, rad",1);
//        gr->Colorbar(">cbuBUk");
//        gr->Box(); gr->Axis("xyz"); gr->Grid();
//        gr->EndFrame();

//        gr->NewFrame();
//        gr->Alpha(true);
//        if(flagWithSetRanges)
//        {
//            gr->Rotate(rxrotate,rzrotate);
//            gr->SetRanges(xrang_,xrang__,yrang_,yrang__,zrang_,zrang__);
//        }
//        else
//        {
//            gr->Rotate(60,40);
//            gr->SetRanges(x,y,a);
//        }
//        gr->Surf(x,y,a,"PRqrqy");
////        gr->Surf(x,y,b,"cbuBUk");
//        gr->Label('x',"x, m*"+QByteArray::number(sourceEnvir->getLengthUnitMult(),'e',0),1);
//        gr->Label('y',"y, m*"+QByteArray::number(sourceEnvir->getLengthUnitMult(),'e',0),1);
//        gr->Label('z',"Zenit angles \\theta_{max}, rad",1);
//        gr->Colorbar(">PRqrqy");
//        gr->Box(); gr->Axis("xyz"); gr->Grid();
//        gr->EndFrame();
//        return gr->GetNumFrame();
        gr->WritePNG(QByteArray("04-angles-"+nameProj+".png").data());
        return 0;
    }
    else if (flagDraw == 4)//animation
    {

        int totalTempMax = 0;     //посчитаем максимальное количество отражений

        if(countRaysTestDraw > 0)
        {
            do
            {
                if(!flagDEBUGParallel)
                    this->sendRay();
                else
                {
                    QFuture<void> future = QtConcurrent::run(this, &Coherence::sendRayMNOGOPOTOCHNIJ, 0);
                    future.waitForFinished();
                    this->setMapResultRay(0);
                }
            }while(pointsX.isEmpty());
            dialogranges->setParams(//-screenVec[0]->isEdgeOfScreenSquare()/2.0,screenVec[0]->isEdgeOfScreenSquare()/2.0,
                                //-screenVec[0]->isEdgeOfScreenSquare()/2.0,screenVec[0]->isEdgeOfScreenSquare()/2.0,
                                    -(std::sqrt(std::pow(pointsX.first(),2)+std::pow(pointsY.first(),2)))
                                      ,(std::sqrt(std::pow(pointsX.first(),2)+std::pow(pointsY.first(),2)))
                                   ,-(std::sqrt(std::pow(pointsX.first(),2)+std::pow(pointsY.first(),2)))
                                      ,(std::sqrt(std::pow(pointsX.first(),2)+std::pow(pointsY.first(),2)))
                                   ,pointRsou[2]-5e-6, pointsZscr[0]+5e-6
                                    );
            dialogranges->exec();
        }
        else
        {
            mglExpr a(screenVec[0]->getFScreen().data());
            dialogranges->setParams(-screenVec[0]->isEdgeOfScreenSquare()/2.0
                                    ,screenVec[0]->isEdgeOfScreenSquare()/2.0
                                    ,-screenVec[0]->isEdgeOfScreenSquare()/2.0
                                    ,screenVec[0]->isEdgeOfScreenSquare()/2.0
                                   ,sourceEnvir->getZ_0_()-5e-6
                                    ,std::fabs(a.Eval(0.0,0.0,0.0))+5e-6);
            dialogranges->exec();
        }
//        this->sendRay();
//        dialogranges->setParams(//-screenVec[0]->isEdgeOfScreenSquare()/2.0,screenVec[0]->isEdgeOfScreenSquare()/2.0,
//                                //-screenVec[0]->isEdgeOfScreenSquare()/2.0,screenVec[0]->isEdgeOfScreenSquare()/2.0,
//                                screenVec[0]->pointsXscreen.first(),screenVec[0]->pointsXscreen.last(),
//                                screenVec[0]->pointsYscreen.first(),screenVec[0]->pointsYscreen.last(),
//                                pointsZsou-10,pointsZscr[0]+10);
//        dialogranges->exec();

        mglData grtemp(100,100,100);

        QVector< QVector<double> > testCoefOfReflxx,testCoefOfReflyy,testCoefOfReflzz,testCoefOfReflcolor;
//        countNumOfReflections = 1; //в том смысле, что рисовать будем один график! а не в том смысле, что отражений 1
        testCoefOfReflxx.resize(countNumOfReflections);//xx[0]-массив первых точек отражения yy zz аналогично
        testCoefOfReflyy.resize(countNumOfReflections);
        testCoefOfReflzz.resize(countNumOfReflections);
        testCoefOfReflcolor.resize(countNumOfReflections);

        mglData aa,bb,cc,qq;
        bool aaa;
//        long int l;//чисто для qDebug()
        for(i = 0; i < countRaysTestDraw; ++i)
        {
//            for(j = 0; j < countNumOfReflections; ++j)//итак, берём одну точку, если не хватает точек отражения присваиваем единицу
//            {
//                l=0;//чисто для qDebug()
                do
                {
//                    qDebug() << j << countNumOfReflections << pointsX.size();
                    aaa = false;
                    do//создание непустой точки пересечения, т.к. поиск луча
                    {
                        if(!flagDEBUGParallel)
                            this->sendRay();
                        else
                        {
                            QFuture<void> future = QtConcurrent::run(this, &Coherence::sendRayMNOGOPOTOCHNIJ, 0);
                            future.waitForFinished();
                            this->setMapResultRay(0);
                        }
                    }
                    while( (pointsX.isEmpty())/* || (j >= pointsX.size())*/ );
//                    if (j == 0)
//                    {
                        for(k=0;k<i;++k)//проверка со всеми другими точками на близость
                        {
//                            qDebug() << std::sqrt(std::pow(xx[j][k]-pointsX[j],2) + std::pow(yy[j][k]-pointsY[j],2) + std::pow(zz[j][k]-pointsZ[j],2));
                            if(
                                    std::pow(testCoefOfReflxx[0][k]-pointsX[0],2) + std::pow(testCoefOfReflyy[0][k]-pointsY[0],2) + std::pow(testCoefOfReflzz[0][k]-pointsZ[0],2)
                                    < std::pow(testDeltaNorma,2)
                                    )
                            {
                                aaa = true;
                                break;
                            }
                        }
//                    }
//                    qDebug() << ++l;//для проверки сколько чего требуется...
                }
                while( aaa );
                //нужный луч получен, хорошо!

                long int tempMax = pointsX.size();
                qDebug() << "The ray N." << i << "with number of reflections:" << tempMax;
                if(totalTempMax <= tempMax)
                    totalTempMax = tempMax;

//                qDebug() << "kol-vo tochek" << tempMax << "pointsX,Y,Z" << pointsX << pointsY << pointsZ << anglesGlacing;
                if( tempMax >= countNumOfReflections)//
                {
                    for(j = 0; j < countNumOfReflections; ++j)
                    {
                        testCoefOfReflxx[j].push_back(pointsX[j]);
                        testCoefOfReflyy[j].push_back(pointsY[j]);
                        testCoefOfReflzz[j].push_back(pointsZ[j]);
    //                qDebug() << "tetha[0],z[0]:" <<anglesGlacing[0] << pointsZ[0];
                        testCoefOfReflcolor[j].push_back(capillaryVec[flagNumCap]->getCoefOfRefl(anglesGlacing[j],sourceEnvir->getAverageFrequency()));
                    }
                }
                else
                {
                    for(j = 0; j < tempMax; ++j)
                    {
                        testCoefOfReflxx[j].push_back(pointsX[j]);//в нулевой вектор - первые точки отражения лучей, в нулевой вектор коэфе - коэфе для них
                        testCoefOfReflyy[j].push_back(pointsY[j]);//в первый вектор - вторые точки отражения лучей, в первый ветокр коэф. - коэф для них по порядку для лучей
                        testCoefOfReflzz[j].push_back(pointsZ[j]);
    //                qDebug() << "tetha[0],z[0]:" <<anglesGlacing[0] << pointsZ[0];
                        testCoefOfReflcolor[j].push_back(capillaryVec[flagNumCap]->getCoefOfRefl(anglesGlacing[j],sourceEnvir->getAverageFrequency()));
//                        qDebug() << "color:" << capillaryVec[flagNumCap]->getCoefOfRefl(anglesGlacing[j],sourceEnvir->getAverageFrequency());
                    }
                    for(j = tempMax; j < countNumOfReflections; ++j)//все массивы равны, какому из лучей не хватило точек отражения - дополняем нулями и коэф - единицами
                    {
                        testCoefOfReflxx[j].push_back(pointsXscr[0]);
                        testCoefOfReflyy[j].push_back(pointsYscr[0]);
                        testCoefOfReflzz[j].push_back(pointsZscr[0]);
    //                qDebug() << "tetha[0],z[0]:" <<anglesGlacing[0] << pointsZ[0];
                        testCoefOfReflcolor[j].push_back(1.0);
                    }
                }

//                qDebug() << "testCoefOfReflcolor" << testCoefOfReflcolor;
//                if(anglesGlacing.size() >= 2)
//                for( int ll=0; ll < 1 /*anglesGlacing.size()*/; ++ll)
//                    qDebug() << anglesGlacing[0] << anglesGlacing[1] << capillaryVec[flagNumCap]->getCoefOfRefl(anglesGlacing[ll],sourceEnvir->getAverageFrequency())
//                                << capillaryVec[flagNumCap]->getCoefOfRefl(anglesGlacing[ll+1],sourceEnvir->getAverageFrequency());

//        }

        }//прогнали все лучи

//        for(i = 0; i < screenVec[0]->isMeshScreenXY(); ++i)
//        {
//            QVector< QVector<double> >::const_iterator it = totalPointsX[0][i].constBegin();
//            for(; it != totalPointsX[0][i].constEnd(); ++it)
//            {
//                qDebug() << (*it)[0];
//            }
//        }

        /// создали массив для нужного количества отражений, теперь важно его правильно обработать:
        for(i = countNumOfReflections-1; i >=1; --i)//i - это по сути номер коэф отражения для первых точек отражений лучей и т.д. по порядку.
        {//для каждого j луча умножаем всё в i=0
            for(int j = 0; j < testCoefOfReflcolor.at(i).size(); ++j)
            {
//                qDebug() << i << j << testCoefOfReflcolor[i][j];
                for(int k = i-1; k>=0; --k)
                {
//                        qDebug() << "LLL" << k << j << testCoefOfReflcolor.at(k).at(j);
                        if(testCoefOfReflcolor.at(k).at(j) != 1)
                            testCoefOfReflcolor[i][j] = testCoefOfReflcolor.at(i).at(j) * testCoefOfReflcolor.at(k).at(j);
                }
            }
        }
        //<- сделаем в обратном порядке!!
        //хорошо, раньше нельзя было так поступать... а если какое число окажетс 0.9999999999999999999999 в самом начале? Верно, тогда много раз умножится... и получим 0 на выходе! Это и получается.


/// ANIMATION!!!!!!!! --------------------------------------------------
        if (flagWithAnimation)
        {
            //        gr->SetQuality(1);
            gr->ResetFrames();
            char str[32];
            gr->StartGIF(QByteArray("04 - animationCoefOfRefl"+nameProj+".gif").data(),1000);
            for(j = 0; j < countNumOfReflections; ++j)
            {
                aa.Set(testCoefOfReflxx[0].toStdVector());
                bb.Set(testCoefOfReflyy[0].toStdVector());
                cc.Set(testCoefOfReflzz[0].toStdVector());
                qq.Set(testCoefOfReflcolor[j].toStdVector());

                for(int ii = 0; ii < 18; ++ii)
//                    for(int jj = 0; jj < 4; ++jj)
                    {
                        gr->NewFrame();
//                        gr->Title(("Coeficient of reflection:\ncolor - val. after "+QByteArray::number((int)j+1)+" refl.;\nposition - point of first refl."),"",5);

                        gr->Rotate(60,40+ii*20);
                        gr->Alpha(true);

                        gr->SetRanges(xrang_,xrang__,yrang_,yrang__,zrang_,zrang__);
                        gr->SetRange('c',0,1);
                        QByteArray botbot = "<0";
                        QByteArray endend = ">0";

                        for(i = 0; i < countCapillaries; ++i)
                        {
                            gr->CutOff(("("+capillaryVec[i]->getFBottomOfCap()+botbot+") | (" + capillaryVec[i]->getFEndOfCap()+endend+")").data());
                            gr->Fill(grtemp,capillaryVec[i]->getF0().data(),"");
                            //      qDebug() << "AAAAAAAAAAAAAAAAAA" << f0_0 << f0_1;
                            //            gr->Surf3(0.0,grtemp,colors[i],"");
                            try {
                                gr->Surf3(0.0,grtemp,colors[i],"");
                                gr->CutOff("");
                            } catch (...) {
                                gr->Surf3(0.0,grtemp,"i","");
                                gr->CutOff("");
                            }

                            //            gr->Fill(grtemp,capillaryVec[i]->getFBottomOfCap().data(),"");
                            //            gr->Surf3(0.0,grtemp,"W");
                            //            gr->Fill(grtemp,capillaryVec[i]->getFEndOfCap().data(),"");
                            //            gr->Surf3(0.0,grtemp,"W");
                        }
                        gr->Alpha(false);
                        gr->Dots(aa,bb,cc,qq,"Nn{c4}{e9}qrR");
                        if(flagDEBUGKkolvomaxrays)
                        {
                            gr->AddLegend(QByteArray("max. number of refl.: "+QByteArray::number(totalTempMax)).constData(),"");
                        }
                        gr->Colorbar(">");
                        gr->Label('x',"x, "+lengthUnit,0);
                        gr->Label('y',"y, "+lengthUnit,0);
                        gr->Label('z',"z, "+lengthUnit,0);
                        gr->Axis("xyz"); gr->Box();  gr->Grid();
                        if(flagDEBUGKkolvomaxrays)
                        {
                            gr->Legend();
                        }
                        gr->EndFrame();
                    }
            }
            gr->CloseGIF();
            return gr->GetNumFrame();
        }
        else
        {
//            gr->ResetFrames();
            for(j = (countNumOfReflections-1); j < countNumOfReflections; ++j)
            {
                aa.Set(testCoefOfReflxx[0].toStdVector());
                bb.Set(testCoefOfReflyy[0].toStdVector());
                cc.Set(testCoefOfReflzz[0].toStdVector());
                qq.Set(testCoefOfReflcolor[j].toStdVector());

//                gr->NewFrame();

//                gr->Title(("Coeficient of reflection:\ncolor - val. after "+QByteArray::number((int)j+1)+" refl.;\nposition - point of first refl."),"",5);

                gr->Rotate(rxrotate,rzrotate,ryrotate);
                gr->Alpha(true);

                gr->SetRanges(xrang_,xrang__,yrang_,yrang__,zrang_,zrang__);
                gr->SetRange('c',0,1);
                QByteArray botbot = "<0";
                QByteArray endend = ">0";

                for(i = 0; i < countCapillaries; ++i)
                {
                    gr->CutOff(("("+capillaryVec[i]->getFBottomOfCap()+botbot+") | (" + capillaryVec[i]->getFEndOfCap()+endend+")").data());
                    gr->Fill(grtemp,capillaryVec[i]->getF0().data(),"");
                    //      qDebug() << "AAAAAAAAAAAAAAAAAA" << f0_0 << f0_1;
                    //            gr->Surf3(0.0,grtemp,colors[i],"");
                    try {
                        gr->Surf3(0.0,grtemp,colors[i],"");
                        gr->CutOff("");
                    } catch (...) {
                        gr->Surf3(0.0,grtemp,"i","");
                        gr->CutOff("");
                    }

                    //            gr->Fill(grtemp,capillaryVec[i]->getFBottomOfCap().data(),"");
                    //            gr->Surf3(0.0,grtemp,"W");
                    //            gr->Fill(grtemp,capillaryVec[i]->getFEndOfCap().data(),"");
                    //            gr->Surf3(0.0,grtemp,"W");
                }
                gr->Alpha(false);
                gr->Dots(aa,bb,cc,qq,"Nn{c4}{e9}qrR");
//                qDebug() << "TTTTTTT" << testCoefOfReflcolor[j] << "num" << testCoefOfReflcolor[j].size();
                if(flagDEBUGKkolvomaxrays)
                {
                    gr->AddLegend(QByteArray("max. number of refl.: "+QByteArray::number(totalTempMax)).constData(),"");
                }
                gr->Colorbar(">Nn{c4}{e9}qrR");
                gr->Label('x',"x, "+lengthUnit,0);
                gr->Label('y',"y, "+lengthUnit,0);
                gr->Label('z',"z, "+lengthUnit,0);
                gr->Axis("xyz"); gr->Box();  gr->Grid();
                if(flagDEBUGKkolvomaxrays)
                {
                    gr->Legend();
                }
//                gr->EndFrame();
//                return gr->GetNumFrame();

                gr->WritePNG(QByteArray("02-testcoefofrefl-"+nameProj+".png").data());
            }
        }
        return 0;
    }
    else if (flagDraw == 5)//total intensity
    {
        long i,j;
        long n = screenVec[0]->isMeshScreenX();
        mglData a(n,n),x(n),y(n);

        double tempMinX, tempMinY, tempMinA;
        double tempMaxX, tempMaxY, tempMaxA;

        tempMinX = screenVec[0]->pointsXscreen.value(0);
        tempMinY = screenVec[0]->pointsYscreen.value(0);
        tempMaxX = screenVec[0]->pointsXscreen.value(0);
        tempMaxY = screenVec[0]->pointsYscreen.value(0);
        for (j =0; j<n; ++j )
        {
            if(screenVec[0]->pointsXscreen.value(j) < tempMinX)
                tempMinX = screenVec[0]->pointsXscreen.value(j);
            if(screenVec[0]->pointsYscreen.value(j) < tempMinY)
                tempMinY = screenVec[0]->pointsYscreen.value(j);
            if(screenVec[0]->pointsXscreen.value(j) > tempMaxX)
                tempMaxX = screenVec[0]->pointsXscreen.value(j);
            if(screenVec[0]->pointsYscreen.value(j) > tempMaxY)
                tempMaxY = screenVec[0]->pointsYscreen.value(j);
        }
        tempMaxA = std::sqrt(std::pow(zcRe[0],2) + std::pow(zcIm[0],2));
        tempMinA = std::sqrt(std::pow(zcRe[0],2) + std::pow(zcIm[0],2));
        for (i = 0; i < screenVec[0]->isMeshScreenXY();++i)
        {
            if (std::sqrt(std::pow(zcRe[i],2) + std::pow(zcIm[i],2)) > tempMaxA)
                tempMaxA = std::sqrt(std::pow(zcRe[i],2) + std::pow(zcIm[i],2));
            if (std::sqrt(std::pow(zcRe[i],2) + std::pow(zcIm[i],2)) < tempMinA)
                tempMinA = std::sqrt(std::pow(zcRe[i],2) + std::pow(zcIm[i],2));
        }
        for (j=0;j<n;j++)
        {
            for(i=0;i<n;i++)
            {
                a.a[i+n*j] = std::sqrt(std::pow(zcRe[i+n*j],2) + std::pow(zcIm[i+n*j],2));
            }
            x.a[j] = screenVec[0]->pointsXscreen[j];
            y.a[j] = screenVec[0]->pointsYscreen[j];
        }

//        a.Norm();
//        gr->Title("Density distribution of the photons");
        if(flagWithSetRanges)
        {
            dialogranges->setParams(tempMinX,tempMaxX,
                                    tempMinY,tempMaxY,
                                    0,tempMaxA);
            dialogranges->exec();
            gr->Rotate(rxrotate,rzrotate,ryrotate);
            gr->SetRange('x',xrang_,xrang__);
            gr->SetRange('y',yrang_,yrang__);
            gr->SetRange('z',zrang_-1e-13,zrang__+1e-13);
            gr->SetRange('c',zrang_-1e-13,zrang__+1e-13);
//            gr->SetRanges(x,y,a);

        }
        else
        {
            gr->Rotate(0,0);
            if(flagDebugDrawTitle)
            {
                gr->SetRange('x',-2,2);
                gr->SetRange('y',-2,2);
                gr->SetRange('z',0,7001);
                gr->SetRange('c',0,7001);
            }
            else
            {
                gr->SetRanges(x,y,a);
            }
        }

        //gr->Alpha(true);
//        a.Norm();
        gr->Surf(x,y,a,"Nn{c4}{e9}qrR");
        gr->Label('x',"x, "+lengthUnit,1);
        gr->Label('y',"y, "+lengthUnit,1);
//        gr->Label('z',"I",1); // (kg/Gs^3)
        gr->Colorbar();
        gr->Box(); gr->Axis(); gr->Grid();
        gr->WritePNG("02-totalIntensity-"+nameProj+".png");
        return 0;
    }
    else if (flagDraw == 71)//many spectrum
    {
        //временно вставим сюда:
        countR_0 = 1;
        absV.clear(); nu_i.clear();
        nu_i_imput.clear(); //ЗАЧЕМ???????????????????

        k=100;
        for(int l=0; l<k ;++l)
        {
            nu_i.push_back((30000+l*100)*1e6/4.135667516);
            nu_i_imput.push_back((32900+l*50)*1e6/4.135667516);//ЗАЧЕМ????????????????????
            absV.push_back(1e-9);//считаем, что это не модуль квадрат спектра, а просто спектр.
        }
        //временно вставим это сюда...


        int n = nu_i.size();
        mglData ay(n);
        mglData ax(n);
//        countR_0=1;
        qDebug() << "WWWWWWWWWWWWWWWWWWWWWWWWW" <<n;
        for(int m=0;m<countR_0;++m)
        {
            gr->NewFrame();
            qDebug() << spectrumTotalR_0Nu[m];
            ay.Set(spectrumTotalR_0Nu[m].toStdVector());
            qDebug() << "WWWWWWWWWWWWWWWWWWWWWWWWW" <<n;
            for(int i=0;i<n;++i)
                ax.a[i]=nu_i[i]*4.135667516e-6;
            ay.Norm();

            gr->SetRanges(ax,ay);
                        //nu_i.first()*4.135667516e-6,nu_i.last()*4.135667516e-6
    //                    sourceEnvir->getAverageFrequency()-2*(1/sourceEnvir->getTau())
    //                    ,sourceEnvir->getAverageFrequency()+2*(1/sourceEnvir->getTau())
//                        ,1,1);

//            QByteArray asdf = "P_1 = "+QByteArray::number(spectrumSumInputIntensWithoutNonRefl[m],'g',10)+"*P_0\n"
//                   + "P_2 = "+ QByteArray::number(spectrumSumIntensScreen[m],'g',10)+"*P_0\n"
//                    +"l_{source} = 0.5;"+"L = "+QByteArray::number(tempLengthCap[m],'g',10)
//                    +"\nR = 1;" + " r_0 = 0.000001";

//            gr->Puts(mglPoint(33000,1.2),asdf.data());
            gr->Plot(ax,ay,"oG");
            gr->Line(mglPoint(33000,1.0),mglPoint(33000,0.0),"r");
            gr->Axis(); gr->Grid("xy","b");
            gr->Label('x',"\\omega, eV",0);

            gr->Label('y',"",0);
            gr->WritePNG("spectrum"+QByteArray::number(m)+".png");

            gr->EndFrame();
        }

//        if(flagWithAnimation)
//        {
//            //        gr->SetQuality(1);
//            gr->ResetFrames();
//            char str[32];
//            gr->StartGIF("animationSpectrum.gif");

//            aa.Set(testCoefOfReflxx[0].toStdVector());
//            bb.Set(testCoefOfReflyy[0].toStdVector());
//            cc.Set(testCoefOfReflzz[0].toStdVector());
//            qq.Set(testCoefOfReflcolor[j].toStdVector());


//            for(int ii=0; ii<10;++ii)
//                for(int jj = 0; jj < 4; ++jj)
//                {
//                    gr->NewFrame();
//                    gr->Title(("Coeficient of reflection:\ncolor - val. after "+QByteArray::number((int)j+1)+" refl.;\nposition - point of first refl."),"",5);

//                    gr->Rotate(60,40+ii*20);

//                    gr->SetRanges(xrang_,xrang__,yrang_,yrang__,zrang_,zrang__);

//                    gr->Alpha(false);
//                    gr->Dots(aa,bb,cc,qq,"3BbcyrR");

//                    gr->Colorbar(">");
//                    gr->Label('x',"x, m*"+QByteArray::number(sourceEnvir->getLengthUnitMult(),'e',0),0);
//                    gr->Label('y',"y, m*"+QByteArray::number(sourceEnvir->getLengthUnitMult(),'e',0),0);
//                    gr->Label('z',"z, m*"+QByteArray::number(sourceEnvir->getLengthUnitMult(),'e',0),0);
//                    gr->Axis("xyz"); gr->Box();  gr->Grid();
//                    gr->EndFrame();
//                }
//        }
//        gr->CloseGIF();
          return gr->GetNumFrame();
//        return 0;
    }
    else if (flagDraw == 70)//test spectrum - DEBUG. Без окошка управления scale
    {
        if(true) //тор
        {
            int m =0;
            this->spectrumSetTor(m);
        }

        int maxCountPoints = 0;
        if(countRaysTestDraw > 0)
        {
            if(!flagDEBUGParallel)
                this->sendRay();
            else
            {
                QFuture<void> future = QtConcurrent::run(this, &Coherence::sendRayMNOGOPOTOCHNIJ, 0);
//                QFuture<bool> future1 = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, i+1);
                future.waitForFinished();
//                future1.waitForFinished();
//                this->sendRayMNOGOPOTOCHNIJ(ne);
                this->setMapResultRay(0);
            }
            dialogranges->setParams(//-screenVec[0]->isEdgeOfScreenSquare()/2.0,screenVec[0]->isEdgeOfScreenSquare()/2.0,
                                //-screenVec[0]->isEdgeOfScreenSquare()/2.0,screenVec[0]->isEdgeOfScreenSquare()/2.0,
                                    -5e-6,5e-6
                                   ,-5e-6,5e-6
//                                   ,pointsZsou-5e-6
                                   ,0.0 ,pointsZscr[0]+5e-6
                                   , 60.0, 0.0, 40.0);
            dialogranges->exec();
        }
        else
        {
            mglExpr a(screenVec[0]->getFScreen().data());
            dialogranges->setParams(//-screenVec[0]->isEdgeOfScreenSquare()/2.0,screenVec[0]->isEdgeOfScreenSquare()/2.0,
                                //-screenVec[0]->isEdgeOfScreenSquare()/2.0,screenVec[0]->isEdgeOfScreenSquare()/2.0,
                                    -5e-6,5e-6
                                   ,-5e-6,5e-6
                                   ,sourceEnvir->getZ_0_()-5e-6, std::fabs(a.Eval(0.0,0.0,0.0))+5e-6);
            dialogranges->exec();
        }
//        gr->Title("","",5);
        gr->Rotate(rxrotate,rzrotate,ryrotate);
//        gr->Rotate(60,40);
        gr->Alpha(true);
        gr->SetRanges(xrang_,xrang__,yrang_,yrang__,zrang_,zrang__);

        mglData grtemp(100,100,100);
        for(i = 0; i < countRaysTestDraw; ++i)
        {

            if(!flagDEBUGParallel)
                this->sendRay();
            else
            {
                QFuture<void> future = QtConcurrent::run(this, &Coherence::sendRayMNOGOPOTOCHNIJ, 0);
//                QFuture<bool> future1 = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, i+1);
                future.waitForFinished();
//                future1.waitForFinished();
//                this->sendRayMNOGOPOTOCHNIJ(ne);
                this->setMapResultRay(0);
            }
            int countPoints = pointsX.size();
            if(maxCountPoints < countPoints)
            {
                maxCountPoints = countPoints;
            }
            qDebug() << "       Ray passed through the capillary, number of reflections:" << countPoints;
//            qDebug() << anglesGlacing;
//            if(!anglesGlacing.empty())
//            for( int ll=0; ll < 1/*anglesGlacing.size()*/; ++ll)
//                qDebug() << capillaryVec[flagNumCap]->getCoefOfRefl(anglesGlacing[ll],sourceEnvir->getAverageFrequency());

            mglData x(countPoints+2), y(countPoints+2), z(countPoints+2);
            x.a[0] = pointRsou[0];
            y.a[0] = pointRsou[1];
            z.a[0] = pointRsou[2];
            for (j= 0; j < countPoints; ++j)
            {
                 x.a[j+1] = pointsX[j];
                 y.a[j+1] = pointsY[j];
                 z.a[j+1] = pointsZ[j];
            }
            x.a[countPoints+1] = pointsXscr[0];
            y.a[countPoints+1] = pointsYscr[0];
            z.a[countPoints+1] = pointsZscr[0];
            gr->Plot(x,y,z,"2i");
        }

        QByteArray botbot = "<0";
        QByteArray endend = ">0";


        for(i = 0; i < countCapillaries; ++i)
        {
            gr->CutOff(("("+capillaryVec[i]->getFBottomOfCap()+botbot+") | (" + capillaryVec[i]->getFEndOfCap()+endend+")").data());
            gr->Fill(grtemp,capillaryVec[i]->getF0().data(),"");
            //      qDebug() << "AAAAAAAAAAAAAAAAAA" << f0_0 << f0_1;
//            gr->Surf3(0.0,grtemp,colors[i],"");
            try {
                gr->Surf3(0.0,grtemp,colors[i],"");
                gr->CutOff("");
            } catch (...) {
                gr->Surf3(0.0,grtemp,"i","");
                gr->CutOff("");
            }
            gr->CutOff("");
//            gr->Fill(grtemp,capillaryVec[i]->getFBottomOfCap().data(),"");
//            gr->Surf3(0.0,grtemp,"W");
//            gr->Fill(grtemp,capillaryVec[i]->getFEndOfCap().data(),"");
//            gr->Surf3(0.0,grtemp,"W");
        }



        for(i=0; i < countScreens; ++i)
        {
            gr->Fill(grtemp,screenVec[i]->getFScreen().data(),"");
            gr->Surf3(0.0,grtemp,"c","");
        }
//        gr->FSurf(QByteArray::number(0.0,'g',15).data(),"W","");
//        gr->FSurf(QByteArray::number(200.0,'g',15).data(),"W","");
//        gr->FSurf(QByteArray::number(400.0,'g',15).data(),"c","");
        gr->AddLegend(QByteArray("max. number of refl.: "+QByteArray::number(maxCountPoints)).constData(),"");
//        gr->Puts((QByteArray("max number of refl.: ")+QByteArray::number(maxCountPoints)));
//        gr->Puts(mglPoint(0.1,0.1,zrang__+2.0),(QByteArray("max number of refl.: ")+QByteArray::number(maxCountPoints)));
        gr->Label('x',"x, "+lengthUnit,1);
        gr->Label('y',"y, "+lengthUnit,1);
        gr->Label('z',"z, "+lengthUnit,1);
        gr->Box(); gr->Axis(); gr->Grid(); gr->Legend();

        return 0;
    }
    else if (flagDraw == 8)//interference term zzz
    {
        long int i,j, n=screenVec[0]->isMeshScreenX();
        mglData a(n,n),x(n),y(n),atc(n,n);

        for (j=0;j<n;j++)
        {
            for(i=0;i<n;i++)
            {
                atc.a[i+n*j] = zcRe[i+n*j];
            }
            x.a[j] = screenVec[0]->pointsXscreen[j];
            y.a[j] = screenVec[0]->pointsYscreen[j];
        }
//        gr->Title("Interference term");
//        atc.Norm(-1,1);
        if(flagWithSetRanges)
        {
            dialogranges->setParams(x.Minimal(),x.Maximal(),
                                    y.Minimal(),y.Maximal(),
                                    atc.Minimal(),atc.Maximal());
            dialogranges->exec();
            gr->SetRanges(xrang_,xrang__,yrang_,yrang__,zrang_,zrang__);
            gr->Rotate(rxrotate,rzrotate,ryrotate);
        }
        else
        {
            gr->Rotate(60,10);
            gr->SetRanges(x,y,atc);
        }
        //gr->Alpha(true);
//        a.Norm();
        gr->Surf(x,y,atc,"Nn{c4}{e9}qrR");
        gr->Label('x',"x, "+lengthUnit,1);
        gr->Label('y',"y, "+lengthUnit,1);
//        gr->Label('z',"I",1); // (kg/Gs^3)
        gr->Colorbar(">Nn{c4}{e9}qrR");
        gr->Box(); gr->Axis(); gr->Grid();
        gr->WritePNG("interferTerm_"+nameProj+".png");
        return 0;
    }

    else if (flagDraw == 11)//многокучный метод
    {
//        for(int p = 100; p < DEBUGCOUNTIMG; ++p)//то же подправить в методе вычисления
//        {
//            nameProj = QByteArray::number(p);
//            this->LoadCoherence();
            long int i,j, n=screenVec[0]->isMeshScreenX();

            mglData a(n,n),x(n),y(n);

            for (j=0;j<n;j++)
            {
                for(i=0;i<n;i++)
                {
                    if(znc[i+n*j] != 0)
                        a.a[i+n*j] = zcRe[i+n*j]/znc[i+n*j];
                }
                x.a[j] = screenVec[0]->pointsXscreen[j];
                y.a[j] = screenVec[0]->pointsYscreen[j];
            }
//            gr->StartGIF("animMGL.gif");
            //        gr->NewFrame();
            QByteArray nameImg = "coherent" + nameProj + ".png";
            mglExpr expr1(capillaryVec[0]->getFEndOfCap());
            mglExpr expr2(screenVec[0]->getFScreen());
//            gr->NewFrame();
            gr->SetSize(1024,768);
            gr->SetFontSizePT(9);
//            gr->SetTicks('x',0.25);
//            gr->SetTicks('y',0.25);


            gr->Title(("Degree of coherence\n d_2 = "
                       +QByteArray::number(0.01*(std::fabs(expr2.Eval(0,0,0))-std::fabs(expr1.Eval(0,0,0))))
//                       +QByteArray::number(2*(p-100))
                       +" mm").data());
            //        a.Norm(-1,1);
            gr->SetRanges(x.Minimal(),x.Maximal(),
                          y.Minimal(),y.Maximal(),
                          -1,1);
//            gr->SetRange('c',-1,1);
            gr->SetTicks('x',0.5,10);
            gr->SetTicks('y',0.5,10);
//            gr->SetTicks('z',0.25);
//            gr->SetTicks('c',0.5);
            gr->Rotate(0,0,0);
//            gr->SetAlphaDef(0.99);
//            gr->Alpha(true);

//            gr->Surf(x,y,a,"Bbu{e7}qrR");
            gr->Surf(x,y,a,"Nn{c4}{e9}qrR");

            gr->Label('x',"x, "+lengthUnit,1);
            gr->Label('y',"y, "+lengthUnit,1);
//            gr->Label('z',"",1); // (kg/Gs^3)
//            gr->Colorbar(">Bbu{e7}qrR");
            gr->Colorbar(">Nn{c4}{e9}qrR");

            gr->Box(); gr->Axis(); gr->Grid("xyz","k","");

            //        std::fabs(expr2.Eval(0,0,0))-std::fabs(expr1.Eval(0,0,0));

            gr->WritePNG(nameImg.data());
//            gr->Finish();
//            gr->EndFrame();

//        }
            return 0;
//        return gr->GetNumFrame();
    }
    return 0;
}

void Coherence::slotSetRanges()
{
    xrang_ = dialogranges->getXR_();
    yrang_ = dialogranges->getYR_();
    zrang_ = dialogranges->getZR_();
    xrang__ = dialogranges->getXR__();
    yrang__ = dialogranges->getYR__();
    zrang__ = dialogranges->getZR__();
    rxrotate = dialogranges->getRx();
    ryrotate = dialogranges->getRy();
    rzrotate = dialogranges->getRz();
//    qDebug() << xrang_ << xrang__ << yrang_ << yrang__ << zrang_ << zrang__;
}

bool Coherence::tryLoadTotal() const
{
    bool ret = false;
    QByteArray add;
    add = "totalDate" + nameProj + ".dat";
    QFile fileTotalDate(add.data());
    if (fileTotalDate.open(QIODevice::ReadOnly))
    {
        ret = true;
    }
    fileTotalDate.close();
    return (ret);
}

int Coherence::LoadTotal(/*QByteArray sadd*/)
{
    int ret = 1;
    QByteArray add;
//    if( sadd == "")
//    {
    add = "totalDate" + nameProj + ".dat";
//    qDebug() << add;
//    }
//    else
//    {
//        add = "totalDate" + sadd + ".dat";
//    }
    try {
        QFile fileTotalDate(add.data());
        if (fileTotalDate.open(QIODevice::ReadOnly))
        {
            this->clearTotal();
            QDataStream stream(&fileTotalDate);
            stream
                    >> totalLength      //групп длин лучей для каждого экрана из списка... в каждой точке на экране таких групп много...
                    >> totalNumOfRefl
                    >> totalAngles      //здесь для каждой длины луча - вектор скользящих углов.
                    >> totalPointRSou   //для каждого экрана из списка для каждой точки на экране - для каждого луча - точка на источнике
                    >> totalPointRScr   //для каждого экрана из списка для каждой точки на экране - для каждого луча - точка на источнике
                    >> totalNumOfCap    //для каждого экрана из списка для каждой точки на экране - для каждого луча - номер капилляра прохождения
//                    >> totalAverCoefOfRefl//здесь для каждой длины луча - суммарный коэффициенты отражения для средней частоты
//                    >> totalPointsX//здесь для каждой длины луча - вектор X координат
//                    >> totalPointsY//здесь для каждой длины луча - вектор Y координат
//                    >> totalPointsZ//здесь для каждой длины луча - вектор Z координат
                    >> totalMaxTheta    //здесь максимальный угол отклонения по всем лучам в точке на экране
                    >> totalAverTheta   //минимальный угол отклонения по всем лучам в точка не экране
                    >> totalAmpl
                    >> totalPhase;
            ret = 0;
        }
        fileTotalDate.close();
    } catch (...)
    {
        qDebug() << QObject::trUtf8("File was not loaded!");
    }
    return ret;
}

int Coherence::LoadCoherence(/*QByteArray sadd*/)
{
    znc.clear();
//    zcminus.clear();
    zcIm.clear();
    zcRe.clear();

    int ret = 1;
    QByteArray add;
//    if( sadd == "")
//    {
    add = "totalCoherence" + nameProj + ".dat";
//    }
//    else
//    {
//        add = "totalCoherence" + sadd + ".dat";
//    }
    try {
        QFile fileTotalDate(add.data());
        if (fileTotalDate.open(QIODevice::ReadOnly))
        {
//            this->clearTotal(); загружается после loadTotal, значит обнулять уже не нужно!!! Иначе углы все обнулишь!
            QDataStream stream(&fileTotalDate);
            stream
                            >> znc
//                            >> zcminus
                            >> zcIm
                            >> zcRe;
            ret = 0;
        }
        fileTotalDate.close();
    } catch (...)
    {
        qDebug() << QObject::trUtf8("File was not loaded!");
    }
    return ret;
}

int Coherence::LoadSpectrumExp(/*QByteArray sadd*/)
{
    int ret = 1;
    QByteArray add;
//    if( sadd == "")
//    {
    add = "totalSpectrum" + nameProj + ".dat";
//    }
//    else
//    {
//        add = "totalSpectrum" + sadd + ".dat";
//    }
    try {
        QFile fileTotalDate(add.data());
        if (fileTotalDate.open(QIODevice::ReadOnly))
        {
//            this->clearTotal(); загружается после loadTotal, значит обнулять уже не нужно!!! Иначе углы все обнулишь!
            QDataStream stream(&fileTotalDate);
            stream
                    >> spectrumScreen //для каждого r_0, для каждой точки i на экране, для каждой nu_i вектор значения спектра(для каждой точки).
                    >> spectrumTotalR_0Nu //для каждого r_0 для каждой Nu значение
                    >> spectrumIntensScreen //для каждого r_0, для каждой точки i на экране значение интенсивности
                    >> spectrumSumIntensScreen //для каждого r_0, суммарная интенсивность на экране
                    >> spectrumSumInputIntensWithoutNonRefl //для каждого r_0, суммарная входная интенсивность без просвета
                    >> spectrumSumInputIntensWithNonRefl;//с пройденной без отражения
            ret = 0;
        }
        fileTotalDate.close();
    } catch (...)
    {
        qDebug() << QObject::trUtf8("File totalSpectrum*.dat was not loaded!");
    }
    return ret;
}

int Coherence::LoadDegOfCoh(/*QByteArray sadd*/)
{
    xgeneral.clear();
    ygeneral.clear();
    zgeneral.clear();
    zgeneralIm.clear();

    zzzRe.clear();
    zzzIm.clear();
    zzzNORM.clear();

    zzzTotal.clear();
    zzzNORMTotal.clear();

    int ret = 1;
    QByteArray add;
//    if( sadd == "")
//    {
    add = "totalDegOfCoh" + nameProj + ".dat";
//    }
//    else
//    {
//        add = "totalCoherence" + sadd + ".dat";
//    }
    try {
        QFile fileTotalDate(add.data());
        if (fileTotalDate.open(QIODevice::ReadOnly))
        {
//            this->clearTotal(); загружается после loadTotal, значит обнулять уже не нужно!!! Иначе углы все обнулишь!
            QDataStream stream(&fileTotalDate);
            stream
                            >> xgeneral
                            >> ygeneral
                            >> zgeneral
                            >> zgeneralIm
                            >> zzzRe
                            >> zzzIm
                            >> zzzNORM
                            >> zzzTotal
                            >> zzzNORMTotal;
            ret = 0;
        }
        fileTotalDate.close();
    } catch (...)
    {
        qDebug() << QObject::trUtf8("File totalCoherence*.dat was not loaded!");
    }
    return ret;
}

int Coherence::SaveTotal(/*QByteArray sadd*/) const
{
    int ret = 1;
    QByteArray add;
//    if( sadd == "")
//    {
    add = "totalDate" + nameProj + ".dat";
//    }
//    else
//    {
//        add = "totalDate" + sadd + ".dat";
//    }
    QFile fileTotalDate(add.data());
    try {
        if (fileTotalDate.open(QIODevice::WriteOnly))
        {
            QDataStream stream(&fileTotalDate);
            stream
                    << totalLength      //групп длин лучей для каждого экрана из списка... в каждой точке на экране таких групп много...
                    << totalNumOfRefl
                    << totalAngles      //здесь для каждой длины луча - вектор скользящих углов.
                    << totalPointRSou   //для каждого экрана из списка для каждой точки на экране - для каждого луча - точка на источнике
                    << totalPointRScr //для каждого экрана из списка для каждой точки на экране - для каждого луча - точка на экране
                    << totalNumOfCap    //для каждого экрана из списка для каждой точки на экране - для каждого луча - номер капилляра прохождения
//                    << totalAverCoefOfRefl//здесь для каждой длины луча - суммарный коэффициенты отражения для средней частоты
//                    << totalPointsX//здесь для каждой длины луча - вектор X координат
//                    << totalPointsY//здесь для каждой длины луча - вектор Y координат
//                    << totalPointsZ//здесь для каждой длины луча - вектор Z координат
                    << totalMaxTheta    //здесь максимальный угол отклонения по всем лучам в точке на экране
                    << totalAverTheta   //минимальный угол отклонения по всем лучам в точка не экране
                    << totalAmpl
                    << totalPhase;
            ret = 0;
        }
        fileTotalDate.close();
    } catch (...)
    {
        qDebug() << QObject::trUtf8("File totalDate*.dat was not saved!");
    }
    return ret;
}

int Coherence::SaveIntens(/*QByteArray sadd*/) const
{
    int ret = 1;
    QByteArray add;
//    if( sadd == "")
//    {
    add = "totalCoherence" + nameProj + ".dat";
//    }
//    else
//    {
//        add = "totalCoherence" + sadd + ".dat";
//    }
    QFile fileTotalDate(add.data());
    try {
        if (fileTotalDate.open(QIODevice::WriteOnly))
        {
            QDataStream stream(&fileTotalDate);
            stream
                    << znc
//                    << zcminus
                    << zcIm
                    << zcRe;
            ret = 0;
        }
        fileTotalDate.close();
    } catch (...)
    {
        qDebug() << QObject::trUtf8("File totalCoherence*.dat was not loaded!");
    }
    return ret;
}

int Coherence::SaveSpectrumExp(/*QByteArray sadd*/) const
{
    int ret = 1;
    QByteArray add;
//    if( sadd == "")
//    {
    add = "totalSpectrum" + nameProj + ".dat";
//    }
//    else
//    {
//        add = "totalSpectrum" + sadd + ".dat";
//    }
    QFile fileTotalDate(add.data());
    try {
        if (fileTotalDate.open(QIODevice::WriteOnly))
        {
            QDataStream stream(&fileTotalDate);
            stream
                    << spectrumScreen //для каждого r_0, для каждой точки i на экране, для каждой nu_i вектор значения спектра(для каждой точки).
                    << spectrumTotalR_0Nu //для каждого r_0 для каждой Nu значение
                    << spectrumIntensScreen //для каждого r_0, для каждой точки i на экране значение интенсивности
                    << spectrumSumIntensScreen //для каждого r_0, суммарная интенсивность на экране
                    << spectrumSumInputIntensWithoutNonRefl //для каждого r_0, суммарная входная интенсивность без просвета
                    << spectrumSumInputIntensWithNonRefl;//с пройденной без отражения
            ret = 0;
        }
        fileTotalDate.close();
    } catch (...)
    {
        qDebug() << QObject::trUtf8("File totalSpectrum*.dat was not loaded!");
    }
    return ret;
}

int Coherence::SaveDegOfCoh(/*QByteArray sadd = ""*/) const
{
    int ret = 1;
    QByteArray add;
//    if( sadd == "")
//    {
    add = "totalDegOfCoh" + nameProj + ".dat";
//    }
//    else
//    {
//        add = "totalCoherence" + sadd + ".dat";
//    }
    QFile fileTotalDate(add.data());
    try {
        if (fileTotalDate.open(QIODevice::WriteOnly))
        {
            QDataStream stream(&fileTotalDate);
            stream
                    << xgeneral
                    << ygeneral
                    << zgeneral
                    << zgeneralIm
                    << zzzRe
                    << zzzIm
                    << zzzNORM
                    << zzzTotal
                    << zzzNORMTotal;
            ret = 0;
        }
        fileTotalDate.close();
    } catch (...)
    {
        qDebug() << QObject::trUtf8("File totalDegOfCoh*.dat was not loaded!");
    }
    return ret;
}

//int Coherence::SaveCoherence(QByteArray nameFileTotalZNC, QByteArray nameFileTotalZC, QByteArray nameFileTotalZZZ)
//{
  /*   QFile fileZNC(nameFileTotalZNC.data());
    // QFile fileTotalLength(nameFileTotalLength.data());
     if (fileZNC.open(QIODevice::WriteOnly))
     {
         QDataStream stream(&fileZNC);
         stream << znc;
     }
     else
     {
         qDebug() << "#####Error#####";
     }
     fileZNC.close();

     QFile fileZC(nameFileTotalZC.data());
//     QFile fileZC(nameFileTotalAngles.data());
     if (fileZC.open(QIODevice::WriteOnly))
     {
         QDataStream stream(&fileZC);
         stream << zc;
     }
     else
     {
         qDebug() << "#####Error#####";
     }
     fileZC.close();

     QFile fileZZZ(nameFileTotalZZZ.data());
//     QFile fileZZZ(nameFileTotalAngles.data());
     if (fileZZZ.open(QIODevice::WriteOnly))
     {
         QDataStream stream(&fileZZZ);
         stream << zzz;
     }
     else
     {
         qDebug() << "#####Error#####";
     }
     fileZZZ.close();*/
//     return 0;
//}

void Coherence::setLengthOfRay()
{
    int i = 0;//разная длина для разных экранов - логично....
//    qDebug() << "countOfScreens" << countScreens;
    lengthOfRay[0] = 0;
    numOfRefl = pointsX.size();//   используется не только здесь, поэтому параметр всего класса

    if (pointsX.isEmpty())
    {
        for(i = 0; i < countScreens; ++i )
        {
            lengthOfRay[i] += std::sqrt(std::pow(pointsXscr.at(i) - pointRsou[0],2) +
                                        std::pow(pointsYscr.at(i) - pointRsou[1],2) +
                                        std::pow(pointsZscr.at(i) - pointRsou[2],2));
        }
    }
    else
    {
        lengthOfRay[0] += std::sqrt(std::pow(pointsX.at(0)-pointRsou.at(0),2) +
                                    std::pow(pointsY.at(0)-pointRsou.at(1),2) +
                                    std::pow(pointsZ.at(0)-pointRsou.at(2),2));
        for (i = 1; i < numOfRefl; ++i)
        {
            lengthOfRay[0] += std::sqrt(std::pow(pointsX.at(i)-pointsX.at(i-1),2)
                                        + std::pow(pointsY.at(i)-pointsY.at(i-1),2)
                                        + std::pow(pointsZ.at(i)-pointsZ.at(i-1),2));
        }
        for (i = 1; i < countScreens; ++i)
        {
            lengthOfRay[i] = lengthOfRay[0];
        }
        for (i = 0; i < countScreens; ++i)
        {
            lengthOfRay[i] += std::sqrt( std::pow(pointsXscr.at(i)-pointsX.last(),2)
                                         + std::pow(pointsYscr.at(i)-pointsY.last(),2)
                                         + std::pow(pointsZscr.at(i)-pointsZ.last(),2) );
        }
    }
}

void Coherence::sendRay()
{
    bool flagCapture = false;   //если луч не захватился ни одни капилляром, то выбираем новый луч сразу.
    flagNumCap = -1;    //номер капилляра, которым захватился луч
    long i; //счётчик
    int captureRay = -1;

    QVector<double> prevP; prevP.resize(numOfVar);//как ещё можно перейти нормально к предыдущей точке????????????????????????????

//    qDebug() << "SendRay 0:";
    while (captureRay < 0)
    {
        if(mathforcap->rayGoesUp == 1)//если луч летит от источника
        {
            flagCapture = false;
            while (!flagCapture)
            {
                sourceEnvir->getRandomX_0Y_0Z_0(pointRsou.data());//берём рандомную точку на источнике
                sourceEnvir->getRandomPhi_0Theta_0(directingVectorLine); // пока не будем сохранять углы источника
                flagCapture = false;
                prevP = pointRsou;
                for(i = 0; (i < countCapillaries)&&(flagCapture == false); ++i)
                {
                    flagCapture = capillaryVec[i]->tryCaptureSou(prevP.data(),directingVectorLine);
                    if (flagCapture)
                        flagNumCap = i;
                }
//                qDebug() << "Send Ray 1: prevP2:" <<  prevP2[0] << prevP2[1] << prevP2[2];
//                pointsX.push_back(prevP2[0]); pointsY.push_back(prevP2[1]); pointsZ.push_back(prevP2[2]);
                pointsX.clear(); pointsY.clear(); pointsZ.clear();
                //временно добавляем её в начало массива - потом внутри она удалится, а останется в points!sou
                pointsX.push_back(pointRsou.at(0)); pointsY.push_back(pointRsou.at(1)); pointsZ.push_back(pointRsou.at(2));
            }
        }
        else // если rayGoesUp == 0, то только один экран!!!!!!!!!!
        {
            double tempRandPhi_0 = 0; double tempRandTheta_0 = 0; //можно и не обнулять, случайные углы будем получать...
            double tempRandX_0 = 0; double tempRandY_0 = 0; double tempRandZ_0 = 0; //и точки источника
            double prevP2[numOfVar];//точка, куда сохраняется первая точка источника, а потом используется для след. точек
            flagCapture = false;
            while(flagCapture == false)
            {
                screenVec[0]->getRandomX_0Y_0Z_0(tempRandX_0, tempRandY_0, tempRandZ_0);
                screenVec[0]->getRandomPhi_0Theta_0(tempRandPhi_0, tempRandTheta_0);
                pointsXscr[0] = tempRandX_0; pointsYscr[0] = tempRandY_0; pointsZscr[0] = tempRandZ_0;
                pointsX.clear(); pointsY.clear(); pointsZ.clear();
                pointsX.push_back(tempRandX_0); pointsY.push_back(tempRandY_0); pointsZ.push_back(tempRandZ_0);//временно добавляем её в начало массива - потом внутри она удалится, а останется в points!scr
                //                pointsX.push_back(tempRandX_0); pointsY.push_back(tempRandY_0); pointsZ.push_back(tempRandZ_0); //первая точка - экран
                //                anglesGlacing.push_back(0.0); //соответствует точке экрана
                anglesScrPhi[0] = tempRandPhi_0; anglesScrTheta[0] = tempRandTheta_0;
                prevP2[0] = tempRandX_0; prevP2[1] = tempRandY_0; prevP2[2] = tempRandZ_0;
                for(i = 0; (i < countCapillaries)&&(flagCapture == false); ++i)
                {
                    flagCapture = capillaryVec[i]->tryCaptureScr(prevP2,tempRandPhi_0,tempRandTheta_0);
                    if(flagCapture)
                        flagNumCap = i;
                }
                directingVectorLine[0] = std::cos(tempRandPhi_0)*std::sin(tempRandTheta_0);
                directingVectorLine[1] = std::sin(tempRandPhi_0)*std::sin(tempRandTheta_0);
                directingVectorLine[2] = std::cos(tempRandTheta_0);
            }
        }


//        qDebug() << "Send Ray 1: directingVectorLine:" << directingVectorLine[0] << directingVectorLine[1] << directingVectorLine[2];
        if(!flagDEBUGSearchAllPoints)
        {
            captureRay = capillaryVec[flagNumCap]->CalculationOfPassageOfTheBeam(directingVectorLine
                                                                    ,pointsX,pointsY,pointsZ,anglesGlacing,0);//???????????????????????????
        }
        else
        {
            captureRay = capillaryVec[flagNumCap]->CalculationOfPassageOfTheBeamALL(directingVectorLine
                                                                        ,pointsX,pointsY,pointsZ,anglesGlacing);//???????????????????????????
        }
        if(captureRay == 2) //надо искать последнюю точку - на экране
        {
//            qDebug() << "                                   Всё нашли, ищем точку на экране";

            for (i = 0; i < countScreens; ++i)
            {
                if (!pointsX.isEmpty())
                {
                    prevP[0] = pointsX.last();
                    prevP[1] = pointsY.last();
                    prevP[2] = pointsZ.last();
                }
                else
                {
//                    qDebug() << "                                   Луч ни разу не отразился!";
                    prevP = pointRsou;
                }
//                qDebug() << "!@@directingVectorLine" << directingVectorLine[0] << directingVectorLine[1] << directingVectorLine[2];
//                qDebug() << "prevP" << prevP[0] << prevP[1] << prevP[2];
                if( screenVec.at(i)->CalculationOfPassageOfTheBeam(directingVectorLine, prevP.data()
                                                                ,pointsXscr[i],pointsYscr[i],pointsZscr[i]
                                                                ,anglesScrPhi[i],anglesScrTheta[i]) != 0)
                {
//                    qDebug() << QObject::trUtf8("### ERROR Point on the screen was not found!");
                    captureRay = -13;
//                    return true;
                }

            }
        }
        if(captureRay == 3)//последняя точка - на источнике
        {
            if (!pointsX.isEmpty())
            {
//                qDebug() << "                                   Количество отражений от капилляра:" << pointsX.size();
                prevP[0] = pointsX.last();
                prevP[1] = pointsY.last();
                prevP[2] = pointsZ.last();
            }
            else
            {
//                qDebug() << "Луч ни разу не отразился!";
                prevP[0] = pointsXscr[0];
                prevP[1] = pointsYscr[0];
                prevP[2] = pointsZscr[0];
            }
            if ( sourceEnvir->CalculationOfPassageOfTheBeam(directingVectorLine, prevP.data()
                                                            ,pointRsou[0],pointRsou[1],pointRsou[2]) != 0)
            {
//                qDebug() << QObject::trUtf8("### ERROR Point on the source was not found");
                captureRay = -13;
//                return true;
            }
        }

        //луч прошёл по капилляру, если captureRay==0, то всё хорошо... если нет, то луч дефективный
    }
    if (captureRay != -13)
    {

        this->setLengthOfRay();
//        return false;

    }
//    return true;
}//--------------------------------------------луч прогнали и создали:----------------------------------------------
//    double pointsXsou,pointsYsou,pointsZsou;            //999
//    QVector<double> pointsX, pointsY, pointsZ;          //888 контейнеры с точками отражения, первая точка - положение точечного источника
//    QVector<double> pointsXscr, pointsYscr, pointsZscr; //999 точка прихода одного луча для всех экранов
//    double directingVectorLine[MathForCap::numOfVar];   //999 направляющий вектор прямой
//    QVector<double> anglesGlacing;                      //888 углы отражения только на капилляре
//    QVector<double> anglesScrPhi,anglesScrTheta;        //999 углы в точке прихода луча на экране для всех экранов
//    QVector<double> lengthOfRay;                        //888 длины путей лучей для всех экранов

void Coherence::setTotal()
{
    long index;
    int l;

//    totalAngles.clear();?????????????????????????????????????????????????нужно сделать очиститель для всех этих массивов, ибо снова не вызвать их!!!
    for (l = 0; l < countScreens; ++l)
    {
        index = screenVec[l]->getIndexOfLastPoint(pointsXscr[l],pointsYscr[l]);
        totalLength[l][index].push_back(lengthOfRay[l]);
        totalNumOfRefl[l][index].push_back(numOfRefl);

        totalAngles[l][index].push_back(anglesGlacing);
        totalNumOfCap[l][index].push_back(flagNumCap);

        totalPointRSou[l][index].push_back(pointRsou);
        totalPointRScr[l][index].push_back( (QVector<double>() << pointsXscr.at(l) << pointsYscr.at(l) << pointsZscr.at(l) ) );

        totalAmpl[l][index].push_back( sourceEnvir->getAmpl(pointRsou) );
        totalPhase[l][index].push_back(sourceEnvir->getPhase(pointRsou) );

//        if(anglesGlacing.size() == 0) //ПОМНИ ПРО ТО, ЧТО ЕСТЬ И ПУСТЫЕ ВЕКТОРЫ!!!!!
//        qDebug() << "sdfsdfsdfsdfsfsdfsfdffdsdfsfdsfsdfsdfsdfsdf_____" << anglesGlacing << anglesGlacing.size();

//        totalPointsX[l][index].push_back(pointsX); //ЛИШНЮЮ ПАМЯТЬ ЗАНИМАЕТ => не нужно!!!
//        totalPointsY[l][index].push_back(pointsY);
//        totalPointsZ[l][index].push_back(pointsZ);
//        totalAverCoefOfRefl[l][index].push_back(setAverCoefRefl()); // туда же

        totalScrTheta[l][index].push_back(anglesScrTheta.at(l));

        if(anglesScrTheta.at(l) > totalMaxTheta.at(l).at(index))
        {
            totalMaxTheta[l][index] = anglesScrTheta.at(l);
        }
//        if(anglesScrTheta.at(l) < totalMinTheta.at(l).at(index))
//        {
//            totalMinTheta[l][index] = anglesScrTheta.at(l);
//        }
    }
}//----------------------------------------//записали углы и длины в общие итоговы массивы

double Coherence::setAverCoefRefl()
{
    double resultcoef = 1.0;
    long int i;
    long int tempSizeAngles = anglesGlacing.size();

//    totalAverCoefOfRefl.clear();
    for(i = 0; i < tempSizeAngles; ++i)
    {
        resultcoef *= capillaryVec[flagNumCap]->getCoefOfRefl(anglesGlacing[i],sourceEnvir->getAverageFrequency());
    }
    return resultcoef;
}

void Coherence::setTempCoefReflNuI(QVector<double> &anglesGlacingR, QVector<double> &tempR)
{
//    long int i,j;
    QVector<double>::const_iterator itcAngG;

    tempR.clear(); tempR.resize(nu_i.size());

    QVector<double>::const_iterator itcNu_i = nu_i.constBegin();
    QVector<double>::iterator itTempR = tempR.begin();

    for(; itcNu_i != nu_i.end(); ++itcNu_i,++itTempR)
    {
        *itTempR = 1.0;//если не отражался, то так и останется тут с единицей...
        for(itcAngG = anglesGlacingR.constBegin(); itcAngG != anglesGlacingR.constEnd(); ++itcAngG)
        {
            *itTempR *= capillaryVec[flagNumCap]->getCoefOfRefl(*itcAngG,*itcNu_i);
        }
    }
//    qDebug() << sourceEnvir->getZ_0_() << tempR;
}//создаёт tempR

void Coherence::clearTotal()
{
    long int i,j;

    totalAmpl.clear(); totalPhase.clear();
    totalAmpl.resize(countScreens); totalPhase.resize(countScreens);
    totalPointsX.clear(); totalPointsY.clear(); totalPointsZ.clear();
    totalPointsX.resize(countScreens); totalPointsY.resize(countScreens); totalPointsZ.resize(countScreens);
    totalAngles.clear(); totalLength.clear(); totalAverCoefOfRefl.clear();
    totalNumOfRefl.clear();
    totalAngles.resize(countScreens); totalLength.resize(countScreens); totalAverCoefOfRefl.resize(countScreens);
    totalNumOfRefl.resize(countScreens);
    totalMaxTheta.clear(); totalAverTheta.clear(); totalScrTheta.clear();
    totalMaxTheta.resize(countScreens); totalAverTheta.resize(countScreens); totalScrTheta.resize(countScreens);
    totalNumOfCap.clear(); totalPointRSou.clear();
    totalNumOfCap.resize(countScreens); totalPointRSou.resize(countScreens);
    totalPointRScr.clear(); totalPointRScr.resize(countScreens);

    for (i = 0; i < countScreens; ++i)
    {
//        qDebug() << screenVec[i]->isMeshScreenXY();
        totalAmpl[i].resize( screenVec[i]->isMeshScreenXY() ); totalPhase[i].resize( screenVec[i]->isMeshScreenXY() );
        totalPointsX[i].resize( screenVec[i]->isMeshScreenXY() ); totalPointsY[i].resize( screenVec[i]->isMeshScreenXY() ); totalPointsZ[i].resize( screenVec[i]->isMeshScreenXY() );
        totalAngles[i].resize( screenVec[i]->isMeshScreenXY() );
        totalMaxTheta[i].resize( screenVec[i]->isMeshScreenXY() ); totalAverTheta[i].resize( screenVec[i]->isMeshScreenXY() ); totalScrTheta[i].resize( screenVec[i]->isMeshScreenXY() );
        totalLength[i].resize( screenVec[i]->isMeshScreenXY() );
        totalNumOfRefl[i].resize( screenVec[i]->isMeshScreenXY() );
        totalAverCoefOfRefl[i].resize( screenVec[i]->isMeshScreenXY());
        totalNumOfCap[i].resize(screenVec[i]->isMeshScreenXY());
        totalPointRSou[i].resize(screenVec[i]->isMeshScreenXY());
        totalPointRScr[i].resize(screenVec[i]->isMeshScreenXY());
    }

    for (i = 0; i < countScreens; ++i)
    {
        for(j = 0; j < screenVec[i]->isMeshScreenXY(); ++j)
        {
            totalMaxTheta[i][j] = 0.0;
//            totalMinTheta[i][j] = M_PI;
        }
    }
}

int Coherence::DAQ()
{
    qDebug() << "Processing the data...:";
    QTime time; time.start();

    long int i,j;

    QProgressDialog * pprd = new QProgressDialog("Processing the data...", "&Cancel", 0 , countRays);
    pprd->setMinimumDuration(0);
    pprd->setWindowTitle("Please Wait");


    this->clearTotal();

//    double tempSchet = 100./countRays;
    if(!flagDEBUGParallel)
    {

        for(i = 0; i < countRays; ++i)
        {
//            if( ((long)(i*tempSchet)-(long)((i-1)*tempSchet)) > 0.9 )
//                qDebug() << i*tempSchet << " %";
            pprd->setValue(i);
            qApp->processEvents();
            if(pprd->wasCanceled())
            {
                qDebug() << ":...data processing is canceled!    @@@ elapsed time:" << elapsCap(time.elapsed());
                delete pprd;
                return -1;
            }
            this->sendRay();
            this->setTotal();
        }
    }
    else
    {
//        QFuture<void> future;
//        for(i=0;i<countRays;++i)
//        {
//            future = QtConcurrent::run(this, &Coherence::sendRayMNOGOPOTOCHNIJ, i);
//            future.waitForFinished();
//        }
        long tempChislo = countRays - (countRays%kolvoProcDAQ);
        QVector< QFuture<void> > futureVec; futureVec.resize(kolvoProcDAQ);
        for(i = 0; i < tempChislo; i+=kolvoProcDAQ)
        {
            pprd->setValue(i);
            qApp->processEvents();
            if(pprd->wasCanceled())
            {
                qDebug() << ":...data processing is canceled!    @@@ elapsed time:" << elapsCap(time.elapsed());
                delete pprd;
                return -1;
            }
            for(j = i; j < i + kolvoProcDAQ; ++j)
            {

//                if( ((j*tempSchet) - (long)(j*tempSchet)) == 0.)
//                    qDebug() << j*tempSchet << " %";

                futureVec[(j-i)] = QtConcurrent::run(this, &Coherence::sendRayMNOGOPOTOCHNIJ, j);
            }
            for(j = i; j < i + kolvoProcDAQ; ++j)
            {
                futureVec[(j-i)].waitForFinished();
            }
            for(j = i; j < i + kolvoProcDAQ; ++j)
            {
                this->setTotalMNOGOPOTOCHNIJ(j);
            }
        }
        for(j = tempChislo; j < countRays; ++j)
        {
            pprd->setValue(j);
            qApp->processEvents();
            if(pprd->wasCanceled())
            {
                qDebug() << ":...data processing is canceled!   @@@ elapsed time:" << elapsCap(time.elapsed());
                delete pprd;
                return -1;
            }
//            if( ((j*tempSchet)-(long)(j*tempSchet)) == 0.)
//                qDebug() << j*tempSchet << " %";
            futureVec[j-tempChislo] = QtConcurrent::run(this, &Coherence::sendRayMNOGOPOTOCHNIJ, j);
        }
        for(j = tempChislo; j < countRays; ++j)
        {
            futureVec[j-tempChislo].waitForFinished();
        }
        for(j = tempChislo; j < countRays; ++j)
        {
            this->setTotalMNOGOPOTOCHNIJ(j);
        }
    }

    //        tempTotalCoef = cap->GetAndFormTCofRAndCoefsOfRefl();
      //  qDebug() << total;
   //mglQT *mglQT = new mglQT(&capA,"Graph");
   // return mglQT->Run();
    QVector<double>::const_iterator totalscrtheta;
    for(i = 0; i < screenVec[0]->isMeshScreenXY(); ++i)
    {
        for(totalscrtheta = totalScrTheta[0][i].constBegin();
            totalscrtheta != totalScrTheta[0][i].constEnd();
            ++totalscrtheta)
        {
            totalAverTheta[0][i] += *totalscrtheta;
        }
        totalAverTheta[0][i] /= totalScrTheta[0][i].size();
    }
    pprd->setValue(countRays);
    delete pprd;
    qDebug() << ":...Processing is completed!   @@@ elapsed time:" << elapsCap(time.elapsed());
    return 0;
}

int Coherence::mainIntegrationINCOHAmpl()
{
    qDebug() << "mainIntegrationINCOHAmpl:";
    double delay = 0.0;
    double tempIntegrNCoh = sourceEnvir->integrationAmpl(countPointsIntegr,delay);
    long int i,j;
    try {
        QVector<long int> totalcountlength;
        totalcountlength.resize(screenVec[0]->isMeshScreenXY());
        znc.clear(); znc.resize(screenVec[0]->isMeshScreenXY());
        for(i = 0; i < screenVec[0]->isMeshScreenXY(); ++i)
        {

//            if (i%1000==0)
//                qDebug() << 100*i/(screenVec[0]->isMeshScreenXY()) << "%";
            totalcountlength[i] = totalLength[0][i].size();
            for (j = 0; j < totalcountlength[i]; ++j)
            {
                qDebug() << "array N:" << i << "size array:" << totalcountlength[i] <<j;
                if(flagWithNonReflRadiation == 1)
                {
                        znc[i] += totalAverCoefOfRefl[0][i][j]*tempIntegrNCoh;
                }
                else
                {
                    if( totalAngles[0][i][j].size() != 0)
                    {
                        znc[i] += totalAverCoefOfRefl[0][i][j]*tempIntegrNCoh;
                    }
//                    else
//                    {
//                qDebug() << "======================" << totalcountlength[i];
//                        znc[i] += tempIntegrNCoh;
//                    }
                }
//                qDebug() << tempIntegrNCoh;
            }

        }
        flagDraw = 1;//??????????????????????

        qDebug() << ":mainIntegrationINCOHAmpl!";
    }
    catch (...)
    {
        qDebug() << "###ERROR!";
    }

    return 0;
}

int Coherence::mainIntegrationCOHAmpl()
{
    qDebug() << "mainIntegrationCOHAmpl:";
    long int i,j,k;
    long int tempLengthSize;
    double delay = 0.0;
    try {
//        double tempIntegrCoh;
        zcIm.clear(); zcIm.resize(screenVec[0]->isMeshScreenXY());
        for(i = 0 ; i < screenVec[0]->isMeshScreenXY(); ++i)
        {
            if (i%1000==0)
                qDebug() << 100*i/(screenVec[0]->isMeshScreenXY()) << "%";
//            if (  QString::number((1.0*i/screenVec[0]->isMeshScreenXY()),'g',3).endsWith("0") == true)
//                qDebug() << 100*i/(screenVec[0]->isMeshScreenXY()) << "%";
            tempLengthSize = totalLength[0][i].size();
            for(j = 0; j < tempLengthSize-1; ++j)
            {
                qDebug() << "array N:" << i << "size array:" << tempLengthSize <<j;
//                if (j%1000==0)
//                    qDebug() << "    " << 100*j/(tempLengthSize-1) << "%";
                for(k = j+1; k < tempLengthSize; ++k)
                {
                    if(flagWithNonReflRadiation == 1)
                    {
                        delay = std::fabs(totalLength[0][i][k] - totalLength[0][i][j]);
                        zcIm[i] += 2*std::sqrt(totalAverCoefOfRefl[0][i][j]*totalAverCoefOfRefl[0][i][k])*sourceEnvir->integrationAmpl(countPointsIntegr,delay);
                    }
                    else if(flagWithNonReflRadiation == 0)
                    {
                        if( (!totalAngles[0][i][j].isEmpty()) && (!totalAngles[0][i][k].isEmpty()) )
                        {
                            delay = std::fabs(totalLength[0][i][k] - totalLength[0][i][j]);
                            zcIm[i] += 2*std::sqrt(totalAverCoefOfRefl[0][i][j]*totalAverCoefOfRefl[0][i][k])*sourceEnvir->integrationAmpl(countPointsIntegr,delay);
                        }
                        //                    else
                        //                    {

                        //                    }
                    }
                }
//                qDebug() << "totalAverCoefOfRefl" << totalAverCoefOfRefl[0][i][j-1] << totalAverCoefOfRefl[0][i][j] <<  zc[i];
            }
        }

        flagDraw = 2;//????????????????

        qDebug() << "mainIntegrationCOHAmpl!!";
    } catch (...) {
        qDebug() << "###ERROR HRENOVO!";
    }
    return 0;
}

//спeциально для простой многопоточности:::
void Coherence::setTempCoefReflNuIStaticMNOGOPOTOCHNIJ(const QVector<double> &anglesGlacingR, QVector<double> &tempR, const int &tflagNumCap)
{
    QVector<double>::const_iterator itcAngG;//итератор для прохождения по всем углам отражения луча

//    tempR.clear(); tempR.resize(nu_i.size());//nu_i постоянно для всех лучей, следовательно делать это здесь ни к чему

    QVector<double>::const_iterator itcNu_i = nu_i.constBegin(); //итератор для прохождения по всем частотам
    QVector<double>::iterator itTempR = tempR.begin(); //итератор для прохождения по всем частотам в массиве, который нам и нужен!
    for(
        ;itcNu_i != nu_i.constEnd();
        ++itcNu_i, ++itTempR)
    {
        *itTempR = 1.0;//если не отражался, то так и останется тут с единицей...
        for(
            itcAngG = anglesGlacingR.constBegin()
            ; itcAngG != anglesGlacingR.constEnd();
            ++itcAngG)
        {
            /// начало поиска коэф отражения:::
            std::complex<double> tempComplSqrt = std::sqrt(
                        std::complex<double>(
                            (1. - std::pow((capillaryVec.at(tflagNumCap)->getPlasmaFrequency())/(*itcNu_i),2))
                            , capillaryVec.at(tflagNumCap)->getEpsilonCap().imag()
                            ) - std::pow(std::cos(*itcAngG) ,2)
                            );
          //  qDebug() << tempComplSqrt.real() << tempComplSqrt.imag();
            double tempsin = std::sin(*itcAngG);
        //    std::complex<double> tempComplR = (tempsin - tempComplSqrt)/(tempsin + tempComplSqrt);
            /// итого:::
            *itTempR *= std::pow(std::abs( (tempsin - tempComplSqrt)/(tempsin + tempComplSqrt) ),2);
                    //capillaryVec[flagNumCap]->getCoefOfRefl(*itcAngG,*itcNu_i);
        }
    }
}//создаёт tempR

void Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ(long & i)
{
    long j = 0, k = 0;
//    long int l = 0;
    double delay = 0;//это разность хода
//    double delay2 = 0;//можно использовать, но не рекоммендуется
//    double testLength = 0;
//    double testSouLengthh = 0;//можно использовать, но не рекоммендуется
    double temp; //просто вспомогательная переменная
//    double norm = 0.0;

    long int tempSizeNUI = nu_i.size();
    QVector<double> tempR1, tempR2;//здесь хранятся коэф. отражения для каждой конкретной частоты для двух выборочных лучей - оно нафиг не надо, тупо темп
    tempR1.resize(tempSizeNUI); tempR2.resize(tempSizeNUI);
    QVector<double>::const_iterator itR1, itR2, itabsV, itnu2PiC;
    QVector<double>::iterator ittempZnci,ittempZcImi,ittempZcRei;//,ittempIncoh;
    long tempLengthSize = totalLength.at(0).at(i).size(); //это сколько у нас лучей

    for(j = 0; j < tempLengthSize-1; ++j)
//    for(j = 0; j < tempLengthSize; ++j) //как в формуле!!!
    {
        setTempCoefReflNuIStaticMNOGOPOTOCHNIJ(totalAngles.at(0).at(i).at(j),tempR2,totalNumOfCap.at(0).at(i).at(j));
        for(k = j; k < tempLengthSize; ++k)
//        for(k = 0; k < tempLengthSize; ++k) //как в формуле!!!!!??????????
        {
            //начало для когерентного
            if(flagWithNonReflRadiation == 1)
            {
                setTempCoefReflNuIStaticMNOGOPOTOCHNIJ(totalAngles.at(0).at(i).at(k),tempR1,totalNumOfCap.at(0).at(i).at(k));
                delay = totalLength.at(0).at(i).at(k) - totalLength.at(0).at(i).at(j)
                    + (sourceEnvir->getSpeedOfLightForCap()*(totalPhase.at(0).at(i).at(k) - totalPhase.at(0).at(i).at(j)));
//                delay2 = std::fabs(totalLength.at(0).at(i).at(k) - totalLength.at(0).at(i).at(j));
//                testLength =
//                        std::pow(totalPointRScr.at(0).at(i).at(k).at(0) - totalPointRScr.at(0).at(i).at(j).at(0),2)
//                        + std::pow(totalPointRScr.at(0).at(i).at(k).at(1) - totalPointRScr.at(0).at(i).at(j).at(1),2);
//                qDebug() << testLength << sourceEnvir->getWaveLength2() << (testLength < sourceEnvir->getWaveLength2());
//                testSouLengthh =
//                        std::pow(totalPointRSou.at(0).at(i).at(k).at(0) - totalPointRSou.at(0).at(i).at(j).at(0),2)
//                        + std::pow(totalPointRSou.at(0).at(i).at(k).at(1) - totalPointRSou.at(0).at(i).at(j).at(1),2);
//                if( (testLength < sourceEnvir->getWaveLength2()) )
//                {
//                    norm = 0.0;
//                    for(itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                        ; itR2 != tempR2.constEnd()
//                        ; ++itR1, ++itR2, ++itabsV)
//                    {
//                        norm += (*itR2)*(*itabsV)*(*itR1)*(*itabsV);
//                    }
//                    norm = std::sqrt(norm);
                      for(ittempZnci = tempZnc[i].begin(),
                          ittempZcRei = tempZcRe[i].begin(),
                          ittempZcImi = tempZcIm[i].begin(),
//                          ittempIncoh = tempIncoh[i].begin(),
                          itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin(), itnu2PiC = nu2PiC.constBegin()
                          ; itR1 != tempR1.constEnd()
                          ;
                          ++itR1, ++itR2, ++itabsV, ++itnu2PiC
                          , ++ittempZnci
                          , ++ittempZcRei
                          , ++ittempZcImi
//                          , ++ittempIncoh
                          )
                       {
//                          if( !( (testSouLengthh < sourceEnvir->getWaveLength())&&(std::fabs(delay2) < 8e-8) ) )
//                          {
                            temp = std::sqrt((*itR1)*(*itR2))*(*itabsV)*totalAmpl[0][i][j]*totalAmpl[0][i][k];//((sourceEnvir->getWaveLength2()-testLength)/sourceEnvir->getWaveLength2());//*totalAmpl[0][i][j]*totalAmpl[0][i][k];
//                            temp = 1;
                            *ittempZcRei += 2*temp*std::cos((*itnu2PiC)*delay);
                            *ittempZcImi += 2*temp*std::sin((*itnu2PiC)*delay);

//                            qDebug() << "j" << j << "k" << k << "i" << i << "size"<< totalLength.at(0).at(i).size() << "delay" << delay
//                                     << "cos" << std::cos((*itnu2PiC)*delay) << "sin" << std::sin((*itnu2PiC)*delay)
//                                     << "*ittempZcRei" << *ittempZcRei << "*ittempZcImi" << *ittempZcImi;
                            *ittempZnci += 0.5*((*itR2)*(*itabsV) + (*itR1)*(*itabsV));//((sourceEnvir->getWaveLength2()-testLength)/sourceEnvir->getWaveLength2());
//                            *ittempIncoh += 0.5*((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                          }
//                          else
//                          {
//                            *ittempIncoh += 0.5*((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                          }
                       }
//                  }
//                    for(ittempZnci = tempZnc[i].begin(), ittempTci = tempTc[i].begin(),
//                        ittempZci = tempZc[i].begin(),
//                        itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin(), itnu2PiC = nu2PiC.constBegin()
//                        ; itR1 != tempR1.constEnd()
//                        ;
//                        ++itR1, ++itR2, ++itabsV, ++itnu2PiC
//                        , ++ittempZnci, ++ittempTci
//                        , ++ittempZci)
//                    {

//                    }
//                }
//                else
//                {
//                    for(ittempZnci = tempZnc[i].begin(), itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                        ; itR2 != tempR2.constEnd()
//                        ; ++ittempZnci, ++itR1, ++itR2, ++itabsV)
//                    {
//                        *ittempZnci += ((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                    }
//                }
            }
            else if(flagWithNonReflRadiation == 0)
            {
                if( (!totalAngles.at(0).at(i).at(j).isEmpty()) && (!totalAngles.at(0).at(i).at(k).isEmpty()) )
                {
                    setTempCoefReflNuIStaticMNOGOPOTOCHNIJ(totalAngles.at(0).at(i).at(k),tempR1,totalNumOfCap.at(0).at(i).at(k));
                    delay = totalLength.at(0).at(i).at(k) - totalLength.at(0).at(i).at(j)
                               + sourceEnvir->getSpeedOfLightForCap()*(totalPhase.at(0).at(i).at(k) - totalPhase.at(0).at(i).at(j));
//                    if( testLength < sourceEnvir->getWaveLength2() )//временно отключим!!!!!!!!!!!!!!!!
//                    {
//                        for(itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                            ; itR2 != tempR2.constEnd()
//                            ; ++itR1, ++itR2, ++itabsV)
//                        {
//                            norm += (*itR2)*(*itabsV)*(*itR1)*(*itabsV);
//                        }
//                        norm = std::sqrt(norm);
                        for(ittempZnci = tempZnc[i].begin(),
                            ittempZcRei = tempZcRe[i].begin(),
                            ittempZcImi = tempZcIm[i].begin(),
//                            ittempIncoh = tempIncoh[i].begin(),
                            itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin(), itnu2PiC = nu2PiC.constBegin()
                            ; itR1 != tempR1.constEnd()
                            ;
                            ++itR1, ++itR2, ++itabsV, ++itnu2PiC
                            , ++ittempZcImi
                            , ++ittempZnci
                            , ++ittempZcRei
//                            , ++ittempIncoh
                            )
                        {
                            temp = std::sqrt((*itR1)*(*itR2))*(*itabsV)*totalAmpl[0][i][j]*totalAmpl[0][i][k];//*totalAmpl[0][i][j]*totalAmpl[0][i][k];
                            *ittempZcRei += 2*temp*std::cos((*itnu2PiC)*delay);
                            *ittempZcImi += 2*temp*std::sin((*itnu2PiC)*delay);
                            *ittempZnci += 0.5*((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                            *ittempIncoh += 0.5*((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
                        }
//                    }
//                    else
//                    {
//                        for(ittempZnci = tempZnc[i].begin(), itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                            ; itR2 != tempR2.constEnd()
//                            ; ++ittempZnci, ++itR1, ++itR2, ++itabsV)
//                        {
//                            *ittempZnci += ((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                        }
//                    }
                }
            }

            //конец для когерентного
        }
        //начало для некогерентного
//        if(flagWithNonReflRadiation == 1)
//        {
//            for(ittempIncoh = tempIncoh[i].begin()
//                , itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                ; itR2 != tempR2.constEnd()
//                ; ++ittempIncoh, ++itR2, ++itabsV)
//            {
//                *ittempIncoh += (*itR2)*(*itabsV);
//            }
//        }
//        else if(flagWithNonReflRadiation == 0)
//        {
//            if (!totalAngles[0][i][j].isEmpty())
//            {
//                for(ittempIncoh = tempIncoh[i].begin()
//                    , itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                    ; itR2 != tempR2.constEnd()
//                    ; ++ittempIncoh, ++itR2, ++itabsV)
//                {
//                    *ittempIncoh += (*itR2)*(*itabsV);
//                }
//            }
//        }
        //конец некогрентного
    }
}

void Coherence::threadPrepare(/*long & i*/) // не нужный в итоге ментод.... DELET ME!!
{/*
    long j = 0, k = 0;
    long i = screenVec[0]->getIndexOfLastPoint(pointR_1_X,pointR_1_Y); //основная точка
    long tempSizeBasis = totalLength.at(0).at(i).size(); //количество лучей в основной точке

    double delay = 0;//это разность хода
    double testLength = 0;
    double temp; //просто вспомогательная переменная
//    double norm = 0.0;

    long int tempSizeNUI = nu_i.size();
    QVector<double> tempR1, tempR2;//здесь хранятся коэф. отражения для каждой конкретной частоты для двух выборочных лучей - оно нафиг не надо, тупо темп
    tempR1.resize(tempSizeNUI); tempR2.resize(tempSizeNUI);
    QVector<double>::const_iterator itR1, itR2, itabsV, itnu2PiC;
    QVector<double>::iterator ittempZnci,ittempZci,ittempTci,ittempIncoh;
    QVector<double>::iterator ittempBasis;

    long int tempLengthSize = totalLength.at(0).at(i).size(); //это сколько у нас лучей

    for(j = 0; j < tempLengthSize-1; ++j)
    {
//        qDebug() << "array N:" << i << "size array:" << tempLengthSize <<j;
        //                qDebug() << "2." << 100.0*j/(tempLengthSize-1) << "%";

        Coherence::setTempCoefReflNuIStaticMNOGOPOTOCHNIJ(totalAngles.at(0).at(i).at(j),tempR2,totalNumOfCap.at(0).at(i).at(j));
        for(k = j+1; k < tempLengthSize; ++k)
        {
            //начало для когерентного
            if(flagWithNonReflRadiation == 1)
            {
                setTempCoefReflNuIStaticMNOGOPOTOCHNIJ(totalAngles.at(0).at(i).at(k),tempR1,totalNumOfCap.at(0).at(i).at(k));
                delay = totalLength.at(0).at(i).at(k) - totalLength.at(0).at(i).at(j)
                            + sourceEnvir->getSpeedOfLightForCap()*(totalPhase.at(0).at(i).at(k) - totalPhase.at(0).at(i).at(j));
                testLength =
                        std::pow(totalPointRScr.at(0).at(i).at(k).at(0) - totalPointRScr.at(0).at(i).at(j).at(0),2)
                        + std::pow(totalPointRScr.at(0).at(i).at(k).at(1) - totalPointRScr.at(0).at(i).at(j).at(1),2);
//                qDebug() << testLength << sourceEnvir->getWaveLength2() << (testLength < sourceEnvir->getWaveLength2());
                if( (testLength < sourceEnvir->getWaveLength2()) )
                {
//                    norm = 0.0;
//                    for(itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                        ; itR2 != tempR2.constEnd()
//                        ; ++itR1, ++itR2, ++itabsV)
//                    {
//                        norm += (*itR2)*(*itabsV)*(*itR1)*(*itabsV);
//                    }
//                    norm = std::sqrt(norm);
                      for(ittempZnci = tempZnc[i].begin(), ittempTci = tempZcRe[i].begin(),
                            ittempZci = tempZcIm[i].begin(),
                          ittempIncoh = tempIncoh[i].begin(),
                            ittempBasis = tempBasis[i].begin(),
                            itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin(), itnu2PiC = nu2PiC.constBegin()
                            ; itR1 != tempR1.constEnd()
                            ;
                            ++itR1, ++itR2, ++itabsV, ++itnu2PiC
                            , ++ittempZnci, ++ittempTci
                            , ++ittempZci
                            , ++ittempIncoh
                            , ++ittempBasis)
                       {
//                          if( !( (testSouLengthh < sourceEnvir->getWaveLength())&&(std::fabs(delay2) < 8e-8) ) )
//                          {
                            temp = 2*std::sqrt((*itR1)*(*itR2))*(*itabsV);//((sourceEnvir->getWaveLength2()-testLength)/sourceEnvir->getWaveLength2());// *totalAmpl[0][i][j]*totalAmpl[0][i][k];
                            *ittempTci += temp*std::cos((*itnu2PiC)*delay);
                            *ittempZci += temp*std::sin((*itnu2PiC)*delay);
                            *ittempZnci += ((*itR2)*(*itabsV) + (*itR1)*(*itabsV));//((sourceEnvir->getWaveLength2()-testLength)/sourceEnvir->getWaveLength2());
                            *ittempIncoh += 0.5*((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                          }
//                          else
//                          {
//                            *ittempIncoh += 0.5*((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                          }
                       }
                  }
//                    for(ittempZnci = tempZnc[i].begin(), ittempTci = tempTc[i].begin(),
//                        ittempZci = tempZc[i].begin(),
//                        itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin(), itnu2PiC = nu2PiC.constBegin()
//                        ; itR1 != tempR1.constEnd()
//                        ;
//                        ++itR1, ++itR2, ++itabsV, ++itnu2PiC
//                        , ++ittempZnci, ++ittempTci
//                        , ++ittempZci)
//                    {

//                    }
//                }
//                else
//                {
//                    for(ittempZnci = tempZnc[i].begin(), itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                        ; itR2 != tempR2.constEnd()
//                        ; ++ittempZnci, ++itR1, ++itR2, ++itabsV)
//                    {
//                        *ittempZnci += ((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                    }
//                }
            }
            else if(flagWithNonReflRadiation == 0)
            {
                if( (!totalAngles[0][i][j].isEmpty()) && (!totalAngles.at(0).at(i).at(k).isEmpty()) )
                {
                    setTempCoefReflNuIStaticMNOGOPOTOCHNIJ(totalAngles.at(0).at(i).at(k),tempR1,totalNumOfCap.at(0).at(i).at(k));
                    delay = totalLength.at(0).at(i).at(k) - totalLength.at(0).at(i).at(j)
                               + sourceEnvir->getSpeedOfLightForCap()*(totalPhase.at(0).at(i).at(k) - totalPhase.at(0).at(i).at(j));
                    if( testLength < sourceEnvir->getWaveLength2() )
                    {
//                        for(itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                            ; itR2 != tempR2.constEnd()
//                            ; ++itR1, ++itR2, ++itabsV)
//                        {
//                            norm += (*itR2)*(*itabsV)*(*itR1)*(*itabsV);
//                        }
//                        norm = std::sqrt(norm);
                        for(ittempZnci = tempZnc[i].begin(), ittempTci = tempZcRe[i].begin(),
                            ittempZci = tempZcIm[i].begin(),
                            itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin(), itnu2PiC = nu2PiC.constBegin()
                            ; itR1 != tempR1.constEnd()
                            ;
                            ++ittempZci,
                            ++itR1, ++itR2, ++itabsV, ++itnu2PiC
                            ,++ittempZnci, ++ittempTci)
                        {
                            temp = 2*std::sqrt((*itR1)*(*itR2))*(*itabsV);// *totalAmpl[0][i][j]*totalAmpl[0][i][k];
                            *ittempTci += temp*std::cos((*itnu2PiC)*delay);
                            *ittempZci += temp*std::sin((*itnu2PiC)*delay);
                            *ittempZnci += ((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
                        }
                    }
//                    else
//                    {
//                        for(ittempZnci = tempZnc[i].begin(), itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                            ; itR2 != tempR2.constEnd()
//                            ; ++ittempZnci, ++itR1, ++itR2, ++itabsV)
//                        {
//                            *ittempZnci += ((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                        }
//                    }
                }
            }

            //конец для когерентного
        }
        //начало для некогерентного
//        if(flagWithNonReflRadiation == 1)
//        {
//            for(ittempIncoh = tempIncoh[i].begin()
//                , itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                ; itR2 != tempR2.constEnd()
//                ; ++ittempIncoh, ++itR2, ++itabsV)
//            {
//                *ittempIncoh += (*itR2)*(*itabsV);
//            }
//        }
//        else if(flagWithNonReflRadiation == 0)
//        {
//            if (!totalAngles[0][i][j].isEmpty())
//            {
//                for(ittempIncoh = tempIncoh[i].begin()
//                    , itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                    ; itR2 != tempR2.constEnd()
//                    ; ++ittempIncoh, ++itR2, ++itabsV)
//                {
//                    *ittempIncoh += (*itR2)*(*itabsV);
//                }
//            }
//        }
        //конец некогрентного
    }
*/
}

//Теперь хорошо работает: 08.06.2013::
void Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ_2(long & i) //РАБОТАЕТ НЕ ПРАВИЛЬНО - теперь работает 19.05.13 НЕ РАБОТАЕТ! Усреднение по области == интенсивность! - рискнём снова... 24.05.13... последняя попытка... тупо по формуле
{
    long j, k;
//    long int l = 0;
    double delay = 0;//это разность хода
//    double testLength = 0;
//    double testSouLengthh = 0; //пока что не используем - расстояние между точками на источнике
//    double delay2 = 0; //пока что не используем - обрезанная длина хода
    double temp; //просто вспомогательная переменная
//    double norm = 0.0;

    long tempSizeNUI = nu_i.size(); //дискретизация спектра
    QVector<double> tempR1, tempR2;//здесь хранятся коэф. отражения для каждой конкретной частоты для двух выборочных лучей - оно нафиг не надо, тупо темп
    tempR1.resize(tempSizeNUI);
    tempR2.resize(tempSizeNUI);
    QVector<double>::const_iterator itR1, itR2, itabsV, itnu2PiC;
    QVector<double>::iterator ittempZ, ittempZIm, ittempZNORM;
//    QVector<double>::iterator ittempZNorm, ittempZImNorm;
//    long tempLengthSize = totalLength.at(0).at(i).size(); //это сколько у нас лучей

    //sizeForPointIndex = totalLength.at(0).at(pointIndex).size(); !!!!!!!!!!
    //БУДЬ ОСТОРОЖЕН, НУЖНО СОЗДАТЬ ЗАНОВО, в том методе, в котором будешь вызывать этот...

    //!!! НУЖНА ПРОВЕРКА pointIndex !!!???????????????????????????
    //!!! ПЕРВЫЙ ЭТАП - СТЕПЕНЬ ВЗАИМНОЙ КОГЕРЕНТНОСТИ!
//    int pointIndex = i;

    if( ( totalLength.at(0).at(i).size() >= (kolvoExp) )&&( !totalLength.at(0).at(pointIndex).isEmpty() ) ) //!МОЖЕТ ВТОРОЕ УСЛОВИЕ ПРОВЕРЯТЬ НЕ НАДО.. ЗАЧЕМ???
    {
//    for(j = 0; j < sizeForPointIndex; ++j)//!!!!!!!!!!!!!!!!!!!!!!sizeForPointIndex
//    for(j = 0; j < 1; ++j)
//    {
        j = iterExp;
//        qDebug() << "array N:" << i << "size array:" << tempLengthSize <<j;
        //                qDebug() << "2." << 100.0*j/(tempLengthSize-1) << "%";
        setTempCoefReflNuIStaticMNOGOPOTOCHNIJ(totalAngles.at(0).at(pointIndex).at(j),tempR2,totalNumOfCap.at(0).at(pointIndex).at(j));
//        for(k = j; (k < tempLengthSize)&&(k==j); ++k)
//        for(k = 0; k < tempLengthSize; ++k)
//        for(k = j; k < j+1; ++k)
//        {
        k = j;
            //начало для когерентного
            if(flagWithNonReflRadiation == 1)
            {
//                qDebug() << "    QQQQQQQQQQ" << totalAngles.at(0).at(i).at(k); //ЕСЛИ ЭТО ХРЕНЬ ПУСТАЯ, ТО ОНА ПЕРЕДАЁТ ПРОСТО ПУСТОЙ КОНЕЙНЕР В ФУНКЦИЮ...
                setTempCoefReflNuIStaticMNOGOPOTOCHNIJ(totalAngles.at(0).at(i).at(k),tempR1,totalNumOfCap.at(0).at(i).at(k));

                delay = totalLength.at(0).at(i).at(k) - totalLength.at(0).at(pointIndex).at(j)
                        + sourceEnvir->getSpeedOfLightForCap()*(totalPhase.at(0).at(i).at(k) - totalPhase.at(0).at(pointIndex).at(j));
//                delay2 = std::fabs(totalLength.at(0).at(i).at(k) - totalLength.at(0).at(i).at(j));
//                testLength =
//                        std::pow(totalPointRScr.at(0).at(i).at(k).at(0) - totalPointRScr.at(0).at(index).at(j).at(0),2)
//                        + std::pow(totalPointRScr.at(0).at(i).at(k).at(1) - totalPointRScr.at(0).at(index).at(j).at(1),2);
//                qDebug() << testLength << sourceEnvir->getWaveLength2() << (testLength < sourceEnvir->getWaveLength2());
//                testSouLengthh =
//                        std::pow(totalPointRSou.at(0).at(i).at(k).at(0) - totalPointRSou.at(0).at(i).at(j).at(0),2)
//                        + std::pow(totalPointRSou.at(0).at(i).at(k).at(1) - totalPointRSou.at(0).at(i).at(j).at(1),2);
//                if( (testLength < sourceEnvir->getWaveLength2()) )//хорошо бы написать смещение и оставить проверку!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//                {
//                    norm = 0.0;
//                    for(itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                        ; itR2 != tempR2.constEnd()
//                        ; ++itR1, ++itR2, ++itabsV)
//                    {
//                        norm += (*itR2)*(*itabsV)*(*itR1)*(*itabsV);
//                    }
//                    norm = std::sqrt(norm);
                      for(
                          ittempZ = tempzzzRe[i].begin(),
                          ittempZIm = tempzzzIm[i].begin(),
                          ittempZNORM = tempzzzNORM[i].begin(),
                          itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin(), itnu2PiC = nu2PiC.constBegin()
                          ; itR1 != tempR1.constEnd()
                          ;
                          ++itR1, ++itR2, ++itabsV, ++itnu2PiC
                          , ++ittempZ
                          , ++ittempZIm
                          , ++ittempZNORM)
                       {
//                          if( !( (testSouLengthh < sourceEnvir->getWaveLength())&&(std::fabs(delay2) < 8e-8) ) )
//                          {
                            temp = std::sqrt((*itR1)*(*itR2))*(*itabsV)*totalAmpl[0][i][j]*totalAmpl[0][i][k];//((sourceEnvir->getWaveLength2()-testLength)/sourceEnvir->getWaveLength2());/
                            *ittempZ += temp*std::cos((*itnu2PiC)*delay);
                            *ittempZIm += temp*std::sin((*itnu2PiC)*delay);
//                            qDebug() << std::cos((*itnu2PiC)*delay) << std::sin((*itnu2PiC)*delay) << delay;
                            *ittempZNORM += temp;
//                            *ittempZci += temp*std::sin((*itnu2PiC)*delay);
//                            *ittempZnci += ((*itR2)*(*itabsV) + (*itR1)*(*itabsV));//((sourceEnvir->getWaveLength2()-testLength)/sourceEnvir->getWaveLength2());
//                            *ittempIncoh += 0.5*((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                          }
//                          else
//                          {
//                            *ittempIncoh += 0.5*((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                          }
                       }

//                  }
//                    for(ittempZnci = tempZnc[i].begin(), ittempTci = tempTc[i].begin(),
//                        ittempZci = tempZc[i].begin(),
//                        itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin(), itnu2PiC = nu2PiC.constBegin()
//                        ; itR1 != tempR1.constEnd()
//                        ;
//                        ++itR1, ++itR2, ++itabsV, ++itnu2PiC
//                        , ++ittempZnci, ++ittempTci
//                        , ++ittempZci)
//                    {

//                    }
//                }
//                else
//                {
//                    for(ittempZnci = tempZnc[i].begin(), itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                        ; itR2 != tempR2.constEnd()
//                        ; ++ittempZnci, ++itR1, ++itR2, ++itabsV)
//                    {
//                        *ittempZnci += ((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                    }
//                }
            }
            else if(flagWithNonReflRadiation == 0)
            {
                if( (!totalAngles.at(0).at(pointIndex).at(j).isEmpty()) && (!totalAngles.at(0).at(i).at(k).isEmpty()) )
                {
                    setTempCoefReflNuIStaticMNOGOPOTOCHNIJ(totalAngles.at(0).at(i).at(k),tempR1,totalNumOfCap.at(0).at(i).at(k));
                    delay = totalLength.at(0).at(i).at(k) - totalLength.at(0).at(pointIndex).at(j)
                               + sourceEnvir->getSpeedOfLightForCap()*(totalPhase.at(0).at(i).at(k) - totalPhase.at(0).at(pointIndex).at(j));
//                    if( testLength < sourceEnvir->getWaveLength2() )
//                    {
//                        for(itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                            ; itR2 != tempR2.constEnd()
//                            ; ++itR1, ++itR2, ++itabsV)
//                        {
//                            norm += (*itR2)*(*itabsV)*(*itR1)*(*itabsV);
//                        }
//                        norm = std::sqrt(norm);
                        for(
                              ittempZ = tempzzzRe[i].begin(),
                              ittempZIm = tempzzzIm[i].begin(),
                              ittempZNORM = tempzzzNORM[i].begin(),
                              itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin(), itnu2PiC = nu2PiC.constBegin()
                              ; itR1 != tempR1.constEnd()
                              ;
                              ++itR1, ++itR2, ++itabsV, ++itnu2PiC
                              , ++ittempZ
                              , ++ittempZIm
                              , ++ittempZNORM)
                         {
  //                          if( !( (testSouLengthh < sourceEnvir->getWaveLength())&&(std::fabs(delay2) < 8e-8) ) )
  //                          {
                              temp = std::sqrt((*itR1)*(*itR2))*(*itabsV)*totalAmpl[0][i][j]*totalAmpl[0][i][k];;//((sourceEnvir->getWaveLength2()-testLength)/sourceEnvir->getWaveLength2());//*totalAmpl[0][i][j]*totalAmpl[0][i][k];
                              *ittempZ += temp*std::cos((*itnu2PiC)*delay);
                              *ittempZIm += temp*std::sin((*itnu2PiC)*delay);
                              *ittempZNORM += temp;
  //                            *ittempZci += temp*std::sin((*itnu2PiC)*delay);
  //                            *ittempZnci += ((*itR2)*(*itabsV) + (*itR1)*(*itabsV));//((sourceEnvir->getWaveLength2()-testLength)/sourceEnvir->getWaveLength2());
  //                            *ittempIncoh += 0.5*((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
  //                          }
  //                          else
  //                          {
  //                            *ittempIncoh += 0.5*((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
  //                          }
                         }
//                    }
//                    else
//                    {
//                        for(ittempZnci = tempZnc[i].begin(), itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                            ; itR2 != tempR2.constEnd()
//                            ; ++ittempZnci, ++itR1, ++itR2, ++itabsV)
//                        {
//                            *ittempZnci += ((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                        }
//                    }
                }
            }

            //конец для когерентного
//        }

        //начало для некогерентного
//        if(flagWithNonReflRadiation == 1)
//        {
//            for(ittempIncoh = tempIncoh[i].begin()
//                , itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                ; itR2 != tempR2.constEnd()
//                ; ++ittempIncoh, ++itR2, ++itabsV)
//            {
//                *ittempIncoh += (*itR2)*(*itabsV);
//            }
//        }
//        else if(flagWithNonReflRadiation == 0)
//        {
//            if (!totalAngles[0][i][j].isEmpty())
//            {
//                for(ittempIncoh = tempIncoh[i].begin()
//                    , itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                    ; itR2 != tempR2.constEnd()
//                    ; ++ittempIncoh, ++itR2, ++itabsV)
//                {
//                    *ittempIncoh += (*itR2)*(*itabsV);
//                }
//            }
//        }
        //конец некогрентного
//    }
    }

    //!!! ВТОРОЙ ЭТАП!!! НОРМИРОВКА - ИНТЕНСИВНОСТЬ!!!
    /*
    if( (totalLength.at(0).at(i).size() >= 8) )
            //&&(!totalLength.at(0).at(pointIndex).isEmpty()) )
    {
//    for(j = 0; j < sizeForPointIndex; ++j)//!!!!!!!!!!!!!!!!!!!!!!sizeForPointIndex
    for(j = 0; j < 8; ++j)
    {
//        qDebug() << "array N:" << i << "size array:" << tempLengthSize <<j;
        //                qDebug() << "2." << 100.0*j/(tempLengthSize-1) << "%";
        Coherence::setTempCoefReflNuIStaticMNOGOPOTOCHNIJ(totalAngles.at(0).at(i).at(j),tempR2,totalNumOfCap.at(0).at(i).at(j));
//        for(k = j; (k < tempLengthSize)&&(k==j); ++k)
//        for(k = 0; k < tempLengthSize; ++k)
//        for(k = j; k < j+1; ++k)
//        {
        if(j == 0)
            k = 1;
        else if(j == 1)
            k = 0;
        else if(j == 2)
            k = 3;
        else if(j == 3)
            k = 2;
        else if(j == 4)
            k = 5;
        else if(j == 5)
            k = 4;
        else if(j == 6)
            k = 7;
        else if(j == 7)
            k = 6;
//начало для когерентного
            if(flagWithNonReflRadiation == 1)
            {
                setTempCoefReflNuIStaticMNOGOPOTOCHNIJ(totalAngles.at(0).at(i).at(k),tempR1,totalNumOfCap.at(0).at(i).at(k));
                delay = totalLength.at(0).at(i).at(k) - totalLength.at(0).at(i).at(j)
                            + sourceEnvir->getSpeedOfLightForCap()*(totalPhase.at(0).at(i).at(k) - totalPhase.at(0).at(i).at(j));
//                delay2 = std::fabs(totalLength.at(0).at(i).at(k) - totalLength.at(0).at(i).at(j));
//                testLength =
//                        std::pow(totalPointRScr.at(0).at(i).at(k).at(0) - totalPointRScr.at(0).at(index).at(j).at(0),2)
//                        + std::pow(totalPointRScr.at(0).at(i).at(k).at(1) - totalPointRScr.at(0).at(index).at(j).at(1),2);
//                qDebug() << testLength << sourceEnvir->getWaveLength2() << (testLength < sourceEnvir->getWaveLength2());
//                testSouLengthh =
//                        std::pow(totalPointRSou.at(0).at(i).at(k).at(0) - totalPointRSou.at(0).at(i).at(j).at(0),2)
//                        + std::pow(totalPointRSou.at(0).at(i).at(k).at(1) - totalPointRSou.at(0).at(i).at(j).at(1),2);
//                if( (testLength < sourceEnvir->getWaveLength2()) )//хорошо бы написать смещение и оставить проверку!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//                {
//                    norm = 0.0;
//                    for(itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                        ; itR2 != tempR2.constEnd()
//                        ; ++itR1, ++itR2, ++itabsV)
//                    {
//                        norm += (*itR2)*(*itabsV)*(*itR1)*(*itabsV);
//                    }
//                    norm = std::sqrt(norm);
                      for(
                          ittempZNorm = tempzzzReNorm[i].begin(),
                          ittempZImNorm = tempzzzImNorm[i].begin(),
                          itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin(), itnu2PiC = nu2PiC.constBegin()
                          ; itR1 != tempR1.constEnd()
                          ;
                          ++itR1, ++itR2, ++itabsV, ++itnu2PiC
                          , ++ittempZNorm
                          , ++ittempZImNorm)
                       {
//                          if( !( (testSouLengthh < sourceEnvir->getWaveLength())&&(std::fabs(delay2) < 8e-8) ) )
//                          {
                            temp = std::sqrt((*itR1)*(*itR2))*(*itabsV);//((sourceEnvir->getWaveLength2()-testLength)/sourceEnvir->getWaveLength2());// *totalAmpl[0][i][j]*totalAmpl[0][i][k];
                            *ittempZNorm += temp*std::cos((*itnu2PiC)*delay);
                            *ittempZImNorm += temp*std::sin((*itnu2PiC)*delay);
//                            *ittempZci += temp*std::sin((*itnu2PiC)*delay);
//                            *ittempZnci += ((*itR2)*(*itabsV) + (*itR1)*(*itabsV));//((sourceEnvir->getWaveLength2()-testLength)/sourceEnvir->getWaveLength2());
//                            *ittempIncoh += 0.5*((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                          }
//                          else
//                          {
//                            *ittempIncoh += 0.5*((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                          }
                       }
//                  }
//                    for(ittempZnci = tempZnc[i].begin(), ittempTci = tempTc[i].begin(),
//                        ittempZci = tempZc[i].begin(),
//                        itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin(), itnu2PiC = nu2PiC.constBegin()
//                        ; itR1 != tempR1.constEnd()
//                        ;
//                        ++itR1, ++itR2, ++itabsV, ++itnu2PiC
//                        , ++ittempZnci, ++ittempTci
//                        , ++ittempZci)
//                    {

//                    }
//                }
//                else
//                {
//                    for(ittempZnci = tempZnc[i].begin(), itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                        ; itR2 != tempR2.constEnd()
//                        ; ++ittempZnci, ++itR1, ++itR2, ++itabsV)
//                    {
//                        *ittempZnci += ((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                    }
//                }
            }
            else if(flagWithNonReflRadiation == 0)
            {
                if( (!totalAngles.at(0).at(i).at(j).isEmpty()) && (!totalAngles.at(0).at(i).at(k).isEmpty()) )
                {
                    setTempCoefReflNuIStaticMNOGOPOTOCHNIJ(totalAngles.at(0).at(i).at(k),tempR1,totalNumOfCap.at(0).at(i).at(k));
                    delay = totalLength.at(0).at(i).at(k) - totalLength.at(0).at(i).at(j)
                               + sourceEnvir->getSpeedOfLightForCap()*(totalPhase.at(0).at(i).at(k) - totalPhase.at(0).at(i).at(j));
//                    if( testLength < sourceEnvir->getWaveLength2() )
//                    {
//                        for(itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                            ; itR2 != tempR2.constEnd()
//                            ; ++itR1, ++itR2, ++itabsV)
//                        {
//                            norm += (*itR2)*(*itabsV)*(*itR1)*(*itabsV);
//                        }
//                        norm = std::sqrt(norm);
                        for(
                              ittempZNorm = tempzzzReNorm[i].begin(),
                              ittempZImNorm = tempzzzImNorm[i].begin(),
                              itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin(), itnu2PiC = nu2PiC.constBegin()
                              ; itR1 != tempR1.constEnd()
                              ;
                              ++itR1, ++itR2, ++itabsV, ++itnu2PiC
                              , ++ittempZNorm
                              , ++ittempZImNorm)
                         {
  //                          if( !( (testSouLengthh < sourceEnvir->getWaveLength())&&(std::fabs(delay2) < 8e-8) ) )
  //                          {
                              temp = std::sqrt((*itR1)*(*itR2))*(*itabsV);//((sourceEnvir->getWaveLength2()-testLength)/sourceEnvir->getWaveLength2());// *totalAmpl[0][i][j]*totalAmpl[0][i][k];
                              *ittempZNorm += temp*std::cos((*itnu2PiC)*delay);
                              *ittempZImNorm += temp*std::sin((*itnu2PiC)*delay);
  //                            *ittempZci += temp*std::sin((*itnu2PiC)*delay);
  //                            *ittempZnci += ((*itR2)*(*itabsV) + (*itR1)*(*itabsV));//((sourceEnvir->getWaveLength2()-testLength)/sourceEnvir->getWaveLength2());
  //                            *ittempIncoh += 0.5*((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
  //                          }
  //                          else
  //                          {
  //                            *ittempIncoh += 0.5*((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
  //                          }
                         }
//                    }
//                    else
//                    {
//                        for(ittempZnci = tempZnc[i].begin(), itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                            ; itR2 != tempR2.constEnd()
//                            ; ++ittempZnci, ++itR1, ++itR2, ++itabsV)
//                        {
//                            *ittempZnci += ((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                        }
//                    }
                }
            }

            //конец для когерентного
//        }

        //начало для некогерентного
//        if(flagWithNonReflRadiation == 1)
//        {
//            for(ittempIncoh = tempIncoh[i].begin()
//                , itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                ; itR2 != tempR2.constEnd()
//                ; ++ittempIncoh, ++itR2, ++itabsV)
//            {
//                *ittempIncoh += (*itR2)*(*itabsV);
//            }
//        }
//        else if(flagWithNonReflRadiation == 0)
//        {
//            if (!totalAngles[0][i][j].isEmpty())
//            {
//                for(ittempIncoh = tempIncoh[i].begin()
//                    , itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                    ; itR2 != tempR2.constEnd()
//                    ; ++ittempIncoh, ++itR2, ++itabsV)
//                {
//                    *ittempIncoh += (*itR2)*(*itabsV);
//                }
//            }
//        }
        //конец некогрентного
    }
    }
    */
    //!!!!! КОНЕЦ ВТОРОГО ЭТАПА!!!!

    //!!! ПОПЫТКА РАЗДЕЛИТЬ НА ДВА МАССИВА !!!
    /*
    if( (totalLength.at(0).at(i).size() >= 2) )
            //&&(!totalLength.at(0).at(pointIndex).isEmpty()) )
    {
//    for(j = 0; j < sizeForPointIndex; ++j)//!!!!!!!!!!!!!!!!!!!!!!sizeForPointIndex
    for(j = 1; j < 2; ++j)
    {
//        qDebug() << "array N:" << i << "size array:" << tempLengthSize <<j;
        //                qDebug() << "2." << 100.0*j/(tempLengthSize-1) << "%";
        Coherence::setTempCoefReflNuIStaticMNOGOPOTOCHNIJ(totalAngles.at(0).at(pointIndex).at(j),tempR2,totalNumOfCap.at(0).at(pointIndex).at(j));
//        for(k = j; (k < tempLengthSize)&&(k==j); ++k)
//        for(k = 0; k < tempLengthSize; ++k)
        for(k = j; k < j+1; ++k)
        {
            //начало для когерентного
            if(flagWithNonReflRadiation == 1)
            {
                setTempCoefReflNuIStaticMNOGOPOTOCHNIJ(totalAngles.at(0).at(i).at(k),tempR1,totalNumOfCap.at(0).at(i).at(k));
                delay = totalLength.at(0).at(i).at(k) - totalLength.at(0).at(pointIndex).at(j)
                            + sourceEnvir->getSpeedOfLightForCap()*(totalPhase.at(0).at(i).at(k) - totalPhase.at(0).at(pointIndex).at(j));
//                delay2 = std::fabs(totalLength.at(0).at(i).at(k) - totalLength.at(0).at(i).at(j));
//                testLength =
//                        std::pow(totalPointRScr.at(0).at(i).at(k).at(0) - totalPointRScr.at(0).at(index).at(j).at(0),2)
//                        + std::pow(totalPointRScr.at(0).at(i).at(k).at(1) - totalPointRScr.at(0).at(index).at(j).at(1),2);
//                qDebug() << testLength << sourceEnvir->getWaveLength2() << (testLength < sourceEnvir->getWaveLength2());
//                testSouLengthh =
//                        std::pow(totalPointRSou.at(0).at(i).at(k).at(0) - totalPointRSou.at(0).at(i).at(j).at(0),2)
//                        + std::pow(totalPointRSou.at(0).at(i).at(k).at(1) - totalPointRSou.at(0).at(i).at(j).at(1),2);
//                if( (testLength < sourceEnvir->getWaveLength2()) )//хорошо бы написать смещение и оставить проверку!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//                {
//                    norm = 0.0;
//                    for(itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                        ; itR2 != tempR2.constEnd()
//                        ; ++itR1, ++itR2, ++itabsV)
//                    {
//                        norm += (*itR2)*(*itabsV)*(*itR1)*(*itabsV);
//                    }
//                    norm = std::sqrt(norm);
                      for(
                          ittempZNorm = tempzzzReNorm[i].begin(),
                          ittempZImNorm = tempzzzImNorm[i].begin(),
                          itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin(), itnu2PiC = nu2PiC.constBegin()
                          ; itR1 != tempR1.constEnd()
                          ;
                          ++itR1, ++itR2, ++itabsV, ++itnu2PiC
                          , ++ittempZNorm
                          , ++ittempZImNorm)
                       {
//                          if( !( (testSouLengthh < sourceEnvir->getWaveLength())&&(std::fabs(delay2) < 8e-8) ) )
//                          {
                            temp = std::sqrt((*itR1)*(*itR2))*(*itabsV);//((sourceEnvir->getWaveLength2()-testLength)/sourceEnvir->getWaveLength2());// *totalAmpl[0][i][j]*totalAmpl[0][i][k];
                            *ittempZNorm += temp*std::cos((*itnu2PiC)*delay);
                            *ittempZImNorm += temp*std::sin((*itnu2PiC)*delay);
//                            *ittempZci += temp*std::sin((*itnu2PiC)*delay);
//                            *ittempZnci += ((*itR2)*(*itabsV) + (*itR1)*(*itabsV));//((sourceEnvir->getWaveLength2()-testLength)/sourceEnvir->getWaveLength2());
//                            *ittempIncoh += 0.5*((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                          }
//                          else
//                          {
//                            *ittempIncoh += 0.5*((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                          }
                       }
//                  }
//                    for(ittempZnci = tempZnc[i].begin(), ittempTci = tempTc[i].begin(),
//                        ittempZci = tempZc[i].begin(),
//                        itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin(), itnu2PiC = nu2PiC.constBegin()
//                        ; itR1 != tempR1.constEnd()
//                        ;
//                        ++itR1, ++itR2, ++itabsV, ++itnu2PiC
//                        , ++ittempZnci, ++ittempTci
//                        , ++ittempZci)
//                    {

//                    }
//                }
//                else
//                {
//                    for(ittempZnci = tempZnc[i].begin(), itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                        ; itR2 != tempR2.constEnd()
//                        ; ++ittempZnci, ++itR1, ++itR2, ++itabsV)
//                    {
//                        *ittempZnci += ((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                    }
//                }
            }
            else if(flagWithNonReflRadiation == 0)
            {
                if( (!totalAngles.at(0).at(pointIndex).at(j).isEmpty()) && (!totalAngles.at(0).at(i).at(k).isEmpty()) )
                {
                    setTempCoefReflNuIStaticMNOGOPOTOCHNIJ(totalAngles.at(0).at(i).at(k),tempR1,totalNumOfCap.at(0).at(i).at(k));
                    delay = totalLength.at(0).at(i).at(k) - totalLength.at(0).at(pointIndex).at(j)
                               + sourceEnvir->getSpeedOfLightForCap()*(totalPhase.at(0).at(i).at(k) - totalPhase.at(0).at(pointIndex).at(j));
//                    if( testLength < sourceEnvir->getWaveLength2() )
//                    {
//                        for(itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                            ; itR2 != tempR2.constEnd()
//                            ; ++itR1, ++itR2, ++itabsV)
//                        {
//                            norm += (*itR2)*(*itabsV)*(*itR1)*(*itabsV);
//                        }
//                        norm = std::sqrt(norm);
                        for(
                              ittempZNorm = tempzzzReNorm[i].begin(),
                              ittempZImNorm = tempzzzImNorm[i].begin(),
                              itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin(), itnu2PiC = nu2PiC.constBegin()
                              ; itR1 != tempR1.constEnd()
                              ;
                              ++itR1, ++itR2, ++itabsV, ++itnu2PiC
                              , ++ittempZNorm
                              , ++ittempZImNorm)
                         {
  //                          if( !( (testSouLengthh < sourceEnvir->getWaveLength())&&(std::fabs(delay2) < 8e-8) ) )
  //                          {
                              temp = std::sqrt((*itR1)*(*itR2))*(*itabsV);//((sourceEnvir->getWaveLength2()-testLength)/sourceEnvir->getWaveLength2());// *totalAmpl[0][i][j]*totalAmpl[0][i][k];
                              *ittempZNorm += temp*std::cos((*itnu2PiC)*delay);
                              *ittempZImNorm += temp*std::sin((*itnu2PiC)*delay);
  //                            *ittempZci += temp*std::sin((*itnu2PiC)*delay);
  //                            *ittempZnci += ((*itR2)*(*itabsV) + (*itR1)*(*itabsV));//((sourceEnvir->getWaveLength2()-testLength)/sourceEnvir->getWaveLength2());
  //                            *ittempIncoh += 0.5*((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
  //                          }
  //                          else
  //                          {
  //                            *ittempIncoh += 0.5*((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
  //                          }
                         }
//                    }
//                    else
//                    {
//                        for(ittempZnci = tempZnc[i].begin(), itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                            ; itR2 != tempR2.constEnd()
//                            ; ++ittempZnci, ++itR1, ++itR2, ++itabsV)
//                        {
//                            *ittempZnci += ((*itR2)*(*itabsV) + (*itR1)*(*itabsV));
//                        }
//                    }
                }
            }

            //конец для когерентного
        }

        //начало для некогерентного
//        if(flagWithNonReflRadiation == 1)
//        {
//            for(ittempIncoh = tempIncoh[i].begin()
//                , itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                ; itR2 != tempR2.constEnd()
//                ; ++ittempIncoh, ++itR2, ++itabsV)
//            {
//                *ittempIncoh += (*itR2)*(*itabsV);
//            }
//        }
//        else if(flagWithNonReflRadiation == 0)
//        {
//            if (!totalAngles[0][i][j].isEmpty())
//            {
//                for(ittempIncoh = tempIncoh[i].begin()
//                    , itR2 = tempR2.constBegin(), itabsV = absV.constBegin()
//                    ; itR2 != tempR2.constEnd()
//                    ; ++ittempIncoh, ++itR2, ++itabsV)
//                {
//                    *ittempIncoh += (*itR2)*(*itabsV);
//                }
//            }
//        }
        //конец некогрентного
    }
    }
    */
    //!!! КОНЕЦ ПОПЫТКИ РАЗДЕЛИТЬ НА ДВЕ КАРТИНКИ!!!
}


void Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ_3(long & i)
{
    long j, k;
//    pointIndex = screenVec[0]->getIndexOfLastPoint(pointR_1_X,pointR_1_Y); //основная точка ->создаётся в конструкторе

    double delay = 0;//это разность хода
    double temp; //просто вспомогательная переменная

    long tempSizeNUI = nu_i.size(); //дискретизация спектра
    QVector<double> tempR1, tempR2;//здесь хранятся коэф. отражения для каждой конкретной частоты для двух выборочных лучей - оно нафиг не надо, тупо темп
    tempR1.clear(); tempR2.clear();
    tempR1.resize(tempSizeNUI); tempR2.resize(tempSizeNUI);
    QVector<double>::const_iterator itR1, itR2, itabsV, itnu2PiC;
//    QVector<double>::iterator ittempZnci,ittempZci,ittempTci,ittempIncoh;
//    QVector<double>::iterator ittempZ, ittempZIm;
    double asd = 0., asdf = 0.;

    j = numberForPointIndex;
    k = 0;

    if( (!totalLength.at(0).at(i).isEmpty())&&(!totalLength.at(0).at(pointIndex).isEmpty())
            &&(j!=-1) )
    {
//        for(j = 0; j < 1; ++j)
//        {
            Coherence::setTempCoefReflNuIStaticMNOGOPOTOCHNIJ(totalAngles.at(0).at(pointIndex).at(j),tempR2,totalNumOfCap.at(0).at(pointIndex).at(j));
//            for(k = 0; k < 1; ++k)
//            {
                if(flagWithNonReflRadiation == 1)
                {
                    setTempCoefReflNuIStaticMNOGOPOTOCHNIJ(totalAngles.at(0).at(i).at(k),tempR1,totalNumOfCap.at(0).at(i).at(k));
                    delay = totalLength.at(0).at(i).at(k) - totalLength.at(0).at(pointIndex).at(j)
                            + sourceEnvir->getSpeedOfLightForCap()*(totalPhase.at(0).at(i).at(k) - totalPhase.at(0).at(pointIndex).at(j));

                    for(
//                        ittempZ = tempGeneral[i].begin(),
//                         ittempZIm = tempGeneralIm[i].begin(),
                         itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin(), itnu2PiC = nu2PiC.constBegin()
                         ; itR1 != tempR1.constEnd()
                         ;
//                         ++itR1, ++itR2, ++itabsV, ++itnu2PiC,
                        ++itR1, ++itR2, ++itabsV, ++itnu2PiC
//                        , ++ittempZ
//                        , ++ittempZIm
                        )
                    {
                        temp = std::sqrt((*itR1)*(*itR2))*(*itabsV);//((sourceEnvir->getWaveLength2()-testLength)/sourceEnvir->getWaveLength2());//*totalAmpl[0][i][j]*totalAmpl[0][i][k];
//                        *ittempZ += temp*std::cos((*itnu2PiC)*delay);
//                        *ittempZIm += temp*std::sin((*itnu2PiC)*delay);
                        asd += temp*std::cos((*itnu2PiC)*delay);
                        asdf += temp*std::sin((*itnu2PiC)*delay);
                    }

                }
                else if(flagWithNonReflRadiation == 0)
                {
                    if( (!totalAngles.at(0).at(pointIndex).at(j).isEmpty()) && (!totalAngles.at(0).at(i).at(k).isEmpty()) )
                    {
                        setTempCoefReflNuIStaticMNOGOPOTOCHNIJ(totalAngles.at(0).at(i).at(k),tempR1,totalNumOfCap.at(0).at(i).at(k));
                        delay = totalLength.at(0).at(i).at(k) - totalLength.at(0).at(pointIndex).at(j)
                                + sourceEnvir->getSpeedOfLightForCap()*(totalPhase.at(0).at(i).at(k) - totalPhase.at(0).at(pointIndex).at(j));
                        for(
//                            ittempZ = tempGeneral[i].begin(),
//                            ittempZIm = tempGeneralIm[i].begin(),
                            itR1 = tempR1.constBegin(), itR2 = tempR2.constBegin(), itabsV = absV.constBegin(), itnu2PiC = nu2PiC.constBegin()
                            ; itR1 != tempR1.constEnd()
                            ;
                            ++itR1, ++itR2, ++itabsV, ++itnu2PiC
//                            , ++ittempZ
//                            , ++ittempZIm
                            )
                        {
                            temp = std::sqrt((*itR1)*(*itR2))*(*itabsV)*totalAmpl[0][i][j]*totalAmpl[0][i][k];//((sourceEnvir->getWaveLength2()-testLength)/sourceEnvir->getWaveLength2());// *totalAmpl[0][i][j]*totalAmpl[0][i][k];
//                            *ittempZ += temp*std::cos((*itnu2PiC)*delay);
//                            *ittempZIm += temp*std::sin((*itnu2PiC)*delay);
                            asd += temp*std::cos((*itnu2PiC)*delay);
                            asdf += temp*std::sin((*itnu2PiC)*delay);
                        }
                    }
                }
//            }
//        }

        m_mutex.lock();
        zgeneral.push_back(asd);
        zgeneralIm.push_back(asdf);
        xgeneral.push_back(totalPointRScr.at(0).at(i).at(0).at(0)); //k = 0
        ygeneral.push_back(totalPointRScr.at(0).at(i).at(0).at(1));
        m_mutex.unlock();
    }
}

int Coherence::mainIntegrationConvolutionMNOGOPOTOCHNIJdegOfCoh()
{
    qDebug() << "Performing calculations...:";
    QTime time; time.start();//считаем, сколько времени прошло, от начала счёта

    QProgressDialog * pprd = new QProgressDialog("Processing the data...", "&Cancel", 0 , screenVec.at(0)->isMeshScreenXY());
    pprd->setMinimumDuration(0);
    pprd->setWindowTitle("Please Wait");

    long i,j;

        absV.clear(); nu_i.clear();
//        qDebug() << "clear:" << absV << nu_i << "countPointsIntegr" << countPointsIntegr;
        sourceEnvir->prepareConvolution(countPointsIntegr, absV, nu_i);
//        qDebug() << "nu_i" << nu_i;
//        qDebug() << "absV" << absV;
//        long tempSizeNUI = nu_i.size();
        nu2PiC.clear();
        QVector<double>::const_iterator it1 = nu_i.begin();
        for (; it1 != nu_i.end(); ++it1)
        {
            nu2PiC.push_back( (*it1) * 2 * M_PI / sourceEnvir->getSpeedOfLightForCap() );
        }

//        qDebug() << "nu2Pic" << nu2PiC;
  //      /////////////////////////////////////
//        tempzzzIm.clear();
//        tempzzzRe.clear();
//        tempzzzIm.resize(screenVec.at(0)->isMeshScreenXY());
//        tempzzzRe.resize(screenVec.at(0)->isMeshScreenXY());
//        for(i = 0 ; i < screenVec.at(0)->isMeshScreenXY(); ++i)
//        {
//            tempzzzIm[i].resize(tempSizeNUI);
//            tempzzzRe[i].resize(tempSizeNUI);
//        }
     //   //////////////////////////////////

        xgeneral.clear(); xgeneral.resize(screenVec[0]->isMeshScreenXY());
        ygeneral.clear(); ygeneral.resize(screenVec[0]->isMeshScreenXY());
        zgeneral.clear(); zgeneral.resize(screenVec[0]->isMeshScreenXY());
        zgeneralIm.clear(); zgeneralIm.resize(screenVec[0]->isMeshScreenXY());

///     //СЮДА МНОГОПОТОЧНОСТЬ!/////////////////////////////
//        QVector<int> lst;
//        lst.resize(screenVec[0]->isMeshScreenXY());
//        QVector<int>::iterator itlst = lst.begin();
//        for(i = 0
//            ;itlst != lst.end()
//            ; ++itlst, ++i)
//        {
//            (*itlst) = i;
//        }
//        QFuture<bool> specialFuture = QtConcurrent::mapped(lst,this,&Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ);//не работает
        //------------------------------------------------------
//        double tempSchet = 100./screenVec.at(0)->isMeshScreenXY(); //для счётчика готовности
        int tempChislo = screenVec.at(0)->isMeshScreenXY() - (screenVec.at(0)->isMeshScreenXY()%kolvoProc); //для счётчика готовности
        QVector< QFuture<void> > futureVec; futureVec.resize(kolvoProc); //для управления потоками
//        qDebug() << screenVec[0]->isMeshScreenXY() << "tempChislo" <<tempChislo;

        //второй цикл, свёртка
//        qDebug() << "Stage II:";
//        qDebug() << pointIndex;
        numberForPointIndex = 0;
        sizeForPointIndex = totalLength.at(0).at(pointIndex).size();
        if(sizeForPointIndex > 0)
        {
            while( totalAngles.at(0).at(pointIndex).at(numberForPointIndex).isEmpty() )
            {
                ++numberForPointIndex;
                if(numberForPointIndex >= sizeForPointIndex)
                {
                    qDebug() << "There is no points in... (" << pointR_1_X << "," << pointR_1_Y << ") Program needs more points-rays.";
                    if( totalLength.at(0).at(pointIndex).isEmpty() )
                    {
                        numberForPointIndex = -1;
                        return -1;
                    }
                    else
                    {
                        qDebug() << "ok, take first element.";
                        numberForPointIndex = 0;
                        break;
                    }
                    // ???????????????????????????????????????????????????????????????????????????

                }
            } //создали номер точки, относительно которой будем обрабатывать остальное...
        }
        else
        {
            qDebug() << "There is no points in... (" << pointR_1_X << "," << pointR_1_Y << ") Program needs more points-rays.";
            numberForPointIndex = -1;
            return -1;
        }

//        numberForPointIndex = 2;
//        qDebug() << "asdasdasdas" << totalAngles.at(0).at(pointIndex).at(numberForPointIndex).size();

        for(i = 0; i < tempChislo; i += kolvoProc)
        {
            for(j = i; j < i + kolvoProc; ++j)
            {
//                if ( ( (long)(j*tempSchet)-(long)((j-1)*tempSchet) ) > 0.9 )
//                    qDebug() << (long)(j*tempSchet) << "%";
                futureVec[(j-i)] = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ_3, j);
            }
//            threadMainIntegrConvolMNOGOPOTOCHNIJ(tempZc[i],tempZnc[i],totalLength[0][i]
//                                               ,totalAngles[0][i]);

//            this->threadMainIntegrConvol(array
//                                   , totalAngles[0][i]
//                                   , flagWithNonReflRadiation
//                                   , gPF ,gEC);

//            QFuture<void> future = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, i);
//            QFuture<void> future1 = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, i+1);
//            QFuture<void> future2 = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, i+2);
//            QFuture<void> future3 = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, i+3);
//            QFuture<void> future4 = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, i+4);
//            QFuture<void> future5 = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, i+5);

//            future.waitForFinished();
//            future1.waitForFinished();
//            future2.waitForFinished();
//            future3.waitForFinished();
//            future4.waitForFinished();
//            future5.waitForFinished();
            pprd->setValue(i);
            qApp->processEvents();
            if(pprd->wasCanceled())
            {
                qDebug() << ":...calculating is canceled!    @@@ elapsed time:" << elapsCap(time.elapsed());
                delete pprd;
                return -1;
            }
            for(j = i; j < i + kolvoProc; ++j)
            {
                futureVec[(j-i)].waitForFinished();
            }
        }

        for(j = tempChislo; j < screenVec[0]->isMeshScreenXY(); ++j)
        {
            futureVec[(j-tempChislo)] = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ_3, j);

            pprd->setValue(j);
            qApp->processEvents();
            if(pprd->wasCanceled())
            {
                qDebug() << ":...calculating is canceled!    @@@ elapsed time:" << elapsCap(time.elapsed());
                delete pprd;
                return -1;
            }
        }
        for(j = tempChislo; j < screenVec[0]->isMeshScreenXY(); ++j)
        {
            futureVec[(j-tempChislo)].waitForFinished();
        }
///      //////////////////////////////////////////////

//        qDebug() << "Stage III:" << "Save results:";

//        zzzRe.clear(); zzzRe.resize(screenVec[0]->isMeshScreenXY());
//        zzzIm.clear(); zzzIm.resize(screenVec[0]->isMeshScreenXY());
////        zcminus.clear(); zcminus.resize(screenVec[0]->isMeshScreenXY());

////        double tempSchet2 = 100./screenVec[0]->isMeshScreenXY();
//        double ttt = 1;
//        for(i = 0; i < screenVec[0]->isMeshScreenXY(); ++i)
//        {
////            if( ((long)(i*tempSchet2)-(long)((i-1)*tempSchet2)) > 0.9 )
////                qDebug() << (long)i*tempSchet2 << " %";

//            for(l = 0; l < tempSizeNUI; ++l)
//            {
//                if(!totalLength.at(0).at(i).isEmpty())
//                {
//                    ttt = totalLength.at(0).at(i).size();
//                    zzzRe[i] += tempzzzRe[i][l]/ttt;
//                    zzzIm[i] += tempzzzIm[i][l]/ttt;
//                }
//            }
//        }

    pprd->setValue(screenVec.at(0)->isMeshScreenXY());
    qDebug() << ":...Processing is completed!   @@@ elapsed time:" << elapsCap(time.elapsed());
    delete pprd;
    return 0;
}

int Coherence::mainIntegrationConvolutionMNOGOPOTOCHNIJ()
{
    // -1. totalLength.at(0).at(i).size()

    qDebug() << "Performing calculations...:";

    QTime time; time.start();//считаем, сколько времени прошло, от начала счёта

    QProgressDialog * pprd = new QProgressDialog("Processing the data...", "&Cancel", 0 , screenVec.at(0)->isMeshScreenXY());
    pprd->setMinimumDuration(0);
    pprd->setWindowTitle("Please Wait");

    long i,l,j;
//        totalScreen.clear(); totalScreenX.clear(); totalScreenY.clear();
//        totalScreen.resize(screenVec[0]->isMeshScreenXY());
//        totalScreenX.resize(screenVec[0]->isMeshScreenXY());
//        totalScreenY.resize(screenVec[0]->isMeshScreenXY());

        tempZcIm.clear(); tempZnc.clear();
        tempZcRe.clear();
//        tempIncoh.clear();
//        tempGeneral.clear(); tempGeneralIm.clear();
//        tempBasis.clear();
        tempZcIm.resize(screenVec.at(0)->isMeshScreenXY()); tempZnc.resize(screenVec.at(0)->isMeshScreenXY());
        tempZcRe.resize(screenVec.at(0)->isMeshScreenXY());
//        tempIncoh.resize(screenVec[0]->isMeshScreenXY());
//        tempGeneral.resize(screenVec[0]->isMeshScreenXY());
//        tempGeneralIm.resize(screenVec[0]->isMeshScreenXY());
//        tempBasis.resize(screenVec[0]->isMeshScreenXY());

        absV.clear(); nu_i.clear();

        sourceEnvir->prepareConvolution(countPointsIntegr, absV, nu_i);
        long tempSizeNUI = nu_i.size();
        nu2PiC.clear();
        QVector<double>::const_iterator it1 = nu_i.begin();
        for (; it1 != nu_i.end(); ++it1)
        {
            nu2PiC.push_back( (*it1) * 2 * M_PI / sourceEnvir->getSpeedOfLightForCap() );
        }

  //      /////////////////////////////////////
//        tempzzzIm.clear();
//        tempzzzRe.clear();
//        tempzzzIm.resize(screenVec.at(0)->isMeshScreenXY());
//        tempzzzRe.resize(screenVec.at(0)->isMeshScreenXY());
//        for(i = 0 ; i < screenVec.at(0)->isMeshScreenXY(); ++i)
//        {
//            tempzzzIm[i].resize(tempSizeNUI);
//            tempzzzRe[i].resize(tempSizeNUI);
//        }
     //   //////////////////////////////////
        qDebug() << "aaa";

        for(i = 0 ; i < screenVec.at(0)->isMeshScreenXY(); ++i)
        {
            tempZcIm[i].resize(tempSizeNUI);
            tempZnc[i].resize(tempSizeNUI);
            tempZcRe[i].resize(tempSizeNUI);
//            tempIncoh[i].resize(tempSizeNUI);
//            tempGeneral[i].resize(tempSizeNUI);
//            tempGeneralIm[i].resize(tempSizeNUI);
//            tempBasis[i].resize(tempSizeNUI);
        }

//        xgeneral.clear(); xgeneral.resize(screenVec[0]->isMeshScreenXY());
//        ygeneral.clear(); ygeneral.resize(screenVec[0]->isMeshScreenXY());
//        zgeneral.clear(); zgeneral.resize(screenVec[0]->isMeshScreenXY());
//        zgeneralIm.clear(); zgeneralIm.resize(screenVec[0]->isMeshScreenXY());

///     //СЮДА МНОГОПОТОЧНОСТЬ!/////////////////////////////
//        QVector<int> lst;
//        lst.resize(screenVec[0]->isMeshScreenXY());
//        QVector<int>::iterator itlst = lst.begin();
//        for(i = 0
//            ;itlst != lst.end()
//            ; ++itlst, ++i)
//        {
//            (*itlst) = i;
//        }
//        QFuture<bool> specialFuture = QtConcurrent::mapped(lst,this,&Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ);//не работает
        //------------------------------------------------------
//        double tempSchet = 100./screenVec.at(0)->isMeshScreenXY(); //для счётчика готовности
        int tempChislo = screenVec.at(0)->isMeshScreenXY() - (screenVec.at(0)->isMeshScreenXY()%kolvoProc); //для счётчика готовности
        QVector< QFuture<void> > futureVec; futureVec.resize(kolvoProc); //для управления потоками
//        qDebug() << screenVec[0]->isMeshScreenXY() << "tempChislo" <<tempChislo;

        for(i = 0 ; i < tempChislo; i += kolvoProc)
        {
            for(j = i; j < i + kolvoProc; ++j)
            {
//                if ( ( (long)(j*tempSchet)-(long)((j-1)*tempSchet) ) > 0.9 )
//                    qDebug() << (long)(j*tempSchet) << "%";
                futureVec[(j-i)] = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, j);
            }
//            threadMainIntegrConvolMNOGOPOTOCHNIJ(tempZc[i],tempZnc[i],totalLength[0][i]
//                                               ,totalAngles[0][i]);

//            this->threadMainIntegrConvol(array
//                                   , totalAngles[0][i]
//                                   , flagWithNonReflRadiation
//                                   , gPF ,gEC);

//            QFuture<void> future = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, i);
//            QFuture<void> future1 = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, i+1);
//            QFuture<void> future2 = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, i+2);
//            QFuture<void> future3 = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, i+3);
//            QFuture<void> future4 = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, i+4);
//            QFuture<void> future5 = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, i+5);

//            future.waitForFinished();
//            future1.waitForFinished();
//            future2.waitForFinished();
//            future3.waitForFinished();
//            future4.waitForFinished();
//            future5.waitForFinished();
            pprd->setValue(i);
            qApp->processEvents();
            if(pprd->wasCanceled())
            {
                qDebug() << ":...calculating is canceled!    @@@ elapsed time:" << elapsCap(time.elapsed());
                delete pprd;
                return -1;
            }

            for(j = i; j < i + kolvoProc; ++j)
            {
                futureVec[(j-i)].waitForFinished();
            }
        }
        for(j = tempChislo; j < screenVec.at(0)->isMeshScreenXY(); ++j)
        {
            futureVec[(j-tempChislo)] = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, j);

            pprd->setValue(j);
            qApp->processEvents();
            if(pprd->wasCanceled())
            {
                qDebug() << ":...calculating is canceled!    @@@ elapsed time:" << elapsCap(time.elapsed());
                delete pprd;
                return -1;
            }
        }
        for(j = tempChislo; j < screenVec.at(0)->isMeshScreenXY(); ++j)
        {
            futureVec[(j-tempChislo)].waitForFinished();
        }

///      //////////////////////////////////////////////

//        qDebug() << "Stage III:" << "Save results:";

        znc.clear(); znc.resize(screenVec[0]->isMeshScreenXY());//сформируем отдельно
        zcIm.clear(); zcIm.resize(screenVec[0]->isMeshScreenXY());//сформируем отдельно
        zcRe.clear(); zcRe.resize(screenVec[0]->isMeshScreenXY());

//        zcminus.clear(); zcminus.resize(screenVec[0]->isMeshScreenXY());

//        double tempSchet2 = 100./screenVec[0]->isMeshScreenXY();
        double ttt = 1;
        for(i = 0; i < screenVec[0]->isMeshScreenXY(); ++i)
        {
//            if( ((long)(i*tempSchet2)-(long)((i-1)*tempSchet2)) > 0.9 )
//                qDebug() << (long)i*tempSchet2 << " %";

            for(l = 0; l < tempSizeNUI; ++l)
            {
                if(!totalLength.at(0).at(i).isEmpty())
                {
                    ttt = totalLength.at(0).at(i).size();
                    zcIm[i] += tempZcIm[i][l];///totalLength.at(0).at(i).size();
                    znc[i] += tempZnc[i][l]/ttt;
//                    znc[i] += ttt;
                    zcRe[i] += tempZcRe[i][l];
//                    zzzRe[i] += tempzzzRe[i][l]/ttt;
//                    zzzIm[i] += tempzzzIm[i][l]/ttt;
//                    zcminus[i] += tempIncoh[i][l]/ttt;
//                    zgeneral[i] += tempGeneral[i][l]/ttt;
//                    zgeneralIm[i] += tempGeneralIm[i][l]/ttt;
                }
            }
//                zc[i] += tempZc[i][tempSizeNUI/2];
//                znc[i] += tempZnc[i][tempSizeNUI/2];
        }

    pprd->setValue(screenVec.at(0)->isMeshScreenXY());
    qDebug() << ":...Processing is completed!   @@@ elapsed time:" << elapsCap(time.elapsed());
    delete pprd;
    return 0;
}

int Coherence::mainIntegrationConvolutionMNOGOPOTOCHNIJDegOfCoh_NEW()//колдуем по-новому)))
{
    long i,l,j; //счётчики

    qDebug() << QObject::tr("Performing calculations...:");
    QTime time; time.start();//считаем, сколько времени прошло, от начала счёта
    QProgressDialog * pprd = new QProgressDialog("Processing the data...", "&Cancel", 0 , kolvoExp*screenVec.at(0)->isMeshScreenXY());
    pprd->setMinimumDuration(0);
    pprd->setWindowTitle("Please Wait");

    absV.clear(); nu_i.clear();

    sourceEnvir->prepareConvolution(countPointsIntegr, absV, nu_i);
    long tempSizeNUI = nu_i.size();//ну а сколько равно nu_i.size? может столько же, сколько и countPointsInteger?
    //нет, конечно, равно столько, сколько нужно для попадания в диапазон между средним числом и разбросом...
    //а в случае -1, т.е задания спектра вручную вообще определяется руками <-1:начальное значение nu:шаг:кол-во шагов:уравнение модуля в квадрате спектра>
    //или <-1:test.txt> в исходной папке....
    nu2PiC.clear();
    QVector<double>::const_iterator it1 = nu_i.begin();
    for (; it1 != nu_i.end(); ++it1)
    {
        nu2PiC.push_back( (*it1) * 2 * M_PI / sourceEnvir->getSpeedOfLightForCap() );
    }

    zzzRe.clear();
    zzzIm.clear();
    zzzNORM.clear();

    zzzRe.resize(kolvoExp);
    zzzIm.resize(kolvoExp);
    zzzNORM.resize(kolvoExp);
    for(i = 0; i < kolvoExp; ++i)
    {
        zzzRe[i].resize(screenVec[0]->isMeshScreenXY());
        zzzIm[i].resize(screenVec[0]->isMeshScreenXY());
        zzzNORM[i].resize(screenVec[0]->isMeshScreenXY());
    }

    for(iterExp = 0; iterExp < kolvoExp; ++iterExp)
    {
        qDebug() << "iterExp:" << iterExp << "kolvoExp:" << kolvoExp;

        tempzzzRe.clear();
//        tempzzzReNorm.clear();
        tempzzzIm.clear();
//        tempzzzImNorm.clear();
        tempzzzNORM.clear();

        tempzzzRe.resize(screenVec.at(0)->isMeshScreenXY());
        tempzzzIm.resize(screenVec.at(0)->isMeshScreenXY());
        tempzzzNORM.resize(screenVec.at(0)->isMeshScreenXY());

//        tempzzzImNorm.resize(screenVec.at(0)->isMeshScreenXY());
//        tempzzzReNorm.resize(screenVec.at(0)->isMeshScreenXY());
        pprd->setValue(0);
        for(i = 0 ; i < screenVec.at(0)->isMeshScreenXY(); ++i)
        {
            tempzzzRe[i].fill(0.,tempSizeNUI);
            tempzzzIm[i].fill(0.,tempSizeNUI);
            tempzzzNORM[i].fill(0.,tempSizeNUI);
            //            tempzzzImNorm[i].resize(tempSizeNUI);
            //            tempzzzReNorm[i].resize(tempSizeNUI);
            if(i%100 == 0)
                pprd->setLabelText("Preparing for calculations.....");
            if(i%200 == 0)
                pprd->setLabelText("Preparing for calculations...");
            if(i%300 == 0)
                pprd->setLabelText("Preparing for calculations...");
            qApp->processEvents();
        }
        pprd->setLabelText("Processing the data...");

        ///     //СЮДА МНОГОПОТОЧНОСТЬ!/////////////////////////////
        //        QVector<int> lst;
        //        lst.resize(screenVec[0]->isMeshScreenXY());
        //        QVector<int>::iterator itlst = lst.begin();
        //        for(i = 0
        //            ;itlst != lst.end()
        //            ; ++itlst, ++i)
        //        {
        //            (*itlst) = i;
        //        }
        //        QFuture<bool> specialFuture = QtConcurrent::mapped(lst,this,&Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ);//не работает
        //------------------------------------------------------
        //        double tempSchet = 100./screenVec.at(0)->isMeshScreenXY(); //для счётчика готовности
        int tempChislo = screenVec.at(0)->isMeshScreenXY() - (screenVec.at(0)->isMeshScreenXY()%kolvoProc); //для счётчика готовности
        QVector< QFuture<void> > futureVec;
        futureVec.resize(kolvoProc); //для управления потоками

        //        qDebug() << screenVec[0]->isMeshScreenXY() << "tempChislo: " << tempChislo; выводит количество элементов в контейнерах или количество детекторов на экране
        sizeForPointIndex = totalLength.at(0).at(pointIndex).size(); //!!!!!!!!!!!!!!ВАЖНО!!!!!!!!!!!!!!
        qDebug() << "sizeForPointIndex = totalLength.at(0).at(pointIndex).size():" << sizeForPointIndex;

        //!!!!!!!!!НУЖНО ДОБАВИТЬ ПРОВЕРКУ!!!!!!!!!!!!!! sizeForPointIndex

        for(i = 0 ; i < tempChislo; i += kolvoProc)
        {
            for(j = i; j < i + kolvoProc; ++j)
            {
                //                if ( ( (long)(j*tempSchet)-(long)((j-1)*tempSchet) ) > 0.9 )
                //                    qDebug() << (long)(j*tempSchet) << "%";
                futureVec[(j-i)] = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ_2, j);
            }
            //            threadMainIntegrConvolMNOGOPOTOCHNIJ(tempZc[i],tempZnc[i],totalLength[0][i]
            //                                               ,totalAngles[0][i]);

            //            this->threadMainIntegrConvol(array
            //                                   , totalAngles[0][i]
            //                                   , flagWithNonReflRadiation
            //                                   , gPF ,gEC);

            //            QFuture<void> future = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, i);
            //            QFuture<void> future1 = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, i+1);
            //            QFuture<void> future2 = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, i+2);
            //            QFuture<void> future3 = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, i+3);
            //            QFuture<void> future4 = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, i+4);
            //            QFuture<void> future5 = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ, i+5);

            //            future.waitForFinished();
            //            future1.waitForFinished();
            //            future2.waitForFinished();
            //            future3.waitForFinished();
            //            future4.waitForFinished();
            //            future5.waitForFinished();
            pprd->setValue(i + (iterExp*screenVec.at(0)->isMeshScreenXY()));
            qApp->processEvents();
            if(pprd->wasCanceled())
            {
                qDebug() << ":...calculating is canceled!    @@@ elapsed time:" << elapsCap(time.elapsed());
                delete pprd;
                return -1;
            }
            for(j = i; j < i + kolvoProc; ++j)
            {
                futureVec[(j-i)].waitForFinished();
            }
        }
        for(j = tempChislo; j < screenVec.at(0)->isMeshScreenXY(); ++j)
        {
            futureVec[(j-tempChislo)] = QtConcurrent::run(this, &Coherence::threadMainIntegrConvolMNOGOPOTOCHNIJ_2, j);

            pprd->setValue(j + iterExp*screenVec.at(0)->isMeshScreenXY());
            qApp->processEvents();
            if(pprd->wasCanceled())
            {
                qDebug() << ":...calculating is canceled!    @@@ elapsed time:" << elapsCap(time.elapsed());
                delete pprd;
                return -1;
            }
        }
        for(j = tempChislo; j < screenVec.at(0)->isMeshScreenXY(); ++j)
        {
            futureVec[(j-tempChislo)].waitForFinished();
        }

        ///      //////////////////////////////////////////////
        //      спокойненько посчитали всё и занесли в temp... контейнеры...
        //        qDebug() << "Stage III:" << "Save results:";
        //      теперь сохраняем дорогой нам результат:
        //      сделали во внешнем цикле повтор этих операций...

//        zzzReNorm.clear(); zzzReNorm.resize(screenVec[0]->isMeshScreenXY());
//        zzzImNorm.clear(); zzzImNorm.resize(screenVec[0]->isMeshScreenXY());

        double ttt = 1;
        for(i = 0; i < screenVec[0]->isMeshScreenXY(); ++i)
        {
            //            if( ((long)(i*tempSchet2)-(long)((i-1)*tempSchet2)) > 0.9 )
            //                qDebug() << (long)i*tempSchet2 << " %";

            for(l = 0; l < tempSizeNUI; ++l)
            {
                //                if(!totalLength.at(0).at(i).isEmpty())
                //                {
                //                    ttt = totalLength.at(0).at(i).size();
                zzzRe[iterExp][i] += tempzzzRe[i][l];
                zzzIm[iterExp][i] += tempzzzIm[i][l];
                zzzNORM[iterExp][i] += tempzzzNORM[i][l];
//                zzzReNorm[i] += tempzzzReNorm[i][l];
//                zzzImNorm[i] += tempzzzImNorm[i][l];
                //                }
            }
        }
        //        for(i = 0; i < screenVec[0]->isMeshScreenXY(); ++i)
        //        {
        //            ttt = std::sqrt( std::sqrt(std::pow(zzzReNorm[i],2) + std::pow(zzzImNorm[i],2)) );
        //            if(ttt!=0)
        //            {
        //                zzzRe[i] /= ttt;
        //                zzzIm[i] /= ttt;
        //            }
        //        }
    }
    pprd->setValue(kolvoExp*screenVec.at(0)->isMeshScreenXY());
    delete pprd;
    qDebug() << ":...Processing is completed!   @@@ elapsed time:" << elapsCap(time.elapsed());
    return 0;
}

int Coherence::mainIntegrationConvolution()
{
    qDebug() << "Performing calculations...:";
    long int i,j,k,l;
    long int tempSizeNUI;//туда же, что и предыдущее???????????????????
    try
    {
        tempZcIm.clear(); tempZnc.clear();
        tempZcIm.resize(screenVec[0]->isMeshScreenXY()); tempZnc.resize(screenVec[0]->isMeshScreenXY());
        absV.clear(); nu_i.clear();
        sourceEnvir->prepareConvolution(countPointsIntegr, absV, nu_i);

        nu2PiC.clear();
        QVector< double >::const_iterator it1 = nu_i.begin();
        for(; it1 != nu_i.end(); ++it1)
        {
            nu2PiC.push_back( (*it1) * 2 * M_PI / sourceEnvir->getSpeedOfLightForCap() );
        }

        tempSizeNUI = nu_i.size();
        for(i = 0 ; i < screenVec[0]->isMeshScreenXY(); ++i)
        {
            tempZcIm[i].resize(tempSizeNUI);
            tempZnc[i].resize(tempSizeNUI);
        }

        for(i = 0 ; i < screenVec[0]->isMeshScreenXY(); ++i)
        {
            if (i%1000==0)
                qDebug() << "1." << 100 * i/ screenVec[0]->isMeshScreenXY() << "%";
            //СЮДА МНОГОПОТОЧНОСТЬ!///////////////////////////////////////
            QVector<double> tempR1, tempR2;//здесь хранятся коэф. отражения для каждой конкретной частоты для двух выборочных лучей - оно нафиг не надо, тупо темп
//            long int tempSizeNUI;//туда же, что и предыдущее

            double delay = 0.0;//это разность хода
            long int tempLengthSize = totalLength[0][i].size(); //это сколько у нас лучей
            for(j = 0; j < tempLengthSize-1; ++j)
            {
                qDebug() << "array N:" << i << "size array:" << tempLengthSize <<j;
        //                qDebug() << "2." << 100.0*j/(tempLengthSize-1) << "%";
                this->setTempCoefReflNuI(totalAngles[0][i][j],tempR2);

                for(k = j+1; k < tempLengthSize; ++k)
                {

                    //начало для когерентного
                    if(flagWithNonReflRadiation == 1)
                    {
                        this->setTempCoefReflNuI(totalAngles[0][i][k],tempR1);
                        delay = std::fabs(totalLength[0][i][k] - totalLength[0][i][j]);
                        if ( delay < mathforcap->getAccuracy())
                            for(l = 0; l < tempSizeNUI; ++l)
                            {
                                tempZcIm[i][l] += 2*std::sqrt(tempR1[l]*tempR2[l])*absV[l]*std::cos(nu2PiC[l]*delay);
                            }
                        else
                        {
                            for(l = 0; l < tempSizeNUI; ++l)
                            {
                                tempZnc[i][l] += tempR2[l]*absV[l];
                                tempZnc[i][l] += tempR1[l]*absV[l];
                            }
                        }
                    }
                    else if(flagWithNonReflRadiation == 0)
                    {
                        if( (!totalAngles[0][i][j].isEmpty()) || (!totalAngles[0][i][k].isEmpty()) )
                        {
                            this->setTempCoefReflNuI(totalAngles[0][i][k],tempR1);
                            delay = std::fabs(totalLength[0][i][k] - totalLength[0][i][j]);
                            if ( delay < mathforcap->getAccuracy())
                                for(l = 0; l < tempSizeNUI; ++l)
                                {
                                    tempZcIm[i][l] += 2*std::sqrt(tempR1[l]*tempR2[l])*absV[l]*std::cos(nu2PiC[l]*delay);
                                }
                            else
                            {
                                for(l = 0; l < tempSizeNUI; ++l)
                                {
                                    tempZnc[i][l] += tempR2[l]*absV[l];
                                    tempZnc[i][l] += tempR1[l]*absV[l];
                                }
                            }
                        }
                    }

                    //конец для когерентного
                }
//                //начало для некогерентного
//                if(flagWithNonReflRadiation == 1)
//                {
//                    for(l = 0; l < tempSizeNUI; ++l)
//                    {
//                        tempZnc[i][l] += tempR2[l]*absV[l];
//                    }
//                }
//                else if(flagWithNonReflRadiation == 0)
//                {
//                    if (!totalAngles[0][i][j].isEmpty())
//                    {
//                        for(l = 0; l < tempSizeNUI; ++l)
//                            tempZnc[i][l] += tempR2[l]*absV[l];
//                    }
//                }
//                //конец некогрентного
            }
            //////////////////////////////////////////////
        }
//        double dnu = nu_i[2]-nu_i[1];

        znc.clear(); znc.resize(screenVec[0]->isMeshScreenXY());//сформируем отдельно
        zcIm.clear(); zcIm.resize(screenVec[0]->isMeshScreenXY());//сформируем отдельно
        for(i = 0 ; i < screenVec[0]->isMeshScreenXY(); ++i)
        {
            for(l =0; l < tempSizeNUI; ++l)
            {
                zcIm[i] += tempZcIm[i][l];
                znc[i] += tempZnc[i][l];
            }
//                zc[i] += tempZc[i][tempSizeNUI/2];
//                znc[i] += tempZnc[i][tempSizeNUI/2];
        }

    }
    catch (...)
    {
        qDebug() << "###ERROR!";
    }
//    qDebug() << ":...calculations are performed!   @@@ elapsed time:" << elapsCap(time.elapsed());
    return 0;
}

void Coherence::spectrumSetTor(const int &m)
{
    double R = 1;
//    double r_0 = 1e-6;
    tempRCap.push_back(R);
    double L = 0.002*(m+1)*1;

    tempLengthCap.push_back(L);//только для вывод на экран...
    double cosphi = std::sqrt(1-std::pow((L/R),2));
    double sinphi = (L/R);
    double xShift = R*(1-cosphi);
    QSettings settings;
//            settings.setValue("CurrentSettings/xShift_0",QString::number(xShift,'g',16));
//            settings.setValue("CurrentSettings/yShift_0",QString::number(0.));
    QByteArray scrEndCap = QByteArray::number(sinphi,'g',16)+"*(x -" + QByteArray::number(xShift,'g',15) + ")+"
            +QByteArray::number(cosphi,'g',16)+
            "*(z -"+QByteArray::number(L,'g',16)+")";
//    qDebug() << scrEndCap;
    settings.setValue("CurrentSettings/fEndOfCap_0",scrEndCap);
    settings.setValue("CurrentSettings/fScreen_0",scrEndCap);
    screenVec.clear();
    capillaryVec.clear();
    for (int i=0; i < countScreens; ++i)
        screenVec.push_back(new Screen(i+1,this));
    for (int i = 0; i < countCapillaries; ++i)
        capillaryVec.push_back(new Capillary(i+1,this));
}

int Coherence::mainSpectrumExp()
{

    int i,j,l,m;

    tempLengthCap.clear(); tempRCap.clear(); tempr0Cap.clear();

    spectrumScreen.clear(); spectrumIntensScreen.clear(); spectrumSumIntensScreen.clear(); spectrumSumInputIntensWithoutNonRefl.clear();
    spectrumTotalR_0Nu.clear(); spectrumSumInputIntensWithNonRefl.clear();

    countR_0 = 1;//количество экспериментов... то есть сколько радиусов посчитаем))) Предлагаю отдельно всё считать...?

    spectrumScreen.resize(countR_0); spectrumIntensScreen.resize(countR_0); spectrumSumIntensScreen.resize(countR_0); spectrumSumInputIntensWithoutNonRefl.resize(countR_0);
    spectrumTotalR_0Nu.resize(countR_0); spectrumSumInputIntensWithNonRefl.resize(countR_0);

    absV.clear(); nu_i.clear();
    nu_i_imput.clear(); //что это????????????????????
//    sourceEnvir->prepareConvolution(countPointsIntegr, absV, nu_i);

    int tempSizeNUI = 150;//количество дискретизации спектра

    for (l = 0; l < tempSizeNUI; ++l)
    {
        nu_i.push_back((30000+l*100)*1e6/4.135667516);
        nu_i_imput.push_back((32900+l*50)*1e6/4.135667516); // что это???????????????????
        absV.push_back(1e-9); //считаем, что это не модуль квадрат спектра, а просто спектр.
    }

//    nu2PiC.clear();
//    QVector< double >::const_iterator it1 = nu_i.begin();
//    for(; it1 != nu_i.end(); ++it1)
//    {
//        nu2PiC.push_back( (*it1) * 2 * M_PI / sourceEnvir->getSpeedOfLightForCap() );
//    }

    for(m=0; m < countR_0; ++m)
    {
        spectrumScreen[m].resize(screenVec[0]->isMeshScreenXY());
        spectrumIntensScreen[m].resize(screenVec[0]->isMeshScreenXY());
        spectrumTotalR_0Nu[m].fill(0,tempSizeNUI);
    }
    for(m = 0; m < countR_0; ++m)
    {
        for(i = 0; i < screenVec[0]->isMeshScreenXY(); ++i)
        {
            spectrumScreen[m][i].resize(tempSizeNUI);
        }
    }

    //    QVector< QVector< QVector< QVector<double> > > > spectrumScreen;//для каждого r_0, для каждой точки i на экране, для каждой nu_i вектор значения спектра(для каждой точки).
    //    QVector< QVector<double> > spectrumTotalR_0Nu;//для каждого r_0 для каждой Nu значение
    //    QVector< QVector<double> > spectrumIntensScreen;//для каждого r_0, для каждой точки i на экране значение интенсивности
    //    QVector<double> spectrumSumIntensScreen;//для каждого r_0, суммарная интенсивность на экране
    //    QVector<double> spectrumSumInputIntens;//для каждого r_0, суммарная входная интенсивность
    //  /// проверка, раз два ОСНОВНОЙ ЦИКЛ!
    double tempa;
    for(m = 0; m < countR_0; ++m)
    {
        if(DEBUGintSpectrumTorOrCap == 0) //циллиндр
        {
            qDebug() << "!cylynder!";
            tempLengthCap.push_back(DEBUGdouleInitLength + 500*m);
            QSettings settings;
            QByteArray scrEndCap = "z-" + QByteArray::number(tempLengthCap[m],'g',6);
            settings.setValue("CurrentSettings/fEndOfCap_0",scrEndCap);
            settings.setValue("CurrentSettings/fScreen_0",scrEndCap); //забили новую заглушку и экран точно такой же.
            screenVec.clear();
            capillaryVec.clear();
            for (int i=0; i < countScreens; ++i)
                screenVec.push_back(new Screen(i+1,this));
            for (int i = 0; i < countCapillaries; ++i)
                capillaryVec.push_back(new Capillary(i+1,this)); //создали новые экраны и капилляры... В общем-то надо избавляться от таких
            //метод и ставить многопоточность в один файл установки... <начальное значение:шаг:кол-во шагов>
        }
        else if(DEBUGintSpectrumTorOrCap == 1) //тор
        {
            this->spectrumSetTor(m);
        }

//      //    ui->centralWidget->setEnabled(false);
//          this->setEnabled(false);
//          if(coherence->DAQ() == 0)
//          {
//              coherence->SaveTotal();
//              this->DAQdone();
//          }
//          this->setEnabled(true);
//          QIcon icon_;
//          icon_.addFile(QString::fromUtf8(":/images/Warning"), QSize(), QIcon::Normal, QIcon::Off);
//          ui->toolButton_calcIntens->setIcon(icon_);
//          ui->toolButton_calcCoher_2->setIcon(icon_);
//      //    ui->centralWidget->setEnabled(true);
   ///-это из calcTheRays....

        //        double newRad = std::sqrt((m+1)*1.0);
        //нужно задать изменяющиеся параметры
//        QByteArray tempF0 = "x^2+y^2-"+QByteArray::number(newRad);
//        capillaryVec[0]->setF0(tempF0);
//        double angleTheta = 30.0/33000.0;
//        sourceEnvir->slotSetZ_0_(-newRad/(std::tan(angleTheta))-1e-13);
//        sourceEnvir->slotSetZ_0__(-newRad/(std::tan(angleTheta)));
//        angleTheta -= 5e-5;
//        sourceEnvir->slotSetTheta_0_(-angleTheta);
//        sourceEnvir->slotSetTheta_0__(angleTheta);

        //итак,
        for(i = 0 ; i < screenVec[0]->isMeshScreenXY(); ++i)
        {
            if (i%1000==0)
                qDebug() << "1." << 100 * i/ screenVec[0]->isMeshScreenXY() << "%";
            QVector<double> tempR2;//здесь хранятся коэф. отражения для каждой конкретной частоты для двух выборочных лучей

            long int tempLengthSize = totalLength[0][i].size(); //это сколько у нас лучей
            for(j = 0; j < tempLengthSize; ++j)
            {
//                qDebug() << "array N:" << i << "size array:" << tempLengthSize <<j;

                this->setTempCoefReflNuI(totalAngles[0][i][j],tempR2);

                if( !totalAngles[0][i][j].isEmpty() )
                {
                    for(l = 0; l < tempSizeNUI; ++l)
                    {
                        tempa = tempR2[l]*((4.135667517e-6)*absV[l]);
                        spectrumScreen[m][i][l].push_back(tempa);//eV
                        spectrumTotalR_0Nu[m][l] += tempa;//eV
                        spectrumSumIntensScreen[m] += tempa;
                        spectrumIntensScreen[m][i] += tempa;//eV
                        spectrumSumInputIntensWithNonRefl[m] += (4.135667517e-6)*absV[l];//eV
                        spectrumSumInputIntensWithoutNonRefl[m] += (4.135667517e-6)*absV[l];//eV
                    }
                }
                else
                {
                    for(l = 0; l < tempSizeNUI; ++l)
                    {
                        spectrumSumInputIntensWithNonRefl[m] += (4.135667517e-6)*absV[l];//eV
                    }
                }
            }
        }

    }

    for(m = 0; m < countR_0; ++m)
    {
        spectrumSumInputIntensWithNonRefl[m] /= countRays;//работа, произведённая одной частицей, на неё будем нормировать, I_0 по сути
        spectrumSumInputIntensWithoutNonRefl[m] /= (countRays*spectrumSumInputIntensWithNonRefl[m]);//сколько из неё захватилось капилляром
        spectrumSumIntensScreen[m] /= (countRays*spectrumSumInputIntensWithNonRefl[m]);//сколько из неё мы получили на экране
        qDebug() << "Result:::" << m << 1 << spectrumSumInputIntensWithoutNonRefl[m] << spectrumSumIntensScreen[m];
    }
    qDebug() << "saving to file...";
    this->SaveSpectrumExp();//сохраняем это всё в файлы..
    qDebug() << "done!";

    return 0;
}

void Coherence::sendRayMNOGOPOTOCHNIJ(const long int &numOfExpRay) //к сожалению, многопоточно не работает!!!
{
    // +-- 1. Переписать [] в QVector<double>
    // +: не используется 2. Проверить, используется ли где-то ещё directionVectorLine[]
    // + 3. flagNumCap надо хранить и передавать отдельно
    // - 4. переписать полностью ход от экрана
    // - 5. Этот QMap очень плох здесь, разобрать как работают параллельные вычисления, и всё переписать!!!!

    long i;//счётчик
//    сюда всё запишется:
//    QMap< int, QList< QVector<double> > > result;
//..............................
    /// ------------------- ни что иное, как новые значения, которые будут созданы...--------------------
//    int flagNumCapPar = -1; //-> QVector<double>;
//    *(numCapPar.begin()) = -1;//номер капилляра, которым захватился луч
    QVector<double> numCapPar; numCapPar.resize(1); //номер  капилляра, которым захватился луч -> храним в массиве для удобства сохранения, на самом деле не важно!
    QVector<double> pointRSouPar; pointRSouPar.resize(numOfVar);
//    double pointsXsouPar, pointsYsouPar, pointsZsouPar; //<-- заменены на ветор!
    QVector<double> pointsXPar, pointsYPar, pointsZPar;             //888 контейнеры с точками отражения, первая точка - положение точечного источника
    QVector<double> pointsXscrPar, pointsYscrPar, pointsZscrPar;    //999 точка прихода одного луча для всех экранов
    pointsXscrPar.resize(countScreens); pointsYscrPar.resize(countScreens); pointsZscrPar.resize(countScreens);
//    directingVectorLine[numOfVar];                                //999 направляющий вектор прямой
    QVector<double> directingVectorLinePar; directingVectorLinePar.resize(numOfVar);
    QVector<double> anglesGlacingPar;                               //888 углы отражения только на капилляре
    QVector<double> anglesScrPhiPar, anglesScrThetaPar;              //999 углы в точке прихода луча на экране для всех экранов
    anglesScrPhiPar.resize(countScreens); anglesScrThetaPar.resize(countScreens);
    QVector<double> lengthOfRayPar;
    lengthOfRayPar.resize(countScreens); lengthOfRayPar[0] = 0;
    /// ----------------------------------------------------------------------------------------------------

    bool flagCapture = false; //если луч не захватился ни одни капилляром, то выбираем новый луч сразу.

    int captureRay = - 1; //значение, возвращаемое функцией прохождения сквозь капилляр
    QVector<double> prevP;
    prevP.resize(numOfVar);//точка, куда сохраняется первая точка источника, а потом используется для след. точек
//    qDebug() << "SendRay 0:";

    while (captureRay < 0)
    {

        if(mathforcap->rayGoesUp == 1)//если луч летит от источника
        {
            flagCapture = false;
            while (!flagCapture)
            {
                sourceEnvir->getRandomX_0Y_0Z_0(pointRSouPar.data());//берём рандомную точку на источнике
                sourceEnvir->getRandomPhi_0Theta_0(directingVectorLinePar.data()); // пока не будем сохранять углы источника
                flagCapture = false;
                prevP = pointRSouPar;
                for(i = 0; (i < countCapillaries)&&(!flagCapture); ++i)
                {
//                    m_mutex.lock();
                    flagCapture = capillaryVec.at(i)->tryCaptureSou(prevP.data(),directingVectorLinePar.data());
//                    m_mutex.unlock();
                    //после вызова здесь в prevP2 храниться точка пересечения c bottom уравнением. -> может понадобится, а может и нет))
                    //но хранит не долго, далее сотрётся при поиске точки на источнике
                    if(flagCapture)
                    {
                        *(numCapPar.begin()) = i;
                    }
                }
//                qDebug() << "Send Ray 1: prevP2:" <<  prevP2[0] << prevP2[1] << prevP2[2];
//                pointsX.push_back(prevP2[0]); pointsY.push_back(prevP2[1]); pointsZ.push_back(prevP2[2]);
                pointsXPar.clear(); pointsYPar.clear(); pointsZPar.clear();
//                временно добавляем её в начало массива - потом внутри она удалится, а останется в points!sou
                pointsXPar.push_back(pointRSouPar.at(0)); pointsYPar.push_back(pointRSouPar.at(1));  pointsZPar.push_back(pointRSouPar.at(2));
            }
        }
        else // если rayGoesUp == 0, то только один экран!!!!!!!!!! луч проходит от экрана
        {
            double tempRandPhi_0 = 0; double tempRandTheta_0 = 0; //можно и не обнулять, случайные углы будем получать...
            double tempRandX_0 = 0; double tempRandY_0 = 0; double tempRandZ_0 = 0; //и точки источника

            flagCapture = false;
            while(flagCapture == false)
            {
                screenVec[0]->getRandomX_0Y_0Z_0(tempRandX_0, tempRandY_0, tempRandZ_0);
                screenVec[0]->getRandomPhi_0Theta_0(tempRandPhi_0, tempRandTheta_0);
                pointsXscrPar[0] = tempRandX_0; pointsYscrPar[0] = tempRandY_0; pointsZscrPar[0] = tempRandZ_0;
                pointsXPar.clear(); pointsYPar.clear(); pointsZPar.clear();
//                pointsX.push_back(tempRandX_0); pointsY.push_back(tempRandY_0); pointsZ.push_back(tempRandZ_0); //первая точка - экран
//                anglesGlacing.push_back(0.0); //соответствует точке экрана
                anglesScrPhiPar[0] = tempRandPhi_0; anglesScrThetaPar[0] = tempRandTheta_0;
                prevP[0] = tempRandX_0; prevP[1] = tempRandY_0; prevP[2] = tempRandZ_0;
                for(i = 0; (i < countCapillaries)&&(flagCapture == false); ++i)
                {
                    flagCapture = capillaryVec[i]->tryCaptureScr(prevP.data(),tempRandPhi_0,tempRandTheta_0);
                    if(flagCapture)
                        *(numCapPar.begin()) = i;
                }
            }
            directingVectorLinePar[0] = std::cos(tempRandPhi_0)*std::sin(tempRandTheta_0);
            directingVectorLinePar[1] = std::sin(tempRandPhi_0)*std::sin(tempRandTheta_0);
            directingVectorLinePar[2] = std::cos(tempRandTheta_0);
        }

//        m_mutex.lock();
//        qDebug() << "Send Ray 1: directingVectorLine:" << directingVectorLine[0] << directingVectorLine[1] << directingVectorLine[2];
            captureRay = capillaryVec[(int)(*(numCapPar.constBegin()))]->CalculationOfPassageOfTheBeam(directingVectorLinePar.data()
                                                                    ,pointsXPar,pointsYPar,pointsZPar
                                                                                 ,anglesGlacingPar,0);//???????????????????????????
//        m_mutex.unlock();

        if(captureRay == 2) //надо искать последнюю точку - на экране
        {
//            qDebug() << "                                   Всё нашли, ищем точку на экране";
            for (i = 0; i < countScreens; ++i)
            {
                if (!pointsXPar.isEmpty())
                {
                    prevP[0] = pointsXPar.last();
                    prevP[1] = pointsYPar.last();
                    prevP[2] = pointsZPar.last();
                }
                else
                {
//                    qDebug() << "                                   Луч ни разу не отразился!";
                    prevP = pointRSouPar;
                }
//                qDebug() << "!@@directingVectorLine" << directingVectorLine[0] << directingVectorLine[1] << directingVectorLine[2];
//                qDebug() << "prevP" << prevP[0] << prevP[1] << prevP[2];
//                m_mutex.lock();
                if( screenVec[i]->CalculationOfPassageOfTheBeam(directingVectorLinePar.data(), prevP.data()
                                                                ,pointsXscrPar[i],pointsYscrPar[i],pointsZscrPar[i]
                                                                ,anglesScrPhiPar[i],anglesScrThetaPar[i]) != 0)
                {
//                    qDebug() << QObject::trUtf8("### ERROR Point on the screen was not found!");
                    captureRay = -13;
//                    return true;
                }
//                m_mutex.unlock();
            }
        }
        if(captureRay == 3)//последняя точка - на источнике
        {
            if (!pointsXPar.isEmpty())
            {
//                qDebug() << "                                   Количество отражений от капилляра:" << pointsX.size();
                prevP[0] = pointsXPar.last();
                prevP[1] = pointsYPar.last();
                prevP[2] = pointsZPar.last();
            }
            else
            {
//                qDebug() << "Луч ни разу не отразился!";
                prevP = pointsXscrPar;
            }
            if ( sourceEnvir->CalculationOfPassageOfTheBeam(directingVectorLinePar.data(), prevP.data()
                                                            ,pointRSouPar[0],pointRSouPar[1],pointRSouPar[2]) != 0)
            {
//                qDebug() << QObject::trUtf8("### ERROR Point on the source was not found");
                captureRay = -13;
//                return true;
            }
        }
//      луч прошёл по капилляру, если captureRay==0, то всё хорошо... если нет, то луч дефективный

    }

//    m_mutex.lock();
    //Начало счёта длин луча!!!
    if (captureRay != -13)
    {
        //разная длина для разных экранов, потому и i
//        lengthOfRayPar.resize(countScreens); //выше уже обнулил
//        lengthOfRayPar[0] = 0;

//        long int tempSize = pointsXPar.size();//DELETE ME

        QVector<double>::const_iterator itpointsXPar, itpointsYPar, itpointsZPar;
        QVector<double>::const_iterator itpointRSouPar = pointRSouPar.constBegin();
        if ( pointsXPar.isEmpty() )
        {
            for(i = 0; i < countScreens; ++i )
            {
                lengthOfRayPar[i] += std::sqrt( std::pow(pointsXscrPar[i] - *itpointRSouPar,2) +
                                            std::pow(pointsYscrPar[i] - *(itpointRSouPar+1),2) +
                                            std::pow(pointsZscrPar[i] - *(itpointRSouPar+2),2) );
            }
        }
        else
        {
            itpointsXPar = pointsXPar.constBegin();
            itpointsYPar = pointsYPar.constBegin();
            itpointsZPar = pointsZPar.constBegin();

            lengthOfRayPar[0] += std::sqrt( std::pow( *itpointsXPar - *itpointRSouPar, 2) +
                                         std::pow( *itpointsYPar - *(itpointRSouPar+1), 2) +
                                         std::pow( *itpointsZPar - *(itpointRSouPar+2), 2) );
            ++itpointsXPar;
            ++itpointsYPar;
            ++itpointsZPar;
            for (
                 ; itpointsXPar != pointsXPar.constEnd()
                 ;  ++itpointsXPar, ++itpointsYPar, ++itpointsZPar
                 )
            {
                lengthOfRayPar[0] += std::sqrt(std::pow(*itpointsXPar - *(itpointsXPar-1),2)
                                            + std::pow(*itpointsYPar - *(itpointsYPar-1),2)
                                            + std::pow(*itpointsZPar - *(itpointsZPar-1),2));
            }
            for (i = 1; i < countScreens; ++i)
            {
                lengthOfRayPar[i] = lengthOfRayPar[0];
            }
            for (i = 0; i < countScreens; ++i)
            {
                lengthOfRayPar[i] += std::sqrt(std::pow(pointsXscrPar[i] - *(itpointsXPar-1),2)
                                             + std::pow(pointsYscrPar[i] - *(itpointsYPar-1),2)
                                             + std::pow(pointsZscrPar[i] - *(itpointsZPar-1),2));
            }
        }
//        return false;
    }

//    return true;
    // сохранение результатов:
//    m_mutex.lock();
    mapResultRay.insert(numOfExpRay, (QList< QVector<double> >()
                                      << numCapPar
                                      << pointRSouPar
                                      << pointsXPar << pointsYPar << pointsZPar
                                      << pointsXscrPar << pointsYscrPar << pointsZscrPar
                                      << directingVectorLinePar
                                      << anglesGlacingPar
                                      << anglesScrPhiPar << anglesScrThetaPar
                                      << lengthOfRayPar)
                        );
//    qDebug() << mapResultRay;
//    m_mutex.unlock();
} //--------------------------------------------луч прогнали и следует далее создать:----------------------------------------------
//    int flagNumCap
//    double pointsXsou,pointsYsou,pointsZsou;            //999
//    QVector<double> pointsX, pointsY, pointsZ;          //888 контейнеры с точками отражения, первая точка - положение точечного источника
//    QVector<double> pointsXscr, pointsYscr, pointsZscr; //999 точка прихода одного луча для всех экранов
//    double directingVectorLine[MathForCap::numOfVar];   //999 направляющий вектор прямой
//    QVector<double> anglesGlacing;                      //888 углы отражения только на капилляре
//    QVector<double> anglesScrPhi,anglesScrTheta;        //999 углы в точке прихода луча на экране для всех экранов
//    QVector<double> lengthOfRay;                        //888 длины путей лучей для всех экранов

void Coherence::setMapResultRay(const long & numOfExpRay)
{
    QMap< long int, QList< QVector<double> > >::const_iterator itmap = mapResultRay.find(numOfExpRay);
//    qDebug() << itmap.value();
    flagNumCap = (int)(itmap.value().at(0).at(0));
    pointRsou = itmap.value().at(1);
    pointsX = itmap.value().at(2);
    numOfRefl = itmap.value().at(2).size();
    pointsY = itmap.value().at(3);
    pointsZ = itmap.value().at(4);
    pointsXscr = itmap.value().at(5);
    pointsYscr = itmap.value().at(6);
    pointsZscr = itmap.value().at(7);
    directingVectorLine[0] = itmap.value().at(8).at(0);
    directingVectorLine[1] = itmap.value().at(8).at(1);
    directingVectorLine[2] = itmap.value().at(8).at(2);
    anglesGlacing = itmap.value().at(9);
    anglesScrPhi = itmap.value().at(10);
    anglesScrTheta = itmap.value().at(11);
    lengthOfRay = itmap.value().at(12);
//    qDebug() << "flagNumCap" << flagNumCap;
//    qDebug() << "pointsXsou" << pointsXsou;
//    qDebug() << "pointsYsou" << pointsYsou;
//    qDebug() << "pointsZsou" << pointsZsou;
//    qDebug() << "pointsX" << pointsX;
//    qDebug() << "pointsY" << pointsY;
//    qDebug() << "pointsZ" << pointsZ;
//    qDebug() << "pointsXscr" << pointsXscr;
//    qDebug() << "pointsYscr" << pointsYscr;
//    qDebug() << "pointsZscr" << pointsZscr;
//    qDebug() << "directingVectorLine[0]" << directingVectorLine[0];
//    qDebug() << "directingVectorLine[1]" << directingVectorLine[1];
//    qDebug() << "directingVectorLine[2]" << directingVectorLine[2];
//    qDebug() << "anglesGlacing" << anglesGlacing;
//    qDebug() << "anglesScrPhi" << anglesScrPhi;
//    qDebug() << "anglesScrTheta" << anglesScrTheta;
//    qDebug() << "lengthOfRay" << lengthOfRay;
} //записывает все основные параметры для одного луча, как я делал в прошлом.

void Coherence::setTotalMNOGOPOTOCHNIJ(const long &numOfExpRay) //после каждого нового луча добавляем прямо в Coherence::DAQ
{
    // - 1. нету numOfRefl и не уверен, что правильно работает, что с l???????????

    QVector<double> tempRSou; tempRSou.resize(numOfVar);
    QMap< long int, QList< QVector<double> > >::const_iterator itmap = mapResultRay.find(numOfExpRay);
//    qDebug() << itmap.value();
    flagNumCap = (int)(itmap.value().at(0).at(0)); //+  - не знаю, может понадобится для рисовалок - оставим здесь.
//+    pointRsou = itmap.value().at(1);
//    pointsX = itmap.value().at(2);
//    pointsY = itmap.value().at(3);
//    pointsZ = itmap.value().at(4);
//    pointsXscr = itmap.value().at(5);
//    pointsYscr = itmap.value().at(6);
//    pointsZscr = itmap.value().at(7);
//    directingVectorLine[0] = itmap.value().at(8).at(0);
//    directingVectorLine[1] = itmap.value().at(8).at(1);
//    directingVectorLine[2] = itmap.value().at(8).at(2);
//+    anglesGlacing = itmap.value().at(9);
//    anglesScrPhi = itmap.value().at(10);
//+    anglesScrTheta = itmap.value().at(11);
//+    lengthOfRay = itmap.value().at(12);

    long int index;
    int l = 0;

    for (l = 0; l < countScreens; ++l)
    {
        index = screenVec[l]->getIndexOfLastPoint(itmap.value().at(5).at(l),itmap.value().at(6).at(l));
        totalLength[l][index].push_back(itmap.value().at(12).at(l));
        totalAngles[l][index].push_back(itmap.value().at(9));
        totalNumOfCap[l][index].push_back((int)(itmap.value().at(0).at(0)));

        tempRSou = itmap.value().at(1);
        totalPointRSou[l][index].push_back(tempRSou);
        totalPointRScr[l][index].push_back(
                    (QVector<double>() << itmap.value().at(5).at(l) << itmap.value().at(6).at(l) << itmap.value().at(7).at(l))
                    );
        totalAmpl[l][index].push_back(sourceEnvir->getAmpl(tempRSou));
        totalPhase[l][index].push_back(sourceEnvir->getPhase(tempRSou));

//        totalPointsX[l][index].push_back(itmap.value().at(2)); //НЕ НУЖНО!!!
//        totalPointsY[l][index].push_back(itmap.value().at(3)); //НЕ НУЖНО!!!
//        totalPointsZ[l][index].push_back(itmap.value().at(4)); //НЕ НУЖНО!!!
//        totalAverCoefOfRefl[l][index].push_back(setAverCoefRefl()); //НЕ НУЖНО!!!
        if(itmap.value().at(11).at(l) > totalMaxTheta[l][index])
        {
            totalMaxTheta[l][index] = itmap.value().at(11).at(l);
        }
        if(itmap.value().at(11).at(l) < totalAverTheta[l][index])
        {
            totalAverTheta[l][index] = itmap.value().at(11).at(l);
        }
    }
} //новый метод, сохраняет только то, что нужен. Не пригоден для рисования точек отражения - они не сохраняются.


void Coherence::setAverageDegOfCoh()
{
    long i,j;
    long sizeScr = screenVec[0]->isMeshScreenXY();
    long sizeExp = kolvoExp;

    zzzTotal.clear(); zzzTotal.fill(0.,sizeScr);
    zzzNORMTotal.clear(); zzzNORMTotal.fill(1.,sizeScr);

    for(i = 0; i < sizeScr; ++i)
    {
        for(j = 0; j < sizeExp; ++j)
        {
            zzzTotal[i] += std::sqrt(std::pow(zzzRe[j][i],2) + std::pow(zzzIm[j][i],2));
            zzzNORMTotal[i] += zzzNORM[j][i];
        }
        zzzTotal[i] /= sizeExp;
        zzzNORMTotal[i] /= sizeExp;
    }

}
