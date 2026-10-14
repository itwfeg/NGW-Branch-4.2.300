#ifndef RPI_ON_PAUSE_BUTTON_SIM_H
#define RPI_ON_PAUSE_BUTTON_SIM_H

#include <QObject>
#include <QTimer>
#include "raspberrypitypes.h"

#include "../onpausebuttonsim.h"

class RaspPiOnPauseButtonSim: public QOOnPauseButtonSimbject
{
    Q_OBJECT

public:
    explicit RaspPiOnPauseButtonSim(RaspberryPi::pin_t pin);

public slots:
    void slt_SimulatePress();
    void slt_SimulateUnpress();

private:
    RaspberryPi::pin_t m_pin;

};

#endif // RPI_ON_PAUSE_BUTTON_SIM_H
