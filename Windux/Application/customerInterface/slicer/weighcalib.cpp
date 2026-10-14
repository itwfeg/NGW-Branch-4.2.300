#include "weighcalib.h"
#include "ui_weighcalib.h"
#include <QtDebug>
#include <QDebug>
#include <qtimer.h>
#include <iostream>
#include "standardpopup.h"
#include "calibratetypepopup.h"
#include "savecalibration.h"
#include <math.h>
#include <chrono>
#include <QDateTime>
#include <QMessageBox>
#include <QTimer>

#define NUMBEROFSTEPS 7 //Equal to the number of weight categories

weighcalib::weighcalib(QWidget *parent) :
    QDialog(parent),
    mode(displayPounds),
    ui(new Ui::weighcalib)
{
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    ui->setupUi(this);
    initStyle();

    configReady = false;
    weightSetup = false;
    readingCounts = false;

    currentStep = 0;
    QSizePolicy  sp = ui->btnStart->sizePolicy();
    sp.setRetainSizeWhenHidden(true);
    ui->btnStart->setSizePolicy(sp);
    sp = ui->btnStep->sizePolicy();
    sp.setRetainSizeWhenHidden(true);
    ui->btnStep->setSizePolicy(sp);

    ui->btnStep->setHidden(true);
    ui->btncompletecalib->setEnabled(false);

    //To Weigher
    connect(this, SIGNAL(sig_ReqConfig()), (servicemenu*) this->parent(), SLOT(slt_ReqConfig()));
    connect(this, SIGNAL(sig_WriteConfig(WeigherConfiguration&)), (servicemenu*) this->parent(), SLOT(slt_WriteConfig(WeigherConfiguration &)));
    connect(this, SIGNAL(sig_RezeroWeigher()), (servicemenu*) this->parent(), SLOT(slt_RezeroWeigher()));
    connect(this, SIGNAL(sig_SetWeigherMode(WeighingMode)), (servicemenu*) this->parent(), SLOT(slt_SetWeigherMode(WeighingMode)));
    connect(this, SIGNAL(sig_WeigherReset()), (servicemenu*) this->parent(), SLOT(slt_WeigherReset()));

    //From Weigher
    connect((servicemenu*) this->parent(), SIGNAL(sig_WeigherConfigChanged(WeigherConfiguration&)), this, SLOT(slt_WeigherConfigChanged(WeigherConfiguration&)));
    connect((servicemenu*) this->parent() , SIGNAL(sig_WeigherKeyPressed()), this, SLOT(slt_WeigherKeyPressed()));
    connect((servicemenu*) this->parent() , SIGNAL(sig_WeigherStatus(char*,int)), this, SLOT(slt_WeigherStatus(char*,int)));

    connect(ui->btnStart, SIGNAL(clicked(bool)), SLOT(slt_BeginCalib()));
    connect(ui->btnStep, SIGNAL(clicked(bool)), SLOT(slt_NextStep()));
    connect(ui->btncompletecalib, SIGNAL(clicked(bool)), SLOT(slt_CompleteCalibration()));

    QTimer::singleShot(0,this, SLOT(init()));

}

weighcalib::~weighcalib()
{
    delete ui;
}

void weighcalib::initStyle()
{
    ui->lblCount->setText("<font color='white'>Count</font>");
    ui->lblCount->setStyleSheet("QLabel { background-color : black }");
}

void weighcalib::createLbsModel()
{
    int weighValues[7] = {0,5,10,15,10,5,0};
    for(int row = 2; row < ui->mainGrid->rowCount(); row++){
        QLabel * lbl = dynamic_cast<QLabel*> ( ui->mainGrid->itemAtPosition(row,1)->widget());
        lbl->setText(QString::number(weighValues[row -2]));
    }
    weightSetup = true;
}

