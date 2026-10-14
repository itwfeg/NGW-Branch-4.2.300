#ifndef TYPES_H
#define TYPES_H

#include <wiringPi.h>

namespace RaspberryPi
{
    typedef int pin_t;

    enum gpioValue_e
    {
        GpioLow = LOW,
        GpioHigh = HIGH
    };

    enum gpioPullUpType_e
    {
        GpioPullNone = PUD_OFF,
        GpioPullUp = PUD_UP,
        GpioPullDown = PUD_DOWN
    };

    enum gpioEdgeType_e
    {
        GpioEdgeFalling = INT_EDGE_FALLING,
        GpioEdgeRising = INT_EDGE_RISING,
        GpioEdgeBoth = INT_EDGE_BOTH,
        GpioEdgeSetup = INT_EDGE_SETUP
    };
}

#endif // TYPES_H
