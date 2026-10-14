#ifndef FIRMWAREVERSIONDIALOG_H
#define FIRMWAREVERSIONDIALOG_H


#include "../sessiondialog.h"

class QLabel;
class MadpInternalConfig;

namespace Ui {
class FirmwareVersionDialog;
}

class FirmwareVersionDialog : public SessionDialog
{
    Q_OBJECT

    typedef FirmwareVersionDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     */
    explicit FirmwareVersionDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);

    /**
     * Destructor.
     */
    virtual ~FirmwareVersionDialog();

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

private Q_SLOTS:

    // ========================================================================
    // Slots
    // ========================================================================

   /**
    * SLOT when the Print button is clicked.
    *
    * @param[in] none
    * @returns none
    */
   void onPrintButtonClicked();

private:

    Ui::FirmwareVersionDialog *ui;

    MadpInternalConfig *internalConfig_;

    QStringList appVersionList;
    QStringList bootVersionList;

    QStringList appVersionListSort;
    QStringList bootVersionListSort;

    QList<QLabel*> application;
    QList<QLabel*> bootloader;
    QList<QLabel*> node;

    int msg_counter_;

    QString weigherVer_;
    QString applicationVer_;
    QString serialNumber_;
    QString modelName_;
    QString tofVer_;
    QString tofBootVer_;
    bool receivedBoot_;

    QString cycleCounts_;
    int printerNumber_;
    QString printerOneLabelCounts_;
    QString printerTwoLabelCounts_;
    QString printerThreeLabelCounts_;

    bool callTheseJustOneTimeEver_;

    // get model name based on model number
    void updateModelName(int modelNumber);

    /**
     * Method to initialize all variables and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * updates the version info in own method
     *
     */
    void updateVersionInfo(int updateInfo);


};

#endif // FIRMWAREVERSIONDIALOG_H
