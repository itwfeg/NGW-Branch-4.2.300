/**
 * @file openauth.h
 * Open authentication implementation based off RFC 4226 and RFC 6238
 * https://tools.ietf.org/html/rfc4226
 * https://tools.ietf.org/html/rfc6238
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef OPENAUTH_H
#define OPENAUTH_H

#include <QByteArray>
#include <QCryptographicHash>
#include <QMap>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Open authentication.
class OpenAuth
{
    typedef OpenAuth _Myt;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] key  base32 encoded key
     * @param[in] period  password period (in seconds)
     * @param[in] algorithm  algorithm type
     */
    OpenAuth( const QString &key, unsigned int period = 30, QCryptographicHash::Algorithm algorithm = QCryptographicHash::Sha1 );

    /// Constructor.
    /**
     * @param[in] key  key buffer
     * @param[in] period  password period (in seconds)
     * @param[in] algorithm  algorithm type
     */
    OpenAuth( const QByteArray &key, unsigned int period = 30, QCryptographicHash::Algorithm algorithm = QCryptographicHash::Sha1 );

    /// Constructor.
    /**
     * @param[in] other  object to copy
     */
    OpenAuth( const _Myt &other );

    /// Destructor.
    virtual ~OpenAuth();

    // ========================================================================
    // Operators
    // ========================================================================

    /// Assignment operator.
    /**
     * @param[in] rhs  object to copy
     * @return  reference to this
     */
    _Myt & operator = ( const _Myt &rhs );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve code length.
    /**
     * @return  code length
     */
    virtual unsigned int codeLength() const {return codeLength_;}

    /// Set code length.
    /**
     * @param[in] value  code length
     */
    virtual void setCodeLength( unsigned int value ) {codeLength_ = value;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Generate code.
    /**
     * @return  code
     */
    virtual QString generateCode() const;

    /// Validate code.
    /**
     * @param[in] code  code to validate
     * @param[in] delta  how many time periods (plus or minus) code may be off
     * @return  @c true if match, @c false otherwise
     */
    virtual bool validateCode( const QString &code, int delta = 1 ) const;

protected:

    unsigned int codeLength_;                       ///< Code length.

    // ========================================================================
    // Methods
    // ========================================================================

    /// Decode base32 string.
    /**
     * @param[in] s  string to decode
     * @param[out] a  output array
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool decode( const QString &s, QByteArray &a ) const;

    /// Generate code.
    /**
     * @param[in] message  message to generate code from
     * @param[in] length  code length
     * @return  code
     */
    virtual QString generateCode( const QByteArray &message, int length = 6 ) const;

private:

    enum {DEFAULT_CODE_LENGTH = 6};

    QByteArray key_;
    unsigned int period_;
    QCryptographicHash::Algorithm algorithm_;

    QMap<unsigned int, QChar> encodeTable_;
    QMap<QChar, unsigned int> decodeTable_;

    static const char *alphabet_;
    static const unsigned int powTable_[];

    /// Initialize.
    void initialize();

    /// Generate message array.
    static QByteArray messageArray( quint64 message );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // OPENAUTH_H
