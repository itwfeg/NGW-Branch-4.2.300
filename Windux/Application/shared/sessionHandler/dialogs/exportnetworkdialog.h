/**
 * @file exportnetworkdialog.h
 * Export network settings dialog.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef EXPORTNETWORKDIALOG_H
#define EXPORTNETWORKDIALOG_H

#include "../sessiondialog.h"

class QLabel;
class QPushButton;
class QCheckBox;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Export network settings dialog.
class ExportNetworkSettingsDialog : public SessionDialog
{
    Q_OBJECT

    typedef ExportNetworkSettingsDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    /// Export types.
    enum ExportType
    {
        CurrentNetworkSettings,                     ///< Export the current network settings.
        LastKnownWorkingNetworkSettings             ///< Export the last known working settings.
    };

    enum ExportNetwork
    {
        networkYes,                                    ///<export network settings
        networkNo
    };

    enum ExportHostname                            ///<export hostname
    {
        hostnameYes,
        hostnameNo
    };

    enum ExportNTP                                ///<export ntp settings
    {
        ntpYes,
        ntpNo
    };
    enum ExportNTPRemove                                ///<export ntpRemove settings
    {
        ntpRemoveYes,
        ntpRemoveNo
    };


    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit ExportNetworkSettingsDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~ExportNetworkSettingsDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve export type.
    /**
     * @return  export type
     */
    virtual ExportType exportType() const {return exportType_;}

    /// Retrieve export network choice.
    /**
     * @return  export network choice
     */
    virtual ExportNetwork exportNetwork() const {return exportNetwork_;}

    /// Retrieve export hsotname choice.
    /**
     * @return  export hostname choice
     */
    virtual ExportHostname exportHostname() const {return exportHostname_;}

    /// Retrieve export ntp choice.
    /**
     * @return  export ntp choice
     */
    virtual ExportNTP exportNTP() const {return exportNTP_;}

    /// Retrieve export ntpRemove choice.
    /**
     * @return  export ntpRemvoe choice
     */
    virtual ExportNTPRemove exportNTPRemove() const {return exportNTPRemove_;}

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const;

private Q_SLOTS:

    /// Slot for button clicked.
    void onButtonClicked();
    /// Slot for NTP Remove checkbox toggled.
    void onNTPRemoveClicked();
    /// Slot for NTP Info checkbox toggled.
    void onNTPInfoClicked();
    /// Slot for enabling state of network device export
    void manageNetworkDeviceExport();

private:

    ExportType exportType_;
    ExportNetwork exportNetwork_;
    ExportHostname exportHostname_;
    ExportNTP exportNTP_;
    ExportNTPRemove exportNTPRemove_;
    QLabel *message_;
    QLabel *choicesMessage_;
    QPushButton *nowServing_;
    QPushButton *scale_;
    QLabel *spacer_;
    QCheckBox *networkInfo_;
    QCheckBox *hostnameInfo_;
    QCheckBox *ntpInfo_;
    QCheckBox *ntpRemove_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // EXPORTNETWORKDIALOG_H
