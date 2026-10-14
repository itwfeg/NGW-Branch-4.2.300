#ifndef SCALEINTERFACE_H
#define SCALEINTERFACE_H


#include <QMetaType>

class ScaleInterface
{
public:
    enum State
    {
        zero,                                       ///< Weigher at zero.
        unstable,                                   ///< Weigher unstable.
        stable,                                     ///< Weigher at stable weight.
        underWeight,                                ///< Weigher under zero.
        overWeight,                                 ///< Weigher over weight limit.
        outOfMaint,                                 ///< Weigher out of maintenance zone.
        error
    };
    //this says weight in  pounds but the weight is not always in pounds. In metric mode the counts come in as KG and therefore the weight is in KG
	typedef float weightInPounds_t;
    ScaleInterface() {
        qRegisterMetaType<ScaleInterface::State>("ScaleInterface::State");
		qRegisterMetaType<ScaleInterface::weightInPounds_t>("ScaleInterface::weightInPounds_t");
	}

    virtual void ZeroScale() = 0;

signals:
    virtual void sig_weightUpdated(ScaleInterface::State state, ScaleInterface::weightInPounds_t weight) = 0;
    virtual void sig_OnWeigherKeyPressed() = 0;



public slots:

};

#endif // SCALEINTERFACE_H
