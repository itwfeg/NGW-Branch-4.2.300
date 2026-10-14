#ifndef SERVICEPPPDISTANCE_H
#define SERVICEPPPDISTANCE_H
#include "screenwidget.h"

class DomCurrentPLU;
class MadpScaleConfig;
class DomLabelApplierConfig;
class QIntValidator;
class MadpInternalConfig;

namespace Ui {
class servicePPPDistance;
}
class servicePPPDistance  : public ScreenWidget
{
    Q_OBJECT

    typedef servicePPPDistance _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit servicePPPDistance(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~servicePPPDistance();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:

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
    Ui::servicePPPDistance *ui;
    MadpInternalConfig *internalConfigData;

    void initialize();
};

#endif // SERVICEPPPDISTANCE_H
