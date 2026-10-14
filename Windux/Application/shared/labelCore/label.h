/**
 * @file label.h
 * Label object.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
* @addtogroup labelViewerCsu
 * @{
 */

#ifndef LABEL_H
#define LABEL_H

#include "dataobject.h"
#include "tagdatarecord.h"

#include "labelform.h"
#include "encoding.h"

#include <QList>

DECLARE_DATA_OBJECT( label, dataObjectType::LABEL );                            ///< object type
DECLARE_TAG_DATA_RECORD_TYPE( label, tagDataCommand::READ_ALL, 'RT71' );        ///< tag data read all command
DECLARE_TAG_DATA_RECORD_TYPE( label, tagDataCommand::WRITE, 'RT9C' );           ///< tag data write command
DECLARE_TAG_DATA_RECORD_TYPE( label, tagDataCommand::DELETE_ONE, 'RTA7' );      ///< tag data delete one command
DECLARE_TAG_DATA_RECORD_TYPE( label, tagDataCommand::DELETE_ALL, 'RT46' );      ///< tag data delete all command

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class label
 * @brief Label object.
 *
 * A label data stream is the sequence of bytes that make up a given label. This stream contains
 * the bytes of each of the label components, as described in the following sections, and in the
 * same order. This data stream follows the big-ending byte ordering style.
 *
 * Here is the layout of a stream:
 * @code
 *                          +---------------------------+
 *                          | Header                    |
 *                          +---------------------------+
 *                 /        | Label stock descriptor    |
 *                |         +---------------------------+
 *                |         | Pointer to form 1         |  -----+
 *  Form list    <          +---------------------------+       |
 *                |         | Pointer to form 2         |  -----+---+
 *                |         +---------------------------+       |   |
 *                 \        | ...                       |       |   |
 *                          +===========================+       |   |
 *                 /        | Form 1                    |  <----+   |
 *  Form          |         +---------------------------+           |
 *  definitions  <          | Form 1                    |  <--------+
 *                |         +---------------------------+
 *                 \        | ...                       |
 *                          +===========================+
 *                          | Constant data             |
 *                          +---------------------------+
 * @endcode
 *
 * The header is a data structure containing the following elements:
 * @code
 *                  Length
 *   Name           (Bytes) Description                                 Values
 *   ============== ======= =========================================== ===========================
 *   Major Version  1       Major version of this specification         1
 *   Minor Version  1       Minor version of this specification         0
 *   Type           4       Differentiates this resource from other     "LABL"
 *                          resources such as Fonts or Graphics.
 *   Id             2       Numeric identifier of this Label            0-65535 (the actual maximum is scale dependent)
 *   Loaded         1       Indicates if data follows the header        0=False 1=True (always use this)
 *   Padding        1       Ignored
 *   Length         2       Length of the label data (including this    0-65535
 *                          header) in bytes (unnibblized).
 *   Name           32      Name Identifier of this Label.              0-32 characters
 * @endcode
 *
 * The label stock descriptor is also a data structure containing the following elements:
 * @code
 *                  Length
 *   Name           (Bytes) Description                                 Values
 *   ============== ======= =========================================== ===========================
 *   Size.h         2       Horizontal size (across the direction of    0-65535
 *                          the stock)
 *   Size.v         2       Vertical size (along the direction of the   0-65535
 *                          stock)
 *   Orientation    2       How the Label is rendered on the Label      0=portrait labels 90=landscape labels
 *                          Stock (rotation).
 *   Skew           2       Adjusts alignment with print head. Used to  0-99 (usually set to 0)
 *                          shift right the whole label image in order
 *                          to center a label within a wider paper.
 * @endcode
 *
 * The form list contains a list of offsets that point to each Form. The last offset is all Nulls to
 * denote the end of the list. Each offset points to a Form (described below):
 * @code
 *                  Length
 *   Name           (Bytes) Description                                 Values
 *   ============== ======= =========================================== ===========================
 *   Form1Offset    4       Offset from beginning of Label to Form1     51-65535
 *   Form2Offset    4       Offset from beginning of Label to Form2     51-65535
 *   Form3Offset    4       Offset from beginning of Label to Form3     51-65535
 *   ...
 *   Terminator     4       Denotes end of Form List                    0
 * @endcode
 *
 * @sa labelForm
 */
class label : public dataObject< dataObjectType::LABEL, int >
{
    typedef label _Myt;
    typedef dataObject< dataObjectType::LABEL, int > _Mybase;

public:

    /**
     * List of @c labelForm objects.
     */
    typedef QList<labelForm> labelFormList;

    /**
     * Key list type.
     */
    typedef QList<int> key_list_type;

    /**
     * List type.
     */
    typedef QList<_Myt> list_type;

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     */
    label();

    /**
     * Constructor.
     *
     * @param[in] id  label id
     */
    label( int id );

    /**
     * Constructor.
     *
     * This version of the constructor creates a @c label object from a base16 encoded string.
     *
     * @param[in] data  object encoded in base16
     */
    label( const QString& data, base16_encoding_t );

    /**
     * Constructor.
     *
     * This version of the constructor creates a @c label object from a base64 encoded string.
     *
     * @param[in] data  object encoded in base64
     */
    label( const QString& data, base64_encoding_t );

    /**
     * Constructor.
     *
     * @param[in,out] is  input stream to extract data from
     */
    label( QDataStream& is );

