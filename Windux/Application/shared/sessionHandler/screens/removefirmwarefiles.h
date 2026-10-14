/**
 * @file removeFirmwareFiles.h
 * @brief Widget to remove wrapper's firmware files.
  *
  * @copyright Copyright (c) 2013 Hobart Corporation
  * @author Todd
  * @addtogroup Service Mode CSU
  * @{
*/

#ifndef REMOVEFIRMWAREFILES_H
#define REMOVEFIRMWAREFILES_H

#include "screenwidget.h"

class MadpScaleConfig;
class QIntValidator;
class MadpInternalConfig;

namespace Ui {
class removeFirmwareFiles;
}

/**
* @class removeFirmwareFiles
* @brief removes a wrapper's firmware files
*
*/

class removeFirmwareFiles  : public ScreenWidget
{
    Q_OBJECT

    typedef removeFirmwareFiles _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit removeFirmwareFiles(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~removeFirmwareFiles();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:

    void removeFirwareFiles_clicked();

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

    /**
     * Check for specific error message is handled by screen.
     *
     * Session will call this method upon receiving an error message. The default error handling
     * (i.e. popup an error dialog) shall be invoked unless @c true is returned from
     * this property.
     *
     * @returns  @c true to use custom error handler, @c false otherwise
     */
    virtual bool isErrorMessageHandler( const SessionMessage& errorMsg ) const;

private:
    Ui::removeFirmwareFiles *ui;
    MadpInternalConfig *internalConfigData;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    int makeValidatorTop(int digits);

    MadpScaleConfig *scaleConfigData;

    QString scaleModel;
};

#endif // REMOVEFIRMWAREFILES
