#ifndef SERVICENEWWRAPPERLACONFIG_H
#define SERVICENEWWRAPPERLACONFIG_H
#include "screenwidget.h"

class DomCurrentPLU;
class MadpScaleConfig;
class DomLabelApplierConfig;
class QIntValidator;
class MadpInternalConfig;


namespace Ui {
class serviceNewWrapperLAConfig;
}

// Label applier config on two printer systems

class serviceNewWrapperLAConfig  : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperLAConfig _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit serviceNewWrapperLAConfig(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceNewWrapperLAConfig();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:
// Sample Slot Methods for various widgets


    /**
    * SLOT method called when product or class change to signal a save on close of form
    *
    * @param[in] none
    * @returns  none
    */
    void doSave();
    void prePrintStateChange();

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
    Ui::serviceNewWrapperLAConfig *ui;
    MadpInternalConfig *internalConfigData;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method to save widget's fields with new values.
     *
     * @param[in] none
     * @returns  none
     */
    void saveConfig();

    int makeValidatorTop(int digits);

    DomCurrentPLU *currentPLUData;
    MadpScaleConfig *scaleConfigData;

    DomLabelApplierConfig *labelApplierConfigData;
    QIntValidator *overdriveValidator_;
    QIntValidator *primaryOffsetValidator_;
    QIntValidator *secondaryOffsetValidator_;

    bool init;
    bool hasChanged;

    QString scaleModel;
    QString isScaleMetric;

    unsigned long plate1DistFromHome;
    unsigned long plate2DistFromHome;

    bool prePrintStateChanged;

   ///////////////////

};

#endif // SERVICENEWWRAPPERLACONFIG_H
