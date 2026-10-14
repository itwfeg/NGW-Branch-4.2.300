#include "alertmessagepopup.h"
#include "ui_alertmessagepopup.h"

#include <QDebug>

AlertMessagePopup::AlertMessagePopup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AlertMessagePopup),
    uniqueId(-1)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    connect(this->ui->m_pCompletedButton, SIGNAL(clicked(bool)), this, SLOT(slt_messageAccepted()));
    connect(this->ui->m_pIgnoreButton, SIGNAL(clicked(bool)), this, SLOT(slt_messageRejected()));
}

void AlertMessagePopup::setMessage(QString message)
{
    ui->m_pBody->setText(message);
}

void AlertMessagePopup::setHeader(QString header){
    ui->m_pHeader->setText(header);
}

void AlertMessagePopup::setLastCompleted(QString lastCompleted){
    ui->m_pLastCompleted->setText(lastCompleted);
}

void AlertMessagePopup::setUniqueId(const int id){
    this->uniqueId = id;
}

void AlertMessagePopup::slt_messageAccepted(){
    qDebug() << "Accepted message: " << this->uniqueId;
    emit sig_messageAccepted(this->uniqueId); //Emit signal to msgbox
    this->accept();
}

void AlertMessagePopup::slt_messageRejected(){
    qDebug() << "Rejected message: " << this->uniqueId;
    emit sig_messageRejected(this->uniqueId); //Emit signal to ...nowhere?
    this->reject();
}

AlertMessagePopup::~AlertMessagePopup()
{
    delete ui;
}
