#include "dialogsetrangesetc.h"
#include "ui_dialogsetrangesetc.h"

DialogSetRangesETC::DialogSetRangesETC(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSetRangesETC)
{
    ui->setupUi(this);
    slotSetRx(0); slotSetRz(0);
    connect(ui->doubleSpinBox_x_,SIGNAL(valueChanged(double)),this,SLOT(slotSetXR_(double)));
    connect(ui->doubleSpinBox_y_,SIGNAL(valueChanged(double)),this,SLOT(slotSetYR_(double)));
    connect(ui->doubleSpinBox_z_,SIGNAL(valueChanged(double)),this,SLOT(slotSetZR_(double)));
    connect(ui->doubleSpinBox_x__,SIGNAL(valueChanged(double)),this,SLOT(slotSetXR__(double)));
    connect(ui->doubleSpinBox_y__,SIGNAL(valueChanged(double)),this,SLOT(slotSetYR__(double)));
    connect(ui->doubleSpinBox_z__,SIGNAL(valueChanged(double)),this,SLOT(slotSetZR__(double)));
    connect(ui->doubleSpinBox_rx, SIGNAL(valueChanged(double)),this,SLOT(slotSetRx(double)));
    connect(ui->doubleSpinBox_ry, SIGNAL(valueChanged(double)),this,SLOT(slotSetRy(double)));
    connect(ui->doubleSpinBox_rz, SIGNAL(valueChanged(double)),this,SLOT(slotSetRz(double)));
}

DialogSetRangesETC::~DialogSetRangesETC()
{
    delete ui;
}

void DialogSetRangesETC::setParams(double xr_, double xr__, double yr_, double yr__, double zr_, double zr__, double rxt, double ryt, double rzt)
{
    ui->doubleSpinBox_x_->setValue(xr_);
    ui->doubleSpinBox_y_->setValue(yr_);
    ui->doubleSpinBox_z_->setValue(zr_);
    ui->doubleSpinBox_x__->setValue(xr__);
    ui->doubleSpinBox_y__->setValue(yr__);
    ui->doubleSpinBox_z__->setValue(zr__);
    ui->doubleSpinBox_rx->setValue(rxt);
    ui->doubleSpinBox_ry->setValue(ryt);
    ui->doubleSpinBox_rz->setValue(rzt);
}

void DialogSetRangesETC::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void DialogSetRangesETC::on_buttonBox_accepted()
{
    this->reject();
}

void DialogSetRangesETC::on_pushButton_anglreset_clicked()
{
    ui->doubleSpinBox_rx->setValue(60.);
    ui->doubleSpinBox_ry->setValue(0.);
    ui->doubleSpinBox_rz->setValue(40.);
}

void DialogSetRangesETC::on_pushButton_z_reset_clicked()
{
    ui->doubleSpinBox_z_->setValue(0.);
}

void DialogSetRangesETC::on_pushButton_2_clicked()
{
    ui->doubleSpinBox_x_->setValue(ui->doubleSpinBox_x_->value()/2);
    ui->doubleSpinBox_x__->setValue(ui->doubleSpinBox_x__->value()/2);
    ui->doubleSpinBox_y_->setValue(ui->doubleSpinBox_y_->value()/2);
    ui->doubleSpinBox_y__->setValue(ui->doubleSpinBox_y__->value()/2);
}

void DialogSetRangesETC::on_pushButton_clicked()
{
    ui->doubleSpinBox_x_->setValue(ui->doubleSpinBox_x_->value()*2);
    ui->doubleSpinBox_x__->setValue(ui->doubleSpinBox_x__->value()*2);
    ui->doubleSpinBox_y_->setValue(ui->doubleSpinBox_y_->value()*2);
    ui->doubleSpinBox_y__->setValue(ui->doubleSpinBox_y__->value()*2);
}
