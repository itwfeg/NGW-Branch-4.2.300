/**
 * @file temperaturegraph.h
 * Widget for temperature graphs.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef TEMPERATUREGRAPH
#define TEMPERATUREGRAPH

#include "dbquery.h"

#include <QList>
#include <QWidget>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget for temperature graphs.
class temperatureGraphWidget : public QWidget
{
    Q_OBJECT

    typedef temperatureGraphWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    temperatureGraphWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~temperatureGraphWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set temperature profile.
    /**
     * @param[in] value  profile
     */
    virtual void setProfile( const profileInfo &value ) {tempProfile_ = value;}

    /// Retrieve temperature profile.
    /**
     * @return  profile
     */
    virtual profileInfo profile() const {return tempProfile_;}

    /// Set use Celsius flag.
    /**
     * @param[in] value  @c true to use Celsius, @c false otherwise
     */
    virtual void setUseCelsius( bool value ) {useCelsius_ = value;}

    /// Retrieve use Celsius flag.
    /**
     * @return  @c true to use Celsius, @c false otherwise
     */
    virtual bool useCelsius() const {return useCelsius_;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Refresh graph.
    /**
     * @param[in] filterBottom  filter bottom (epoch time in seconds)
     * @param[in] filterTop  filter top (epoch time in seconds)
     * @param[in] temps  temperature data
     */
    virtual void refresh( unsigned int filterBottom, unsigned int filterTop, const QList<tempInfo> &temps );

    /// Translate text.
    virtual void translate();

protected:

    unsigned int filterBottom_;                     ///< Graph filter bottom.
    unsigned int filterTop_;                        ///< Graph filter top.

    profileInfo tempProfile_;                       ///< Temp profile.
    QList<tempInfo> temps_;                         ///< Temperature points.

    // ========================================================================
    // Events
    // ========================================================================

    /// Paint event.
    /**
     * @param[in,out] e  event
     */
    virtual void paintEvent( QPaintEvent *e );

private:

    bool renderGraphLines_;

    bool useCelsius_;

    /// Convert temperature.
    qreal convertTemp( qreal temp ) const;

    /// Temperature unit.
    QString tempUnit() const;

    /// Compute bounding coordinates.
    void computeTempBounding( int &bottom, int &top ) const;

    /// Check if temp profile is valid.
    bool isTempProfileValid() const;

    /// Convert temp to integter.
    static int toInt( qreal temp );

    /// Retrieve the UTC offset in seconds.
    static int utcOffset();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TEMPERATUREGRAPH
