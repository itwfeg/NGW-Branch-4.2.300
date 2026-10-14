/**
 * @file labelform.h
 * Label form classes.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
* @addtogroup labelViewerCsu
 * @{
 */

#ifndef LABELFORM_H
#define LABELFORM_H

#include <QString>
#include <QRect>

#include "labelformtype.h"
#include "labelformflags.h"

class QDataStream;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class labelForm
 * @brief Label form object.
 *
 * This class represents a form that resides on a label.  A label is composed of many forms.
 *
 * This structure is repeated once for each Form instance.  There should be one for each non-NULL
 * entry in the Form List:
 * @code
 *                  Length
 *   Name           (Bytes) Description                                 Values
 *   ============== ======= =========================================== ===========================
 *   Origin.h       2       Horizontal start                            0-65535
 *   Origin.v       2       Vertical start                              0-65535
 *   Size.h         2       Horizontal size                             0-65535
 *   Size.v         2       Vertical size                               0-65535
 *   Flags          2       Bitwise combination of flags                labelFormFlags value
 *   Form Type      1       The type of the Form, that is, the type of  labelFormType value
 *                          data to print in the form (i.e., Total
 *                          Price or Description).
 *   H. Justific.   1       Specifies that this Form's contents should  0=Left 1=Center 2=Right
 *                          be Left, Right, or Center justified
 *   V. Justific.   2       Specifies the number of dots to shift down  0-65535
 *   or Form Num.           this Form's contents. Also the number
 *                          associated with a Label Reference Form.
 *   Form Subtype   1       Provides extra information for certain      0-255
 *                          form types.
 *   Padding        1       Ignored
 *   Rotation       2       Specifies that this Form should be rotated  0, 90, 180, or 270 degrees
 *                          relative to the Label.
 *   Font Family    40      Specifies the family name of the default    text
 *   Name or                font to be used to render text in this
 *   Graphic Name           form. Also the name associated with a       text
 *                          Graphic Reference Form. Family name of any
 *                          true type or open type font in the scale.
 *   Font Point     2       Specifies the point size of the default     0-65535
 *   Size                   font to be used to render text in this
 *                          form.
 *   Tag            6       A tag to associate a Nutrition Data Element text
 *                          with this form
 *   Data           4       Pointer to a text string.                   0-4294967295
 * @endcode
 */
class labelForm
{
    typedef labelForm _Myt;

public:

    /**
     * How large a @c labelForm object is when serialized.
     */
    enum {BINARY_IMAGE_SIZE = 70};

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     */
    labelForm();

    /**
     * Constructor.
     *
     * @param[in] type  form type
     */
    labelForm( labelFormType::type type );

    /**
     * Constructor.
     *
     * @param[in,out] is  input stream to extract data from
     */
    labelForm( QDataStream& is );

    /**
     * Constructor.
     *
     * @param[in] rhs  object to copy
     */
    labelForm( const _Myt& rhs );

    /**
     * Destructor
     */
    virtual ~labelForm();

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
    ///////////////////////////////////////////////////////////////////////////

    /**
     * Retrieve form bounding rect.
     *
     * @returns  bounding rect
     */
    virtual QRect rect() const {return rect_;}

    /**
     * Set form bounding rect.
     *
     * @param[in] value  bounding rect
     */
    virtual void setRect( const QRect& value ) {rect_ = value;}

    /**
     * Retrieve default label form size.
     *
     * @returns  size
     */
    static QSize defaultSize() {return defaultSize_;}

    /**
     * Retrieve form type.
     *
     * @returns  form type
     */
    virtual labelFormType::type type() const {return type_;}

    /**
     * Set form type.
     *
     * @param[in] type  form type
     * @param[in] subType  form sub type
     */
    virtual void setType( labelFormType::type type, int subType = 0 ) {type_ = type; subType_ = subType;}

    /**
     * Retrieve form type in text format.
     *
     * @returns  type in text format
     */
    virtual QString typeText() const;

    /**
     * Retrieve form flags.
     *
     * @returns  flags
     */
    virtual labelFormFlags::type flags() const {return flags_;}

