/**
 * @file superEmailSetup.h
 * @brief Widget to set the recipient of emailed alerts.
  *
  * @copyright Copyright (c) 2013 Hobart Corporation
  * @author Todd
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/

#ifndef SUPEREMAILSETUP_H
#define SUPEREMAILSETUP_H

#include "screenwidget.h"

class QTableWidgetItem;
class QIntValidator;

namespace Ui {
class superEmailSetup;
}

/**
* @class superEmailSetup
* @brief  Changes a product's reporting class
*  
*/

class superEmailSetup  : public ScreenWidget
{
    Q_OBJECT

    typedef superEmailSetup _Myt;
    typedef ScreenWidget _Mybase;
    
public:

	typedef enum		//	HTA-113: These should match up with what is in EmailAlertsManager.h
	{
		EMAILALERT_OUT_OF_LABELS,
        EMAILALERT_CASSETTE_OPEN,
		EMAILALERT_GCD_PLAYLIST_ERROR,
		EMAILALERT_GCD_FTP_ERROR,
		EMAILALERT_SCALE_OFFLINE,
		EMAILALERT_LABEL_PRESENT,
        EMAILALERT_FPP_UPDATE_FAILED,

		EMAILALERT_INVALID
	} EmailAlertTypes;

    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit superEmailSetup(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~superEmailSetup();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:

    /**
     * SLOT to improve existing HLX code by preventing spaces in hostnames
     *
     * @param[in] none
     * @returns  none
     */
     void preventSpacesInHostnames(const QString&);

     /**
      * SLOT to catch a button click to switch between hostname and IP mail server
      *
      * @param[in] none
      * @returns  none
      */
     void switchIPHostnameButClicked();

     /**
      * SLOT to direct save any changes in the alerts
      *
      * @param[in] none
      * @returns  none
      */
     void alertCheckboxChanged(QTableWidgetItem*);

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
    Ui::superEmailSetup *ui;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void loadFromToServerIsDNS();

    /**
     * Method to save all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void saveFromToIPOrDNS();

    bool isIPSelected;

    bool loading;
    bool ignoreSignals_;

    QIntValidator *ipFirstOctet_;
    QIntValidator *ipNonFirstOctet_;

    QString alertMsg;
    QStringList alertTypeList;	//	HTA-113
    QStringList alertList;
    QStringList alertCBList;
    QStringList notifyCBList;
    QStringList delayList;

};

#endif // superEmailSetup
