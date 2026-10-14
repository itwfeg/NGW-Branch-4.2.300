
#ifndef BARCODETYPE_H
#define BARCODETYPE_H

#include <QList>
#include <QPair>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Barcode type.
enum barcodeType
{
    BT_UNKNOWN,
    BT_CODE39,
    BT_CODE25,
    BT_CODE25_NI,
    BT_CODE128,
    BT_CODABAR,
    BT_EAN8,
    BT_EAN13,
    BT_UPCA,
    BT_UPCE,
    BT_PATCH,
    BT_PDF417,
    BT_DATAMATRIX,
    BT_DATABAR,
    BT_QRCODE
};

/// Barcode.
typedef QPair<barcodeType, QString> barcode;

/// Barcode list.
typedef QList<barcode> barcodeList;

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // BARCODETYPE_H
