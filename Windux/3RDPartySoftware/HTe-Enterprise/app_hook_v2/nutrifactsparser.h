/**
 * @file nutrifactsparser.h
 * Nutrition facts data parser.
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef NUTRIFACTSPARSER_H
#define NUTRIFACTSPARSER_H

#include <QMap>
#include <QMutex>
#include <QString>

class QJsonObject;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Nutrition facts data parser.
class nutritionFactsParser
{
    typedef nutritionFactsParser _Myt;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] obj  nutrition fact data
     */
    nutritionFactsParser( const QJsonObject& obj );

    /// Deconstructor.
    virtual ~nutritionFactsParser();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve nutrifacts string.
    /**
     * @return  nutrifacts
     */
    virtual QString nutrifacts() const {return nutrifacts_;}

protected:

    /// Map of string to string.
    typedef QMap<QString, QString> stringMap;

    static QMutex fieldMappingMutex_;               ///< Mutex to control field mapping
    static stringMap fieldMapping_;                 ///< Mapping of nutrifact fields

    QString nutrifacts_;                            ///< Nutrifacts string.

    // ========================================================================
    // Methods
    // ========================================================================

    /// Parse nutrition data into byte array.
    /**
     * @param[in] obj  nutrition data
     */
    virtual void parseData( const QJsonObject& obj );

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Load data mapping from file.
    /**
     * @param[in] filename  ini file
     * @param[in] keyBase  data value key base
     * @param[out] mapping  data mapping
     */
    static void loadMapping( const QString& filename, const QString& keyBase, QMap<QString, QString>& mapping );

private:

    // not implemented
    nutritionFactsParser() = delete;

    // not implemented
    nutritionFactsParser( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // NUTRIFACTSPARSER_H
