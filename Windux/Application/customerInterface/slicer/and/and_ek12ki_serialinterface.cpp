#include "and_ek12ki_serialinterface.h"
#include <wiringSerial.h>
#include "hx711.h"
#define MAX_FILTER_READINGS 8

hx711::AtoDType FilterReadings[MAX_FILTER_READINGS];
hx711::AtoDType CurrentReading;
uint FilterAccumulatorSize;
uint FilterAccumulatorShiftFactor;
hx711::AtoDType WeigherReadingAccumulator;
hx711 instance;
hx711::CountsStruct *Counts;

// Where the calculating of the actual weight occurs
void AND_EK12Ki_SerialInterface_Parser::Weigh()
{
    Counts = (hx711::CountsStruct*)malloc(500);
    instance.FIRFilter(Counts);

    if(CurrentReading >= (FilterAccumulatorSize))
        CurrentReading = 0;

    WeigherReadingAccumulator -= FilterReadings[CurrentReading];
    FilterReadings[CurrentReading] = Counts->FIRCounts;
    WeigherReadingAccumulator += FilterReadings[CurrentReading];

    Counts->AvgFilterCounts = (WeigherReadingAccumulator / FilterAccumulatorSize);

    CurrentReading++;

    instance.update(Counts->AvgFilterCounts);
    Counts->AvgFilterCounts = instance.getValue();
    ScaleInterface::weightInPounds_t weight = (ScaleInterface::weightInPounds_t)(Counts->AvgFilterCounts);
    ScaleInterface::weightInPounds_t ounces;

    weight = weight/453.59237/291.1;

    float x = weight - (int)weight;
    ounces = x/.0625;

    //qDebug("%f",weight);

    //float error = 10;
    //if(average[0] >= average[1]-error && average[0] <= average[1]+error && average[0] >= average[2]-error && average[0] <= average[2]+error && average[0] >= average[3]-error && average[0] <= average[3]+error && average[0] >= average[4]-error && average[0] <= average[4]+error
    //       && average[1] >= average[2]-error && average[1] <= average[2]+error && average[1] >= average[3]-error && average[1] <= average[3]+error && average[1] >= average[4]-error && average[1] <= average[4]+error && average[2] >= average[3]-error && average[2] <= average[3]+error
    //       && average[2] >= average[4]-error && average[2] <= average[4]+error && average[3] >= average[4]-error && average[3] <= average[4]+error)
    //{
        emit sig_Weight(weight,ounces);
    //}
}
  
AND_EK12Ki_SerialInterface_WorkerThread::AND_EK12Ki_SerialInterface_WorkerThread() :
    m_shouldRun(true),
    m_fd(-1)
{
    (void)connect(
        &m_parser, SIGNAL(sig_Weight(ScaleInterface::weightInPounds_t,ScaleInterface::weightInPounds_t)),
        this, SIGNAL(sig_weightReceived(ScaleInterface::weightInPounds_t,ScaleInterface::weightInPounds_t)));
}

void AND_EK12Ki_SerialInterface_WorkerThread::run()
{
    instance.run();
    instance.tare();
    uint i = 0;
    CurrentReading = 0;
    WeigherReadingAccumulator = 0;
    FilterAccumulatorShiftFactor = 3;
    FilterAccumulatorSize = 2;

    for(i = 0; i < MAX_FILTER_READINGS;i++)
    {
        FilterReadings[i] = 0;
    }
    while (m_shouldRun)
    {
        m_parser.Weigh();
    }
}

AND_EK12Ki_SerialInterface::AND_EK12Ki_SerialInterface() :
    m_fd(-1)
{
    m_fd = serialOpen("/dev/ttyAMA0", 9600);
    serialFlush(m_fd);
    m_workerThread.SetFd(m_fd);
    (void)connect(
        &m_workerThread, SIGNAL(sig_weightReceived(ScaleInterface::weightInPounds_t,ScaleInterface::weightInPounds_t)),
        this, SIGNAL(sig_weightUpdated(ScaleInterface::weightInPounds_t,ScaleInterface::weightInPounds_t)));
    m_workerThread.start();
}

AND_EK12Ki_SerialInterface::~AND_EK12Ki_SerialInterface()
{
    m_workerThread.Terminate();
    (void)m_workerThread.wait();
    serialClose(m_fd);
}

void AND_EK12Ki_SerialInterface::Zero()
{
    instance.tare();
}

