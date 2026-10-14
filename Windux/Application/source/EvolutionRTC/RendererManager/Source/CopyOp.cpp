//#include "vxWorks.h"
#include "BitBlt.h"


#include "utilstring.h"
#include "Debug.h"



const UINT16  leftMasks[] =
{
	 0xffff, 0x7fff, 0x3fff, 0x1fff,
	 0x0fff, 0x07ff, 0x03ff, 0x01ff,
	 0x00ff, 0x007f, 0x003f, 0x001f,
	 0x000f, 0x0007, 0x0003, 0x0001
};


const UINT16  rightMasks[] =
{
	 0x0000, 0x8000, 0xc000, 0xe000,
	 0xf000, 0xf800, 0xfc00, 0xfe00,
	 0xff00, 0xff80, 0xffc0, 0xffe0,
	 0xfff0, 0xfff8, 0xfffc, 0xfffe
};


const UINT16  oldRightMasks[] = 
{
    0x0000,   /* Fill 0th position */
    0x0100, 0x0300, 0x0700, 0x0f00,
    0x1f00, 0x3f00, 0x7f00, 0xff00,
    0xff01, 0xff03, 0xff07, 0xff0f,
    0xff1f, 0xff3f, 0xff7f, 0xffff,
    0xffff
};


static const UINT8 PixelsPerWord = 16;
const UINT8 oldPixelsPerWord = 15;    // Used in algorithms ported from Ultima


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



/* This function is a BITBLT function that will transfer a rectangle of graphical
	data from one bitap to another with a transparent background (i.e., what is
	already in the section where the copy is going to happen will show through). */
