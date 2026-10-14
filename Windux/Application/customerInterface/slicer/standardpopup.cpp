#include "standardpopup.h"
#include "ui_standardpopup.h"

void standardPopup::slt_WeigherKeyPressed()
{
    this->accept();
}

standardPopup::standardPopup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::standardPopup)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);;
}

void standardPopup::setMessage(QString message)
{
    ui->message->setText(message);
}

void standardPopup::setHeader(QString header){
    ui->Title->setText(header);
}

void standardPopup::setStandardButtons(QDialogButtonBox::StandardButtons buttons){
    this->ui->buttonBox->setStandardButtons(buttons);
}
standardPopup::~standardPopup()
{
    delete ui;
}
