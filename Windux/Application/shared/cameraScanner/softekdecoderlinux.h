#ifndef SOFTEKDECODERLINUX_H
#define SOFTEKDECODERLINUX_H

#include "decoder.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Softek barcode decoder for linux.
class softekDecoderLinux : public decoder
{
    typedef softekDecoderLinux _Myt;
    typedef decoder _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    softekDecoderLinux( QObject *parent = 0 );

    /// Destructor.
    virtual ~softekDecoderLinux();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve codec required for decoder.
    virtual QString codec() const;

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Decode image.
    /**
     * @param[in] fileName  image file name
     * @return  list of decoded barcodes
     */
    virtual barcodeList decodeImage( const QString& fileName );

private:

    void *session_;

    /// Initialize.
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SOFTEKDECODERLINUX_H