void weighcalib::createKilogramsModel()
{
    int weighValues[7] = {0,2,4,6,4,2,0};
    for(int row = 2; row < ui->mainGrid->rowCount(); row++){
        QLabel * lbl = dynamic_cast<QLabel*> ( ui->mainGrid->itemAtPosition(row,1)->widget());
        lbl->setText(QString::number(weighValues[row -2]));
    }
    weightSetup = true;
}

void weighcalib::slt_weight(bool check)
{
    //qDebug()<<"gd"<<check;
    if(check == true)
    {
        ui->lblType->setText(tr("kg"));
        createKilogramsModel();
    }
    else
    {
        ui->lblType->setText(tr("lbs"));
        createLbsModel();
    }
}

void weighcalib::slt_WeigherConfigChanged(WeigherConfiguration & config)
{
    currentConfig = WeigherConfiguration(config);
    configReady = true;
}

void weighcalib::slt_WeigherKeyPressed()
{
    qDebug() << "Weigher Key Pressed ";
    Q_EMIT sig_WeigherKeyPressed();
}

void weighcalib::slt_WeigherStatus(char *buffer, int buffLen)
{
    Q_EMIT sig_WeigherStatus(buffer,buffLen);

    const uint16_t flags( be16toh( *reinterpret_cast<const uint16_t*>( &buffer[2] ) ) );
    const uint32_t counts( be32toh( *reinterpret_cast<const uint32_t*>( &buffer[4] ) ) );
    //const uint32_t rawCounts( be32toh( *reinterpret_cast<const uint32_t*>( &buffer[8] ) ) );
    const uint32_t filteredUncalibratedCounts( be32toh( *reinterpret_cast<const uint32_t*>( &buffer[12] ) ) );
    //const uint32_t ADCounts( be32toh( *reinterpret_cast<const uint32_t*>( &buffer[16] ) ) );

    if(flags & SCALE_WARMUP && !weigherInWarmup){
        weigherInWarmup = true;
        QMessageBox *mbox = new QMessageBox(this);
        mbox->setWindowTitle(tr("SYSTEM MESSAGE"));
        mbox->setText( tr("Weigher detected and in warmup. Please wait..."));
        mbox->setModal(true);
        mbox->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);;
        mbox->setStandardButtons(0);
        connect(this, SIGNAL(sig_WeigherWarmupFinished()), mbox, SLOT(accept()));
        //use single shot so that the program is not blocked.
        QTimer::singleShot(0, mbox, SIGNAL(exec()));
    }else if(!(flags & SCALE_WARMUP) && weigherInWarmup){
        Q_EMIT sig_WeigherWarmupFinished();
        weigherInWarmup = false;
    }
    // prevent changing of current counts and uncalibrated counts while capturing values
    if(weightSetup && !readingCounts){
        currentCounts = counts;
        currentFilteredUncalibratedCounts = filteredUncalibratedCounts;

        QLabel * lblCounts = dynamic_cast<QLabel*> ( ui->mainGrid->itemAtPosition(currentStep + 2,2)->widget());
        lblCounts->setText(QString::number(counts));

        QLabel * lbl = dynamic_cast<QLabel*> ( ui->mainGrid->itemAtPosition(currentStep + 2,4)->widget());
        lbl->setText(QString::number(filteredUncalibratedCounts));
    }



}

void weighcalib::slt_GainChanged(long gain)
{
    gainFactor = gain;
}

