#include "calibratetypepopup.h"
#include "ui_calibratetypepopup.h"

calibrateTypePopup::calibrateTypePopup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::calibrateTypePopup)
{
    ui->setupUi(this);
    //ui->btnKilograms->hide();
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    connect(ui->btnPounds, SIGNAL(clicked(bool)), SLOT(accept()));
    connect(ui->btnKilograms, SIGNAL(clicked(bool)), SLOT(reject()));
}

calibrateTypePopup::~calibrateTypePopup()
{
    delete ui;
}
