#ifndef SOFTEKDECODERWIN_H
#define SOFTEKDECODERWIN_H

#include "decoder.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Softek barcode decoder for windows.
class softekDecoderWin : public decoder
{
    typedef softekDecoderWin _Myt;
    typedef decoder _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    softekDecoderWin( QObject *parent = 0 );

    /// Destructor.
    virtual ~softekDecoderWin();

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

#endif // SOFTEKDECODERWIN_H