void weighcalib::slt_NextStep()
{
    bool readingCounts = true;
    //QLabel * delta = dynamic_cast<QLabel*> ( ui->mainGrid->itemAtPosition(currentStep + 2,3)->widget());
    QLabel * zeroed = dynamic_cast<QLabel*> ( ui->mainGrid->itemAtPosition(currentStep + 2,2)->widget());
    QLabel * nonZeroed = dynamic_cast<QLabel*> ( ui->mainGrid->itemAtPosition(currentStep + 2,4)->widget());
    int tempDelta = abs(currentFilteredUncalibratedCounts - currentNonZeroed);
    bool deltaChangeTooBig = false;
    if (currentStep < NUMBEROFSTEPS - 1){

        if(currentStep == 0){
            stepNonZeroedCounts[currentStep] = currentFilteredUncalibratedCounts;
            lastStep = currentStep;
            currentStep += 1;

            lastZeroed = currentZeroed;
            currentZeroed= currentCounts;
            lastNonZeroed = currentNonZeroed;
            currentNonZeroed= currentFilteredUncalibratedCounts;
            lastDelta = currentDelta;
            currentDelta = 0;
            setDelta();
        } else if(currentStep == 1){
            // For the first step, compare the delta with the scale's tolerance.
            // Bad if the delta is below the tolerance.
            if(tempDelta > 51067){ //51067 tolerance for AWS SAMBA weigher
                stepNonZeroedCounts[currentStep] = currentFilteredUncalibratedCounts;
                lastStep = currentStep;
                currentStep += 1;

                lastZeroed = currentZeroed;
                currentZeroed= currentCounts;
                lastNonZeroed = currentNonZeroed;
                currentNonZeroed= currentFilteredUncalibratedCounts;
                lastDelta = currentDelta;
                currentDelta = abs(currentNonZeroed - lastNonZeroed);
                setDelta();

            }else{
                deltaChangeTooBig = true;
            }
        } else if (abs(tempDelta - currentDelta) < stepDeltaChange[currentStep] ){
            stepNonZeroedCounts[currentStep] = currentFilteredUncalibratedCounts;
            lastStep = currentStep;
            currentStep += 1;

            lastZeroed = currentZeroed;
            currentZeroed= currentCounts;
            lastNonZeroed = currentNonZeroed;
            currentNonZeroed= currentFilteredUncalibratedCounts;
            lastDelta = currentDelta;
            currentDelta = abs(currentNonZeroed - lastNonZeroed);
            setDelta();
        }else{
            deltaChangeTooBig = true;
        }
    }else if (abs(tempDelta - currentDelta) < stepDeltaChange[currentStep] ){
        stepNonZeroedCounts[currentStep] = currentFilteredUncalibratedCounts;
        currentStep += 1;
        weightSetup = false;
        setDelta();
        ui->btnStep->setHidden(true);
        ui->btncompletecalib->setEnabled(true);

    }else{
        deltaChangeTooBig = true;
    }
    if(deltaChangeTooBig){
        standardPopup d(this);
        d.setMessage(tr("Change in delta is too large"));
        d.setStandardButtons(QDialogButtonBox::Ok);
        d.exec();
    }
    readingCounts = false;
}

void weighcalib::slt_BeginCalib()
{
    this->blockSignals(false);
    if(!configReady){
        Q_EMIT sig_ReqConfig();
        QEventLoop loop(this);
        connect((servicemenu* )this->parent(), SIGNAL(sig_WeigherConfigChanged(WeigherConfiguration&)), &loop, SLOT(quit()));
        QTimer::singleShot(2000, &loop, SLOT(quit()));
        loop.exec();
    }
    if(configReady){
        originConfig = WeigherConfiguration(currentConfig);
        lastDelta = 0;
        lastStep = 0;
        lastZeroed = 0;
        lastNonZeroed = 0;
        currentDelta = 0;
        currentStep = 0;
        currentZeroed = 0;
        currentNonZeroed = 0;

        calibrateTypePopup d(this);
        d.setModal(true);
        bool weight;
        if (QDialog::Accepted==d.exec()){
            weight = false;
            mode = displayPounds;
            stepDesiredCounts = &stepDesiredCountsAvoir[0];
            currentConfig.setFlags(currentConfig.getFlags() & 0xFE);
        }else{
            weight = true;
            mode = displayKilograms;
            stepDesiredCounts = &stepDesiredCountsMetric[0];
            currentConfig.setFlags(currentConfig.getFlags() | 0x01);
        }

        currentConfig.setConfigType(WgConfigType::WgCalibInProgress);
        Q_EMIT sig_WriteConfig(currentConfig);


        standardPopup pop(this);
        pop.setMessage(tr("Press the weigher calibration switch to continue."));
        pop.setModal(true);
        connect(this, SIGNAL(sig_WeigherKeyPressed()), &pop, SLOT(slt_WeigherKeyPressed()));
        if(QDialog::Accepted==pop.exec()){
            slt_weight(weight);
            Q_EMIT sig_SetWeigherMode(WeighingMode::WeightTracking);
            ui->btnStep->setHidden(false);
            ui->btnStart->setHidden(true);
        }
    }else{
        std::cout << "weigher not connected" << std::endl;
        standardPopup d(this);
        d.setMessage(tr("Timeout: Weigher Not Connected"));
        d.exec();
    }


}

