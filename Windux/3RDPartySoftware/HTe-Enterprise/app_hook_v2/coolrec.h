/**
 * @file coolrec.h
 * Cool record support.
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef COOLREC_H
#define COOLREC_H

#include <QMap>
#include <QMutex>
#include <QObject>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QString>

class QSqlTableModel;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Cool record support.
class coolRecord : public QObject
{
    typedef coolRecord _Myt;
    typedef QObject _Mybase;

public:

    enum type
    {
        PreText,
        Country,
        ProductionStep,
        Category,
        Text
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] t  cool type
     * @param[in] obj  cool data
     * @param[in] dept  cool department
     * @param[in] parent  parent object
     * @param[in] db  database
     */
    coolRecord( type t, const QJsonObject& obj, int dept = 0, QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase() );

    /// Deconstructor.
    virtual ~coolRecord();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check of valid.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool isValid() const {return (0 < num_);}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Insert/Update record.
    /**
     * @return  @c true if successful, @c false otherwise
     */
    virtual bool insertUpdate();

protected:

    QSqlDatabase db_;                               ///< Database.

    int dept_;                                      ///< COOL department.

    type t_;                                        ///< COOL record type.
    QJsonObject obj_;                               ///< COOL object.

    int num_;                                       ///< COOL num.

    QString primaryKey_;                            ///< Primary key.

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve table name.
    /**
     * @return  table name
     */
    virtual QString tableName() const {return tableNames_[t_];}

    /// Retrieve check field name.
    /**
     * @return  check field name
     */
    virtual QString checkFieldName() const {return checkFieldNames_[t_];}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Create country records.
    /**
     * @param[in] a  records
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool createCountries( const QJsonArray& a );

    /// Create pre-text records.
    /**
     * @param[in] a  records
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool createPreText( const QJsonArray& a );

    /// Create production step records.
    /**
     * @param[in] a  records
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool createProdSteps( const QJsonArray& a );

private:

    /// Map of string to string.
    typedef QMap<QString, QString> stringMap;

    /// Map of cool type to string.
    typedef QMap<type, QString> typeMap;

    static QMutex m_;                               ///< Mutex to control field mapping
    static stringMap colptxsMapping_;               ///< Mapping of COOL Pre-Text fields
    static stringMap colctrsMapping_;               ///< Mapping of COOL Countries fields
    static stringMap colstpsMapping_;               ///< Mapping of COOL Production Steps fields
    static stringMap colcatsMapping_;               ///< Mapping of COOL Categories fields
    static stringMap coltxtsMapping_;               ///< Mapping of COOL Text fields

    static typeMap tableNames_;                     ///< Table names.
    static typeMap checkFieldNames_;                ///< Check field names.
    static typeMap primaryKeyNames_;                ///< Id (key) field names.

    /// Create category linked record.
    bool createCatLinkedRecords( const QString& table, const QString& key, const QString& check, const QJsonArray& a );

    /// Load data mapping from file.
    static void loadMapping( const QString& filename, const QString& keyBase, QMap<QString, QString>& mapping );

    // not implemented
    coolRecord() = delete;

    // not implemented
    coolRecord( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // COOLREC_H