void CopyOP(UltimaBitMap *srcBitmap, UltimaRectangle *srcRect, UltimaBitMap *destBitmap,
		UltimaRectangle *destRect)
{
	INT32 srcWidth, srcHeight, destWidth, destHeight;
	UINT16 srcFullWords = 0, srcPitch, srcDelta, destFullWords = 0, destDelta, i, destWord;
	UINT16 destLeftMask, destRightMask;
	UINT16 *srcBitPlanePtr, *destBitPlanePtr;
	UINT8 srcLeftPixels, srcRightPixels, destLeftPixels, destRightPixels, skew;
	UINT8 srcStartPixel, destStartPixel;
	BOOL leftShift = FALSE;
	splitUINT32 srcWindow, destWindow;
	
	//initialize to get rid of uninitialized warning
	srcWindow.split.left = 0;
	srcWindow.split.right = 0;

	Debug::trace("CopyOP", 28, "Going to perform CopyOP");
	Debug::trace("CopyOP", 28, "Source Bitmap with pitch=" + util_string::inttostring(srcBitmap->pitch) + " (" +
			util_string::inttostring(srcBitmap->bounds.left) + "," + util_string::inttostring(srcBitmap->bounds.top) + "), (" +
			util_string::inttostring(srcBitmap->bounds.right) + "," + util_string::inttostring(srcBitmap->bounds.bottom) + ")");
	Debug::trace("CopyOP", 28, "Source Rectangle (" +
			util_string::inttostring(srcRect->left) + "," + util_string::inttostring(srcRect->top) + "), (" +
			util_string::inttostring(srcRect->right) + "," + util_string::inttostring(srcRect->bottom) + ")");
	Debug::trace("CopyOP", 28, "Destination Bitmap with pitch=" + util_string::inttostring(destBitmap->pitch) + " (" +
			util_string::inttostring(destBitmap->bounds.left) + "," + util_string::inttostring(destBitmap->bounds.top) + "), (" +
			util_string::inttostring(destBitmap->bounds.right) + "," + util_string::inttostring(destBitmap->bounds.bottom) + ")");
	Debug::trace("CopyOP", 28, "Destination Rectangle (" +
			util_string::inttostring(destRect->left) + "," + util_string::inttostring(destRect->top) + "), (" +
			util_string::inttostring(destRect->right) + "," + util_string::inttostring(destRect->bottom) + ")");

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
		return;

	// deterine height of the source rectangle
	destHeight = destRect->bottom - destRect->top;
	srcHeight = srcRect->bottom - srcRect->top;
	// Clip if destination is smaller than source. Reducing the height of source will work
	// because the bottom position of the source rectangle is only used for this calculation.
	if ( srcHeight > destHeight )
		srcHeight = destHeight;
	if ( srcHeight <= 0 ) // error
		return;

	// Determine the start pixel and the left and right leftover pixels for later use
	// in other calculations and in the copy algorithm (0 means no dangling pixels)
	srcStartPixel = (UINT8)((srcRect->left - srcBitmap->bounds.left) & 0x0F);
	srcLeftPixels = (PixelsPerWord - srcStartPixel) & 0x0F;
	destStartPixel = (UINT8)((destRect->left - destBitmap->bounds.left) & 0x0F);
	destLeftPixels = (PixelsPerWord - destStartPixel) & 0x0F;
	srcRightPixels = (UINT8)((((srcRect->left - srcBitmap->bounds.left + srcWidth - 1) & 0x0F) + 1) & 0x0F);
//	srcRightPixels = (srcStartPixel + srcWidth) & 0x0F; // Optimization
	destRightPixels = (UINT8)((((destRect->left - destBitmap->bounds.left + srcWidth - 1) & 0x0F) + 1) & 0x0F);
//	destRightPixels = (destStartPixel + srcWidth) & 0x0F; // Optimization

	// Determine the left and right masks to use when copying on the destination rectangle
	destLeftMask = leftMasks[destStartPixel];
	destRightMask = rightMasks[destRightPixels];

	// Determine the skew and shift direction
	if ( srcStartPixel > destStartPixel )
	{
		skew = srcStartPixel - destStartPixel;
		leftShift = TRUE;
	}
	else
		skew = destStartPixel - srcStartPixel;

	// Account for the case where there are left and right pixels in the same word
	if ( (srcStartPixel + srcWidth < PixelsPerWord) && srcLeftPixels )
		srcRightPixels = 0;
	if ( (destStartPixel + srcWidth < PixelsPerWord) && destLeftPixels )
	{
		destRightPixels = 0;
		destLeftMask &= destRightMask;
	}

	// Determine the number of full words in order to later determine the deltas
	// (default values are 0)
	if ( srcLeftPixels && srcWidth > srcLeftPixels )
		srcFullWords = (UINT16)((srcWidth - srcLeftPixels) >> 4);
	else
		srcFullWords = (UINT16)(srcWidth >> 4);
	if ( destLeftPixels && srcWidth > destLeftPixels )
		destFullWords = (UINT16)((srcWidth - destLeftPixels) >> 4);
	else
		destFullWords = (UINT16)(srcWidth >> 4);

	// Determine deltas to increase the pointers when going from one line to the next
	srcDelta = srcBitmap->pitch - ((srcLeftPixels ? 1 : 0) + srcFullWords + (srcRightPixels ? 1 : 0));
	destDelta = destBitmap->pitch - ((destLeftPixels ? 1 : 0) + destFullWords + (destRightPixels ? 1 : 0));

	// Determine the pointers to the source and destination bit planes
	srcBitPlanePtr = srcBitmap->base +
			(srcRect->top - srcBitmap->bounds.top)*srcBitmap->pitch +
			((srcRect->left - srcBitmap->bounds.left) >> 4);
	destBitPlanePtr = destBitmap->base +
			(destRect->top - destBitmap->bounds.top)*destBitmap->pitch +
			((destRect->left - destBitmap->bounds.left) >> 4);


	// Perform the copy for the different cases of shift
	if ( skew == 0 )
	{	// No shift case
		// Go through all of the rows, one by one
		for ( ; srcHeight > 0; --srcHeight )
		{
			// Copy the left partial word, if any
			if ( destLeftPixels )
			{
				destWord = swapWordBytes(*destBitPlanePtr);
				*destBitPlanePtr++ = swapWordBytes((destLeftMask & swapWordBytes(*srcBitPlanePtr++)) |
					destWord);
			}

			// Copy the full words
			for ( i = destFullWords; i > 0; --i )
				*destBitPlanePtr++ |= *srcBitPlanePtr++;

			// Copy the right partial word, if any
			if ( destRightPixels )
			{
				destWord = swapWordBytes(*destBitPlanePtr);
				*destBitPlanePtr++ = swapWordBytes((destRightMask & swapWordBytes(*srcBitPlanePtr++)) |
					destWord);
			}

			// Increment the pointers for the next line
			srcBitPlanePtr += srcDelta;
			destBitPlanePtr += destDelta;
		}
	}
	else
	{
		if ( leftShift )
		{	// Left Shift
			// Go through all of the rows, one by one
			for ( ; srcHeight > 0; --srcHeight )
			{
				srcPitch = srcBitmap->pitch - srcDelta;
				if ( srcLeftPixels )
				{
					srcWindow.split.right = swapWordBytes(*srcBitPlanePtr++);
					--srcPitch;
				}

				// Copy the left partial word, if any
				if ( destLeftPixels )
				{
					srcWindow.split.left = srcWindow.split.right;
					srcWindow.split.right = swapWordBytes(*srcBitPlanePtr++);
					--srcPitch;
					destWindow.dword = srcWindow.dword << skew;
					destWord = swapWordBytes(*destBitPlanePtr);
					*destBitPlanePtr++ = swapWordBytes((destLeftMask & destWindow.split.left) | destWord);
				}

				// Copy the full words
				for ( i = destFullWords; i > 0; --i )
				{
					srcWindow.split.left = srcWindow.split.right;
					srcWindow.split.right = swapWordBytes(*srcBitPlanePtr++);
					--srcPitch;
					destWindow.dword = srcWindow.dword << skew;
					*destBitPlanePtr++ |= swapWordBytes(destWindow.split.left);
				}

				// Copy right partial word, if any
				if ( destRightPixels )
				{
					srcWindow.split.left = srcWindow.split.right;
					if ( srcPitch )
						srcWindow.split.right = swapWordBytes(*srcBitPlanePtr++);
					destWindow.dword = srcWindow.dword << skew;
					destWord = swapWordBytes(*destBitPlanePtr);
					*destBitPlanePtr++ = swapWordBytes((destRightMask & destWindow.split.left) | destWord);
				}

				// Increment the pointers for the next line
				srcBitPlanePtr += srcDelta;
				destBitPlanePtr += destDelta;
			}
		}
		else
		{	// Right Shift
			// Go through all of the rows, one by one
			for ( ; srcHeight > 0; --srcHeight )
			{
				srcPitch = srcBitmap->pitch - srcDelta;
				srcWindow.split.right = swapWordBytes(*srcBitPlanePtr++);
				--srcPitch;

				// Copy the left partial word, if any
				if ( destLeftPixels )
				{
					destWindow.dword = srcWindow.dword >> skew;
					destWord = swapWordBytes(*destBitPlanePtr);
					*destBitPlanePtr++ = swapWordBytes((destLeftMask & destWindow.split.right) | destWord);
				}

				// Copy the full words
				for ( i = destFullWords; i > 0; --i )
				{
					srcWindow.split.left = srcWindow.split.right;
					srcWindow.split.right = swapWordBytes(*srcBitPlanePtr++);
					--srcPitch;
					destWindow.dword = srcWindow.dword >> skew;
					*destBitPlanePtr++ |= swapWordBytes(destWindow.split.right);
				}

				// Copy the right partial word, if any
				if ( destRightPixels )
				{
					srcWindow.split.left = srcWindow.split.right;
					if ( srcPitch )
						srcWindow.split.right = swapWordBytes(*srcBitPlanePtr++);
//					else            // Is this necessary?
//						srcWindow.split.right = 0;
					destWindow.dword = srcWindow.dword >> skew;
					destWord = swapWordBytes(*destBitPlanePtr);
					*destBitPlanePtr++ = swapWordBytes((destRightMask & destWindow.split.right) | destWord);
				}

				// Increment the pointers for the next line
				srcBitPlanePtr += srcDelta;
				destBitPlanePtr += destDelta;
			}
		}
	}
}



