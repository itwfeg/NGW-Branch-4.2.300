/**
 * @file coolrec.h
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#include "common.h"
#include "coolrec.h"
#include "dbutil.h"
#include "stringsjson.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QSettings>
#include <QSqlField>
#include <QSqlTableModel>

#if defined(Q_OS_LINUX)
static const QString INTERNAL_CONFIG( "/usr/share/walmart-apphook/hobart_internal.config" );
#elif defined(Q_OS_WIN32)
static const QString INTERNAL_CONFIG( "hobart_internal.config" );
#endif

QMutex coolRecord::m_( QMutex::Recursive );
coolRecord::stringMap coolRecord::colptxsMapping_;
coolRecord::stringMap coolRecord::colctrsMapping_;
coolRecord::stringMap coolRecord::colstpsMapping_;
coolRecord::stringMap coolRecord::colcatsMapping_;
coolRecord::stringMap coolRecord::coltxtsMapping_;

coolRecord::typeMap coolRecord::tableNames_;
coolRecord::typeMap coolRecord::checkFieldNames_;
coolRecord::typeMap coolRecord::primaryKeyNames_;

///////////////////////////////////////////////////////////////////////////////////////////////////
coolRecord::coolRecord( type t, const QJsonObject& obj, int dept, QObject *parent, QSqlDatabase db ) :
    _Mybase( parent ),
    db_( db ),
    dept_( dept ),
    t_( t ),
    obj_( obj ),
    num_( 0 )
{
    QMutexLocker guard( &m_ );

    // load mappings
    if ( !colcatsMapping_.size() )
    {
        loadMapping( INTERNAL_CONFIG, "colptxs", colptxsMapping_ );
        loadMapping( INTERNAL_CONFIG, "colctrs", colctrsMapping_ );
        loadMapping( INTERNAL_CONFIG, "colstps", colstpsMapping_ );
        loadMapping( INTERNAL_CONFIG, "colcats", colcatsMapping_ );
        loadMapping( INTERNAL_CONFIG, "coltxts", coltxtsMapping_ );

        tableNames_[PreText] = "COLPTxs";
        tableNames_[Country] = "COLCtrs";
        tableNames_[ProductionStep] = "COLStps";
        tableNames_[Category] = "COLCats";
        tableNames_[Text] = "COLTxts";

        checkFieldNames_[PreText] = "LPCheck";
        checkFieldNames_[Country] = "LUCheck";
        checkFieldNames_[ProductionStep] = "LSCheck";
        checkFieldNames_[Category] = "LCCheck";
        checkFieldNames_[Text] = "LXCheck";

        primaryKeyNames_[PreText] = colptxsMapping_.key( "LPNum" );
        primaryKeyNames_[Country] = colctrsMapping_.key( "LUID" );
        primaryKeyNames_[ProductionStep] = colstpsMapping_.key( "LSID" );
        primaryKeyNames_[Category] = colcatsMapping_.key( "LCNum" );
        primaryKeyNames_[Text] = coltxtsMapping_.key( "LXNum" );
    }

    // validate table name
    if ( tableNames_.contains( t ) )
    {
        primaryKey_ = primaryKeyNames_[t];

        // parse record number
        if (( obj.contains( primaryKey_ ) ) && ( obj[primaryKey_].isDouble() ))
            num_ = obj[primaryKey_].toInt();

        LOG_TRACE << "dept " << dept_ << " type " << t_ << " num " << num_;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
coolRecord::~coolRecord()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool coolRecord::insertUpdate()
{
    stringMap mapping;

    if ( PreText == t_ )
        mapping = colptxsMapping_;
    else if ( Country == t_ )
        mapping = colctrsMapping_;
    else if ( ProductionStep == t_ )
        mapping = colstpsMapping_;
    else if ( Category == t_ )
        mapping = colcatsMapping_;
    else if ( Text == t_ )
        mapping = coltxtsMapping_;
    else
    {
        LOG_WARN << "bad cool record type " << t_;
        return false;
    }

    QSqlTableModel tableModel( nullptr, db_ );
    tableModel.setTable( tableName() );
    tableModel.setEditStrategy( QSqlTableModel::OnManualSubmit );

    const QString filter( mapping[primaryKey_] + "=" + QString::number( num_ ) + " and DeptNum=" + QString::number( dept_ ) );

    // check for cool record
    QSqlRecord rec;

    bool recExists( false );

    if ( dbutil::selectRecord( tableModel, filter, rec ) )
    {
        LOG_DEBUG << "cool " << t_ << " " << num_ << " exists ";
        recExists = true;
    }
    else
    {
        LOG_DEBUG << "cool " << t_ << " " << num_ << " does not exist ";
        rec = tableModel.record();

        // set required fields
        dbutil::updateField( rec, "DeptNum", QString::number( dept_ ), true );
    }

    // populate record
    foreach ( const QString& key, obj_.keys() )
    {
        // lookup field mapping
        const stringMap::iterator iter( mapping.find( key ) );

        if ( mapping.end() != iter )
        {
            const QString mappedFieldName( iter.value() );

            if ( mappedFieldName.isEmpty() )
                continue;

            bool useLatin1( false );

            const QString value( dbutil::convertToString( obj_[key], useLatin1 ) );

#if defined( QT_DEBUG )
            LOG_TRACE << "Updating " << qPrintable( tableName() ) << " field " << qPrintable( mappedFieldName ) <<
                " from JsonObject field " << qPrintable( key ) << ", value = " << qPrintable( value );
#endif

            dbutil::updateField( rec, mappedFieldName, value, false, useLatin1 );
        }
    }

    // set check digit
    dbutil::updateField( rec, checkFieldName(), "1" );

    // save!
    if ( dbutil::saveRecordToDatabase( tableModel, tableName(), rec, recExists ) )
    {
        LOG_DEBUG << "saved cool " << t_ << " " << num_;

        if (( obj_.contains( JSON_COOL_CAT_PRE_TEXT ) ) && ( obj_[JSON_COOL_CAT_PRE_TEXT].isArray() ))
            if ( !createPreText( obj_[JSON_COOL_CAT_PRE_TEXT].toArray() ) )
            {
                LOG_WARN << "failed to save cool pre-text records";
                return false;
            }

        if (( obj_.contains( JSON_COOL_CAT_COUNTRY ) ) && ( obj_[JSON_COOL_CAT_COUNTRY].isArray() ))
            if ( !createCountries( obj_[JSON_COOL_CAT_COUNTRY].toArray() ) )
            {
                LOG_WARN << "failed to save cool country records";
                return false;
            }

        if (( obj_.contains( JSON_COOL_CAT_PROD_STEP ) ) && ( obj_[JSON_COOL_CAT_PROD_STEP].isArray() ))
            if ( !createProdSteps( obj_[JSON_COOL_CAT_PROD_STEP].toArray() ) )
            {
                LOG_WARN << "failed to save cool product step records";
                return false;
            }

        return true;
    }

    LOG_WARN << "failed to save cool " << t_ << " " << num_;
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool coolRecord::createCountries( const QJsonArray& a )
{
    return createCatLinkedRecords( "COLCaCs", "LUID", "L2Check", a );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool coolRecord::createPreText( const QJsonArray& a )
{
    return createCatLinkedRecords( "COLCaPs", "LPNum", "L3Check", a );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool coolRecord::createProdSteps( const QJsonArray& a )
{
    return createCatLinkedRecords( "COLCaSs", "LSID", "L1Check", a );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool coolRecord::createCatLinkedRecords( const QString& table, const QString& key, const QString& check, const QJsonArray& a )
{
    QSqlTableModel tableModel( nullptr, db_ );
    tableModel.setTable( table );
    tableModel.setEditStrategy( QSqlTableModel::OnManualSubmit );

    const QString deleteFilter( "LCNum=" + QString::number( num_ ) + " and DeptNum=" + QString::number( dept_ ) );

    // remove old records
    dbutil::deleteRecords( tableModel, deleteFilter );

    // add new records
    foreach ( const QJsonValue& v, a )
        if ( v.isDouble() )
        {
            const int linkedNum( v.toInt() );

            const QString filter(
                key + "=" + QString::number( linkedNum ) +
                    " and LCNum=" + QString::number( num_ ) +
                    " and DeptNum=" + QString::number( dept_ ) );

            // check for record
            QSqlRecord rec;

            if ( dbutil::selectRecord( tableModel, filter, rec ) )
                LOG_TRACE << "record exists " << num_ << " " << linkedNum;
            else
            {
                LOG_DEBUG << "record does not exist " << num_ << " " << linkedNum;
                rec = tableModel.record();

                // set required fields
                dbutil::updateField( rec, key, QString::number( linkedNum ), true );
                dbutil::updateField( rec, "LCNum", QString::number( num_ ), true );
                dbutil::updateField( rec, "DeptNum", QString::number( dept_ ), true );
                dbutil::updateField( rec, check, "1", true );

                // save!
                if ( dbutil::saveRecordToDatabase( tableModel, tableModel.tableName(), rec, false ) )
                    LOG_TRACE << "created record " << num_ << " " << linkedNum;
                else
                {
                    LOG_WARN << "failed to created record " << num_ << " " << linkedNum;
                    return false;
                }
            }
        }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void coolRecord::loadMapping( const QString& filename, const QString& keyBase, QMap<QString, QString>& mapping )
{
    QSettings configs( filename, QSettings::IniFormat );

    // clear existing mapping
    mapping.clear();

    // load in mappings
    for ( int i( 0 ); i < 1000; ++i )
    {
        const QString key( keyBase + QString( "%0" ).arg( i, 3, 10, QChar( '0' ) ) );
        const QString value( configs.value( key ).toString() );

        if ( value.isEmpty() )
            break;

        // split string into key:value pair
        else if ( 2 <= value.length() )
        {
            const int split( value.indexOf( ':' ) );

            if ( 0 < split )
                mapping[value.left( split )] = value.mid( split+1 );
        }
    }
}

