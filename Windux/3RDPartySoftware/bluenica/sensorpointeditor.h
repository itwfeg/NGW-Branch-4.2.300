/**
 * @file sensorpointeditor.h
 * Widget for editing sensor point configuration.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SENSORPOINTEDITOR
#define SENSORPOINTEDITOR

#include "dbquery.h"

#include <QWidget>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget for editing sensor point configuration.
class sensorPointEditorWidget : public QWidget
{
    Q_OBJECT

    typedef sensorPointEditorWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    sensorPointEditorWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~sensorPointEditorWidget();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Create new temp profile.
    virtual void insert();

    /// Load configuration info.
    /**
     * @param[in] id  sensor id
     */
    virtual void load( unsigned int id );

    /// Delete configuration info.
    /**
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool remove();

    /// Save configuration info.
    /**
     * @param[in] isNewRecord  @c true if new record, @c false otherwise
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool save( bool isNewRecord = false );

    /// Translate text.
    virtual void translate();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Refresh form.
    virtual void refresh();

private:

    dbQuery db_;

    sensorPointInfo  record_;

    QLabel *descriptionLabel_;
    QLineEdit *description_;

    QLabel *profileLabel_;
    QComboBox *profile_;

    QLabel *sensorAddressLabel_;
    QComboBox *sensorAddress_;

    QLabel *generateAlertsLabel_;
    QCheckBox *generateAlerts_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SENSORPOINTEDITOR
