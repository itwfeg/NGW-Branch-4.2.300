/**
 * @file mutextrylocker.h
 * Mutex locker guard.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef MUTEXTRYLOCKER_H
#define MUTEXTRYLOCKER_H

#include <QMutex>

///////////////////////////////////////////////////////////////////////////////////////////////////
class mutexTryLocker
{
    typedef mutexTryLocker _Myt;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in,out] m  mutex to @c tryLock()
     * @param[in] timeout  timeout value
     */
    mutexTryLocker( QMutex *m, int timeout = 0 ) : m_( m ), locked_( m->tryLock( timeout ) ) {}

    /// Destructor.
    ~mutexTryLocker() {if ( locked_ ) m_->unlock();}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if mutex owned/locked.
    /**
     * @return  @c true if owned/locked, @c false otherwise
     */
    bool isLocked() const {return locked_;}

protected:

    QMutex *m_;                                     ///< Mutex pointer.
    bool locked_;                                   ///< Set to @c true when mutex owned.

private:

    // not implemented
    mutexTryLocker();

    // not implemented
    mutexTryLocker( const _Myt& );

    // not implemented
    _Myt& operator= ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////


#endif // MUTEXTRYLOCKER_H
