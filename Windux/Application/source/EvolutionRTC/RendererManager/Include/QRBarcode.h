/**
 * @file QRBarcode.h
 * QR Barcode renderer adapter.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef QRBARCODE_H
#define QRBARCODE_H

#include "GenericBarcode.h"
#include "BitMap.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// QR Barcode Adapter
class QRBarcodeAdapter
{
	typedef QRBarcodeAdapter _Myt;

public:

	/// Error correction levels.
	enum eccLevel
	{
	    LevelL = 0,                                     ///< Low; Approx. 7% Recovery Capacity.
	    LevelM,                                         ///< Medium; Approx. 15% Recovery Capacity.
	    LevelQ,                                         ///< Quartile; Approx. 25% Recovery Capacity.
	    LevelH                                          ///< High; Approx. 30% Recovery Capacity.
	};

	/// Render QR barcode image.
	/**
     * @param[in,out] bitmap  bitmap to render upon
     * @param[in] bounds  boundary of QR barcode
     * @param[in] ecc  qr ecc level
     * @param[in] data  data to render
     * @return  @c true upon success, @c false otherwise
	 */
	static bool render(
		BitMap &bitmap,
		const Hobart::Rectangle &bounds,
		eccLevel ecc,
		const char *data );

private:

	// not implemented
	QRBarcodeAdapter();

	// not implemented
	QRBarcodeAdapter( const _Myt &other );

	// not implemented
	_Myt &operator = ( const _Myt &other );
};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // QRBARCODE_H
