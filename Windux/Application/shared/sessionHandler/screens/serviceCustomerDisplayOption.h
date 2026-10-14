/**
  * @file serviceCustomerDisplayOption.h
  * @brief Allows for printing when a GCD is not present (i.e. use as Prepack scale).
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup serviceCustomerDisplayOptionCsu
  * @{
  */

#ifndef SERVICECUSTOMERDISPLAYOPTION_H
#define SERVICECUSTOMERDISPLAYOPTION_H

#include "screenwidget.h"

namespace Ui {
class serviceCustomerDisplayOption;
}

/**
 * @class serviceCustomerDisplayOption
 * @brief Allows for printing when a GCD is not present (i.e. use as Prepack scale).
 *
*/


class serviceCustomerDisplayOption : public ScreenWidget
{
    Q_OBJECT
    
    typedef serviceCustomerDisplayOption _Myt;
    typedef ScreenWidget _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit serviceCustomerDisplayOption(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceCustomerDisplayOption();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public slots:

    /**
     * Slot for when "Allow Printing Without Customer Display" checkbox is changed.
     */
    void AllowPrintingWithoutGcdChanged();

protected:

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private:

    Ui::serviceCustomerDisplayOption *ui;

    QString originalValue;

    bool ignoreSignals_;
};

#endif // SERVICECUSTOMERDISPLAYOPTION_H
