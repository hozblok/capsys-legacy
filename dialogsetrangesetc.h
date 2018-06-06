#ifndef DIALOGSETRANGESETC_H
#define DIALOGSETRANGESETC_H

#include <QDialog>

namespace Ui {
class DialogSetRangesETC;
}

class DialogSetRangesETC : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogSetRangesETC(QWidget *parent = 0);
    ~DialogSetRangesETC();
    void setParams(double xr_= 0,double xr__= 0
            ,double yr_= 0,double yr__= 0
            ,double zr_= 0,double zr__= 0
            ,double rxt= 0,double ryt= 0, double rzt= 0);

    double getXR_() { return xrang_; }
    double getYR_() { return yrang_; }
    double getZR_() { return zrang_; }
    double getXR__() { return xrang__; }
    double getYR__() { return yrang__; }
    double getZR__() { return zrang__; }
    double getRx() { return rx; }
    double getRy() { return ry; }
    double getRz() { return rz; }

protected:
    void changeEvent(QEvent *e);
    
private:
    double xrang_,xrang__,yrang_,yrang__,zrang_,zrang__;
    double rx,ry,rz;
    Ui::DialogSetRangesETC *ui;

private slots:
    void slotSetRx(double t) {rx = t;}
    void slotSetRy(double t) {ry = t;}
    void slotSetRz(double t) {rz = t;}
    void slotSetXR_(double t) {xrang_ = t;}
    void slotSetYR_(double t) {yrang_ = t;}
    void slotSetZR_(double t) {zrang_ = t;}
    void slotSetXR__(double t) {xrang__ = t;}
    void slotSetYR__(double t) {yrang__ = t;}
    void slotSetZR__(double t) {zrang__ = t;}
    void on_buttonBox_accepted();
    void on_pushButton_anglreset_clicked();
    void on_pushButton_z_reset_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
};

#endif // DIALOGSETRANGESETC_H
