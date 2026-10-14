/**
  * @file removePeripheralsDialog.h
  * @brief Dialog for the missing peripherals.
  *
  * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
  * @author Job E. Crew
  * @addtogroup RemovePeripheralsDialogCsu
  * @{
  */


#ifndef REMOVEPERIPHERALSDIALOG_H
#define REMOVEPERIPHERALSDIALOG_H

#include "../sessiondialog.h"
#include <Messages.h>

class QPushButton;              // prolly wont need this anymore

namespace Ui {
class RemovePeripheralsDialog;
}

/**
 * @class RemovePeripheralsDialog
 * @brief Dialog for list of missing Peripherals.
 *
 * This dialog contains information on the list of missing peripherals and the ability to remove them.
 */
class RemovePeripheralsDialog : public SessionDialog
{
    Q_OBJECT

    typedef RemovePeripheralsDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     */
    explicit RemovePeripheralsDialog(QWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~RemovePeripheralsDialog();

    /**
     * Whether or not clicking the overlay rejects the dialog.
     *
     * @returns  @c true if allowed, @c false otherwise
     */
    virtual bool allowOverlayClickDismiss() const;

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

protected:

    /**
     * Slot to allow the widget to handle processing of incoming session messages.
     *
     * @param[in] msg  session message
     */
    void onSessionMessageReceive( const SessionMessage& msg );

private slots:

    /**
    * SLOT method to remove the primary weigher
    *
    * @param[in] none
    * @returns  none
    */
    void removeFirstWeigher();

    /**
    * SLOT method to remove the secondary weigher
    *
    * @param[in] none
    * @returns  none
    */
    void removeSecondWeigher();

    /**
    * SLOT method to remove the primary printer
    *
    * @param[in] none
    * @returns  none
    */
    void removeFirstPrinter();

    /**
    * SLOT method to remove the secondary weigher
    *
    * @param[in] none
    * @returns  none
    */
    void removeSecondPrinter();

    /**
    * SLOT method to remove the third weigher
    *
    * @param[in] none
    * @returns  none
    */
    void removeThirdPrinter();

    /**
    * SLOT method to remove the numeric VFD
    *
    * @param[in] none
    * @returns  none
    */
    void removeNumeric();

    /**
    * SLOT method to remove the marquee VFD
    *
    * @param[in] none
    * @returns  none
    */
    void removeMarquee();

    /**
    * SLOT method to remove the label applier
    *
    * @param[in] none
    * @returns  none
    */
    void removeLabelApplier();

    /**
    * SLOT method to remove the wrapper
    *
    * @param[in] none
    * @returns  none
    */
    void removeWrapper();

    /**
    * SLOT method to remove the GCD
    *
    * @param[in] none
    * @returns  none
    */
    void removeGcd();

    /**
    * SLOT method to leave the page
    *
    * @param[in] none
    * @returns  none
    */
    void onClickDone();


private:

    Ui::RemovePeripheralsDialog *ui;

    //MadpLostPeripheralList *lostPeripheralList_;

    int currentItem;                                                // what item, and in what row, are we currently updating.
    int currentRow;
    QVector<int> rowPosition_;                                      // This is the y coordinate of items moving in the list of Perifs

    QStringList weigherTypeNames;                                   // the list of possible weighers
    QStringList printerTypeNames;                                   // list of possible printers
    QStringList laApplierTypeNames;                                 // list of possible label appliers
    QStringList wrapperTypeNames;                                   // list of possible wrappers
    QStringList numericTypeNames;                                   // list of possible numeric VFDs
    QStringList marqueeTypeNames;                                   // list of possible marquee VFDs
    QStringList gcdTypeNames;                                       // list of possible GSDs

    int weigher1;
    int weigher2;
    int printer1;
    int printer2;
    int printer3;
    int labeler1;
    int wrapper1;
    int numeric1;
    int marquee1;
    int gcd1;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method to display all missing peripherals and give users the opportunity to remove it
     *
     * @param[in] none
     * @returns  none
     */
    void displayElements();

    /**
     * Method to display the primary and secondary weighers
     *
     * @param[in] none
     * @returns  none
     */
    void displayWeighers();

    /**
     * Method to display the primary and secondary weighers
     *
     * @param[in] none
     * @returns  none
     */
    void displayPrinters();

    /**
     * Method to display the GCD ( Graphical Customer Display )
     *
     * @param[in] none
     * @returns  none
     */
    void displayGCD();

    /**
     * Method to display the VFD Numeric
     *
     * @param[in] none
     * @returns  none
     */
    void displayNumeric();

    /**
     * Method to display the VFD Marquee
     *
     * @param[in] none
     * @returns  none
     */
    void displayMarquee();

    /**
     * Method to display the wrapper
     *
     * @param[in] none
     * @returns  none
     */
    void displayWrapper();

    /**
     * Method to display the label applier
     *
     * @param[in] none
     * @returns  none
     */
    void displayLabelApplier();

    /**
     * Method to delete a specific row when the delete button is pressed for that item.
     *
     * @param[in] none
     * @returns  none
     */
    void deletePeripherial(int perifNum);

    /**
     * Method to update the locations of items after a peripherial has been removed
     *
     * @param[in] none
     * @returns  none
     */
    void updateLocations();

    /**
     * Method to show the initial missing peripherial list
     *
     * @param[in] none
     * @returns  none
     */
    void showLostPeripherals();

    /**
     * Method to update the peripherial list after we have the initial missing peripherial list
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void peripheralListChanged(const SessionMessage &msg);

};

#endif // REMOVEPERIPHERALSDIALOG_H
