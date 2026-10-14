/**
 * @file qrmodule.h
 * QR Module and Module Matrix.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef QRMODULE_H
#define QRMODULE_H

#include "qrecclevel.h"
#include "qrversion.h"

#include <list>
#include <vector>

class bitmap;

/// QR Barcode Namespace
namespace qr
{

class poly;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// QR Module.
class module
{
    typedef module _Myt;

public:

    /// Types.
    enum type
    {
        AlignmentPattern = 0,
        Data,
        FinderPattern,
        FormatInfo,
        TimingPattern,
        Separator,
        VersionInfo
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

/// QR Module Matrix.
class moduleMatrix
{
    typedef moduleMatrix _Myt;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] ver  version
     */
    moduleMatrix( version ver );

    /// Constructor.
    /**
     * @param[in] ver  version
     * @param[in] level  ecc level of data
     * @param[in] value  data to set
     */
    moduleMatrix( version ver, eccLevel level, const poly &value );

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

    /// Retrieve number of modules per side.
    /**
     * @return  modules per side
     */
    virtual size_t modulesPerSide() const;

    /// Set data.
    /**
     * @param[in] level  ecc level of data
     * @param[in] value  data to set
     */
    virtual void setData( eccLevel level, const poly &value );

    /// Retrieve version.
    /**
     * @return  version
     */
    virtual version ver() const;

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

    version ver_;                                   ///< Version.

    size_t modulesPerSide_;                         ///< Modules per side.
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

    // ========================================================================
    // Methods
    // ========================================================================

    /// Mask data modules.
    /**
     * @param[in] num  mask to apply
     * @return  score
     */
    virtual size_t mask( size_t num );

    /// Generate matrix (masked) with current data and format info.
    /**
     * @param[in] level  ecc level of data
     */
    virtual void generate( eccLevel level );

private:

    typedef std::pair<size_t, size_t> coordinate;
    typedef std::list<coordinate> coordinateList;

    typedef std::list<module*> moduleList;

    module *data_;
    moduleList formatting_;

    /// Initialize.
    void initialize();

    /// Create alignment pattern.
    void createAlignmentPattern( size_t x, size_t y );

    /// Create finder pattern.
    void createFinderPattern( size_t x, size_t y );

    /// Create format info.
    void createFormatInfo();

    /// Create timing pattern.
    void createTimingPattern();

    /// Create version info.
    void createVersionInfo();

    /// Link modules together in chain.
    module *linkModules( const coordinateList &list, module::type t );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // QRMODULE_H

