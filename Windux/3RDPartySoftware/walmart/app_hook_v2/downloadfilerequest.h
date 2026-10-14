/**
 * @file downloadfilerequest.h
 * Download file request.
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DOWNLOADFILEREQUEST_H
#define DOWNLOADFILEREQUEST_H

#include "abstractrequest.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Download file request.
class downloadFileRequest : public abstractRequest
{
    Q_OBJECT

    typedef downloadFileRequest _Myt;
    typedef abstractRequest _Mybase;

public:

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in] filename  filename of file
     * @param[in,out] parent  parent object
     */
    downloadFileRequest( const QString& filename, QObject *parent = nullptr );

    /// Constructor.
    /**
     * @param[in] filename  filename of file
     * @param[in] requestUuid  request uuid
     * @param[in] parent  parent object
     */
    downloadFileRequest( const QString& filename, const QString& requestUuid, QObject *parent = nullptr );

    /// Destructor.
    virtual ~downloadFileRequest();

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Retrieve filename.
    /**
     * @return  filename
     */
    virtual QString filename() const {return filename_;}

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Convert to @c QJsonDocument.
    /**
     * @return  json object
     */
    virtual QJsonDocument toJsonDoc() const;

protected:

    QString filename_;                              ///< Filename of file

private:

    // not implemented
    downloadFileRequest( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DOWNLOADFILEREQUEST_H
