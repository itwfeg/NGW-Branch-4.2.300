/**
 * @file superApplierRotationOverride.h
 * @brief Supervisor screen for overriding the label applier rotation for all PLUs
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala
 * @addtogroup superApplierRotationOverrideCsu
 * @{
*/
#ifndef SUPERAPPLIERROTATIONOVERRIDE_H
#define SUPERAPPLIERROTATIONOVERRIDE_H

#include "screenwidget.h"

class DomRotationOverrides;
class QComboBox;

namespace Ui {
class superApplierRotationOverride;
}

/**
* @class superApplierRotationOverride
* @brief class for overriding applier rotation for all PLUs
*/

class superApplierRotationOverride  : public ScreenWidget
{
    Q_OBJECT

    typedef superApplierRotationOverride _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
      */
    explicit superApplierRotationOverride(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~superApplierRotationOverride();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:

    /**
     * @brief primaryApplierRotationOverride
     * @param index the index of the primaryApplierOverride cbox
     */
    void primaryApplierRotationOverride(int index);

    /**
     * @brief secondaryApplierRotationOverride
     * @param index the index of the secondaryApplierOverride cbox
     */
    void secondaryApplierRotationOverride(int index);

    /**
     * @brief tertiaryApplierRotationOverride
     * @param index the index of the tertiaryApplierOverride cbox
     */
    void tertiaryApplierRotationOverride(int index);

    /**
     * @brief checkIfHasChanged checks if the override rotation has changed in any way
     */
    void checkIfHasChanged();

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
    Ui::superApplierRotationOverride *ui;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method that disables all of the printer's applier rotation and rotation degrees fields
     *
     * @brief disableAll disables all printer fields
     */
    void disableAll();

    /**
     * Method that enables all of the applier rotation and rotation degrees fields
     *
     * @brief enableAll enables all printer fields
     */
    void enableAll();

    /**
     * Method that enables any printer that is connected to the console.
     *
     * @brief enableConnectedPrinters enables all printer fields that are currently connected
     */
    void enableConnectedPrinters();

    /**
     * Method that enables the applier rotation and rotation degrees combo boxes
     *
     * @param printerNum 0/Printer1, 1/Printer2, 2/Printer3
     * @brief enablePrinter enables the specified printer
     */
    void enablePrinter(int printerNum);

    /**
     * Method that disables the applier rotation and rotation degrees combo boxes
     *
     * @param printerNum 0/Printer1, 1/Printer2, 2/Printer3
     * @returns  none
     * @brief disablePrinter disables the specified printer
     */
    void disablePrinter(int printerNum);

    /**
     * @brief disableRotationDegrees disables the rotation degrees cbox
     * @param printerNum the printer station
     */
    void disableRotationDegrees(int printerNum);

    /**
     * @brief enableRotationDegrees enables the rotation degrees cbox
     * @param printerNum the printer station
     */
    void enableRotationDegrees(int printerNum);

    /**
     * @brief updateRotationOverrideTable updates the applier rotation override table
     * @param msg the applier rotation override read msg
     */
    void updateRotationOverrideTable(DomRotationOverrides msg);

    void setOverrideSetting(QComboBox *myCBox, int rotationVal);		//	HA-79

    //Enum for printerType
    typedef enum printerType
    {
        PrimaryPrinter     = 0,
        SecondaryPrinter   = 1,
        TertiaryPrinter    = 2
    } printerType_et;

    //Keeps track of what our initial values for the applier were
    DomRotationOverrides* initialOverrideValues;

    //True if the rotation override has changed
    bool hasChanged;
};
#endif // SUPERAPPLIERROTATIONOVERRIDE_H
