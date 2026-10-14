/**
 * @file font.h
 * Utility methods for QFont handling
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef FONT_H
#define FONT_H

#include <QFont>
#include <QWidget>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class modifyFontPointSize
 * @brief Functors that modify the font point size.
 */
class modifyFontPointSize
{
public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /// Constructor.
    modifyFontPointSize() {}

    /**
     * Constructor.
     *
     * @param[in] pointSize  font point size
     */
    modifyFontPointSize( int pointSize ) : pointSize_( pointSize ) {}

    /// Destructor.
    virtual ~modifyFontPointSize() {}

    ///////////////////////////////////////////////////////////////////////////
    // Operators

    /**
     * Function operator.
     *
     * @param[in] widget  widget to modify
     */
    template <class T>
    void operator () ( T *widget ) const {setFontPointSize( widget, pointSize_ );}

    /**
     * Function operator.
     *
     * @param[in] widget  widget to modify
     * @param[in] pointSize  font point size
     */
    template <class T>
    void operator () ( T *widget, int pointSize ) const {setFontPointSize( widget, pointSize );}

private:

    int pointSize_;

    /**
     * Set the font point size.
     */
    template <class T>
    static void setFontPointSize( T *widget, int pointSize )
    {
        QFont f( widget->font() );
        f.setPointSize( pointSize );
        widget->setFont( f );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class modifyFontBold
 * @brief Functors that modify the font boldness.
 */
class modifyFontBold
{
public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /// Constructor.
    modifyFontBold() {}

    /**
     * Constructor.
     *
     * @param[in] bold  font bold
     */
    modifyFontBold( bool bold ) : bold_( bold ) {}

    /// Destructor.
    virtual ~modifyFontBold() {}

    ///////////////////////////////////////////////////////////////////////////
    // Operators

    /**
     * Function operator.
     *
     * @param[in] widget  widget to modify
     */
    template <class T>
    void operator () ( T *widget ) const {setFontBold( widget, bold_ );}

    /**
     * Function operator.
     *
     * @param[in] widget  widget to modify
     * @param[in] bold  font bold
     */
    template <class T>
    void operator () ( T *widget, bool bold ) const {setFontBold( widget, bold );}

private:

    bool bold_;

    /**
     * Set the font boldness.
     */
    template <class T>
    static void setFontBold( T *widget, bool bold )
    {
        QFont f( widget->font() );
        f.setBold( bold );
        widget->setFont( f );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // FONT_H
