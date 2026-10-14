//#include "vxWorks.h"
#include "BitBlt.h"
#include "string.h"
#include "Debug.h"
#include "HobString.h"


const UINT16  startMasks[] =
{
	 0xffff, 0x7fff, 0x3fff, 0x1fff,
	 0x0fff, 0x07ff, 0x03ff, 0x01ff,
	 0x00ff, 0x007f, 0x003f, 0x001f,
	 0x000f, 0x0007, 0x0003, 0x0001
};


const UINT16  endMasks[] =
{
	 0x0000, 0x8000, 0xc000, 0xe000,
	 0xf000, 0xf800, 0xfc00, 0xfe00,
	 0xff00, 0xff80, 0xffc0, 0xffe0,
	 0xfff0, 0xfff8, 0xfffc, 0xfffe
};


const UINT8 PixelsPerWord = 16;



// This structure is defined for a little endian platform where the least significant
// word (the right hand side word) is stored before the most significant one (the left
// hand side word). In a big endian platform, this structure should be defined with
// "left" before "right".
typedef union
{
	UINT32 dword;
	struct
	{
		UINT16 right;
		UINT16 left;
	} split;
} splitUINT32;



/* This function performs a bit aligned         TTTTTTTTT          TRRRRRB
block transfer that copies an area defined      L       R          T     B
by the source bitmap and rectangle to           L       R    to    T     B
another area defined by the destination         BBBBBBBBB          T     B
bitmap and rectangle translating the data                          TLLLLLB
by 90 degrees and using a transparent            Source          Destination
background (i.e., what's underneath will
show through in the new rotated image.*/
/* If source is smaller than destination, then it will be pushed towards destination's
top-left corner. If source is larger, currently its left and bottom areas will be clipped. */
/* This algorithm will attempt to read once, and only once, each of the source image's
words. In order to achieve this goal it will cache 16 destination words into an array
(UINT16 destWords[16]). These words are filled in as source words are read. Once the 16
words are completed they will be pasted onto the destination bitmap with transparent
background. */
/* All computations are performed in pixels. */
void Rotate90(UltimaBitMap *srcBitmap, UltimaRectangle *srcRect, UltimaBitMap *destBitmap,
		UltimaRectangle *destRect)
{
	INT32 srcWidth, srcHeight, destWidth, destHeight, h;
	UINT16 srcFullWords = 0, srcPitch, srcRowWords, srcWalkingBit, srcWord;
	UINT16 *srcBitPlanePtr, *srcFirstWord;
	UINT16 destFullWords = 0, destPitch, destRowWords, destWordRow, destWordColumn, destWalkingBit;
	UINT16 *destBitPlanePtr, *destFirstWord, *destLastWord;
	UINT8 srcLeftPixels, srcRightPixels, destLeftPixels, destRightPixels, destWordsNumber, destIndex;
	UINT16 srcLeftBitMask, srcRightBitMask, destLeftBitMask;
	UINT16 destWords[PixelsPerWord];



	// Set variables necessary to calculate the variables that are used in the loops.
	// Determine width of the source rectangle. This width is not calculated by adding 1 to
	// the difference between right and left because the rectangle comes already set in that
	// way: the pixels are considered to be rectangles and the coordinates represent the
	// lines in the grid that surround them.
	srcWidth = srcRect->right - srcRect->left;
	destHeight = destRect->bottom - destRect->top;
	// Clip if destination is smaller than source. Reducing the width of source will work
	// because the right position of the source rectangle is only used for this calculation.
	if ( srcWidth > destHeight )
		srcWidth = destHeight;
	if ( srcWidth <= 0 ) // error
		return;

	// Determine height of the source rectangle. See above for reason why we are not adding
	// 1 to the difference between bottom and top.
	srcHeight = srcRect->bottom - srcRect->top;
	destWidth = destRect->right - destRect->left;
	// Clip if destination is smaller than source. Reducing the height of source will work
	// because the bottom position of the source rectangle is only used for this calculation.
	if ( srcHeight > destWidth )
		srcHeight = destWidth;
	if ( srcHeight <= 0 ) // error
		return;

	srcLeftPixels = (UINT8)((PixelsPerWord - ((srcRect->left - srcBitmap->bounds.left) & 0x0F)) & 0x0F);
	srcRightPixels = (UINT8)((srcRect->left - srcBitmap->bounds.left + srcWidth) & 0x0F);
	destLeftPixels = (UINT8)((PixelsPerWord - ((destRect->left - destBitmap->bounds.left) & 0x0F)) & 0x0F);
	destRightPixels = (UINT8)((destRect->left - destBitmap->bounds.left + srcHeight) & 0x0F);
	if ( srcLeftPixels && srcWidth > srcLeftPixels )
		srcFullWords = (UINT16)((srcWidth - srcLeftPixels) >> 4);
	else
		srcFullWords = (UINT16)(srcWidth >> 4);
	if ( destLeftPixels )
	{
		if ( srcHeight > destLeftPixels )
			destFullWords = (UINT16)((srcHeight - destLeftPixels) >> 4);
	}
	else
		destFullWords = (UINT16)(srcHeight >> 4);




	// Set variables that remain constant during loops
	srcPitch = srcBitmap->pitch;
	destPitch = destBitmap->pitch;
	srcFirstWord = srcBitmap->base + (srcRect->top - srcBitmap->bounds.top) * srcPitch
			+ ((srcRect->left - srcBitmap->bounds.left) >> 4);
	destFirstWord = destBitmap->base + (destRect->top - destBitmap->bounds.top) * destPitch
			+ ((destRect->left - destBitmap->bounds.left) >> 4);
	if ( srcLeftPixels < srcWidth )
		srcRowWords = ((srcLeftPixels ? 1 : 0) + srcFullWords + (srcRightPixels ? 1 : 0));
	else
		srcRowWords = 1;
	if ( destLeftPixels < srcHeight )
		destRowWords = ((destLeftPixels ? 1 : 0) + destFullWords + (destRightPixels ? 1 : 0));
	else
		destRowWords = 1;
	destLastWord = destFirstWord + destRowWords - 1;
	if ( destLeftPixels )
		destLeftBitMask = 0x0001 << (destLeftPixels - 1);
	else
		destLeftBitMask = 0x8000;

	// Set variables that change during the loops
	if ( srcRowWords == 1 && srcLeftPixels )
		srcLeftBitMask = 0x0001 << (srcLeftPixels - 1);
	else
		srcLeftBitMask = 0x8000;
	destWordRow = 0;
	if ( srcRightPixels )
	{
		if ( srcRowWords == 1 )
			destWordsNumber = (UINT8)srcWidth;
		else
			destWordsNumber = srcRightPixels;
		srcRightBitMask = 0x8000 >> srcRightPixels;
	}
	else
	{
		if ( srcRowWords == 1 )
			destWordsNumber = (UINT8)srcWidth;
		else
			destWordsNumber = PixelsPerWord;
		srcRightBitMask = 0x0000;
	}




	// This loop will cover each of the columns of source words (srcRowWords). There has
	// to be at least one source word (possibly incomplete), otherwise the source image
	// to copy has no bits to rotate.
	do
	{
		// Set things up for running down a column of source bitmap words
		srcBitPlanePtr = srcFirstWord + (--srcRowWords);   // Places srcBPPtr at top of column
		destWalkingBit = destLeftBitMask;
		destWordColumn = 0;
		memset(destWords, 0, PixelsPerWord * sizeof(UINT16));

		// This loop will cover the whole column of source words. One by one, source words
		// are read and processed onto the destWords array. When this array is "completed"
		// it is pasted onto the destination bitmap and then it is reloaded once more from
		// the destination bitmap.
		for ( h = srcHeight; h > 0; --h )
		{
			// Read source image word
			srcWord = swapWordBytes(*srcBitPlanePtr);
			if ( srcWord )  // If the source word has no pixels, there's nothing to rotate
         {
				for ( srcWalkingBit = srcLeftBitMask, destIndex = destWordsNumber - 1; srcWalkingBit > srcRightBitMask;
						srcWalkingBit >>= 1, --destIndex )
				{
					if ( srcWord & srcWalkingBit )
						destWords[destIndex] |= destWalkingBit;
				}
			}

			if ( (destWalkingBit >>= 1) == 0 )
			{	// If the current block of destination words is "complete", we paste it onto
				// the destination bitmap by OR'ing it in order to enable a transparent
            // background.
				destBitPlanePtr = destFirstWord + destWordRow * destPitch + destWordColumn;
				for ( destIndex = 0; destIndex < destWordsNumber; ++destIndex )
				{
					*destBitPlanePtr |= swapWordBytes(destWords[destIndex]);
					destBitPlanePtr += destPitch;
				}

				// Read the next block of destination words (this is always needed because
				// we are assuming that the background is transparent and therefore we need
				// to know what is underneath in the destination bitmap).
				++destWordColumn;
				if ( destWordColumn < destRowWords )
			      memset(destWords, 0, PixelsPerWord * sizeof(UINT16));

				// For each column, on every destination block, with the possible exception
				// of the first one (which is read at the top of the loop), we start filling
				// from the left-most bit since the order in which source words are read
				// corresponds to a left-to-right sequence of destination words.
				destWalkingBit = 0x8000;
			}

			srcBitPlanePtr += srcPitch;
		}

		// If there are values in destWords that have not been pasted onto the destination
		// image, we'll do it now before we go to the next column of source words.
		if ( destRightPixels )
		{
			destBitPlanePtr = destLastWord + destWordRow * destPitch;
			for ( destIndex = 0; destIndex < destWordsNumber; ++destIndex )
			{
				*destBitPlanePtr |= swapWordBytes(destWords[destIndex]);
				destBitPlanePtr += destPitch;
			}
		}

		// Reset some values for the next columns of source bitmap words.
		destWordRow = destWordRow + destWordsNumber;
		srcRightBitMask = 0x0000;
		if ( srcRowWords == 1 && srcLeftPixels )
		{
			srcLeftBitMask = 0x0001 << (srcLeftPixels - 1);
			destWordsNumber = srcLeftPixels;
		}
		else
			destWordsNumber = PixelsPerWord;
	} while ( srcRowWords > 0 );

	return;
}



