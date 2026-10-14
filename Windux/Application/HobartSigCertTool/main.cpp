/**
 * @file main.cpp
 * @brief Application entry point.
 *
 * @copyright
 * Copyright (C) Hobart 2016. All rights reserved.
 */

#include <QByteArray>
#include <QCoreApplication>
#include <QList>
#include <QFile>
#include <QSslCertificate>
#include <QSslError>
#include <QSslKey>
#include <QSslSocket>
#include <QString>

#include <iostream>

#ifndef __linux__
#include <Windows.h>
#include <WinCrypt.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Validate a signature.
/**
 * @param[in] signature  signature to validate
 * @param[in] key  public key
 * @param[in] hash  sha 256 has value
 * @return  @c true if signature matches key and has value, @c false otherwise
 */
bool validateSignature( const QByteArray &signature, const QSslKey &key, const QByteArray &hash )
{
    bool result( false );

    const QByteArray keyBuffer( key.toDer() );

#ifndef __linux__
    HCRYPTPROV cryptProv( NULL );
    HCRYPTKEY cryptPublicKey( NULL );
    HCRYPTHASH cryptHash( NULL );

    CERT_PUBLIC_KEY_INFO *publicKeyInfo( NULL );
    DWORD publicKeyInfoLen( 0 );

    // Note:
    // OpenSSL signatures use big endian format and MS uses little endian format. So we need to
    // reverse the bytes within signature buffer.
    QByteArray buffer( signature );

    for ( int i( 0 ); i < (buffer.length() / 2); ++i )
    {
        const char c( buffer[i] );
        buffer[i] = buffer[buffer.length() - i - 1];
        buffer[buffer.length() - i - 1] = c;
    }

    // acquire provider
    if ( !CryptAcquireContext( &cryptProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT ) )
    {
        if ( NTE_BAD_KEYSET == GetLastError() )
            if ( CryptAcquireContext( &cryptProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT | CRYPT_NEWKEYSET ) )
                goto decode;

        std::cout << "error during CryptAcquireContext: " << GetLastError() << std::endl;
        goto done;
    }

decode:

    // decode key buffer
    if ( !CryptDecodeObjectEx( X509_ASN_ENCODING, X509_PUBLIC_KEY_INFO, (const BYTE*) keyBuffer.constData(), keyBuffer.length(), CRYPT_ENCODE_ALLOC_FLAG, NULL, &publicKeyInfo, &publicKeyInfoLen ) )
    {
        std::cout << "error during CryptDecodeObjectEx: " << GetLastError() << std::endl;
        goto done;
    }

    // import key
    else if ( !CryptImportPublicKeyInfo( cryptProv, X509_ASN_ENCODING, publicKeyInfo, &cryptPublicKey ) )
    {
        std::cout << "error during CryptImportPublicKeyInfo: " << GetLastError() << std::endl;
        goto done;
    }

    // create hash object
    else if ( !CryptCreateHash( cryptProv, CALG_SHA_256, 0, 0, &cryptHash ) )
    {
        std::cout << "error during CryptCreateHash: " << GetLastError() << std::endl;
        goto done;
    }

    // import hash
    else if ( !CryptSetHashParam( cryptHash, HP_HASHVAL, (const BYTE*) hash.constData(), 0 ) )
    {
        std::cout << "error during CryptSetHashParam: " << GetLastError() << std::endl;
        goto done;
    }

    // verify!!
    else if ( !CryptVerifySignature( cryptHash, (const BYTE*) buffer.constData(), buffer.length(), cryptPublicKey, NULL, 0 ) )
    {
        std::cout << "error during CryptVerifySignature: " << GetLastError() << std::endl;
        goto done;
    }

    result = true;

done:

    if ( cryptHash )
        CryptDestroyHash( cryptHash );

    if ( cryptPublicKey )
        CryptDestroyKey( cryptPublicKey );

    if ( cryptProv )
        CryptReleaseContext( cryptProv, 0 );

    if ( publicKeyInfo )
        LocalFree( publicKeyInfo );
#endif

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Create a signature.
/**
 * This method signs a hash with a private key.
 *
 * @param[out] signature  signature to create
 * @param[in] key  private key
 * @param[in] hash  sha 256 value
 * @return  @c true if signature creates successfully, @c false otherwise
 */
bool createSignature( QByteArray &signature, const QSslKey &key, const QByteArray &hash )
{
    bool result( false );

    const QByteArray keyBuffer( key.toDer() );

#ifndef __linux__
    HCRYPTPROV cryptProv( NULL );
    HCRYPTKEY cryptPrivateKey( NULL );
    HCRYPTHASH cryptHash( NULL );

    BYTE *privateKey( NULL );
    DWORD privateKeyLen( 0 );

    BYTE *signatureBlob( NULL );
    DWORD signatureBlobLen( 0 );

    // acquire provider
    if ( !CryptAcquireContext( &cryptProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT ) )
    {
        if ( NTE_BAD_KEYSET == GetLastError() )
            if ( CryptAcquireContext( &cryptProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT | CRYPT_NEWKEYSET ) )
                goto decode;

        std::cout << "error during CryptAcquireContext: " << GetLastError() << std::endl;
        goto done;
    }

decode:

    // decode key buffer
    if ( !CryptDecodeObjectEx( X509_ASN_ENCODING, PKCS_RSA_PRIVATE_KEY, (const BYTE*) keyBuffer.constData(), keyBuffer.length(), CRYPT_ENCODE_ALLOC_FLAG, NULL, &privateKey, &privateKeyLen ) )
    {
        std::cout << "error during CryptDecodeObjectEx: " << GetLastError() << std::endl;
        goto done;
    }

    // import key
    else if ( !CryptImportKey( cryptProv, privateKey, privateKeyLen, NULL, 0, &cryptPrivateKey ) )
    {
        std::cout << "error during CryptImportKey: " << GetLastError() << std::endl;
        goto done;
    }

    // create hash object
    else if ( !CryptCreateHash( cryptProv, CALG_SHA_256, 0, 0, &cryptHash ) )
    {
        std::cout << "error during CryptCreateHash: " << GetLastError() << std::endl;
        goto done;
    }

    // import hash
    else if ( !CryptSetHashParam( cryptHash, HP_HASHVAL, (const BYTE*) hash.constData(), 0 ) )
    {
        std::cout << "error during CryptSetHashParam: " << GetLastError() << std::endl;
        goto done;
    }

    // determine how large signature is
    else if ( !CryptSignHash( cryptHash, AT_KEYEXCHANGE, NULL, 0, NULL, &signatureBlobLen ) )
    {
        std::cout << "error during CryptSignHash: " << GetLastError() << std::endl;
        goto done;
    }
    else if ( !signatureBlobLen )
    {
        std::cout << "invalid blob len!" << std::endl;
        goto done;
    }

    // allocate buffer for signature
    signatureBlob = (LPBYTE) LocalAlloc( 0, signatureBlobLen );

    // sign!!
    if ( !CryptSignHash( cryptHash, AT_KEYEXCHANGE, NULL, 0, signatureBlob, &signatureBlobLen ) )
    {
        std::cout << "error during CryptSignHash: " << GetLastError() << std::endl;
        goto done;
    }

    // Note:
    // OpenSSL signatures use big endian format and MS uses little endian format. So we need to
    // reverse the bytes within signature buffer.
    signature = QByteArray( (char*) signatureBlob, signatureBlobLen );

    for ( int i( 0 ); i < (signature.length() / 2); ++i )
    {
        const char c( signature[i] );
        signature[i] = signature[signature.length() - i - 1];
        signature[signature.length() - i - 1] = c;
    }

    result = true;

done:

    if ( cryptHash )
        CryptDestroyHash( cryptHash );

    if ( cryptPrivateKey )
        CryptDestroyKey( cryptPrivateKey );

    if ( cryptProv )
        CryptReleaseContext( cryptProv, 0 );

    if ( privateKey )
        LocalFree( privateKey );

    if ( signatureBlob )
        LocalFree( signatureBlob );

#endif

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Validate a signed file.
/**
 * This method performs a checksum on @p file then looks for a signature and certificate. The
 * signature must match the checksum and be signed by a trusted authority.
 *
 * @param[in] fileName  filename to validate
 * @return  @c true if file is signed and trusted, @c false otherwise
 */
bool validate( const QString &fileName )
{
    // make sure files exist
    const QString certName( fileName + ".crt" );
    const QString sigName( fileName + ".signed" );

    if (( !QFile::exists( fileName ) ) || ( !QFile::exists( certName ) ) || ( !QFile::exists( sigName ) ))
    {
        std::cout << "One or more files do not exist!" << std::endl;
        return false;
    }

    //
    // Validating a feature signature is a two step process:
    // 1) Validate certificate
    // 2) Validate signature
    //

    // load cert(s) into memory
    QFile certFile( certName );
    QByteArray certBuffer;

    if ( certFile.open( QFile::ReadOnly ) )
    {
        certBuffer = certFile.readAll();
        certFile.close();
    }

    // load cert and chain
    const QList<QSslCertificate> certs( QSslCertificate::fromData( certBuffer ) );

    if ( certs.isEmpty() )
    {
        std::cout << "Invalid certificate!" << std::endl;
        return false;
    }

    foreach ( const QSslCertificate &cert, certs )
        if ( cert.isSelfSigned() )
        {
            std::cout << "Self signed certificate(s) are not allowed!" << std::endl;
            return false;
        }

    // validate
    const QSslCertificate cert( certs.at( 0 ) );

    QList<QSslError> errors( cert.verify( certs.mid( 1 ) ) );

    if ( errors.size() )
    {
        foreach ( const QSslError &error, errors )
            std::cout << qPrintable( error.errorString() ) << std::endl;

        return false;
    }

    // load signature into memory
    QFile sigFile( sigName );
    QByteArray sigBuffer;

    if ( sigFile.open( QFile::ReadOnly ) )
    {
        sigBuffer = sigFile.readAll();
        sigFile.close();
    }
    else
    {
        std::cout << "Error opening signature!" << std::endl;
        return false;
    }

    // generate hash of file
    QFile file( fileName );
    QCryptographicHash hash( QCryptographicHash::Sha256 );

    if ( file.open( QFile::ReadOnly ) )
    {
        hash.addData( &file );
        file.close();
    }
    else
    {
        std::cout << "Error opening file for signature validation!" << std::endl;
        return false;
    }

    // validate!
    if ( !validateSignature( sigBuffer, cert.publicKey(), hash.result() ) )
    {
        std::cout << "Invalid signature!" << std::endl;
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Sign a file.
/**
 * This method performs a checksum on @p file then signs the file using the supplied private key.
 *
 * @param[in] fileName  filename to sign
 * @param[in] certNameOrig  certificate
 * @param[in] privateKeyFileName  private key in pem format
 * @param[in] pass  private key password
 * @return  @c true if file was signed, @c false otherwise
 */
bool sign( const QString &fileName, const QString &certNameOrig, const QString &privateKeyFileName, const QByteArray &pass )
{
    const QString certName( fileName + ".crt" );
    const QString sigName( fileName + ".signed" );

    // copy certificate
    QFile certFile( certName );
    QFile certFileOrig( certNameOrig );

    if (( certFile.open( QFile::WriteOnly | QFile::Truncate ) ) && ( certFileOrig.open( QFile::ReadOnly ) ))
    {
        qint64 bufferLen;
        char buffer[8 * 1024];

        while ( (bufferLen = certFileOrig.read( buffer, sizeof(buffer) )) )
            certFile.write( buffer, bufferLen );

        certFileOrig.close();
        certFile.close();
    }
    else
    {
        std::cout << "Error opening certificate for validation!" << std::endl;
        return false;
    }

    // make sure files exist
    if (( !QFile::exists( fileName ) ) || ( !QFile::exists( certName ) ) || ( !QFile::exists( privateKeyFileName ) ))
    {
        std::cout << "One or more files do not exist!" << std::endl;
        return false;
    }

    // generate hash of file
    QFile file( fileName );
    QCryptographicHash hash( QCryptographicHash::Sha256 );

    if ( file.open( QFile::ReadOnly ) )
    {
        hash.addData( &file );
        file.close();
    }
    else
    {
        std::cout << "Error opening file for signature validation!" << std::endl;
        return false;
    }

    // load private key into memory
    QByteArray privateKeyByteArray;
    QFile privateKeyFile( privateKeyFileName );

    if ( privateKeyFile.open( QFile::ReadOnly ) )
    {
        privateKeyByteArray = privateKeyFile.readAll();
        privateKeyFile.close();
    }
    else
    {
        std::cout << "Error opening private key for validation!" << std::endl;
        return false;
    }

    QSslKey privateKey( privateKeyByteArray, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, pass );

    if ( privateKey.isNull() )
    {
        std::cout << "Error authenticating private key!" << std::endl;
        return false;
    }

    // sign!
    QByteArray signature;

    if ( !createSignature( signature, privateKey, hash.result() ) )
    {
        std::cout << "Failed to create signature!" << std::endl;
        return false;
    }

    // write signature to file
    QFile sigFile( sigName );

    if ( sigFile.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        sigFile.write( signature );
        sigFile.close();
    }
    else
    {
        std::cout << "Error writing signature file!" << std::endl;
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    // validate mode
    //
    // usage:
    // sigcerttool.exe hti-feature-something.tgz
    if ( 2 == argc )
    {
        return (validate( argv[1] ) ? 0 : -1);
    }

    // sign mode
    //
    // usage:
    // sigcerttool.exe hti-feature-something.tgz certificate.crt private-key.pem password
    if ( 5 == argc )
    {
        return (sign( argv[1], argv[2], argv[3], argv[4] ) ? 0 : -1);
    }

    std::cout << "Please specify a file to validate or sign!" << std::endl;
    return -1;
}
