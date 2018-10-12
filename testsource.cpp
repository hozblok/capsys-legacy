#include "testsource.h"

testSource::testSource( /*QObject *parent*/ ) :
    mglDraw()//, QObject(parent)
  , numOfPointsForTest(100)
  , flagDraw(-1)
{

}

int testSource::Draw(mglGraph *gr)
{
    QByteArray lengthUnit = "";
    if(lengthUnitMultPower == -6)
    {
        lengthUnit = "\\mu m";
    }
    else if(lengthUnitMultPower == -5)
    {
        lengthUnit = "10\\cdot\\mu m";
    }
    else if(lengthUnitMultPower == -4)
    {
        lengthUnit = "10^2\\cdot\\mu m";
    }
    else if(lengthUnitMultPower == -3)
    {
        lengthUnit = "mm";
    }
    else if(lengthUnitMultPower == -2)
    {
        lengthUnit = "sm";
    }
    else if(lengthUnitMultPower == -1)
    {
        lengthUnit = "10^-1\\cdotm";
    }
    else if(lengthUnitMultPower == 0)
    {
        lengthUnit = "m";
    }
    else if(lengthUnitMultPower == 1)
    {
        lengthUnit = "10\\cdotm";
    }
    else if(lengthUnitMultPower == 2)
    {
        lengthUnit = "10^2\\cdotm";
    }
    else if(lengthUnitMultPower == 3)
    {
        lengthUnit = "km";
    }

    int i;


//    gr->SetSize(1024,768);
    gr->SetDefScheme("WyqrR");
    gr->SetFontSizePT(8);

    mglExpr mglfSource(fSource.data());
    mglExpr mglfAmpl(fAmpl.data());
    mglExpr mglfPhase(fPhase.data());
    mglExpr mglfScopePhase(fScopePhase.data());

    QVector<double> tx,ty,tz,tc,tphase;
    tx.clear(); ty.clear(); tz.clear();
    tc.clear(); tphase.clear();
    tx.resize(numOfPointsForTest); ty.resize(numOfPointsForTest); tz.resize(numOfPointsForTest);
    tc.resize(numOfPointsForTest); tphase.resize(numOfPointsForTest);

    for(i = 0; i<numOfPointsForTest; ++i)
    {
        do
        {
            tx[i] = ( rand.GetRandomExcExc(x_0_,x_0__) );
            ty[i] = ( rand.GetRandomExcExc(y_0_,y_0__) );
            tz[i] = ( rand.GetRandomExcExc(z_0_,z_0__) );
        }
        while( mglfSource.Eval(tx[i],ty[i],tz[i]) > 0 );
        tc[i] = ( mglfAmpl.Eval(tx[i],ty[i],tz[i]) );
        if( mglfScopePhase.Eval(tx[i],ty[i],tz[i]) > 0 )
        {
            tphase[i] = mglfPhase.Eval(tx[i],ty[i],tz[i]);
        }
        else
        {
            tphase[i] = 0.;
        }
    }

    mglData x(numOfPointsForTest);
    mglData y(numOfPointsForTest);
    mglData c(numOfPointsForTest);
    mglData phase(numOfPointsForTest);

    x.Set(tx.toStdVector().data(), numOfPointsForTest);
    y.Set(ty.toStdVector().data(), numOfPointsForTest);
    c.Set(tc.toStdVector().data(), numOfPointsForTest);
    phase.Set(tphase.toStdVector().data(), numOfPointsForTest);

    if(flagDraw == 0)//measurement
    {
//        gr->SubPlot(2,2,0);
        gr->SetRange('x',x);
        gr->SetRange('y',y);
        gr->SetRange('c',0,1);
        gr->Colorbar(">WyqrR");
        //    gr->SetRanges(0,tauF,-1.5,1.5);
        gr->Dots(x,y,c,"");
        gr->Axis(); gr->Grid();
        gr->Label('x',"x, "+lengthUnit,1);
        gr->Label('y',"y, "+lengthUnit,1);
        //    gr->Label('c',"amplitude, A(x,y)",1);

//        gr->SubPlot(2,2,1);
        //...//

    }
    else if(flagDraw == 1)//time
    {
        gr->SetRange('x',x);
        gr->SetRange('y',y);
        gr->SetRange('c',phase);
        gr->Colorbar(">WyqrR");
        //    gr->SetRanges(AverNu-2*(1/tau),AverNu+2*(1/tau),-1.5,1.5);
        gr->Dots(x,y,phase,"");
        gr->Axis(); gr->Grid();
        gr->Label('x',"x, "+lengthUnit,1);
        gr->Label('y',"y, "+lengthUnit,1);
        //    gr->Label('z',"t_{delay}, ns",1);
    }

    return 0;
}
