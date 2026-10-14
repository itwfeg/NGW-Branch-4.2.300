/**
 * @file mainwindow.h
 * @brief Widget to set system configuration of the scale.
  *
  * @copyright Copyright (c) 2013 Hobart Corporation
  * @author Todd
  * @addtogroup Settings Wizard CSU
  * @{
*/

/*
PROLOGUE: QmSettings (aka Setup Wizard)
SRD: 117
Date: Q4 2012
AUTHOR: Todd Davis
COMPANY: Hobart

PURPOSE: This application serves the REQ for a System settings software wizard
         for the QM Series scales to configure the system hardware's settings for:

         - Time & Date
         - Sound Volume
         - Network Configuration
         - Touchscreen Calibration
         - Administration: Users
         - Timestamp of Last Settings
*/


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QString>
#include <QMouseEvent>
#include "network.h"
#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "../platform/python.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    bool isLinux();
    QString mode;
    static const QString ServicePassword; 

    static const bool USE_BACKDOOR;
    static const QString USERNAME;
    static const QString PASSWORD;
    static const QString PASSPHRASE;

    bool cancelPing;
    int volume;
    int networkCount;

    QDate *pickedDate;
    Network *commonNetwork;
    QString machineID;

   // void changeEvent(QEvent);
    void retranslateUI();
    void loadSavedNetwork(Network* network);

    // reboot vars below
    bool changedTimezone;
    bool changedHostname;
    bool changedAstronomical;

    QString initialHostname;
    QString initialTimeZone;
    bool initialAstronomical;

    void connectDB();
    void disconnectDB();

    ~MainWindow();

signals:
    void outlogtext(QString ver);
protected:
  //  void paintEvent(QPaintEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);
//    void mousePressEvent(QMouseEvent *event);

public slots:
//    void volumeSliderValueChanged(int);
//    void loginLanguageComboValueChange(int);
    void currentChangedSlot(int);
    void brightnessSpinBoxValueChanged(int);
    void volumeSpinBoxValueChanged(int);
    void changedOperatorDisplayBrightnessButtons();
    void changedCustomerDisplayBrightnessButtons();
    void changedCustomerSoundButtons();
    void changedOperatorSoundButtons();
    void datePicked(QDate selectionDate);
    void fillTimeZones();
    void astronomicalCheckboxValueChanged(int);
    void setTimeTab();
    void setTimezone();
    QStringList getWirelessAPs();
    void getUIMACAddress();
    void rebootScale();
    void getUIHostname();


//    bool getDBAstronomicalSetting();
//    int  getDBTimezone();
//    void updateDBTimeDate();
    void revealNetworkGroupBox(int, int);
    void networkTableWidgetSelectionChanged(int);
    void setWirelessNetworkEncryptionCombo(int);
    void loadWirelessSecurityMethodsCombo();
    void setCommonNetwork(Network* network);
    QStringList buildSaveConfigurationOptions(Network* network);
    QStringList buildImportConfigurationOptions(Network* network);
    QStringList buildExportConfigurationOptions();

   // void loadNetworkConnectionsTableView();

private slots:
    QString modelName();
 //   void loginBut_click();
 //   void logoutBut_click();
 //   void addUserBut_click();
 //   void deleteUserBut_click();
    void setVolumeBut_click();
    void setDateTimeBut_click();
//    void cancelNetworkBut_click();
    void saveNetworkBut_click();
    void setHostnameBut_click();
//    void applyNetworkBut_click();
    void calibrateScreenBut_click();
    void setBrightnessBut_click();
    void startPingBut_click();
    void addNetworkBut_click();
    void stopPingBut_click();
//    void exportBut_click();
//    void importBut_click();
    void testVolumeBut_click();
    void volumeIndicator0_click();
    void volumeIndicator1_click();
    void volumeIndicator2_click();
    void volumeIndicator3_click();
    void volumeIndicator4_click();
    void volumeIndicator5_click();
    void volumeIndicator6_click();
    void volumeIndicator7_click();
    void volumeIndicator8_click();
    void volumeIndicator9_click();
    void volumeIndicator10_click();
    void dhcpRadio_disableIPs();
    void staticIPRadio_enableIPs();
    void autoDNSRadio_click();
    void manualDNSRadio_click();
    void refreshNetworksButton_click();
    void readStdOutput();
    void printPingOutput();
    void doPreset(int);
    void importNetworkBut_click();
    void exportNetworkBut_click();



 //   bool userPassAreCorrect(QString, QString);
 //   bool insertUser(QString, QString, int);
 //   bool deleteUser(QString);
 //   bool loadUsers();
 //   void allowUserAccess();
 //   void changeMode();
 //   bool updateVolume(int, int);
 //   bool updateBrightness(int, int);
//    QString encrypt(QString);
  //  int getDBVolume();
 //   int getDBBrightness();
    void setVolumeIndicators(int);
    void setBrightnessIndicators(int);
    void muteButChanged(bool);
 //   bool useVPNConnection();
    void loadNetworkConnections();
    bool isWiredConnected();
//    bool useWirelessConnection();
    QStringList buildNetworkCommandOptions(Network* network);
 //   void loadSavedNetwork(Network *network);

private:
    Ui::MainWindow *ui;
    QLabel *privateIDLabel;
    QLabel *timezoneLabel;
    QLabel *isDaylightSavingsLabel;
    QLabel *operatorIDLabel;
    QPoint m_dragPosition;

    QTranslator *translator1;
    QTranslator *translator2;
    QProcess *pingProcess;
  //  QString machineID;
    bool adapterAlreadyConnected;

    bool visDemoMode;
    python *pythonProcess;
    void execPythonCommand(QString command,
                           QStringList arguments=QStringList());

    QString customerBrightness, operatorBrightness;
    int customerVolume, operatorVolume;
    QString soundDevice;
    void loadDisplayBrightnessValues(QString display);
    void loadSoundVolumeValues(QString device);
    QSqlDatabase database;
    QString alertsText;
    QSqlQuery *query;

    QDir dir;

  //  void setBrightness(QString display);
    bool updateBrightness(int percetnage, QString device);
    bool firstTimeCustomerBrightnessDisplay;
    bool firstTimeCustomerVolume;
    QString displayDevice;

    QString databaseFileLocation;


};

#endif // MAINWINDOW_H
