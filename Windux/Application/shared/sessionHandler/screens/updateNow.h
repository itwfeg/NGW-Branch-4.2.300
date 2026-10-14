///////////


/**
 * @file updateNow.h
 * @brief updateNow
  *
  * @copyright Copyright (c) 2013 Hobart Corporation
  * @author Todd
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/

#ifndef UPDATENOW_H
#define UPDATENOW_H

#include "screenwidget.h"
#include <Messages.h>
#include <QProcess>
#include <QFileSystemWatcher>

class MadpScaleConfig;
class MadpInternalConfig;

namespace Ui {
class updateNow;
}

/**
* @class updateNow
* @brief  Allows User to update from remote source...BLOCKS UI.
*
*/

class updateNow  : public ScreenWidget
{
    Q_OBJECT

    typedef updateNow _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit updateNow(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~updateNow();

    ///////////////////////////////////////////////////////////////////////////
    // Properties


    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:


    /**
     * @brief directoryChanged
     * @param path
     */
    void directoryChanged(const QString & path);

    /**
     * @brief fileChanged
     * @param path
     */
    void fileChanged(const QString & path);

    void noUpdateNowClicked();
    void yesUpdateNowClicked();

    void closeUpdateNowFrame();
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
    Ui::updateNow *ui;

    QProcess *updateNowProcess;

    QFileSystemWatcher * watcher;


    MadpScaleConfig *scaleConfigData;
    bool keepModal;
    bool doneADownload;
    bool doingFirmware;


     ///////////////////////


};



//////////////
#endif // UPDATENOW_H