    /**
     * Constructor.
     *
     * @param[in] rhs  object to copy
     */
    label( const _Myt& rhs );

    /**
     * Destructor
     */
    virtual ~label();

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
    virtual int objectKey() const {return id();}

    /**
     * Retrieve label major version number.
     *
     * @returns  major version number
     */
    virtual int versionMajor() const {return versionMajor_;}

    /**
     * Retrieve label minor version number.
     *
     * @returns  minor version number
     */
    virtual int versionMinor() const {return versionMinor_;}

    /**
     * Set label version.
     *
     * @param[in] major  major version number
     * @param[in] minor  minor version number
     */
    virtual void setVersion( int major, int minor ) {versionMajor_ = major; versionMinor_ = minor;}

    /**
     * Retrieve label id.
     *
     * @returns  id
     */
    virtual int id() const {return id_;}

    /**
     * Set label id.
     *
     * @param[in] value  label id
     */
    virtual void setId( int value ) {id_ = value;}

    /**
     * Retrieve label name.
     *
     * @returns  name
     */
    virtual QString name() const {return name_;}

    /**
     * Set label name.
     *
     * @param[in] value  label name
     */
    virtual void setName( const QString& value ) {name_ = value;}

    /**
     * Check if label loaded.
     *
     * @returns  @c true or @c false
     */
    virtual bool loaded() const {return loaded_;}

    /**
     * Set label loaded.
     *
     * @param[in] value  @c true or @c false
     */
    virtual void setLoaded( bool value ) {loaded_ = value;}

    /**
     * Retrieve label size.
     *
     * @returns  size
     */
    virtual QSize size() const {return size_;}

    /**
     * Set label size.
     *
     * @param[in] value  label size
     */
    virtual void setSize( const QSize& value ) {size_ = value;}

    /**
     * Retrieve label physical size.  Physical size takes into account label orientation such that
     * a landscape label will have dimensions swapped.
     *
     * @returns  physical size
     */
    virtual QSize physicalSize() const;

    /**
     * Retrieve label rotation.
     *
     * @returns  rotation in range [0,360)
     */
    virtual int rotation() const {return rotation_;}

    /**
     * Set label rotation in degrees.
     *
     * @note
     * Currently only the cardinals of 0,90 are supported.
     *
     * @param[in] value  rotation in range [0,360)
     */
    virtual void setRotation( int value ) {rotation_ = value;}

    /**
     * Retrieve label skew.
     *
     * Skew adjusts alignment with print head. It is used to shift right the whole label image in
     * order to center a label within a wider paper.
     *
     * @returns  skew in pixels
     */
    virtual int skew() const {return skew_;}

    /**
     * Set label skew
     *
     * Skew adjusts alignment with print head. It is used to shift right the whole label image in
     * order to center a label within a wider paper.
     *
     * @param[in] value  skew in pixels
     */
    virtual void setSkew( int value ) {skew_ = value;}

    /**
     * Retrieve label form data.
     *
     * @returns  list of forms
     */
    virtual const labelFormList forms() const {return forms_;}

    /**
     * Retrieve label form data.
     *
     * @returns  list of forms
     */
    virtual labelFormList& forms() {return forms_;}

    /**
     * Set label form data.
     *
     * @param[in] value  list of forms
     */
    virtual void setForms( const labelFormList& value ) {forms_ = value;}

    /**
     * @brief Retrieve the Version of a label
     * @return QString of a customer created label's version
     */
    virtual QString customerVersion() const;



    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Retrieve label as base16 encoded string.
     *
     * @returns  encoded image string
     */
    virtual QString serialize( base16_encoding_t ) const;

    /**
     * Retrieve label as base64 encoded string.
     *
     * @returns  encoded image string
     */
    virtual QString serialize( base64_encoding_t ) const;

    /**
     * Serialize object.
     *
     * @param[in,out] os  output stream to insert data into
     */
    virtual void serialize( QDataStream& os ) const;

    /**
     * Set label from base16 encoded string.
     *
     * @param[in] data  string encoded image
     */
    virtual void parse( const QString& data, base16_encoding_t );

    /**
     * Set label from base64 encoded string.
     *
     * @param[in] data  string encoded image
     */
    virtual void parse( const QString& data, base64_encoding_t );
    /**
     * Parse object.
     *
     * @param[in,out] is  input stream to extract data from
     */
    virtual void parse( QDataStream& is );

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

protected:

    /**
     * Default size of a label.
     */
    static const QSize defaultSize_;

private:

    int versionMajor_;
    int versionMinor_;

    int id_;
    QString name_;
    QString customerVersion_;

    bool loaded_;

    QSize size_;
    int rotation_;
    int skew_;

    labelFormList forms_;

    /**
     * Copy object.
     */
    void copy( const _Myt& rhs );

};

/**
 * Serialize object.
 *
 * @param[out] os  output stream to insert data into
 * @param[in] data  @c label object to serialize
 * @returns  reference to @p os
 */
inline QDataStream& operator << ( QDataStream& os, const label& data ) {
    data.serialize( os );
    return os;
}

/**
 * Parse object.
 *
 * @param[in,out] is  input stream to extract data from
 * @param[out] data  @c label object to extract
 * @returns  reference to @p is
 */
inline QDataStream& operator >> ( QDataStream& is, label& data ) {
    data.parse( is );
    return is;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // LABEL_H

