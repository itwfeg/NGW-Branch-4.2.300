/**
  * @file scaleStatistics.h
  * @brief Screen for displaying Scale Statistics.
  *
  * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup scaleStatisticsCsu
  * @{
  */

#ifndef SCALESTATISTICS_H
#define SCALESTATISTICS_H

#include "screenwidget.h"

namespace Ui {
class scaleStatistics;
}

/**
 * @class scaleStatistics
 * @brief Screen for displaying Scale Statistics.
 *
 * This class is used to display scale statistics, including used and available
 * memory and number of various record types in the database.
 */
class scaleStatistics : public ScreenWidget
{
    Q_OBJECT
    
    typedef scaleStatistics _Myt;
    typedef ScreenWidget _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit scaleStatistics(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~scaleStatistics();

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
    Ui::scaleStatistics *ui;
};

#endif // SCALESTATISTICS_H
