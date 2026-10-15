/**
 * @file superUpgradeFirmwareViaUSB.h
 * @brief Widget to set set configuraiton of firmware upgrade via USB.
  *
  * @copyright Copyright (c) 2013 Hobart Corporation
  * @author Todd
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/

#ifndef SUPERUPGRADEFIRMWAREVIAUSB_H
#define SUPERUPGRADEFIRMWAREVIAUSB_H

#include "screenwidget.h"
#include "python.h"

class MadpScaleConfig;
class MadpInternalConfig;

namespace Ui {
class superUpgradeFirmwareViaUSB;
}

/**
* @class superUpgradeFirmwareViaUSB
* @brief  Changes a product's reporting class
*  
*/

class superUpgradeFirmwareViaUSB  : public ScreenWidget
{
    Q_OBJECT

    typedef superUpgradeFirmwareViaUSB _Myt;
    typedef ScreenWidget _Mybase;
    
public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit superUpgradeFirmwareViaUSB(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~superUpgradeFirmwareViaUSB();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:


    /**
     * Slot to upgrade using User provided USB file/s.
     *
     * @param[in] none
     * @returns  none
     */
    void upgrade();

    /**
     * Slot to provide User a file dialog.
     *
     * @param[in] none
     * @returns  none
     */
    void openFile();
    void openDir();

    /**
     * Slot to close Upgrade problem notice.
     *
     * @param[in] none
     * @returns  none
     */
    void notice();

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
    Ui::superUpgradeFirmwareViaUSB *ui;


    /**
     * Method to load combo boxes with predefined colors
     *
     * @param[in] none
     * @returns  none
     */
    //void loadColorBoxes();

    MadpScaleConfig *scaleConfigData;
    MadpInternalConfig *internalConfigData;

    QString firmwareFilename;
    bool loading;


};

#endif // SUPERUPGRADEFIRMWAREVIAUSB
