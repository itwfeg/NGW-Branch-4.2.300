/**
 * @file temperatureviewer.h
 * Widget for viewing sensor temp data.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef TEMPERATUREVIEWER
#define TEMPERATUREVIEWER

#include "dbquery.h"

#include <QWidget>

class temperatureGraphWidget;

class QLabel;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget for viewing sensor temp data.
class temperatureViewerWidget : public QWidget
{
    Q_OBJECT

    typedef temperatureViewerWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    temperatureViewerWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~temperatureViewerWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set sensor filtering.
    /**
     * @param[in] bottom  filter bottom (epoch time in seconds)
     * @param[in] top  filter top (epoch time in seconds)
     */
    virtual void setSensorFilter( unsigned int bottom, unsigned int top = 0 );

    /// Set sensor point id.
    /**
     * @param[in] value  sensor point id
     */
    virtual void setSensorPointID( unsigned int value );

    /// Set sensor id.
    /**
     * @param[in] value  sensor id
     */
    virtual void setSensorID( unsigned int value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate text.
    virtual void translate();

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Refresh sensor data.
    /**
     * @param[in] sid  sensor id
     * @param[in] spid  sensor point id
     * @param[in] filterBottom  filter bottom (epoch time in seconds)
     * @param[in] filterTop  filter top (epoch time in seconds)
     */
    virtual void refresh( unsigned int sid, unsigned int spid, unsigned int filterBottom, unsigned int filterTop );

private:

    dbQuery db_;

    unsigned int sid_;
    unsigned int spid_;

    unsigned int filterBottom_;
    unsigned int filterTop_;

    bool useCelsius_;

    QLabel *descriptionLabel_;
    QLabel *description_;

    QLabel *sensorLabel_;
    QLabel *sensor_;

    QLabel *updatedLabel_;
    QLabel *updated_;

    QLabel *temp_;

    temperatureGraphWidget *graph_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Convert temperature.
    qreal convertTemp( qreal temp ) const;

    /// Format temperature.
    QString formatDisplayTemp( qreal temp ) const;

    /// Retrieve formatted sensor age.
    static QString sensorDataAge( qint64 diff );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TEMPERATUREVIEWER
