/**
 * @file scaleusagetoday.h
 * Scale product usage today.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SCALEUSAGETODAY_H
#define SCALEUSAGETODAY_H

#include <QDateTime>
#include <QList>
#include <QMutex>
#include <QObject>
#include <QJsonObject>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Scale product usage today.
class scaleUsageToday : public QObject
{
    Q_OBJECT

    typedef scaleUsageToday _Myt;
    typedef QObject _Mybase;

public:

    /// List of integer values.
    typedef QList<int> integerList;

    /// Print types.
    enum PrintType
    {
        ManualPrint,                                ///< Manual product print.
        RemotePrint                                 ///< Remote product print.
    };

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Retrieve cake labels.
    /**
     * @return  list of cake labels
     */
    integerList cakeLabels() const;

    /// Set cake labels.
    /**
     * @param[in] value  list of cake labels
     */
    void setCakeLabels( const integerList& value );

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Update product usage.
    /**
     * @param[in] plu  product
     * @param[in] numLabels  number of labels printed
     * @param[in] type  print type
     * @param[in] isCakeLabel  @c true if cake label, @c false otherwise
     */
    void updateUsage( int plu, int numLabels, PrintType type, bool isCakeLabel = false );

    /// Convert to @c QJsonObject.
    /**
     * @return  json object
     */
    QJsonObject toJsonObject() const;

    // ------------------------------------------------------------------------
    // Static Methods
    // ------------------------------------------------------------------------

    /// Retrieve global instance.
    /**
     * @return  pointer to instance
     */
    static _Myt *instance();

private:

    static QMutex instanceMutex_;
    static _Myt *instance_;

    mutable QMutex m_;

    QDateTime stamp_;
    QJsonObject usageToday_;

    integerList cakeLabels_;

    /// Constructor.
    scaleUsageToday( QObject *parent = nullptr );

    /// Destructor.
    ~scaleUsageToday();

    /// Import last usage.
    bool importLastUsage();

    /// Export last usage.
    bool exportLastUsage();

    /// Empty usage object.
    static QJsonObject emptyUsage();

    // not implemented
    scaleUsageToday( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SCALEUSAGETODAY_H
