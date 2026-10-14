#include "xpdfwidgetdemo2.h"
#include "ui_xpdfwidgetdemo2.h"

XpdfWidgetDemo2::XpdfWidgetDemo2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::XpdfWidgetDemo2)
{
    ui->setupUi(this);
}

XpdfWidgetDemo2::~XpdfWidgetDemo2()
{
    delete ui;
}

void XpdfWidgetDemo2::on_pushButton_clicked()
{
    //ui->pdf->loadFile("c:/derekn/src/xpdf/test/times.pdf");
    ui->pdf->loadFile("C:/Users/davisrs/Documents/FasTrack/nitrogen.pdf");
    ui->pdf->enableHyperlinks(true);

}
