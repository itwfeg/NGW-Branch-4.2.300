/**
  * @file manufacturingImageDialog.h
  * @brief Dialog for upgrading from mfg. image
  *
  * @copyright Copyright (c) 2015, ITW Hobart Food Equipment Group, U.S.A.
  * @author Alex Zavala
  * @addtogroup ManufacturingImageDialogCsu
  * @{
  */

#ifndef MANUFACTURINGIMAGEDIALOG_H
#define MANUFACTURINGIMAGEDIALOG_H

#include <sessiondialog.h>

class MadpInternalConfig;

namespace Ui {
class ManufacturingImageDialog;
}

/// Dialog for upgrading from the manufacturing image (AWS,EWS,CLAS)
/**
 * This dialog contains buttons to send model selection of manufacturing image boards for our
 * wrapping devices.
 */
class ManufacturingImageDialog : public SessionDialog
{
    Q_OBJECT

    typedef ManufacturingImageDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit ManufacturingImageDialog(QWidget *parent = 0);

    /// Destructor.
    virtual ~ManufacturingImageDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Whether or not clicking the overlay rejects the dialog.
    /**
     * @return  @c true if allowed, @c false otherwise
     */
    virtual bool allowOverlayClickDismiss() const;

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

protected:

    static const QSize dialogSize_;                 ///< Dialog size.

private slots:

    /// SLOT method to remove the primary weigher
    void awsClicked();

    /// SLOT method to send ews model info
    void ewsClicked();

    /// SLOT method to send clas model info
    void clasClicked();

private:

    Ui::ManufacturingImageDialog *ui;

    /// Initialize.
    void initialize();

    MadpInternalConfig *internalConfigData_;

};

#endif // MANUFACTURINGIMAGEDIALOG_H
