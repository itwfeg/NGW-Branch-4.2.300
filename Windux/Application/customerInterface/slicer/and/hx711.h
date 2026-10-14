#ifndef HX711_H
#define HX711_H
#include <QObject>
#include <stdlib.h>

class hx711 : public QObject
{

    Q_OBJECT
public:
    typedef ulong AtoDType;

    struct counts_struct
    {
        AtoDType AvgFilterCounts;
        AtoDType FIRCounts;
        AtoDType AtoDCounts;
    };
    typedef struct counts_struct CountsStruct;

public:
    hx711();
    float run();
    void setHighPri();
    void power_down_hx711();
    void setup_gpio();
    void reset_converter();
    void set_gain(int);
    long unsigned int read_cnt(long int);
    u_int16_t sizecvt(int);
    long get_offset();
    void set_offset(long int);
    float get_weight();
    void tare();
    void runCalibration(int);
    void FIRFilter(CountsStruct *pCounts);

signals:
    void sig_runCal();


public:
    inline int getValue() {return responsiveValue; }
    inline int getRawValue() {return rawValue; }
    inline bool hasChanged() {return responsiveValueHasChanged; }
    inline bool isSleeping() {return sleeping; }
    void update();
    void update(int rawValue);

    void setSnapMultiplier(float newMultiplier);
    inline void enableSleep() {sleepEnable = true; }
    inline void disableSleep() {sleepEnable = false; }
    inline void enableEdgeSnap() {edgeSnapEnable = true; }
    inline void disableEdgeSnap() { edgeSnapEnable = false; }
    inline void setActivityThreshold(float newThreshold) {activityThreshold = newThreshold; }
    inline void setAnalogResolution(int resolution) {analogResolution = resolution; }

private:
    int pin;
    int analogResolution;
    float snapMultiplier;
    bool sleepEnable;
    float activityThreshold;
    bool edgeSnapEnable;

    float smoothValue;
    unsigned long lastActivityMS;
    float errorEMA;
    bool sleeping;

    int rawValue;
    int responsiveValue;
    int prevResponsiveValue;
    bool responsiveValueHasChanged;

    int getResponsiveValue(int newValue);
    float snapCurve(float x);
};

#endif // HX711_H
