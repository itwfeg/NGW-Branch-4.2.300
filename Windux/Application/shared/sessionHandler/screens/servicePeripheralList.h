/**
  * @file servicePeripheralList.h
  * @brief Screen for displaying information about the hardware peripherals attached to the system.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup servicePeripheralListCsu
  * @{
  */

#ifndef SERVICEPERIPHERALLIST_H
#define SERVICEPERIPHERALLIST_H

#include "screenwidget.h"

namespace Ui {
class servicePeripheralList;
}

/**
 * @class servicePeripheralList
 * @brief Screen for displaying information about the hardware peripherals attached to the system.
 *
 * This hardware can include:
 *
 * Weigher (only first weigher; second weigher not implemented)
 * Printer (first, second and third)
 * Numeric
 * Marquee
 * Label Applier
 * Wrapper (including EvolWrapper)
 * GCD
 */
class servicePeripheralList : public ScreenWidget
{
    Q_OBJECT

    typedef servicePeripheralList _Myt;
    typedef ScreenWidget _Mybase;
    
public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit servicePeripheralList(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~servicePeripheralList();

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
    Ui::servicePeripheralList *ui;

    int tableRow;
    bool weigherUpdated;
    bool printerUpdated;
    bool wrapperUpdated;

    /**
     * Returns the Type for the Weigher based on the Weigher Kind
     */
    QString setWeigherKindText(QString weigherKind);

    /**
     * Returns the Name for the Printer based on the Printer number
     */
    QString setPrinterNameText(QString printerNumber);

    /**
     * Returns the Type for the Printer based on the Printer Kind
     */
    QString setPrinterKindText(QString printerKind);

    /**
     * Returns the Type for the Numeric based on the Numeric Kind
     */
    QString setNumericKindText(QString numericKind);

    /**
     * Returns the Type for the Marquee based on the Marquee Kind
     */
    QString setMarqueeKindText(QString marqueeKind);

    /**
     * Returns the Type for the GCD based on the GCD Kind
     */
    QString setGcdKindText(QString gcdKind);

    /**
     * Returns the Type for the Label Applier based on the Label Applier Kind
     */
    QString setLabelApplierKindText(QString labelApplierKind);

    /**
     * Returns the Type for the Wrapper based on the Wrapper Kind
     */
    QString setWrapperKindText(QString wrapperKind);

    /**
     * Populates a row for the table, given the name, type, s/w version and h/w version
     */
    void updateResultsTable(QString name, QString type, QString swVer, QString hwVer);

    /**
     * @brief updateTranslationInResultsTable changes first column of the results table to the recently selected language.
     */
    void updateTranslationInResultsTable();

    bool primaryPrinterListed;
    bool secondaryPrinterListed;
    bool tertiaryPrinterListed;
    bool weigherListed;
    bool labelApplierListed;
    bool wrapperListed;
    bool evolWrapperListed;
    bool numericListed;
    bool marqueeListed;
    bool graphicalCustomerDisplayListed;
    bool unknownListed;

    int weigherRowNumber;
    int wrapperRowNumber;
    int evolWrapperRowNumber;
    int labelApplierRowNumber;
    int primaryPrinterRowNumber;
    int secondaryPrinterRowNumber;
    int tertiaryPrinterRowNumber;
    int numericRowNumber;
    int marqueeRowNumber;
    int graphicalCustomerDisplayRowNumber;
    int unknownRowNumber;
    QStringList resultsTableDeviceNamesForTranslation;

    QStringList resultsTableDeviceTypesForTranslation;

};

#endif // SERVICEPERIPHERALLIST_H
