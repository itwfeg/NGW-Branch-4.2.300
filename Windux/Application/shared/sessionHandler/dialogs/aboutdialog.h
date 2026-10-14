/**
 * @file aboutdialog.h
 * About scale application dialog.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include "../sessiondialog.h"

class QLabel;
class QPushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// About scale application dialog.
class AboutDialog : public SessionDialog
{
    Q_OBJECT

    typedef AboutDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit AboutDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~AboutDialog();

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve renderer appearance.
    /**
     * @return  appearance object
     */
    virtual hobart::widgetRendererAppearanceBase *rendererAppearance() const;

    /// Retrieve renderer style.
    /**
     * @return  style
     */
    virtual int rendererStyle() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Callback for session messages.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessageReceive( const SessionMessage& msg );

private Q_SLOTS:

    /// Slot for features button pressed.
    void onFeaturesPressed();

    /// Slot for licensing button pressed.
    void onLicensingPressed();

private:

    QLabel *scaleNameLabel_;
    QLabel *scaleName_;

    QLabel *scaleModelLabel_;
    QLabel *scaleModel_;

    QLabel *buildNumberLabel_;
    QLabel *buildNumber_;

    QLabel *versionLabel_;
    QLabel *version_;

    QLabel *osVersionLabel_;
    QLabel *osVersion_;

    QLabel *loaderVersionLabel_;
    QLabel *loaderVersion_;

    QLabel *controllerVersionLabel_;
    QLabel *controllerVersion_;

    QLabel *printerHardwareLabel_;
    QLabel *printerHardware_;

    QLabel *scaleSerialNumberLabel_;
    QLabel *scaleSerialNumber_;

    QLabel *copyrightLabel_;
    QLabel *copyright_;

    QLabel *ipLabel_;
    QLabel *ip_;

    QLabel *hostnameLabel_;
    QLabel *hostname_;

    QLabel *gatewayLabel_;
    QLabel *gateway_;

    QLabel *dnsLabel_;
    QLabel *dns_;

    // FT-1364
    QLabel *smBackendLabel_;
    QLabel *smBackendVersion_;

    QLabel *lastCommLabel_;
    QLabel *lastComm_;

    QLabel *lastItemChangeLabel_;
    QLabel *lastItemChange_;

    QLabel *eanAddress_;
    QLabel *swrevision_;
    QPushButton *features_;
    QPushButton *licensing_;

    QString hwRev_;

    QStringList installedFeatures_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Get gateway IP address.
    QString getGatewayValue();

    /// Get IP address.
    QString getIPValue();

    /// updates the version info in own method
    void updateVersionInfo(int versiondata);
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // ABOUTDIALOG_H
