#ifndef SERVICELABELAPPLIERPPCAL_H
#define SERVICELABELAPPLIERPPCAL_H

#include <QLabel>

#include "screenwidget.h"

class DomCurrentPLU;
class MadpScaleConfig;
class DomLabelApplierConfig;
class QIntValidator;
class MadpInternalConfig;
class MessageBoxDialog;

namespace Ui {
    class serviceLabelApplierPPCal;
}

// service screen for printer plate calibration
class serviceLabelApplierPPCal  : public ScreenWidget
{
    Q_OBJECT

    typedef serviceLabelApplierPPCal _Myt;
    typedef ScreenWidget _Mybase;

public:
    explicit serviceLabelApplierPPCal(SessionMainWindow *mainWindow, ModeWidget *parent = 0);
    virtual ~serviceLabelApplierPPCal();


    //virtual void translate();

    enum calStatus
    {
        _READY,
        _SAMPLING,
        _VERIFY_HOME,
        _HOME_COMPLETE,
        _SAMPLING_SLOPE,
        _SLOPE_CALC,
        _SAVED_CONFIG,
        _COMPLETE,
        _WAIT_FOR_MODE_CHANGE,
        _PLATE0_NOT_HOME,
        _PLATE1_NOT_HOME,
        _PLATE0_TARGET_ALIGNMENT,
        _PLATE1_TARGET_ALIGNMENT,
        _ERROR_SENSOR0,
        _ERROR_SENSOR1,
        _ERROR_CONFIG

    };

private slots:

    void calibButClicked();
    void exitButClicked();
    void nextButClicked();

protected:
    /**
     * Handle screen changing.
     *
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private:
    Ui::serviceLabelApplierPPCal *ui;
    //MadpInternalConfig *internalConfigData;

    //void initialize();

    //QString isScaleMetric;

    unsigned long plate1DistFromHome;
    unsigned long plate2DistFromHome;
    unsigned long calDistance0;
    unsigned long calDistance1;

    bool receivedHomeComplete;
    bool receivedReady;
    bool calibrationStarted;

    MessageBoxDialog *startingCalibrationDialog;

    void initialize();
    void handleCalibStatus( int status, int primaryDist, int secondDist, float primaryInches, float secondaryInches );
    //void handleSecondCalibStatus( int status, int primaryDist, int secondDist, float primaryInches, float secondaryInches );

   ///////////////////

};

#endif // SERVICELABELAPPLIERPPCAL_H
