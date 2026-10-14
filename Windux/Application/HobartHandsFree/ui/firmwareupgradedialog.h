/**
 * @file firmwareupgradedialog.h
 * Dialog for firmware update progress.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef FIRMWAREUPGRADEDIALOG
#define FIRMWAREUPGRADEDIALOG

#include <QDialog>

class QLabel;
class QProgressBar;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog for firmware update progress.
class firmwareUpgradeDialog : public QDialog
{
    Q_OBJECT

    typedef firmwareUpgradeDialog _Myt;
    typedef QDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    firmwareUpgradeDialog( QWidget *parent = 0 );

    /// Destructor.
    virtual ~firmwareUpgradeDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set progress.
    /**
     * @param[in] value  percent
     */
    virtual void setProgress( int value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate text.
    virtual void translate();

private:

    QLabel *title_;
    QLabel *message_;

    QLabel *warning_;

    QProgressBar *progress_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // FIRMWAREUPGRADEDIALOG
