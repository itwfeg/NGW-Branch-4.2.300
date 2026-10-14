#ifndef RPI_KNIFE_RUN_IN_H
#define RPI_KNIFE_RUN_IN_H

#include <QObject>
#include "inputgpio.h"

#include "../kniferunin.h"

class RaspPiKnifeRunIn : public KnifeRunIn
{
    Q_OBJECT

    friend void KnifeRunInIsr();

public:
    explicit RaspPiKnifeRunIn(RaspberryPi::pin_t pin);
    state_e State () const;

private:
    InputGpio m_inputGpio;

};

#endif // RPI_KNIFE_RUN_IN_H