/* This function performs a bit aligned         TTTTTTTTT          BBBBBBBBB
block transfer that copies an area defined      L       R          R       L
by the source bitmap and rectangle to           L       R    to    R       L
another area defined by the destination         BBBBBBBBB          TTTTTTTTT
bitmap and rectangle translating the data
by 180 degrees and using a transparent            Source          Destination
background (i.e., what's underneath will
show through in the new rotated image.*/
/* If source is smaller than destination, then it will be pushed towards destination's
bottom-right corner. If source is larger, currently its right and bottom areas will be clipped. */
/* This algorithm will attempt to read once, and only once, each of the source image's
words. In order to achieve this goal it will use a double word window onto the source image
in order to deal with any shift of bits caused by a difference in bit alignment between
source and destination images. This window is filled in with each new source word read. Once
any necessary shift is performed it will be pasted onto the destination bitmap with transparent
background. */
/* All computations are performed in pixels. */
void Rotate180(UltimaBitMap *srcBitmap, UltimaRectangle *srcRect, UltimaBitMap *destBitmap,
		UltimaRectangle *destRect)
{
	INT32 srcWidth, srcHeight, destWidth, destHeight;
	UINT16 srcFullWords = 0, srcPitch, srcDelta, destFullWords = 0, destDelta, i; // destWord;
	UINT16 destStartMask, destEndMask;
	UINT16 *srcBitPlanePtr, *destBitPlanePtr;
	UINT8 srcStartPixels, srcEndPixels, destStartPixels, destEndPixels, skew;
	UINT8 srcFirstPixel, destFirstPixel;
	BOOL forwardShift = FALSE;
	splitUINT32 srcWindow, destWindow;

	// Determine width of the source rectangle. This width is not calculated by adding 1 to
	// the difference between right and left because the rectangle comes already set in that
	// way: the pixels are considered to be rectangles and the coordinates represent the
	// lines in the grid that surround them.
	srcWidth = srcRect->right - srcRect->left;
	destWidth = destRect->right - destRect->left;
	// Clip if destination is smaller than source. Reducing the width of source will work
	// because the right position of the source rectangle is only used for this calculation.
	if ( srcWidth > destWidth )
		srcWidth = destWidth;
	if ( srcWidth <= 0 ) // error
	{
		Debug::trace("RotateOp.Error",
			std::string("Source width is negative or zero. No rotation performed."));
		return;
	}

	// Determine height of the source rectangle
	destHeight = destRect->bottom - destRect->top;
	srcHeight = srcRect->bottom - srcRect->top;
	// Clip if destination is smaller than source. Reducing the height of source will work
	// because the bottom position of the source rectangle is only used for this calculation.
	if ( srcHeight > destHeight )
		srcHeight = destHeight;
	if ( srcHeight <= 0 ) // error
	{
		Debug::trace("RotateOp.Error",
				HobString("Source height is negative or zero. No rotation performed."));
		return;
	}

	// Determine the start pixel and the left and right leftover pixels for later use
	// in other calculations and in the copy algorithm (0 means no dangling pixels)
	srcFirstPixel = (UINT8)((srcRect->left - srcBitmap->bounds.left) & 0x0F);
	srcStartPixels = (PixelsPerWord - srcFirstPixel) & 0x0F;
	destFirstPixel = (UINT8)((destRect->left - destBitmap->bounds.left + destWidth - 1) & 0x0F);
	destStartPixels = (destFirstPixel + 1) & 0x0F;
	srcEndPixels = (UINT8)((srcFirstPixel + srcWidth) & 0x0F);
	destEndPixels = (UINT8)((PixelsPerWord - ((destRect->left + destWidth - srcWidth - destBitmap->bounds.left) & 0x0F)) & 0x0F);

	// Determine the left and right masks to use when copying on the destination rectangle
	destStartMask = transposeWord(startMasks[PixelsPerWord - destFirstPixel - 1]);
	destEndMask = transposeWord(~startMasks[destEndPixels]);

	// Determine the skew and shift direction
	if ( srcStartPixels > destStartPixels )
	{
		skew = srcStartPixels - destStartPixels;
		forwardShift = TRUE;
	}
	else
		skew = destStartPixels - srcStartPixels;

	// Account for the case where there are left and right pixels in the same word
	if ( (srcFirstPixel + srcWidth < PixelsPerWord) && srcStartPixels )
		srcEndPixels = 0;
	if ( destStartPixels > srcWidth )
	{
		destEndPixels = 0;
		destStartMask &= destEndMask;
	}

	// Determine the number of full words in order to later determine the deltas
	// (default values are 0)
	if ( srcStartPixels )
	{
		if ( srcWidth > srcStartPixels )
			srcFullWords = (UINT16)((srcWidth - srcStartPixels) >> 4);
	}
	else
		srcFullWords = (UINT16)(srcWidth >> 4);
	if ( destStartPixels )
	{
		if ( srcWidth > destStartPixels )
			destFullWords = (UINT16)((srcWidth - destStartPixels) >> 4);
	}
	else
		destFullWords = (UINT16)(srcWidth >> 4);

	// Determine deltas to increase the pointers when going from one line to the next
	srcDelta = srcBitmap->pitch - ((srcStartPixels ? 1 : 0) + srcFullWords + (srcEndPixels ? 1 : 0));
	destDelta = destBitmap->pitch - ((destStartPixels ? 1 : 0) + destFullWords + (destEndPixels ? 1 : 0));

	// Determine the pointers to the source and destination bit planes
	srcBitPlanePtr = srcBitmap->base +
			(srcRect->top - srcBitmap->bounds.top)*srcBitmap->pitch +
			((srcRect->left - srcBitmap->bounds.left) >> 4);
	destBitPlanePtr = destBitmap->base +
			(destRect->bottom - destBitmap->bounds.top)*destBitmap->pitch +
			((destRect->right - destBitmap->bounds.left) >> 4);


	// Perform the rotation for the different cases of shift
	if ( skew == 0 )
	{	// No shift case
		// Go through all of the rows, one by one
		for ( ; srcHeight > 0; --srcHeight )
		{
			// Copy the left partial word, if any
			if ( destStartPixels )
				// There is no need to transpose the mask now because it was already defined in that way
//				*destBitPlanePtr-- = swapWordBytes(transposeWord(        // This is the original
//						(destMask & swapWordBytes(*srcBitPlanePtr++)) |
//						transposeWord(swapWordBytes(*destBitPlanePtr))));  // Next is the optimized
				*destBitPlanePtr-- |= swapWordBytes(destStartMask) & transposeWord(*srcBitPlanePtr++);

			// Copy the full words
			for ( i = destFullWords; i > 0; --i )
				*destBitPlanePtr-- |= transposeWord(*srcBitPlanePtr++);

			// Copy the right partial word, if any
			if ( destEndPixels )
				*destBitPlanePtr-- |= swapWordBytes(destEndMask) & transposeWord(*srcBitPlanePtr++);

			// Increment the pointers for the next line
			srcBitPlanePtr += srcDelta;
			destBitPlanePtr -= destDelta;
		}
	}
	else
	{
		if ( forwardShift )
		{	// Forward Shift
			// Go through all of the rows, one by one
			for ( ; srcHeight > 0; --srcHeight )
			{
				srcPitch = srcBitmap->pitch - srcDelta;
				srcWindow.split.right = swapWordBytes(*srcBitPlanePtr++);
				--srcPitch;

				// Copy the left partial word, if any
				if ( destStartPixels )
				{
					destWindow.dword = srcWindow.dword >> skew;          // Forward Shift
//					*destBitPlanePtr-- = swapWordBytes(transposeWord(         // This is the original
//							(destMask & destWindow.split.right) |
//							transposeWord(swapWordBytes(*destBitPlanePtr))));   // Next is the optimized
					*destBitPlanePtr-- |= swapWordBytes(destStartMask & transposeWord(destWindow.split.right));
				}

				// Copy the full words
				for ( i = destFullWords; i > 0; --i )
				{
					srcWindow.split.left = srcWindow.split.right;
					srcWindow.split.right = swapWordBytes(*srcBitPlanePtr++);
					--srcPitch;
					destWindow.dword = srcWindow.dword >> skew;          // Forward Shift
					// This is the original. Next is the optimized
//					*destBitPlanePtr-- = swapWordBytes(transposeWord(
//							destWindow.split.right | transposeWord(swapWordBytes(*destBitPlanePtr))));
					*destBitPlanePtr-- |= swapWordBytes(transposeWord(destWindow.split.right));
				}

				// Copy the right partial word, if any
				if ( destEndPixels )
				{
					srcWindow.split.left = srcWindow.split.right;
					if ( srcPitch )
						srcWindow.split.right = swapWordBytes(*srcBitPlanePtr++);
					destWindow.dword = srcWindow.dword >> skew;          // Forward Shift
//					*destBitPlanePtr-- = swapWordBytes(transposeWord(          // This is the original
//							(destMask & destWindow.split.right) |
//							transposeWord(swapWordBytes(*destBitPlanePtr))));    // Next is the optimized
					*destBitPlanePtr-- |= swapWordBytes(destEndMask & transposeWord(destWindow.split.right));
				}

				// Increment the pointers for the next line
				srcBitPlanePtr += srcDelta;
				destBitPlanePtr -= destDelta;
			}
		}
		else
		{	// Backward Shift
			// Go through all of the rows, one by one
			for ( ; srcHeight > 0; --srcHeight )
			{
				srcPitch = srcBitmap->pitch - srcDelta;
				if ( srcStartPixels )
				{
					srcWindow.split.right = swapWordBytes(*srcBitPlanePtr++);
					--srcPitch;
				}
				else //sjjc added this case since otherwise split.right may be uninitialized
					srcWindow.split.right = 0;

				// Copy the left partial word, if any
				if ( destStartPixels )
				{
					srcWindow.split.left = srcWindow.split.right;
					srcWindow.split.right = swapWordBytes(*srcBitPlanePtr++);
					--srcPitch;
					destWindow.dword = srcWindow.dword << skew;          // Backward Shift
//					*destBitPlanePtr-- = swapWordBytes(transposeWord(         // This is the original
//							(destMask & destWindow.split.left) |
//							transposeWord(swapWordBytes(*destBitPlanePtr))));   // Next is the optimized
					*destBitPlanePtr-- |= swapWordBytes(destStartMask & transposeWord(destWindow.split.left));
				}

				// Copy the full words
				for ( i = destFullWords; i > 0; --i )
				{
					srcWindow.split.left = srcWindow.split.right;
					srcWindow.split.right = swapWordBytes(*srcBitPlanePtr++);
					--srcPitch;
					destWindow.dword = srcWindow.dword << skew;          // Backward Shift
					// This is the original. Next is the optimized
//					*destBitPlanePtr-- = swapWordBytes(transposeWord(
//							destWindow.split.left | transposeWord(swapWordBytes(*destBitPlanePtr))));
					*destBitPlanePtr-- |= swapWordBytes(transposeWord(destWindow.split.left));
				}

				// Copy right partial word, if any
				if ( destEndPixels )
				{
					srcWindow.split.left = srcWindow.split.right;
					if ( srcPitch )
						srcWindow.split.right = swapWordBytes(*srcBitPlanePtr++);
					destWindow.dword = srcWindow.dword << skew;          // Backward Shift
//					*destBitPlanePtr-- = swapWordBytes(transposeWord(          // This is the original
//							(destMask & destWindow.split.left) |
//							transposeWord(swapWordBytes(*destBitPlanePtr))));    // Next is the optimized
					*destBitPlanePtr-- |= swapWordBytes(destEndMask & transposeWord(destWindow.split.left));
				}

				// Increment the pointers for the next line
				srcBitPlanePtr += srcDelta;
				destBitPlanePtr -= destDelta;
			}
		}
	}

	return;
}