void weighcalib::setDelta()
{
    QLabel * delta = dynamic_cast<QLabel*> ( ui->mainGrid->itemAtPosition(currentStep + 1,3)->widget());
    if( currentStep > 1){
        delta->setText(QString::number(currentDelta));
    }else{
        delta->setText("0");
    }
}

long weighcalib::calculateOptimalGainFactor()
{
    long gf = 0;
    long optGF = 0;
    unsigned long optMSE;

    optMSE = 0X0FFFFFFFF;

    unsigned long long int temp;

    /* Changed range that gain can be optimized from +/- 5 to +/- 500 of orig.
                   calculated gain. Both are 0.0005% of default gain. These comments are
                   per F. Albers who implemented new calibration schema during QX2. (haberrc) */

    temp = (unsigned long long int)((unsigned long long int) stepDesiredCounts[3] * ScalingConstant);
    gf = (long)(temp / (stepNonZeroedCounts[3] - stepNonZeroedCounts[0]));
    gf += 500; // to adjust gain by +/- 500 (0.0005%) to match old way


    for (short i = 0; i < 10; ++i)
    {
        gainFactor = gf;
        calculateMeanSquareError();

        if ((unsigned long)meanSquareError < optMSE)
        {
            optMSE = (unsigned long)meanSquareError;
            optGF = gf;
        }
        gf -= 100;

    }
    gainFactor = optGF;
    meanSquareError = optMSE;

    //qDebug() << "Gain Factor; " << gainFactor << "Mean Square Error: " << meanSquareError;
    return gainFactor;
}

void weighcalib::on_btnCancel_clicked()
{
    Q_EMIT sig_SetWeigherMode(WeighingMode::Manual);
    this->close();
}

void weighcalib::slt_CompleteCalibration()
{
    calculateOptimalGainFactor();

    SaveCalibration d(this);
    d.setGain(gainFactor);
    connect(this, SIGNAL(sig_WeigherStatus(char*,int)), &d, SLOT(slt_WeigherStatus(char*,int)));
    if(d.exec() == QDialog::Accepted){
        //if we are in metric mode but calibrated in lbs
        if((originConfig.getFlags() & 0x01) && mode == displayPounds){
            //qDebug() << "Converting Gain Factor to Metric.\n Before: " << gainFactor;
            gainFactor = convertGainFactorToMetric(gainFactor);
            //qDebug() << "After: " << gainFactor;
            //qDebug() << "Flags: " << currentConfig.getFlags();
            //set metric flag
            currentConfig.setFlags(currentConfig.getFlags() | 0x01);
            //qDebug() << "Flags after: " << currentConfig.getFlags();
        //if we are in avoir mode but calibrated in kilograms
        }else if (!(originConfig.getFlags() & 0x01) && mode == displayKilograms){
            //qDebug() << "Converting Gain Factor to Avoir.\n Before: " << gainFactor;
            gainFactor = convertGainFactorToAvoir(gainFactor);
            //qDebug() << "After: " << gainFactor;
            //qDebug() << "Flags: " << currentConfig.getFlags();
            //clear metric flag
            currentConfig.setFlags(currentConfig.getFlags() & 0xFE);
            //qDebug() << "Flags after: " << currentConfig.getFlags();
        }
        long date = QDateTime::currentMSecsSinceEpoch() / 1000;
        currentConfig.setLastCalibrationDate(date);
        currentConfig.setNumberOfCalibrations(currentConfig.getNumberOfCalibrations() + 1);
        calculateCenterOfMaintZone();
        currentConfig.setCenterOfMaintenanceZone(centerOfMaintenanceZone);
        currentConfig.setGainFactor(gainFactor);
        currentConfig.setConfigType(WgConfigType::WgPermanent);
        Q_EMIT sig_WriteConfig(currentConfig);
    }else{
        Q_EMIT sig_WriteConfig(originConfig);
    }
    Q_EMIT sig_SetWeigherMode(WeighingMode::Manual);
    Q_EMIT sig_WeigherReset();
    standardPopup m(this);
    m.setMessage(tr("Calibration Complete"));
    m.setStandardButtons(QDialogButtonBox::Ok);
    m.exec();
    this->accept();

}

