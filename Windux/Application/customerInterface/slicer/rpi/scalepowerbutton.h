#ifndef SCALE_POWER_BUTTON_H
#define SCALE_POWER_BUTTON_H

#include "raspberrypitypes.h"

class ScalePowerButton
{
public:

    explicit ScalePowerButton(RaspberryPi::pin_t pin);
    void On();

private:
    RaspberryPi::pin_t m_pin;
};

#endif // SCALE_POWER_H
