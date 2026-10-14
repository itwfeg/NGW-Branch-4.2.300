/**
 * @file serviceNewWrapperTestAirCompressor.h
 * @brief Widget to test the wrapper's air compressor.
  *
  * @copyright Copyright (c) 2014 Hobart Corporation
  * @author Alex Zavala
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/
#ifndef SERVICENEWWRAPPERTESTAIRCOMPRESSOR_H
#define SERVICENEWWRAPPERTESTAIRCOMPRESSOR_H

#include "screenwidget.h"
#include <Messages.h>
#include <QPushButton>
#include <QtGui>

class DomCurrentPLU;
class DomEvolWrapperStatus;

namespace Ui {
class serviceNewWrapperTestAirCompressor;
}

/**
* @class serviceNewWrapperTestAirCompressor
* @brief  Class to test the air compressor on the wrapper
*
*/

class serviceNewWrapperTestAirCompressor  : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperTestAirCompressor _Myt;
    typedef ScreenWidget _Mybase;

public:

    /**
     * Valve Open and Close Actions.
     */
    enum ValveAction
    {
        COMPRESSOR_ON,                  // For some reason, when I turn on this compressor, even though the
        COMPRESSOR_OFF,                 // enum ON=0, I have to send a 1.  And then a 0 for OFF. Something is
        COMPRESSOR_TANK_DRAIN_PULSE,    // is wrong somewhere but not in the UI.  A 0 should turn it on,
        REGULATED_VALVE_OPEN,           // but its a 1 that turns it on, and then a 0 is off. LOOK INTO THIS
        REGULATED_VALVE_CLOSE,
        DUMP_VALVE_OPEN,
        DUMP_VALVE_CLOSE,
        TANK_DRAIN_VALVE_OPEN,
        TANK_DRAIN_VALVE_CLOSE,

        MAX_VALVE_ACTIONS
    };

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit serviceNewWrapperTestAirCompressor(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceNewWrapperTestAirCompressor();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:

    /**
     * SLOT method called when btnAirCompressorOn is pressed
     *
     * @param[in] none
     * @returns  none
     * @brief airCompressorOn
     */
    void airCompressorOn();

    /**
     * SLOT method called when btnAirCompressorOff is pressed
     *
     * @param[in] none
     * @returns  none
     * @brief airCompressorOff
     */
    void airCompressorOff();

    /**
     * SLOT method called when butOpenRegulatedValve is pressed
     *
     * @param[in] none
     * @returns  none
     * @brief regulatedValveOpen
     */
    void regulatedValveOpen();

    /**
     * SLOT method called when butCloseRegulatedValve is pressed
     *
     * @param[in] none
     * @returns  none
     * @brief regulatedValveClose
     */
    void regulatedValveClose();

    /**
     * SLOT method called when butOpenDumpValve is pressed
     *
     * @param[in] none
     * @returns  none
     * @brief dumpValveOpen
     */
    void dumpValveOpen();

    /**
     * SLOT method called when butCloseDumpValve is pressed
     *
     * @param[in] none
     * @returns  none
     * @brief dumpValveClose
     */
    void dumpValveClose();

    /**
     * SLOT method called when butOpenTankDrainValve is pressed
     *
     * @param[in] none
     * @returns  none
     * @brief tankDrainValveOpen
     */
    void tankDrainValveOpen();

    /**
     * SLOT method called when butCloseTankDrainValve is pressed
     *
     * @param[in] none
     * @returns  none
     * @brief dumpValveClose
     */
    void tankDrainValveClose();

protected:

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

    /**
     * Check for specific error message is handled by screen.
     *
     * Session will call this method upon receiving an error message. The default error handling
     * (i.e. popup an error dialog) shall be invoked unless @c true is returned from
     * this property.
     *
     * @returns  @c true to use custom error handler, @c false otherwise
     */
    virtual bool isErrorMessageHandler( const SessionMessage& errorMsg ) const;

private:
    Ui::serviceNewWrapperTestAirCompressor *ui;

    int prevPsi_;
    bool autoAirTestOn_;
    bool regulatedValveOpen_;
    bool dumpValveOpen_;
    bool tankDrainValveOpen_;
    bool beenThereDoneThat_;

    DomEvolWrapperStatus evolWrapperStatus_;

    /**
     * @brief highlightButtonRed
     */
    void highlightButtonRed( QPushButton* actionBtn );

    /**
     * @brief highlightButtonGreen
     */
    void highlightButtonGreen(QPushButton* actionBtn);

    /**
     * @brief unhighlightButton
     */
    void unhighlightButton(QPushButton* actionBtn);

};
#endif // SERVICENEWWRAPPERTESTAIRCOMPRESSOR_H