void weighcalib::calculateMeanSquareError()
{
    meanSquareError = 0L;
    long baseCounts = stepNonZeroedCounts[0];

    for (short i = 1; i < (short)NUMBEROFSTEPS - 1; i++)
    {
        long stepError = calculateError(gainFactor, baseCounts, i);
        meanSquareError += (stepError * stepError);
    }

}

long weighcalib::calculateError(long gainFactor, long baseCounts, int step)
{

    unsigned long long int temp;

    temp = (unsigned long long int)((unsigned long long int)gainFactor * ((unsigned long long int)stepNonZeroedCounts[step] - (unsigned long long int)baseCounts));
    temp /= ScalingConstant;
    long error = (long)((unsigned long long int)stepDesiredCounts[step] - temp);


    return error;
}

long weighcalib::calculateCenterOfMaintZone()
{
    /* In this case, we need to write the new configuration parameters back to the weigher's eep.
     * The parameters are:  gain factor, center of maintenance zone, max weight (based on metric/avoir).
     * The following parameters are set based on the weigher type (metric/avoir, scale factor):
     *    auto zero maintenance limit, powerup maintenance zone limit, and operate maintenance zone limit.
     *
     * This was determined via the calibrate_hc11() function in cal_weigh.c (Ultima).
     */


    // AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)


    unsigned long long int temp;
    temp = (unsigned long long int)((unsigned long long int)stepNonZeroedCounts[0]* gainFactor);
    centerOfMaintenanceZone = (long)(temp / ScalingConstant);
    return centerOfMaintenanceZone;
}

uint32_t weighcalib::convertGainFactorToMetric(const uint32_t gainFactor)
{

    long gainL = (long)gainFactor;
    double gainD = (double)gainFactor;

    gainD = gainD * 2.0 * 0.45359237;
    gainD = round(gainD);

    return (uint32_t)gainD;
}

// static
uint32_t weighcalib::convertGainFactorToAvoir(const uint32_t gainFactor)
{
    long gainL = (long)gainFactor;
    double gainD = (double)gainFactor;

    gainD = gainD / (2.0 * 0.45359237);
    gainD = round(gainD);

    return (uint32_t)gainD;
}

void weighcalib::init(){
    Q_EMIT sig_ReqConfig();
    resetAndWaitForWeigher();

}

void weighcalib::resetAndWaitForWeigher(){
    Q_EMIT sig_WeigherReset();

    QEventLoop loop(this);
    connect(this, SIGNAL(sig_WeigherWarmupFinished()), &loop, SLOT(quit()));
    QTimer::singleShot(2000, &loop, SLOT(quit()));
    loop.exec();
    if(!weigherInWarmup){
        //close dialog
    }else{
        standardPopup d(this);
        d.setMessage(tr("Timeout: Weigher still in warmup"));
    }
}

void weighcalib::showEvent(QShowEvent *e)
{
    const QRect p( parentWidget()->geometry() );
    const QRect c( rect() );

    move( p.center() - c.center() );

    QDialog::showEvent( e );
}