/* This function will invert graphics data in a specified rectangle of
	a specified bitmap. */

void InvertOP(UltimaBitMap *bitMap, UltimaRectangle *rect)
{
UINT16 *pBitPlane, mask, rightMask, leftMask, i, oldWord, newWord, rowWords;
INT32  srcTop, srcRecLeft; 
UINT8 endPixel, startPixel;
INT32 recWidth, recHeight, delta;

	recWidth = rect->right - rect->left;
	if(recWidth <= 0)
	{
		//error
	 	return;
	}

	recHeight = rect->bottom - rect->top;	
	if(recHeight <= 0)
	{
		//error
		return;
	}
	endPixel = (UINT8)(oldPixelsPerWord - ((rect->right - bitMap->bounds.left - 1) & 0x0f));
    rightMask = ~(oldRightMasks[endPixel]);

	startPixel = (UINT8)((oldPixelsPerWord + 1) - ((rect->left - bitMap->bounds.left) & 0x0f));
   	leftMask = oldRightMasks[startPixel];


	if (recWidth <= startPixel)
	{
	  rowWords = 1;
	  leftMask &= rightMask;
	}
	else
	{
	  // +2 because of right and left masks
	  rowWords = (UINT16)(((recWidth - startPixel - 1) >> 4) + 2);
	}

	delta = bitMap->pitch - rowWords;

	srcTop = rect->top - bitMap->bounds.top;																	
	srcTop = srcTop * bitMap->pitch ;
	srcRecLeft = rect->left - bitMap->bounds.left;
	srcRecLeft >>= 4;
	srcTop += srcRecLeft;
	pBitPlane  = bitMap->base + srcTop;	


	//complete the bitmap from top to bottom
	for(; recHeight > 0; recHeight--)
	{
		mask = leftMask;
		//complete one row of the bitmap
		for(i = rowWords; i > 0; i--)
		{
			//get word from the bitMap
			oldWord = *pBitPlane;
			newWord = ((oldWord & ~mask) | (~oldWord & mask));

			//replace oldWord with newWord in the bitMap
			*pBitPlane++ = newWord;
	
			//first word done, mask no longer needed till endpixel is reached
			mask = 0xFFFF;
	 		if(i == 2)		//if we are at the end of the row, add the right mask
			{
				mask = rightMask;
			}
		}

		pBitPlane += delta;
	}

}
 


