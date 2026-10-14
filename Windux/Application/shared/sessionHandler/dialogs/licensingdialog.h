/**
 * @file licensingdialog.h
 * Licensing scale application dialog.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LICENSINGDIALOG_H
#define LICENSINGDIALOG_H

#include "../sessiondialog.h"

class QPlainTextEdit;
class QXmlStreamReader;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Licensing scale application dialog.
class LicensingDialog : public SessionDialog
{
    Q_OBJECT

    typedef LicensingDialog _Myt;
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
    explicit LicensingDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~LicensingDialog();

private:

    QPlainTextEdit *text_;

    QString hobartLicenses_;
    QString thirdPartyLicenses_;

    /// Initialize.
    void initialize();

    /// Read Hobart licenses.
    void readHobartLicenses();

    /// Read third pary licenses.
    void readThirdPartyLicenses();

    /// Parse XML license.
    void parseLicense( QXmlStreamReader& xml );

    /// Parse XML license.
    void parseCompany( QXmlStreamReader& xml );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // LICENSINGDIALOG_H
