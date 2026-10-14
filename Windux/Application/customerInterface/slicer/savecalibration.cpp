#include "savecalibration.h"
#include "ui_savecalibration.h"
#include "weighcalib.h"

SaveCalibration::SaveCalibration(QWidget *parent):
    QDialog(parent),
    ui(new Ui::SaveCalibration)
{
    ui->setupUi(this);
    connect(ui->btnSave, SIGNAL(clicked(bool)), this, SLOT(slt_saveButtonClicked()));
    connect(ui->btnCancel, SIGNAL(clicked(bool)), this, SLOT(slt_cancelButtonClicked()));
    connect(this, SIGNAL(sig_GainChanged(long)),(weighcalib*) this->parent(), SLOT(slt_GainChanged(long)));
}

SaveCalibration::~SaveCalibration()
{
    delete ui;
}

long SaveCalibration::getGain() const
{
    return gain;
}

void SaveCalibration::setGain(long value)
{
    gain = value;
    this->ui->gainFactor->setText(QString::number(gain));
}

void SaveCalibration::slt_WeigherStatus(char *buffer, int buffLen)
{

    const uint32_t counts( be32toh( *reinterpret_cast<const uint32_t*>( &buffer[4] ) ) );
    const uint32_t filteredUncalibratedCounts( be32toh( *reinterpret_cast<const uint32_t*>( &buffer[12] ) ) );


    this->ui->zeroedCounts->setText(QString::number(counts));
    this->ui->nonZeroedCounts->setText(QString::number(filteredUncalibratedCounts));
}

void SaveCalibration::slt_saveButtonClicked()
{
    //QEventLoop loop(this);
    //connect(this, SIGNAL(sig_GainChanged(long)),&loop,SLOT(quit()));
    Q_EMIT sig_GainChanged(ui->gainFactor->text().toLong()); //Emit signal to weighcalib

    this->accept();
}

void SaveCalibration::slt_cancelButtonClicked()
{
    this->reject();
}
