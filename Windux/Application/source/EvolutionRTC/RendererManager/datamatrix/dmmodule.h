/**
 * @file dmmodule.h
 * Data Matrix Module and Module Matrix.
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DMMODULE_H
#define DMMODULE_H

#include "dmecclevel.h"
#include "dmsymbolsize.h"

#include <list>
#include <vector>

class bitmap;
class buffer;

/// Data Matrix Barcode Namespace
namespace dm
{

struct symbolInfo;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Data Matrix Module.
class module
{
    typedef module _Myt;

public:

    /// Types.
    enum type
    {
        FinderPattern = 0,
        Data,
        Reserved
    };

    type type_;
    bool value_; // 0 = false = white, 1 = true = black

    _Myt *next_;

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    module();

    /// Destructor.
    virtual ~module();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Data Matrix Module Matrix.
class moduleMatrix
{
    typedef moduleMatrix _Myt;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] s  symbol size
     */
    moduleMatrix( symbolSize s );

    /// Constructor.
    /**
     * @param[in] s  symbol size
     * @param[in] value  data to set
     */
    moduleMatrix( symbolSize s, const buffer &value );

    /// Constructor.
    /**
     * @param[in] other  object to copy
     */
    moduleMatrix( const _Myt &other );

    /// Destructor.
    virtual ~moduleMatrix();

    // ========================================================================
    // Operators
    // ========================================================================

    /// Assignment operator.
    /**
     * @param[in] rhs  object to copy
     * @return  reference to this
     */
    _Myt &operator = ( const _Myt &rhs );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve module at (x, y).
    /**
     * @param[in] x  x offset
     * @param[in] y  y offset
     * @return  reference to module
     */
    virtual const module &at( size_t x, size_t y ) const;

    /// Retrieve module at (x, y).
    /**
     * @param[in] x  x offset
     * @param[in] y  y offset
     * @return  reference to module
     */
    virtual module &at( size_t x, size_t y );

    /// Retrieve matrix height.
    /**
     * @return  height
     */
    virtual size_t height() const;

    /// Set data.
    /**
     * @param[in] value  data to set
     */
    virtual void setData( const buffer &value );

    /// Retrieve symbol size.
    /**
     * @return  size
     */
    virtual symbolSize size() const;

    /// Retrieve matrix width.
    /**
     * @return  width
     */
    virtual size_t width() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Assignment.
    /**
     * @param[in] other  object to copy
     */
    virtual void assign( const _Myt &other );

    /// Print matrix.
    virtual void print() const;

    /// Print module types.
    virtual void printModuleTypes() const;

    /// Render matrix.
    /**
     * @param[in,out] b  bitmap to modify
     * @param[in] x  x coordinate
     * @param[in] y  y coordinate
     * @param[in] width  width of rectangle
     * @param[in] height  height of rectangle
     */
    virtual void render( bitmap &b, size_t x, size_t y, size_t width, size_t height ) const;

protected:

    symbolSize size_;                               ///< Size.

    size_t width_;                                  ///< Symbol Width.
    size_t height_;                                 ///< Symbol Height.
    std::vector<module> modules_;                   ///< Modules.

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set module value.
    /**
     * @param[in] m  module to set
     * @param[in] value  value
     */
    virtual void setModuleValue( module *m, size_t value );

private:

    typedef std::pair<size_t, size_t> coordinate;
    typedef std::list<coordinate> coordinateList;

    size_t mappingWidth_;
    size_t mappingHeight_;

    module *data_;

    /// Initialize.
    void initialize();

    /// Create finder pattern.
    void createFinderPattern( size_t x, size_t y, size_t w, size_t h );

    /// Link modules together in chain.
    module *linkModules( const coordinateList &list, module::type t );

    /// Append module.
    void appendModule( coordinateList &list, int x, int y ) const;

    /// Place utah shaped character.
    void utah( coordinateList &list, int x, int y ) const;

    /// Place corner character.
    void corner1( coordinateList &list ) const;

    /// Place corner character.
    void corner2( coordinateList &list ) const;

    /// Place corner character.
    void corner3( coordinateList &list ) const;

    /// Place corner character.
    void corner4( coordinateList &list ) const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DMATRIXMODULE_H

