/**
 * @file marqueetextfragment.h
 * Fragment of scrolling marquee text.
 *
 * Scrolling marquee text is made up of fragments.  When the fragments are combined you have the
 * entire scrolling text.  Each fragment may have its own properties (i.e. blinking).
 *
 * For example, below the middle fragment would have properties such that it blinks:
 * +------------------------------+---------------------+-----------------+
 * | Here is a marquee text where | some text can blink | and some cannot |
 * +------------------------------+---------------------+-----------------+
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup marqueeEditorCsu
 * @{
 */

#ifndef _MARQUEETEXTFRAGMENT_H_
#define _MARQUEETEXTFRAGMENT_H_

#include <QFontMetrics>
#include <QSize>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class marqueeTextFragment
 * @brief Text fragment of @c marqueeText.
 *
 * This class hold the fragment text and also properties of that text (i.e. if it should blink).
 *
 * To make text blink surround it with the following tags:
 * <BLINKBEGIN>text to blink<BLINKEND>
 */
class marqueeTextFragment
{
    typedef marqueeTextFragment _Myt;

public:

    /**
     * Constructor.
     */
    marqueeTextFragment() : blinks_( false ) {}

    /**
     * Constructor.
     *
     * @param[in] text  fragment text
     * @param[in] m  font metrics
     * @param[in] blinks  @c true if fragment should blink, @c false otherwise
     */
    marqueeTextFragment( const QString& text, const QFontMetrics& m, bool blinks = false ) : blinks_( blinks ) {
        setText( text, m );
    }

    /**
     * Constructor.
     *
     * @param[in] rhs  object to copy
     */
    marqueeTextFragment( const _Myt& rhs ) {copy( rhs );}

    /**
     * Destructor.
     */
    virtual ~marqueeTextFragment() {}

    /**
     * Assignment operator.
     *
     * @param[in] rhs  value to assign from
     * @returns  reference to @c this
     */
    _Myt& operator = ( const _Myt& rhs ) {
        copy( rhs );
        return *this;
    }

    /**
     * Get fragment text.
     *
     * @returns  fragment text
     */
    virtual QString text() const {return text_;}

    /**
     * Set fragment text.
     *
     * @param[in] value  fragment text
     * @param[in] m  font metrics
     */
    virtual void setText( const QString& value, const QFontMetrics& m ) {
        text_ = value;
        textSize_ = m.size( Qt::TextSingleLine, text_ );
    }

    /**
     * Retrieve size of text in pixels.
     *
     * @returns  size of text in pixels
     */
    virtual QSize textSize() const {return textSize_;}

    /**
     * Get fragment blink property.
     *
     * @returns  @c true if text fragment blinks, @c false otherwise
     */
    virtual bool blinks() const {return blinks_;}

    /**
     * Set fragment blink property.
     *
     * @param[in] value  @c true if text fragment should blink, @c false otherwise
     */
    virtual void setBlinks( bool value ) {blinks_ = value;}

private:

    QString text_;
    QSize textSize_;
    bool blinks_;

    /**
     * Copy value from passed in object.
     */
    void copy( const _Myt& rhs ) {
        text_ = rhs.text_;
        textSize_ = rhs.textSize_;
        blinks_ = rhs.blinks_;
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif /*_MARQUEETEXTFRAGMENT_H_*/
