/**
 * @file scaleblockedplus.h
 * Blocked scale products.
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SCALEBLOCKEDPLUS_H
#define SCALEBLOCKEDPLUS_H

#include <QList>
#include <QObject>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Blocked scale products.
class scaleBlockedProducts : public QObject
{
    Q_OBJECT

    typedef scaleBlockedProducts _Myt;
    typedef QObject _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    scaleBlockedProducts( QObject *parent = nullptr );

    /// Destructor.
    virtual ~scaleBlockedProducts();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Add blocked product.
    /**
     * @param[in] value  product number to add
     */
    virtual void addBlockedProduct( int value );

    /// Remove blocked product.
    /**
     * @param[in] value  product number to remove
     */
    virtual void removeBlockedProduct( int value );

    /// Save file.
    virtual bool saveFile() const;

    /// Convert to @c QJsonArray.
    /**
     * @return  json array
     */
    virtual QJsonArray toJsonArray() const;

protected:

    /// List of integer values.
    typedef QList<int> integerList;

    integerList blocked_;

    // ------------------------------------------------------------------------
    // Static Methods
    // ------------------------------------------------------------------------

    /// Retrieve list of blocked plus.
    /**
     * @return  list of blocked products
     */
    static integerList blockedProducts();

private:

    // not implemented
    scaleBlockedProducts( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SCALEBLOCKEDPLUS_H
