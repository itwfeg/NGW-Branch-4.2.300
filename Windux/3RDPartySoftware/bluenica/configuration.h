/**
 * @file configuration.h
 * Widget for viewing and editing configuration.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef CONFIGURATION
#define CONFIGURATION

#include "dbquery.h"

#include <QWidget>

class QLabel;
class QLineEdit;
class QComboBox;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget for viewing and editing configuration.
class configurationWidget : public QWidget
{
    Q_OBJECT

    typedef configurationWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    configurationWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~configurationWidget();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Load configuration info.
    virtual void load();

    /// Save configuration info.
    virtual bool save();

    /// Translate text.
    virtual void translate();

private:

    dbQuery db_;

    QLabel *tempUnitOfMeasureLabel_;
    QComboBox *tempUnitOfMeasure_;

    QLabel *maxTempAgeLabel_;
    QLineEdit *maxTempAge_;

    QLabel *batteryNormalFromLabel_;
    QLineEdit *batteryNormalFrom_;

    QLabel *batteryWarningFromLabel_;
    QLineEdit *batteryWarningFrom_;

    QLabel *commNormalFromLabel_;
    QLineEdit *commNormalFrom_;

    QLabel *commWarningFromLabel_;
    QLineEdit *commWarningFrom_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Validate input.
    static bool validateInput( const QLineEdit *w );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // CONFIGURATION
