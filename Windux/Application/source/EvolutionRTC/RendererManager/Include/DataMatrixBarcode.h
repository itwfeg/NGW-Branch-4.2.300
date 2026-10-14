/**
 * @file DataMatrixBarcode.h
 * Data Matrix Barcode renderer adapter.
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DATAMATRIXBARCODE_H
#define DATAMATRIXBARCODE_H

#include "GenericBarcode.h"
#include "BitMap.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Data Matrix Barcode Adapter
class DataMatrixBarcodeAdapter
{
	typedef DataMatrixBarcodeAdapter _Myt;

public:

	/// Render Data Matrix barcode image.
	/**
     * @param[in,out] bitmap  bitmap to render upon
     * @param[in] bounds  boundary of barcode
     * @param[in] data  data to render
     * @return  @c true upon success, @c false otherwise
	 */
	static bool render(
		BitMap &bitmap,
		const Hobart::Rectangle &bounds,
		const char *data );

private:

	// not implemented
	DataMatrixBarcodeAdapter();

	// not implemented
	DataMatrixBarcodeAdapter( const _Myt &other );

	// not implemented
	_Myt &operator = ( const _Myt &other );
};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DATAMATRIXBARCODE_H
