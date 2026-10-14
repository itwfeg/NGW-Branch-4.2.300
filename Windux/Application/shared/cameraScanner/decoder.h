#ifndef DECODER_H
#define DECODER_H

#include "barcodetype.h"

#include <QMetaType>
#include <QObject>
#include <QString>

Q_DECLARE_METATYPE( barcodeType )
Q_DECLARE_METATYPE( barcode )
Q_DECLARE_METATYPE( barcodeList )

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Abstract barcode decoder.
class decoder : public QObject
{
    Q_OBJECT

    typedef decoder _Myt;
    typedef QObject _Mybase;

Q_SIGNALS:

    /// Signal for when barcode(s) detected.
    /**
     * @param[in] barcodes  list of barcodes
     */
    void decodedBarcodes( const barcodeList& barcodes );

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~decoder();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve codec required for decoder.
    virtual QString codec() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Decode image file.
    /**
     * Upon successfull decode the decodedBarcodes() signal will be invoked.
     *
     * @param[in] fileName  image file name
     */
    virtual void decode( const QString& fileName );

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    decoder( QObject *parent = 0 );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Decode image.
    /**
     * @param[in] fileName  image file name
     * @return  list of decoded barcodes
     */
    virtual barcodeList decodeImage( const QString& fileName ) = 0;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DECODER_H
