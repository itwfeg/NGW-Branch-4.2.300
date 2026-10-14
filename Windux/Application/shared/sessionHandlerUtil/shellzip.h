/**
  * @file shellzip.h
  * Utility class for creating a zip using windows shell.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Randy Blankley
  */

#ifndef SHELLZIP_H
#define SHELLZIP_H

#include <qglobal.h>

// this class will work only under windows
#ifdef Q_OS_WIN

#include <QObject>

class QString;
class QStringList;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class shellZip
 * @brief Utility class for creating a zip using windows shell.
 */
class shellZip : public QObject
{
    Q_OBJECT

    typedef shellZip _Myt;
    typedef QObject _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR/DTOR

    /**
     * Constructor.
     *
     * @param[in] filename  zip file name
     * @param[in] parent  parent object
     */
    explicit shellZip( const QString& filename, QObject *parent = NULL );

    /**
     * Destructor.
     */
    virtual ~shellZip();

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Adds a file to the zip.
     *
     * @param[in] file  file to add
     * @returns  @c true if file added successfully, @c false otherwise
     */
    virtual bool add( const QStringList& list );

private:

    QString filename_;
    bool valid_;

    /**
     * Create zip file.
     */
    static bool createZipFile( const QString& filename );

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

#endif // Q_OS_WIN

#endif // SHELLZIP_H