    /**
     * Set form flags.
     *
     * @param[in] value  form flags
     */
    virtual void setFlags( labelFormFlags::type value ) {flags_ = value;}

    /**
     * Retrieve horizontal alignment.
     *
     * @returns  horizontal alignment
     */
    virtual Qt::AlignmentFlag alignment() const {return alignment_;}

    /**
     * Set horizontal alignment.
     *
     * @param[in] value  horizontal alignment
     */
    virtual void setAlignment( Qt::AlignmentFlag value ) {alignment_ = value;}

    /**
     * Retrieve vertical justification (pixel offset).
     *
     * @returns  vertical justification in pixels
     */
    virtual int verticalJustification() const {return verticalJustification_;}

    /**
     * Set vertical justification (pixel offset).
     *
     * @param[in] value  vertical justification in pixels
     */
    virtual void setVerticalJustification( int value ) {verticalJustification_ = value;}

    /**
     * Retrieve form sub type.
     *
     * @note
     * Sub types are specific to each type and will vary greatly.
     *
     * @returns  sub type
     */
    virtual int subType() const {return subType_;}

    /**
     * Set form sub type.
     *
     * @note
     * Sub types are specific to each type and will vary greatly.
     *
     * @param[in] value  sub type
     */
    virtual void setSubType( int value ) {subType_ = value;}

    /**
     * Retrieve form rotation.
     *
     * @returns  rotation in range [0,360)
     */
    virtual int rotation() const {return rotation_;}

    /**
     * Set form rotation in degrees.
     *
     * @note
     * Currently only the cardinals of 0,90,180,270 are supported.
     *
     * @param[in] value  rotation in range [0,360)
     */
    virtual void setRotation( int value ) {rotation_ = value;}

    /**
     * Retrieve font family name.
     *
     * @returns  family name
     */
    virtual QString fontFamilyName() const {return fontFamilyName_;}

    /**
     * Set font family name.
     *
     * @param[in] value  family name
     */
    virtual void setFontFamilyName( const QString& value ) {fontFamilyName_ = value;}

    /**
     * Retrieve point size of font.
     *
     * @returns  point size
     */
    virtual int fontPointSize() const {return fontPointSize_;}

    /**
     * Set font point size.
     *
     * @param[in] value  point size
     */
    virtual void setFontPointSize( int value ) {fontPointSize_ = value;}

    /**
     * Retrieve form tag.
     *
     * @returns  tag
     */
    virtual QString tag() const {return tag_;}

    /**
     * Set form tag.
     *
     * @param[in] value  tag
     */
    virtual void setTag( const QString& value ) {tag_ = value;}

    /**
     * Retrieve form data.
     *
     * @returns  data
     */
    virtual QString data() const {return data_;}

    /**
     * Set form data.
     *
     * @param[in] value  data
     */
    virtual void setData( const QString& value ) {data_ = value;}

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Serialize object.
     *
     * @param[in,out] os  output stream to insert data into
     */
    virtual void serialize( QDataStream& os ) const;

    /**
     * Parse object.
     *
     * @param[in,out] is  input stream to extract data from
     */
    virtual void parse( QDataStream& is );

protected:

    /**
     * Default size of a label form.
     */
    static const QSize defaultSize_;

private:

    QRect rect_;

    labelFormType::type type_;
    labelFormFlags::type flags_;

    Qt::AlignmentFlag alignment_;
    int verticalJustification_;

    int subType_;
    int rotation_;

    QString fontFamilyName_;
    int fontPointSize_;

    QString tag_;
    QString data_;

    /**
     * Copy object.
     */
    void copy( const _Myt& rhs );

};

/**
 * Serialize object.
 *
 * @param[in,out] os  output stream to insert data into
 * @param[in] data  @c labelForm object to serialize
 * @returns  reference to @p os
 */
inline QDataStream& operator << ( QDataStream& os, const labelForm& data ) {
    data.serialize( os );
    return os;
}

/**
 * Parse object.
 *
 * @param[in,out] is  input stream to extract data from
 * @param[out] data  @c labelForm object to extract
 * @returns  reference to @p is
 */
inline QDataStream& operator >> ( QDataStream& is, labelForm& data ) {
    data.parse( is );
    return is;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // LABELFORM_H
