/**
 * @file profileeditor.h
 * Widget for editing temp profile configuration.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef PROFILEEDITOR
#define PROFILEEDITOR

#include "dbquery.h"

#include <QWidget>

class QDoubleValidator;
class QLabel;
class QLineEdit;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget for editing temp profile configuration.
class tempProfileEditorWidget : public QWidget
{
    Q_OBJECT

    typedef tempProfileEditorWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    tempProfileEditorWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~tempProfileEditorWidget();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Create new temp profile.
    virtual void insert();

    /// Load temp profile.
    /**
     * @param[in] id  sensor id
     */
    virtual void load( unsigned int id );

    /// Delete temp profile.
    virtual bool remove();

    /// Save temp profile.
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

private:

    dbQuery db_;

    profileInfo record_;

    QDoubleValidator *tempValidator_;

    bool useCelsius_;

    QLabel *descriptionLabel_;
    QLineEdit *description_;

    QLabel *warningFromLabel_;
    QLineEdit *warningFrom_;

    QLabel *normalFromLabel_;
    QLineEdit *normalFrom_;

    QLabel *normalToLabel_;
    QLineEdit *normalTo_;

    QLabel *warningToLabel_;
    QLineEdit *warningTo_;

    QLabel *standardLabel_;
    QLineEdit *standard_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Convert temperature (to F if needed).
    qreal convertTemp( qreal temp ) const;

    /// Revert temperature (to C if needed).
    qreal revertTemp( qreal temp ) const;

    /// Format temperature.
    QString formatDisplayTemp( qreal temp ) const;

    /// Validate input.
    static bool validateInput( const QLineEdit *w );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // PROFILEEDITOR
