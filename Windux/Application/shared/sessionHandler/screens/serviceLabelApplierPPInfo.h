#ifndef SERVICELABELAPPLIERPPINFO_H
#define SERVICELABELAPPLIERPPINFO_H

#include <QLabel>

#include "screenwidget.h"

class DomCurrentPLU;
class MadpScaleConfig;
class DomLabelApplierConfig;
class QIntValidator;
class MadpInternalConfig;

namespace Ui {
    class serviceLabelApplierPPInfo;
}

// service screen for printer plate calibration
class serviceLabelApplierPPInfo  : public ScreenWidget
{
    Q_OBJECT

    typedef serviceLabelApplierPPInfo _Myt;
    typedef ScreenWidget _Mybase;

public:
    explicit serviceLabelApplierPPInfo(SessionMainWindow *mainWindow, ModeWidget *parent = 0);
    virtual ~serviceLabelApplierPPInfo();


private slots:

    // no slots so far

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
    Ui::serviceLabelApplierPPInfo *ui;

    QString productID_;
    QString softwareVersion_;
    QString hardwareVersion_;


   ///////////////////

};

#endif // SERVICELABELAPPLIERPPINFO_H
