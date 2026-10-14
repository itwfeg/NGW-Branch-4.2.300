/**
  * @file labelFieldsToPrint.h
  * @brief Allows supervisor to select special fields to print on a label.
  *
  * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup labelFieldsToPrintCsu
  * @{
  */

#ifndef LABELFIELDSTOPRINT_H
#define LABELFIELDSTOPRINT_H

#include "screenwidget.h"

class QWidget;

namespace Ui {
class labelFieldsToPrint;
}

/**
 * @class labelFieldsToPrint
 * @brief Allows supervisor to select special fields to print on a label.
 *
 * Gives you the ability to print the following special fields on a label, providing the Label Type permits it:
 *
 * Sell By Date/Julian Sell By Date, Packed-On Date/Julian Packed-On Date, Product Life Date/Julian Product Life Date,
 * Unit Price, By Count, UPC/EAN Number and Barcode, Currency Symbol, Pre-Print Text Names, Weight on a By Count,
 * "1 For" on a By Count item when the by count is 1, 2nd UPC Check Digit.
 */

class labelFieldsToPrint : public ScreenWidget
{
    Q_OBJECT

    typedef labelFieldsToPrint _Myt;
    typedef ScreenWidget _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit labelFieldsToPrint(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~labelFieldsToPrint();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public slots:

    /**
     * Slot for when a checkbox is clicked or a dropdown is changed.
     * Sets property indicating that the setup needs to be saved
     * when exiting.
     */
    void SetupChanged();

    /**
     * Slot to uncheck One on a by count if we want Weight on a by count
     */
    void changeWeightOnAByCount();

    /**
     * Slot to uncheck Weight on a by count if we want One on a by count
     */
    void changeOneOnAByCount();

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
    Ui::labelFieldsToPrint *ui;

    QVector<QString> allRecordIDsList;
    QVector<QString> namesList;
    QVector<QString> secNamesList;

    QString priceChangedLabelIndicator;

    int numRecords;
    int numOfSecRecords;

    bool bSetupChanged;
    bool initialized;
    bool dataHasBeenInitialized;                                    // data not saved if not received in the first place

    /**
     * Builds the parameter string and saves the setup.
     *
     */
    void SaveDataOnExit();

};

#endif // LABELFIELDSTOPRINT_H
