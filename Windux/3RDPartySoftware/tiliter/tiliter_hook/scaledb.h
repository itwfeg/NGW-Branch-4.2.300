/**
 * @file scaledb.h
 * Scale database management.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SCALEDB_H
#define SCALEDB_H

#include <QObject>

class QJsonArray;
class QJsonObject;
class QSqlDatabase;
class QSqlRecord;
class QSqlTableModel;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Scale database management.
class scaleDatabase : public QObject
{
    Q_OBJECT

    typedef scaleDatabase _Myt;
    typedef QObject _Mybase;

public:

    /// Label name and version pair.
    typedef QPair<QString, QString> labelNameVersionPair;

    /// Mapping of label versions [labelno, labelnameversion]
    typedef QMap<int, labelNameVersionPair> labelVersionMap;

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in] name  database name
     * @param[in] parent  parent object
     */
    scaleDatabase( const QString& name, QObject *parent = nullptr );

    /// Destructor.
    virtual ~scaleDatabase();

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Check of valid.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool isValid() const;

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Retrieve config value from database.
    /**
     * @param[in] configName  config name
     * @return  config value
     */
    virtual QString queryConfigs( const QString& configName ) const;

protected:

    QString dbname_;                                ///< Database name.

    QSqlDatabase *scaleDatabase_;                   ///< Scale database.

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Close database.
    virtual void closeDatabase();

    /// Open database.
    /**
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool openDatabase();

    /// Open database connection for thread.
    /**
     * @return  database connection
     */
    virtual QSqlDatabase openDatabaseConnection() const;

private:

    // not implemented
    scaleDatabase() = delete;

    // not implemented
    scaleDatabase( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SCALEDB_H
