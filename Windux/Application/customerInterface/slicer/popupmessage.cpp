#include "popupmessage.h"
#include "ui_popupmessage.h"

PopupMessage::PopupMessage(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint),
    ui(new Ui::PopupMessage)
{
    ui->setupUi(this);
}

PopupMessage::~PopupMessage()
{
    delete ui;
}

void PopupMessage::setHeader(QString str){
    this->ui->Title->setText(str);
}

void PopupMessage::setMessage(QString str){
    this->ui->Message->setText(str);
}

void PopupMessage::setHeaderFont(QFont font){
    this->ui->Title->setFont(font);
}

void PopupMessage::setMessageFont(QFont font){
    this->ui->Message->setFont(font);
}