/* This function set the area in the specified bitmap bounded by the
   specified rectangle to the specified value.*/

void RectSetOP(UltimaBitMap *bitMap, UltimaRectangle *rect, UINT16 value)
{
UINT16 leftMask, rightMask, rowWords,  mask, oldWord, newWord;
UINT16 *pSrcBitPlane, i;
UINT8 endPixel, startPixel;
INT32  srcTop, srcRecLeft;
INT16 recWidth, recHeight, delta;
   
	//determine width of the rectangle
	recWidth = rect->right - rect->left;
	if(recWidth <= 0)
	{
		return;      //error
	}

	//determine height of the rectangle
	recHeight = rect->bottom - rect->top;
	if(recHeight <= 0)
	{
		return;      //error
	}

	// Verify the rectangle is within the bitmap's bounds
	if ( (rect->top < bitMap->bounds.top || bitMap->bounds.bottom <= rect->top || bitMap->bounds.bottom < rect->bottom) ||
		(rect->left < bitMap->bounds.left || bitMap->bounds.right <= rect->left || bitMap->bounds.right < rect->right) )
	{
		return;      //error
	}

	//determine which bit within the ending word is the endpixel
	endPixel = (UINT8)(oldPixelsPerWord - ((rect->right - bitMap->bounds.left - 1) & 0x0f));
	rightMask = ~(oldRightMasks[endPixel]);	//maskoff the slop that is left over in the word

	//determine which bit within the first word is the startpixel
	startPixel = (UINT8)((oldPixelsPerWord + 1) - ((rect->left - bitMap->bounds.left) & 0x0f));
  	leftMask = oldRightMasks[startPixel];		//maskoff the slop that is left over in the word

	if(recWidth <= startPixel)
	{
		rowWords = 1;	
		leftMask &= rightMask; 
	}
	else
	{
		rowWords = ((recWidth - startPixel - 1) >> 4) + 2;
	}

	delta = bitMap->pitch - rowWords;

	srcTop = rect->top - bitMap->bounds.top;																	
	srcTop = srcTop * bitMap->pitch;
	srcRecLeft = rect->left - bitMap->bounds.left;
	srcRecLeft >>= 4;
	pSrcBitPlane  = bitMap->base + srcTop + srcRecLeft;

	//complete the bitmap from top to bottom
	for(; recHeight > 0; recHeight--)
	{	
	 	mask = leftMask;
		for(i = rowWords; i > 0; i--)
		{
			oldWord = *pSrcBitPlane;
		   	newWord = ((mask & value) | (~mask & oldWord));

		   	*pSrcBitPlane++ = newWord;

			//discard the left mask value
			mask = 0xFFFF;
			//are we on the last word in the row?
			if(i == 2)
			{
				mask = rightMask;
			}

		}

		pSrcBitPlane += delta;
	}
}


