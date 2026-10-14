
#include <QtDebug>
#include <QtGlobal>

// windows only
#ifdef Q_OS_WIN

#include "softekdecoderwin.h"

#include <windows.h>
#include <SoftekBarcodeDLL.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
softekDecoderWin::softekDecoderWin( QObject *parent ) :
    _Mybase( parent ),
    session_( NULL )
{
    // init
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
softekDecoderWin::~softekDecoderWin()
{
    // delete session
    if ( session_ )
    {
        mtDestroyBarcodeInstance( session_ );
        session_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString softekDecoderWin::codec() const
{
    return QString( "image/tiff" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
barcodeList softekDecoderWin::decodeImage( const QString& fileName )
{
    barcodeList barcodes;

    if ( !session_ )
        qWarning() << "invalid barcode session!";
    else
    {
        // process image
        int nBarCodes( mtScanBarCode( session_, fileName.toLatin1().data() ) );

        // error
        if ( nBarCodes < 0 )
            qWarning() << "error processing image:" << nBarCodes;

        else
        {
            // process barcodes
            for ( int i( 1 ); i <= nBarCodes; ++i )
            {
                QString code( mtGetBarString( session_, i ) );
                QString codeType( mtGetBarStringType( session_, i ) );

                qDebug() << "[" << codeType << "]" << code;

                // figure out what type of code
                barcodeType bt( BT_UNKNOWN );

                if ( codeType.contains( "CODE128", Qt::CaseInsensitive ) )
                    bt = BT_CODE128;
                else if ( codeType.contains( "CODE39", Qt::CaseInsensitive ) )
                    bt = BT_CODE39;
                else if ( codeType.contains( "CODE25", Qt::CaseInsensitive ) )
                    bt = BT_CODE25;
                else if ( codeType.contains( "EAN13", Qt::CaseInsensitive ) )
                    bt = BT_EAN13;
                else if ( codeType.contains( "EAN8", Qt::CaseInsensitive ) )
                    bt = BT_EAN8;
                else if ( codeType.contains( "UPCA", Qt::CaseInsensitive ) )
                    bt = BT_UPCA;
                else if ( codeType.contains( "UPCE", Qt::CaseInsensitive ) )
                    bt = BT_UPCE;
                else if ( codeType.contains( "CODABAR", Qt::CaseInsensitive ) )
                    bt = BT_CODABAR;
                else if ( codeType.contains( "PDF417", Qt::CaseInsensitive ) )
                    bt = BT_PDF417;
                else if ( codeType.contains( "DATAMATRIX", Qt::CaseInsensitive ) )
                    bt = BT_DATAMATRIX;
                else if ( codeType.contains( "DATABAR", Qt::CaseInsensitive ) )
                    bt = BT_DATABAR;
                else if ( codeType.contains( "QRCODE", Qt::CaseInsensitive ) )
                    bt = BT_QRCODE;

                // append barcode
                barcodes.append( barcode( bt, code ) );
            }
        }
    }

    return barcodes;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softekDecoderWin::initialize()
{
    // create bardecode session
    if ( !(session_ = mtCreateBarcodeInstance()) )
    {
        qCritical() << "failed to create decoder instance!";
        return;
    }

    // Enter your license key here
    // You can get a trial license key from sales@bardecode.com
    // Example:
    // mtSetLicenseKey( session_, "MY LICENSE KEY" );
    mtSetLicenseKey( session_, "39C477684F7F2957B96A2C80103D3F8D" );

    // Turn off the barcode types you don't want to read (this will increase the speed of your application)
    mtSetReadCode128( session_, 0 );
    mtSetReadCode39( session_, 0 );
    mtSetReadCode25( session_, 0 );
    mtSetReadEAN13( session_, 1 );
    mtSetReadEAN8( session_, 1 );
    mtSetReadUPCA( session_, 1 );
    mtSetReadUPCE( session_, 1 );
    mtSetReadCodabar( session_, 0 );
    mtSetReadPDF417( session_, 0 );
    mtSetReadDataMatrix( session_, 0 );
    mtSetReadDatabar( session_, 1 );
    mtSetReadMicroPDF417( session_, 0 );
    mtSetReadQRCode( session_, 1 );

    // Databar Options is a mask that controls which type of databar barcodes will be read and whether or not
    // the software will look for a quiet zone around the barcode.
    // 1 = 2D-Linkage flag (handle micro-PDf417 barcodes as supplementary data - requires ReadMicroPDF417 to be true).
    // 2 = Read RSS14
    // 4 = Read RSS14 Stacked
    // 8 = Read Limited
    // 16 = Read Expanded
    // 32 = Read Expanded Stacked
    // 64 = Require quiet zone
    mtSetDatabarOptions( session_, 255 );

    // If you want to read more than one barcode then set Multiple Read to 1
    // Setting MutlipleRead to False will make the recognition faster
//    mtSetMultipleRead( session_, 1 );
    mtSetMultipleRead( session_, 0 );

    // If you know the max number of barcodes for a single page then increase speed by setting MaxBarcodesPerPage
//    mtSetMaxBarcodesPerPage( session_, 0 );
    mtSetMaxBarcodesPerPage( session_, 1 );

    // In certain conditions (MultipleRead = false or MaxBarcodesPerPage = 1) the SDK can make fast scan of an image before performing the normal scan. This is useful if only 1 bar code is expected per page.
    mtSetUseFastScan( session_, 1 );

    // Noise reduction takes longer but can make it possible to read some difficult barcodes
    // When using noise reduction a typical value is 10 - the smaller the value the more effect it has.
    // A zero value turns off noise reduction.
    // If you use NoiseReduction then the ScanDirection mask must be either only horizontal or only
    // vertical (i.e 1, 2, 4, 8, 5 or 10).
    // mtSetNoiseReduction( session_, 0 );
    mtSetNoiseReduction( session_, 0 );

    // You may need to set a small quiet zone if your barcodes are close to text and pictures in the image.
    // A value of zero uses the default.
//    mtSetQuietZoneSize( session_, 0 );
    mtSetQuietZoneSize( session_, 1 );

    // LineJump controls the frequency at which scan lines in an image are sampled.
    // The default is 1 - decrease this for difficult barcodes.
//    mtSetLineJump( session_, 1 );
    mtSetLineJump( session_, 2 );

    // You can restrict your search to a particular area of the image if you wish.
    // stSetSetScanRect( TopLeftX, TopLeftY, BottomRightX, BottomRightY, 0 );

    // Set the direction that the barcode reader should scan for barcodes
    // The value is a mask where 1 = Left to Right, 2 = Top to Bottom, 3 = Right To Left, 4 = Bottom to Top
    mtSetScanDirection( session_, 15 );

    // SkewTolerance controls the angle of skew that the barcode toolkit will tolerate. By default
    // the toolkit checks for barcodes along horizontal and vertical lines in an image. This works
    // OK for most barcodes because even at an angle it is possible to pass a line through the entire
    // length. SkewTolerance can range from 0 to 5 and allows for barcodes skewed to an angle of 45
    // degrees.
    mtSetSkewTolerance( session_, 0 );

    // Read most skewed linear barcodes without the need to set SkewTolerance. Currently applies to Codabar, Code 25, Code 39 and Code 128 barcodes only.
    mtSetSkewedLinear( session_, 1 );

    // Read most skewed datamatrix barcodes without the need to set SkewTolerance
    mtSetSkewedDatamatrix( session_, 1 );

    // ColorThreshold is the color value used by the control to decide whether a pixel should be considered to be black or white.
    // The value should be in the range 0 to 255.
    //
    // Please note that if this property is set to a non-zero value than ColorProcessLevel is effectively set to a value of 0. It is
    // recommended to set this property to 0 and control the accuracy of reading from color images through the ColorProcessLevel
    // property.
    mtSetColorThreshold( session_, 0 );

    // ColorProcessingLevel controls how much time the toolkit will searching a color image for a barcode.
    // The default value is 2 and the range of values is 0 to 5. If ColorThreshold is non-zero then
    // ColorProcessingLevel is effectively set to 0.
//    mtSetColorProcessingLevel( session_, 2 );
    mtSetColorProcessingLevel( session_, 5 );

    // MaxLength and MinLength can be used to specify the number of characters you expect to
    // find in a stSet This can be useful to increase accuracy or if you wish to ignore some
    // barcodes in an image.
    mtSetMinLength( session_, 4 );
    mtSetMaxLength( session_, 999 );

    // When the toolkit scans an image it records the number of hits it gets for each barcode that
    // MIGHT be in the image. If the hits recorded for any of the barcodes are >= PrefOccurrence
    // then only these barcodes are returned. Otherwise, any barcode whose hits are >= MinOccurrence
    // are reported. If you have a very poor quality image then try setting MinOccurrence to 1, but you
    // may find that some false positive results are returned.
    // mtSetMinOccurrence( session_, 2 );
    // mtSetPrefOccurrence( session_, 4 );

    // Read Code 39 barcodes in extended mode
    // mtSetExtendedCode39( session_, 1 );

    // Barcode string is numeric
    // mtSetReadNumeric( session_, 1 );

    // Set a regular expression for the barcode
    // mtSetPattern( session_, "^[A-Z]{2}[0-9]{5}$" );

    // If you are scanning at a high resolution and the spaces between bars are
    // larger than 1 pixel then set MinSpaceBarWidth to 2 and increase your read rate.
    // mtSetMinSpaceBarWidth( session_, 2 );

    // MedianFilter is a useful way to clean up higher resolution images where the black bars contain white dots
    // and the spaces contain black dots. It does not work if the space between bars is only 1 pixel wide.
    mtSetMedianFilter( session_, 0 );

    // ReportUnreadBarcodes can be used to warn of the presence of a barcode on a page that the SDK has not been able to decode.
    // It currently has the following important limitations:
    // 1. An unread linear barcode will only be reported if no other barcode was found in the same page.
    // 2. The height of the area for an unread linear barcode will only cover a portion of the barcode.
    // 3. Only 2-D barcodes that fail to error correct will be reported.
    // 4. The barcode type and value will both be set to UNREAD for all unread barcodes.
    // 5. The reporting of unread linear barcodes takes no account of settings for individual barcode types. For example, if ReadCode39 is True and
    // an image contains a single Code 39 barcode then this will be reported as an unread barcode.
    // 6. 2-D barcodes are only reported as unread if the correct barcode types have been enabled.
    // 7. Not all unread barcodes will be detected.
    //
    // The value is a mask with the following values: 1 = linear barcodes, 2 = Datamatrix, 4 = QR-Code, 8 = PDF-417
    mtSetReportUnreadBarcodes( session_, 0 );

    // Time out for reading a barcode from a page in ms. Note that this does not include the time to load the page.
    // 0 means no time out.
    mtSetTimeOut( session_, 5000 );

    // Flags for handling PDF files
    // PdfImageOnly defaults to true and indicates that the PDF documents are simple images.
    mtSetPdfImageOnly( session_, 1 );

    // PdfImageExtractOptions is no longer required in version 8 of the SDK but is retained for future possible use.
    // mtSetPdfImageExtractOptions( session_, 0 );

    // The PdfImageRasterOptions mask controls how images are rasterized when PdfImageOnly is false or when image extraction fails
    // 0 = Use Debenu to render image
    // 4 = Use VeryPDF to render image (x86 only)
    mtSetPdfImageRasterOptions( session_, 0 );

    // PdfDpi and PdfBpp control what sort of image the PDF document is rasterized into
    mtSetPdfDpi( session_, 300 );
    mtSetPdfBpp( session_, 8 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // Q_OS_WIN
