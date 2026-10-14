#include "slicerstatisticsviewer.h"
#include "ui_slicerstatisticsviewer.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QHostAddress>
#include <QNetworkInterface>
#include "slicerversion.h"
#include <string>
#include <unistd.h>
#include <limits.h>
#include "speedkey.h"
#include "slicerstatistics.h"
#include "mainwindow.h"
#include "scripter.h"



SlicerStatisticsViewer::SlicerStatisticsViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SlicerStatisticsViewer)
{
    ui->setupUi(this);

    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    ui->Name->setText(hostname);

    QString adr = "";
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
             adr = address.toString();
    }
    if (adr != ""){
        ui->IP->setText(adr);
    }else{
        ui->IP->hide();
        ui->IPLabel->hide();
    }

    QString str= QString::fromStdString(runScript(" fw_printenv version"));
    str.remove(QRegExp("[\\n\\t\\r]"));
    QStringList ver = str.split("=");
    if(ver.length() == 2){
        ui->UBootVersion->setText(ver.at(1));
    }else{
        ui->UBootVersion->setText(str);
    }
    //qDebug() << str;
    ui->UIVersion->setText( QString::fromStdString( SlicerVersion::buildNumber.substr(4,7)));

    setModal(true);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    connect(ui->okButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    currentStatistics = new SlicerStatistics(this);
    initStats();
}

SlicerStatisticsViewer::~SlicerStatisticsViewer()
{
    delete ui;
    delete currentStatistics;
}

void SlicerStatisticsViewer::addGeneralItem(QString name, QString value){
    QHBoxLayout * layout = new QHBoxLayout(this);
    QLabel * title = new QLabel(this);
    QLabel * text = new QLabel(this);
    title->setText(name);
    text->setText(value);
    QFont font(title->font());
    font.setPointSize(7);
    text->setFont(font);
    title->setFont(font);
    layout->addWidget(title);
    layout->addWidget(text);
    this->ui->generalStatsLayout->insertLayout(ui->generalStatsLayout->count() - 1, layout); // add above the spacer
    repaint();
}

void SlicerStatisticsViewer::addSpeedKeyItem(QString name, QString value){
    QHBoxLayout * layout = new QHBoxLayout(this);
    QLabel * title = new QLabel(this);
    title->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    title->setMaximumWidth((this->size().width() / 2) - 10);
    QLabel * text = new QLabel(this);
    text->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    title->setText(name);
    text->setText(value);
    QFont font(title->font());
    font.setPointSize(7);
    text->setFont(font);
    title->setFont(font);
    layout->addWidget(title);
    layout->addWidget(text);
    this->ui->speedkeyStatsLayout->insertLayout(ui->speedkeyStatsLayout->count() - 1, layout); // add above the spacer
    repaint();
}

void SlicerStatisticsViewer::addMaintenanceItem(QString name, QString value){
    QHBoxLayout * layout = new QHBoxLayout(this);
    QLabel * title = new QLabel(this);
    title->setMaximumWidth((this->size().width() / 2) - 10);
    title->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    QLabel * text = new QLabel(this);
    text->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    title->setText(name);
    text->setText(value);
    QFont font(title->font());
    font.setPointSize(7);
    text->setFont(font);
    title->setFont(font);
    layout->addWidget(title);
    layout->addWidget(text);
    this->ui->maintenanceLayout->insertLayout(ui->maintenanceLayout->count() - 1, layout); // add above the spacer
    repaint();
}

void SlicerStatisticsViewer::initStats(){
    for(int i = 0; i < SlicerStatistics::LastStat; i++){
        QString text;
        QString value;
        switch (i) {
        case SlicerStatistics::SlicerRunningTime:{
            text = tr("Total Run Time (H):");
            float temp = currentStatistics->readStatistic(i).toFloat();
            temp /= 3600000; //msec to hours
            value = QString::number(temp);
            break;
        }
        case SlicerStatistics::LbsWeighed:
            text = tr("Lbs. Weighed:");
            value = currentStatistics->readStatistic(i);
            break;
        default:
            text = "";
            value = "";
            break;
        }
        addGeneralItem(text,value);
        //qDebug() << "Add Item" << i;
    }
    const MainWindow * w = qobject_cast<MainWindow*>(this->parentWidget());
    if (w){
        const QList<speedKey*> keys( w->getSpeedKeys() );
        if (!keys.empty()){
            ui->speedkeyLabel->show();
            foreach ( speedKey *k, keys ){
                //qDebug() << "Add SpeedKey: " << k;
                if(k->val1().toInt() < 2){
                    addSpeedKeyItem(k->text() + " (Lbs.):", QString::number(k->totalWeight()));
                }else{
                    addSpeedKeyItem(k->text() + " (Kg.):", QString::number(k->totalWeight()));
                }
            }
        }else{
           ui->speedkeyLabel->hide();
        }
        const QList<message*> maint = w->getMessages();
        //qDebug() << "maint: " << maint;
        if(!maint.empty()){
            ui->maintenanceLabel->show();
            foreach (message * m, maint) {
                //qDebug() << "Add message: " << m;
                //qDebug() << "last time" << (m->getLastConfirmCompleted()* 1000);
                if (m->getLastConfirmCompleted()){
                    QDateTime time = QDateTime::fromMSecsSinceEpoch(m->getLastConfirmCompleted()* 1000);
                    //qDebug() << "Time:" << time.toString("MM.dd.yy HH:mm:ss");
                    addMaintenanceItem(m->getHeader(), time.toString(tr("MM/dd/yy HH:mm:ss")) );
                }else{
                    addMaintenanceItem(m->getHeader(), tr("Never"));
                }
            }
        } else{
            ui->maintenanceLabel->hide();
        }
    }else{
       ui->speedkeyLabel->hide();
    }
}