/* This function is a BITBLT function that will transfer a rectangle of
   graphical data from one bitmap to another, exclusive ORing the source
   with the destination.*/	  

void CopyXOP(UltimaBitMap *srcBitmap, UltimaRectangle *srcRect, UltimaBitMap *destBitmap, UltimaRectangle *destRect)
{
UINT16 srcWidth, srcHeight, skew, rightMask, leftMask, srcDelta, destDelta;
UINT16 i, newWord, rowWords, destWord, mask;
UINT16 *srcBitPlanePtr, *destBitPlanePtr;
INT32 srcTop, srcRecLeft, destTop, destRecLeft;
UINT8 endPixel, startPixel;
BOOL preLoad = FALSE;
splitUINT32 spLong, temp; 

	//initialize to get rid of warning message
	spLong.split.left = 0;
	spLong.split.right = 0;

	//deterine width of the source rectangle.00000000
	srcWidth = srcRect->right - srcRect->left;
	if(srcWidth <= 0)
	{
		//error
		return;
	}

	//deterine height of the source rectangle
	srcHeight = srcRect->bottom - srcRect->top;
	if(srcHeight <= 0)
	{
		//error
		return;
	}

	skew = ((srcRect->left - destRect->left + destBitmap->bounds.left) - srcBitmap->bounds.left) & 0x0F;

	endPixel = (UINT8)(oldPixelsPerWord - ((destRect->left - destBitmap->bounds.left + srcWidth - 1) & 0x0f));
 
    //deterine mask for right side of the rectangle after the endpixel this will be the last to be loaded
	rightMask = ~(oldRightMasks[endPixel]);

	//added 1 becuase assembly listing shows pixelsperword is 16 for the start pixel only
	startPixel = (UINT8)((oldPixelsPerWord + 1) -  ((destRect->left - destBitmap->bounds.left) & 0x0f));

	leftMask = oldRightMasks[startPixel];
	if (srcWidth <= startPixel)
	{
		rowWords = 1;
		leftMask &= rightMask;
	}
	else
	{
		rowWords = ((srcWidth - startPixel - 1) >> 4) + 2;
	}

	if((skew != 0) && (((destRect->left - destBitmap->bounds.left) & 0x0f) <= ((srcRect->left - srcBitmap->bounds.left) & 0x0f)))
	{
		preLoad = TRUE;
	}

	//explicitly do this to get rid of warning message
	UINT16 boolval = 0;
	if (preLoad)
		boolval = 1;
	
	srcDelta = srcBitmap->pitch - rowWords - boolval;

	destDelta = destBitmap->pitch - rowWords; 

	srcTop = srcRect->top - srcBitmap->bounds.top;																	
	srcTop = srcTop * srcBitmap->pitch ;
	srcRecLeft = srcRect->left - srcBitmap->bounds.left;
	srcRecLeft >>= 4;
	srcBitPlanePtr  = srcBitmap->base + srcTop + srcRecLeft;	

	destTop = destRect->top - destBitmap->bounds.top;																	
	destTop = destTop * destBitmap->pitch;
	destRecLeft = destRect->left - destBitmap->bounds.left;
	destRecLeft >>= 4;
	destBitPlanePtr = destBitmap->base + destTop + destRecLeft;																   

	if(skew == 0)
	{
		skew = 16;
	}

	//complete the bitmap from top to bottom
	for (; srcHeight > 0; srcHeight--)
	{
		if(preLoad == TRUE)
		{
		 spLong.dword = (*srcBitPlanePtr++);
		}

		mask = leftMask;
		//complete a row of the bitmap
		for(i = rowWords; i > 0; i--)
		{
			spLong.split.left = spLong.split.right;	 //swap words
			spLong.split.right = *srcBitPlanePtr++;
			temp.dword = ((UINT32)((UINT32)spLong.dword) << skew);
			newWord = temp.split.left;

			destWord = *destBitPlanePtr;  
    		*destBitPlanePtr++ = ((mask & (newWord ^ destWord)) | (~mask & destWord));
			mask = 0xffff;

			//are we on the last word in the row?
			if (i == 2)
			{
				mask = rightMask;
			}		
		}

		srcBitPlanePtr += srcDelta;
		destBitPlanePtr += destDelta;
	} 

}




