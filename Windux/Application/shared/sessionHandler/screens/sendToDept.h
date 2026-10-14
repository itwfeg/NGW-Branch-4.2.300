///////////


/**
 * @file sendToDept.h
 * @brief sendToDept
  *
  * @copyright Copyright (c) 2013 Hobart Corporation
  * @author Todd
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/

#ifndef SENDTODEPT_H
#define SENDTODEPT_H

#include "screenwidget.h"
#include <Messages.h>
#include <QProcess>
#include <QFileSystemWatcher>
#include "python.h"

class MadpScaleConfig;
class MadpInternalConfig;

namespace Ui {
class sendToDept;
}

/**
* @class sendToDept
* @brief  Allows User to update from remote source...BLOCKS UI.
*
*/

class sendToDept  : public ScreenWidget
{
    Q_OBJECT

    typedef sendToDept _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit sendToDept(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~sendToDept();

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

    void nosendToDeptClicked();
    void yessendToDeptClicked();

    void closesendToDeptFrame();
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
    Ui::sendToDept *ui;

    QProcess *sendToDeptProcess;

    QFileSystemWatcher * watcher;
    void continueSendToDept();

    MadpScaleConfig *scaleConfigData;
    bool keepModal;
    bool doneADownload;
    python *pythonProcess;
    QString primaryScaleIP;
    QString primaryScaleHostname;

     ///////////////////////


};



//////////////
#endif // SENDTODEPT_H
