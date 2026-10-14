/**
 * @file graphic.h
 * Graphic object that can be printed on a label.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
* @addtogroup labelViewerCsu
 * @{
 */

#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "dataobject.h"
#include "tagdatarecord.h"

#include "encoding.h"

#include <QString>
#include <QImage>
#include <QRect>

DECLARE_DATA_OBJECT( graphic, dataObjectType::GRAPHIC );                        ///< object type
DECLARE_TAG_DATA_RECORD_TYPE( graphic, tagDataCommand::READ_ALL, 'RT6W' );      ///< tag data read all command
DECLARE_TAG_DATA_RECORD_TYPE( graphic, tagDataCommand::WRITE, 'RT9W' );         ///< tag data write command
DECLARE_TAG_DATA_RECORD_TYPE( graphic, tagDataCommand::DELETE_ONE, 'RTAW' );    ///< tag data delete one command
DECLARE_TAG_DATA_RECORD_TYPE( graphic, tagDataCommand::DELETE_ALL, 'RT3W' );    ///< tag data delete all command

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class graphic
 * @brief Graphic object.
 */
class graphic : public dataObject< dataObjectType::GRAPHIC, QString >
{
    typedef graphic _Myt;
    typedef dataObject< dataObjectType::GRAPHIC, QString > _Mybase;

public:

    /**
     * Key list type.
     */
    typedef QList<QString> key_list_type;

    /**
     * List type.
     */
    typedef QList<_Myt> list_type;

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     */
    graphic();

    /**
     * Constructor.
     *
     * @param[in] name  graphic name
     */
    graphic( const QString& name );

    /**
     * Constructor.
     *
     * @param[in] name  graphic name
     * @param[in] image  image
     */
    graphic( const QString& name, const QImage& image );

    /**
     * Constructor.
     *
     * This version of the constructor creates a @c graphic object from a base16 encoded string.
     *
     * @param[in] name  graphic name
     * @param[in] data  object encoded in base16
     */
    graphic( const QString& name, const QString& data, base16_encoding_t );

    /**
     * Constructor.
     *
     * This version of the constructor creates a @c graphic object from a base64 encoded string.
     *
     * @param[in] name  graphic name
     * @param[in] data  object encoded in base64
     */
    graphic( const QString& name, const QString& data, base64_encoding_t );

    /**
     * Constructor.
     *
     * @param[in] rhs  object to copy
     */
    graphic( const _Myt& rhs );

    /**
     * Destructor
     */
    virtual ~graphic();

    ///////////////////////////////////////////////////////////////////////////
    // Operators

    /**
     * Assignment operator.
     *
     * @param[in] rhs  value to assign from
     * @returns  reference to @c this
     */
    _Myt& operator = ( const _Myt& rhs );

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve the object key value.
     *
     * This key value is used for data store access.
     *
     * @returns  key value
     */
    virtual QString objectKey() const;

    /**
     * Retrieve graphic name.
     *
     * @returns  graphic name
     */
    virtual QString name() const {return name_;}

    /**
     * Set graphic name.
     *
     * @param[in] value  graphic name
     */
    virtual void setName( const QString& value ) {name_ = value;}

    /**
     * Retrieve image.
     *
     * @returns  image
     */
    virtual const QImage image() const {return image_;}

    /**
     * Retrieve reference to image.
     *
     * @returns  reference to image
     */
    virtual QImage& image() {return image_;}

    /**
     * Set image.
     *
     * @param[in] value  image
     */
    virtual void setImage( const QImage& value ) {image_ = value;}

    /**
     * Retrieve image size.
     *
     * @returns  size
     */
    virtual QSize size() const {return image_.size();}

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Retrieve image as base16 encoded string.
     *
     * @returns  encoded image string
     */
    virtual QString serializeImage( base16_encoding_t ) const;

    /**
     * Retrieve image as base64 encoded string.
     *
     * @returns  encoded image string
     */
    virtual QString serializeImage( base64_encoding_t ) const;

    /**
     * Set image from base16 encoded string.
     *
     * @param[in] data  string encoded image
     */
    virtual void parseImage( const QString& data, base16_encoding_t );

    /**
     * Set image from base64 encoded string.
     *
     * @param[in] data  string encoded image
     */
    virtual void parseImage( const QString& data, base64_encoding_t );

    ///////////////////////////////////////////////////////////////////////////
    // Static Methods

    /**
     * Convert tag data into a list of object key values.
     *
     * @param[in] r  tag data
     * @returns  list of object key values
     */
    static key_list_type toKeyList( const tagDataList& r );

    /**
     * Convert tag data into list of objects.
     *
     * @param[in] r  tag data
     * @returns  list of objects
     */
    static list_type toValueList( const tagDataList& r );

    /**
     * Convert list of objects into tag data.
     *
     * @param[in] values  list of objects
     * @returns  tag data
     */
    static tagDataRecordList toTagDataRecordList( const list_type& values );

private:

    QString name_;
    QImage image_;

    /**
     * Copy object.
     */
    void copy( const _Myt& rhs );

    /**
     * Retrieve image byte array.
     */
    QByteArray imageData() const;

    /**
     * Set image from byte array.
     */
    void setImageData( const QByteArray& data );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // GRAPHIC_H
