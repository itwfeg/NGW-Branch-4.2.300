/**
  * @file serviceWeigherInfo.h
  * @brief Screen for displaying Weigher information for the weigher attached to the system.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup serviceWeigherInfoCsu
  * @{
  */

#ifndef SERVICEWEIGHERINFO_H
#define SERVICEWEIGHERINFO_H

#include "screenwidget.h"

namespace Ui {
class serviceWeigherInfo;
}

/**
 * @class serviceWeigherInfo
 * @brief Screen for displaying Weigher information for the weigher attached to the system.
 */
class serviceWeigherInfo : public ScreenWidget
{
    Q_OBJECT

    typedef serviceWeigherInfo _Myt;
    typedef ScreenWidget _Mybase;
    
public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit serviceWeigherInfo(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceWeigherInfo();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

protected:

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );


private:
    Ui::serviceWeigherInfo *ui;

    bool valuemaxIsEnabled_;            // indicates if value max is off or on

    /**
     * Sends message to turn off the repeating Weigher Counts messages
     *
     */
    void TurnOffInfoCountsMessage() const;

    /**
     * SOF-3153: Hide "Degrees" section if not an "HTm" scale model
     *
     */
    void hideDegreesSection() const;
};

#endif // SERVICEWEIGHERINFO_H