/* This function performs a bit aligned            TTTTTTTTT          BLLLLLT
block transfer that copies an area defined         L       R          B     T
by the source bitmap and rectangle to              L       R    to    B     T
another area defined by the destination            BBBBBBBBB          B     T
bitmap and rectangle translating the data                             BRRRRRT
by -90 degrees (270) and using a transparent        Source          Destination
background (i.e., what's underneath will
show through in the new rotated image.*/
void Rotate270(UltimaBitMap *srcBitmap, UltimaRectangle *srcRect, UltimaBitMap *destBitmap,
		UltimaRectangle *destRect)
{
	INT32 srcWidth, srcHeight, destWidth, destHeight, destWordColumn, h;
	UINT16 srcFullWords = 0, srcPitch, srcRowWords, srcCurrentWord = 0, srcWalkingBit, srcWord;
	UINT16 *srcBitPlanePtr, *srcFirstWord;
	UINT16 destFullWords = 0, destPitch, destRowWords, destWordRow, destWalkingBit;
	UINT16 *destBitPlanePtr, *destFirstWord, *destLastWord;
	UINT8 srcStartPixels, srcEndPixels, destStartPixels, destEndPixels, destWordsNumber, destIndex;
	UINT16 srcStartBitMask, srcEndBitMask, destStartBitMask;
	UINT16 destWords[PixelsPerWord];



	// Set variables necessary to calculate the variables that are used in the loops.
	// Determine width of the source rectangle. This width is not calculated by adding 1 to
	// the difference between right and left because the rectangle comes already set in that
	// way: the pixels are considered to be rectangles and the coordinates represent the
	// lines in the grid that surround them.
	srcWidth = srcRect->right - srcRect->left;
	destHeight = destRect->bottom - destRect->top;
	// Clip if destination is smaller than source. Reducing the width of source will work
	// because the right position of the source rectangle is only used for this calculation.
	if ( srcWidth > destHeight )
		srcWidth = destHeight;
	if ( srcWidth <= 0 ) // error
		return;

	// Determine height of the source rectangle. See above for reason why we are not adding
	// 1 to the difference between bottom and top.
	srcHeight = srcRect->bottom - srcRect->top;
	destWidth = destRect->right - destRect->left;
	// Clip if destination is smaller than source. Reducing the height of source will work
	// because the bottom position of the source rectangle is only used for this calculation.
	if ( srcHeight > destWidth )
		srcHeight = destWidth;
	if ( srcHeight <= 0 ) // error
		return;

	srcStartPixels = (UINT8)((PixelsPerWord - ((srcRect->left - srcBitmap->bounds.left) & 0x0F)) & 0x0F);
	srcEndPixels = (UINT8)((srcRect->left - srcBitmap->bounds.left + srcWidth) & 0x0F);
	destStartPixels = (UINT8)((destRect->left + destWidth - destBitmap->bounds.left) & 0x0F);
	destEndPixels = (UINT8)((PixelsPerWord - ((destRect->left + destWidth - srcHeight - destBitmap->bounds.left) & 0x0F)) & 0x0F);
	if ( srcStartPixels && srcWidth > srcStartPixels )
		srcFullWords = ((UINT16)(srcWidth - srcStartPixels) >> 4);
	else
		srcFullWords = (UINT16)(srcWidth >> 4);
	if ( destStartPixels && srcHeight > destStartPixels )
		destFullWords = (UINT16)((srcHeight - destStartPixels) >> 4);
	else
		destFullWords = (UINT16)(srcHeight >> 4);


	// Set variables that remain constant during loops
	srcPitch = srcBitmap->pitch;
	destPitch = destBitmap->pitch;
	srcFirstWord = srcBitmap->base + (srcRect->top - srcBitmap->bounds.top) * srcPitch
			+ ((srcRect->left - srcBitmap->bounds.left) >> 4);
	destFirstWord = destBitmap->base + (destRect->top - destBitmap->bounds.top) * destPitch
			+ ((destRect->left + destWidth - 1 - destBitmap->bounds.left) >> 4);
	if ( srcStartPixels < srcWidth )
		srcRowWords = ((srcStartPixels ? 1 : 0) + srcFullWords + (srcEndPixels ? 1 : 0));
	else
		srcRowWords = 1;
	if ( destStartPixels < srcHeight )
		destRowWords = ((destStartPixels ? 1 : 0) + destFullWords + (destEndPixels ? 1 : 0));
	else
		destRowWords = 1;
	destLastWord = destFirstWord - destRowWords + 1;
	if ( destStartPixels )
		destStartBitMask = 0x8000 >> (destStartPixels - 1);
	else
		destStartBitMask = 0x0001;

	// Set variables that change during the loops
	if ( srcStartPixels )
		srcStartBitMask = 0x0001 << (srcStartPixels - 1);
	else
		srcStartBitMask = 0x8000;

	if ( srcRowWords == 1 )
	{
		destWordsNumber = (UINT8)srcWidth;
		srcEndBitMask = 0x8000 >> (srcEndPixels - 1);
	}
	else
	{
		if ( srcStartPixels )
			destWordsNumber = srcStartPixels;
		else
			destWordsNumber = PixelsPerWord;
		srcEndBitMask = 0x0001;
	}

	destWordRow = 0;



	// This loop will cover each of the columns of source words (srcRowWords). There has
	// to be at least one source word (possibly incomplete), otherwise the source image
	// to copy has no bits to rotate.
	do
	{
		// Set things up for running down a column of source bitmap words
		srcBitPlanePtr = srcFirstWord + (srcCurrentWord++);   // Places srcBPPtr at top of column
		destWalkingBit = destStartBitMask;
		destWordColumn = destRowWords - 1;
		memset(destWords, 0, PixelsPerWord * sizeof(UINT16));

		// This loop will cover the whole column of source words. One by one, source words
		// are read and processed onto the destWords array. When this array is "completed"
		// it is pasted onto the destination bitmap and then it is reloaded once more from
		// the destination bitmap.
		for ( h = srcHeight; h > 0; --h )
		{
			// Read source image word
			srcWord = swapWordBytes(*srcBitPlanePtr);
			if ( srcWord )  // If the source word has no pixels, there's nothing to rotate
			{
				for ( srcWalkingBit = srcStartBitMask, destIndex = 0; srcWalkingBit >= srcEndBitMask;
						srcWalkingBit >>= 1, ++destIndex )
				{
					if ( srcWord & srcWalkingBit )
						destWords[destIndex] |= destWalkingBit;
				}
			}

			if ( (destWalkingBit <<= 1) == 0 )
			{	// If the current block of destination words is "complete", we paste it onto
				// the destination bitmap by OR'ing it in order to enable a transparent
				// background.
				destBitPlanePtr = destLastWord + destWordRow * destPitch + destWordColumn;
				for ( destIndex = 0; destIndex < destWordsNumber; ++destIndex )
				{
					*destBitPlanePtr |= swapWordBytes(destWords[destIndex]);
					destBitPlanePtr += destPitch;
				}

				// Position into the prior column
				--destWordColumn;
				if ( destWordColumn >= 0 )
					memset(destWords, 0, PixelsPerWord * sizeof(UINT16));

				// For each column, on every destination block, with the possible exception
				// of the first one (which is read at the top of the loop), we start filling
				// from the left-most bit since the order in which source words are read
				// corresponds to a left-to-right sequence of destination words.
				destWalkingBit = 0x0001;
			}

			srcBitPlanePtr += srcPitch;
		}

		// If there are values in destWords that have not been pasted onto the destination
		// image, we'll do it now before we go to the next column of source words.
		if ( destEndPixels )
		{
			destBitPlanePtr = destLastWord + destWordRow * destPitch;
			for ( destIndex = 0; destIndex < destWordsNumber; ++destIndex )
			{
				*destBitPlanePtr |= swapWordBytes(destWords[destIndex]);
				destBitPlanePtr += destPitch;
			}
		}

		// Reset some values for the next columns of source bitmap words.
		destWordRow = destWordRow + destWordsNumber;
		srcStartBitMask = 0x8000;
		if ( (srcCurrentWord == srcRowWords - 1) && srcEndPixels )
		{
			srcEndBitMask = 0x8000 >> (srcEndPixels - 1);
			destWordsNumber = srcEndPixels;
		}
		else
			destWordsNumber = PixelsPerWord;
	} while ( srcCurrentWord < srcRowWords );

	return;
}
