#include "PAL_platformheader.h"
//#include <windowsx.h> // replaced by "PAL" header


#include <string>
#include <stdio.h>
#include <wchar.h>
#include "Renderer.h"
#include "ErrorHandling.h"



#include "Rectangle.h"
#include "ImageControl.h"
#include "FormRecord.h"
#include "LabelStockDescriptor.h"
#include "BitMap.h"
#include "Pen.h"
#include "Label.h"
#include "Point.h"
#include "BitBlt.h"
#include "DrawingPrimitives.h"
#include "Barcode.h"
#include "UpcBarcode.h"
#include "UpcD3Barcode.h"
#include "EanBarcode.h"
#include "Rss14.h"
#include "Ean128CBarcode.h"
#include "QRBarcode.h"
#include "DataMatrixBarcode.h"
#include "WordWrapLineMetrics.h"
#include "CSmartFontHandle.h"
#include "CRendererUtil.h"

#include "Configuration.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifndef PAL_WIN32_RUN

#include <assert.h>
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/tttables.h>
#include <freetype/fttypes.h>
#include <freetype/ftbitmap.h>
#include <freetype/ftwinfnt.h>

// FT-911 moved global variables into method

#endif // !PAL_WIN32_RUN


/* //sjjc 
#define BLACK			(0)
#define WHITE			(1)		//changed ats 07192002
#define TRANS			(255)
#define INVERT			(254)
*/

/*lcg rev 0.1 struct _colorStruct
{
	UGL_RGB rgbColor;
	UGL_COLOR uglColor;
} colorTable[] =
    {
	 { UGL_MAKE_RGB(0, 0, 0), 0},
	 { UGL_MAKE_RGB(255, 255, 255), 0}
	 };
*/




using namespace std;



// User passes in a reference to a Rectangle, which will be set up by this 
bool determineRectangleForField(const ImageControl *ic, FormRecord *form, Hobart::Rectangle &theRect)
{
	bool isTracing = Debug::isTracing("Renderer.Rectangle");

	Debug::trace("Renderer.renderField", "entered determineRectangleForField ... for form number " + util_string::inttostring(form->getFieldType()));

	bool rectValid = true;

	UINT16 verticalOffset = ic->getOffsetFromTable(form->getOriginVertical());

	//create a rectangle and give it size
	Hobart::Rectangle imageRect(form->getOriginVertical(),
    			                form->getOriginVertical() + form->getSizeVertical(),
    			                form->getOriginHorizontal() + form->getSizeHorizontal() + ic->getTotalFormHorizOffset(),
    			                form->getOriginHorizontal() + ic->getTotalFormHorizOffset());

	Hobart::Rectangle rect(ic->getImageBitMapBoundsTop(),
				            ic->getImageBitMapBoundsBottom(),
				            ic->getHorizFormWidth() + ic->getCenteringOffset(),
				            0);

	if ( isTracing )
	{
		Debug::trace("Renderer.Rectangle", "Label Form:");
		imageRect.show();

		Debug::trace("Renderer.Rectangle", "Label Stock:");
		rect.show();
	}

	if ( !imageRect.doesOverlap(rect) )
		rectValid = false;
	else
	{
		const Hobart::Rectangle imageRectBeforeClip( imageRect );

		imageRect.clipRect( &rect );

		if (( isTracing ) && ( imageRect != imageRectBeforeClip ))
		{
			Debug::trace("Renderer.Rectangle", "After Clipping:");
			imageRect.show();
		}

		// Find farthest South field on the label
		if (ic->getImageLines() < imageRect.getRectangleBottom())
		{
			const_cast <ImageControl *> (ic)->setImageLines(imageRect.getRectangleBottom());
			Debug::trace("Renderer.renderField", "image lines " + util_string::inttostring(imageRect.getRectangleBottom()));
		}

		if ( ic->isImageOptionsScrunch() )
		{
			if (form->isFormFlagFloating())
			{
				Debug::trace("Renderer", HobString("Scrunching Field: ") + HobString(Integer::toString((UINT16) form->getFieldType())));
				imageRect.offsetRectangle(0, -verticalOffset);
			}
			else if (verticalOffset)
			{
				// We are supposed to move this field up, since "Scrunch" is true, but this field is not set as floating.
				// Therefore, deactivate the scrunching mechanism by setting the offset in the offset table for this field to be 0.
				// We can also just turn scrunching off, since we will now consider that no field should be moved.
				const_cast<ImageControl*>( ic )->updateOffsetTable( form->getOriginVertical(), 0 );

				// Right now, we don't have a real good way of just turning off the scrunch, so I have to turn off all (set to 0)
				const_cast<ImageControl*>( ic )->setImageOptions( 0 );
			}
		}
		else if( form->isFormFlagStretch() )
		{
			Debug::trace("Renderer", HobString("Stretching Field: ") + HobString(Integer::toString((UINT16) form->getFieldType())));
			imageRect.stretchRectangle(verticalOffset);
		}

		//check the dimension of the rectangle to insure we are still in bounds
		//this should probably be in the rectangle class ats 04012002
		if ( (imageRect.getRectangleLeft() < 0) ||
			 (imageRect.getRectangleTop() < 0) ||
			 (imageRect.getRectangleRight() < 0) ||
			 (imageRect.getRectangleBottom() < 0) )
		{
			Debug::trace("Renderer.Error", "Invalid Rectangle");
			imageRect.show();

			rectValid = false;
		}

		if (!rectValid && imageRect.getRectangleBottom() > ic->getImageBitMapBoundsBottom())
		{
			Debug::trace("Renderer.Error", "Invalid Rectangle -- Bottom is greater than the BitMap Bounds Bottom");
			imageRect.show();

			rectValid = false;
		}
	}

	theRect = imageRect;

	if (!rectValid)
	{
		theRect.setRectangleLeft(0);
		theRect.setRectangleTop(0);
		theRect.setRectangleRight(0);
		theRect.setRectangleBottom(0);
	}

	return rectValid;
}


void erasePostImageFormFields( const ImageControl *ic )
{
	if ( ic->isImageFlagPostImage() )
	{
		BitMap* pDestBitMap = ic->getImageBitMap();

		ic->initFormListIter();
		while ( !ic->isCurrentFormNull() )	//added to process the whole label
		{	
			FormRecord* pForm = ic->getFormFromList();

			if ( pForm && pForm->isFormPostImage() && pDestBitMap )
			{
				Hobart::Rectangle imageRect;
				bool isValidRect = determineRectangleForField(ic, pForm, imageRect);
				if (isValidRect)
				{
					/*need to finish ats 06202002 */
					//erase area of the bitmap at the location of the rectangle and for the size of the rectangle
					eraseRectangle(pDestBitMap, &imageRect, 0x0000);
				}
			}
		}
	}
}


// The purpose of this function is to update "pDestBitMap" (the label's image), which is
// managed by "ic" (the image control structure/object), by rendering "form" over it.
UINT16 renderField(const ImageControl *ic, FormRecord *form, BitMap *pDestBitMap,
                   const LabelStockDescriptor* embeddedLSD, bool subtractDescent)
{
	Debug::trace("Renderer.renderField", "entered ... to render form number " +
			util_string::inttostring(form->getFieldType()));


    if ( (ic->getImageFlags() & form->getFlag()) != 0 || form->getFieldType() == INDIRECT_LABEL_REFERENCE )
    {
		UINT16 verticalOffset = ic->getOffsetFromTable(form->getOriginVertical());

		Hobart::Rectangle imageRect;
		bool isValidRect = determineRectangleForField(ic, form, imageRect);

		if (isValidRect)
		{
			Hobart::Rectangle fieldRect;
	        fieldRect = imageRect;

			if ( form->getFieldType() != INDIRECT_LABEL_REFERENCE )
			{
				switch (form->getFieldRotation())
				{
					case 0:
						imageRect.addToRectangleTop(form->getVerticalJustify());
						break;
					case 90:
						imageRect.addToRectangleLeft(form->getVerticalJustify());
						break;
					case 180:
						imageRect.subtractFromRectangleBottom(form->getVerticalJustify());
						break;
					case 270:
						imageRect.subtractFromRectangleRight(form->getVerticalJustify());
						break;
				}
			}

			if ( form->isFormFlagLineEntity() )
			{
				Pen *newPen = ic->getPen();				//new pen to be resized to draw the line
				Point size(newPen->getPenSize());	//saved pen size, so that we can return the pen to its proper size
				newPen->setPenState(Down);
				newPen->setPenColor(Black);	/* for test only ats 06172002 */
				newPen->setPenSize(form->getFieldSubType(), form->getFieldSubType());

				//create the start and end points for the line we are drawing
				Point startPoint(imageRect.getRectangleLeft(), imageRect.getRectangleTop());
				Point endPoint(imageRect.getRectangleRight(), imageRect.getRectangleBottom());

				drawLine(&startPoint, &endPoint, pDestBitMap, ic->getPen());

				newPen->setPenSize(size.getHorizontal(), size.getVertical());
				fieldRect.addToRectangleBottom(form->getFieldSubType());
			}

			if (form->isFormFieldTextEntity() == true)
			{
				UINT16 border = /*form->isFormFlagRectBorder() ? 2 :*/ 0;
				UINT16 yTop = imageRect.getRectangleTop() + border;
				UINT16 yMax = imageRect.getRectangleBottom() - border - yTop;

				if (form->isDataPresent() == true)
				{
					Hobart::Rectangle saveRect = imageRect;
					BitMap *pBitmap = NULL;
					BitMap tempBitMap;
					BitMap txt_bmp; // FT-911

#if defined(PAL_WIN32_RUN)
					HBITMAP winBitmap = NULL;
#endif
					Util::CSmartFontHandle fontId;
					TEXTMETRIC tm;
                #if defined(PAL_WIN32_RUN)
					HDC fontDrvId = CreateCompatibleDC(NULL);
					Assert(fontDrvId != NULL, Util::CNTException());
					Assert(GetMapMode(fontDrvId) == MM_TEXT, Util::CNTException());
                #endif
					Debug::trace("Renderer.renderField", "rendering text field ...");

///
//					UINT32 width = imageRect.getRectangleRight() - imageRect.getRectangleLeft();
///
					UINT32 height = imageRect.getRectangleBottom() - imageRect.getRectangleTop();
///
//					INT16 bitMapPitch = (INT16)((width - 1) / 16 + 1);
///

					if ( form->getFieldRotation() == 0 )
					{
						pBitmap = pDestBitMap;

			#if defined(PAL_WIN32_RUN)
						winBitmap = CreateBitmap(ic->getImageBitMapWidth(), ic->getImageBitMapHeight(), 1, 1, 
												/* //sjjc pBitmap->getBitMapBase()*/NULL);
						Assert(winBitmap != NULL, Util::CNTException());
			#else
						//
						//Allocate buffer to hold the string bitmap
						//
//#ifdef ORIG_TXT_BMP
#if 1
						INT16 pitch = ((ic->getImageBitMapWidth()-1) / BMP_IDX_WIDTH)+1;
						txt_bmp.allocateBitMap(pitch*ic->getImageBitMapHeight());	
						txt_bmp.setBitMapBounds(0, ic->getImageBitMapWidth(), 0, ic->getImageBitMapHeight());
						txt_bmp.setBitMapPitch(pitch);
#else
						imageRect = Hobart::Rectangle(0, imageRect.getRectangleBottom() - imageRect.getRectangleTop(),
										  imageRect.getRectangleRight() - imageRect.getRectangleLeft(), 0);

						UINT32 width = imageRect.getRectangleRight() - imageRect.getRectangleLeft();
						INT16 bitMapPitch = (INT16)((width - 1) / 16 + 1);
						INT32 tmp_hght = (imageRect.getRectangleBottom() - imageRect.getRectangleTop());

						tempBitMap.allocateBitMap(height * bitMapPitch);
						tempBitMap.setBitMapBounds(imageRect.getRectangleLeft(),
													imageRect.getRectangleRight(),
													imageRect.getRectangleTop(),
													imageRect.getRectangleBottom());
						tempBitMap.setBitMapPitch(bitMapPitch);

						pBitmap = &tempBitMap;

						txt_bmp.allocateBitMap(bitMapPitch * height/*tmp_hght*/);	
						txt_bmp.setBitMapBounds(0,bitMapPitch*16, 0, height/*tmp_hght*/);
						txt_bmp.setBitMapPitch(bitMapPitch);
#endif
			#endif
					}
					else
					{
						if ( form->getFieldRotation() == 180 )
						{
							imageRect = Hobart::Rectangle(0, imageRect.getRectangleBottom() - imageRect.getRectangleTop(),
											  imageRect.getRectangleRight() - imageRect.getRectangleLeft(), 0);
						}
						else // Rotation == 90 || Rotation == 270
						{
							imageRect = Hobart::Rectangle(0, imageRect.getRectangleRight() - imageRect.getRectangleLeft(),
											  imageRect.getRectangleBottom() - imageRect.getRectangleTop(), 0);
						}

						// set up bitmap attributes

						UINT32 width = imageRect.getRectangleRight() - imageRect.getRectangleLeft();
						INT16 bitMapPitch = (INT16)((width - 1) / 16 + 1);


						tempBitMap.allocateBitMap(height * bitMapPitch);
						tempBitMap.setBitMapBounds(imageRect.getRectangleLeft(),
													imageRect.getRectangleRight(),
													imageRect.getRectangleTop(),
													imageRect.getRectangleBottom());
						tempBitMap.setBitMapPitch(bitMapPitch);

						pBitmap = &tempBitMap;
#if 0
Debug::trace("Renderer.TextOut", "pBitmap right is " + inttostring(pBitmap->getBitMapBoundsRight()) + " pBitmap's left " + 
	inttostring(pBitmap->getBitMapBoundsLeft()) + " pBitmap top is " + inttostring(pBitmap->getBitMapBoundsTop()) + " pBitmap bottom is " +
	inttostring(pBitmap->getBitMapBoundsBottom()));

Debug::trace("Renderer.TextOut", "imageRect right is " + inttostring(imageRect.getRectangleRight()) + " imageRect's left " + 
	inttostring(imageRect.getRectangleLeft()) + " imageRect top is " + inttostring(imageRect.getRectangleTop()) + " imageRect bottom is " +
	inttostring(imageRect.getRectangleBottom()));
#endif

/* //sjjc					renderMode.pFramBuffer = (UGL_UINT8 *) pBitmap->getBitMapBase();
						renderMode.uglMode.height = imageRect.getRectangleBottom() - imageRect.getRectangleTop();
						renderMode.uglMode.width =  bitMapPitch * 16;
*/
			#if defined(PAL_WIN32_RUN)
						winBitmap = CreateBitmap(bitMapPitch * 16, imageRect.getRectangleBottom() - imageRect.getRectangleTop(), 1, 1, /* //sjjc pBitmap->getBitMapBase()*/NULL);
						Assert(winBitmap != NULL, Util::CNTException());
			#else
						//
						//Allocate buffer to hold the string bitmap
						//
#ifdef ORIG_TXT_BMP
//
//This section of the ifdef should be deleted after
//QA
//
						INT16 pitch = ((ic->getImageBitMapWidth()-1) / BMP_IDX_WIDTH)+1;
						txt_bmp.allocateBitMap(pitch*ic->getImageBitMapHeight());	
						txt_bmp.setBitMapBounds(0, ic->getImageBitMapWidth(), 0, ic->getImageBitMapHeight());
						txt_bmp.setBitMapPitch(pitch);
#else
						INT32 tmp_hght = (imageRect.getRectangleBottom() - imageRect.getRectangleTop());
						INT16 pitch = bitMapPitch;
						txt_bmp.allocateBitMap(pitch * tmp_hght);	
						txt_bmp.setBitMapBounds(0, pitch*16, 0, tmp_hght);
						txt_bmp.setBitMapPitch(pitch);
#endif
			#endif

						if (Debug::isTracing("Renderer.Rectangle") == true)
						{
							Debug::trace("Renderer.Rectangle", "Rendering text field onto rotated rectangle:");
							imageRect.show();
						}
					}



		#ifdef PAL_WIN32_RUN
					HBITMAP winBitmapOld = SelectBitmap(fontDrvId, winBitmap);
					Assert( winBitmapOld != NULL, Util::CNTException());

					SetTextColor(fontDrvId, COLOR_BLACK);
					SetBkMode(fontDrvId, TRANSPARENT);
//					SetBkColor(fontDrvId, COLOR_WHITE);

					fontId = Util::CSmartFontHandle(fontDrvId, form->getFamilyName(), form->getPointSize());
		#else
					//
					//build CSmarFontHandle upon the freetype library; Windows version builds 
					//same upon the CDC. 
					//
					fontId = Util::CSmartFontHandle(form->getFamilyName(), form->getPointSize());
		#endif
					if ( fontId.null() )
//					if ((fontId = Hobart::CRendererUtil::CreatePrinterFont(fontDrvId, form->getFamilyName(), form->getPointSize())) == NULL)
					{
						Debug::trace("Renderer.Error", "font not found");
						Debug::trace("Renderer.renderField", "exiting 1 ...");
						return ic->getImageLines();
					}
					else
						Debug::trace("Renderer.renderField", "set font to be faceName=" + HobString(form->getFamilyName())
								+ ";pixelSize=" + HobString(Integer::toString(form->getPointSize())));


		#ifdef PAL_WIN32_RUN
					SelectFont(fontDrvId, fontId.Get());
					GetAndFixTextMetrics(fontDrvId, &tm);
					WordWrapLineMetrics wordWrapLineMetrics(fontDrvId, fontId, (UINT16)(tm.tmAscent - tm.tmExternalLeading));
		#else
					//.
					GetAndFixTextMetrics(fontId, &tm);
					WordWrapLineMetrics wordWrapLineMetrics(fontId, (UINT16)(tm.tmAscent - tm.tmExternalLeading));
		#endif
					wordWrapLineMetrics.addWhiteSpaceChar(' ');
					wordWrapLineMetrics.addWhiteSpaceChar('\t');

					if ( form->getFieldType() == REPORT_STREAM )
					{
						Debug::trace("Renderer.stream", 20, "Rendering text onto a REPORT_STREAM form. Text is\n" +
								HobString((const char*)form->getStream()));

						wordWrapLineMetrics.setTextStart((const char*)form->getStream());
					}
					else
					{
						Debug::trace("Renderer.renderField", "renderField():  Going to render text [" + std::string((const char*)form->getData()) + "]");

						wordWrapLineMetrics.setTextStart((const char *) form->getData());
					}

					border = /*form->isFormFlagRectBorder() ? 2 :*/ 0;
					UINT16 xMax = imageRect.getRectangleRight() - border - (imageRect.getRectangleLeft() + border);
					yTop = imageRect.getRectangleTop() + border;
					yMax = imageRect.getRectangleBottom() - border - yTop;

					Debug::trace("Renderer.text", "renderField():  Rendering text onto form with top at " + 
							HobString(Integer::toString(yTop)) + " and max at " + HobString(Integer::toString(yMax)));

					// Process each line of text and render it if it fits in the space available
					bool firstLine = true, done = false;
					while ( !done )
					{
						UINT16 blockOffset = 0;

						// Since the Report Stream is preformatted to indent, do not remove whitespace
						if ( form->getFieldType() != REPORT_STREAM )
						{
							wordWrapLineMetrics.removeLeadingWhitespace();
						}
						drawWordWrapLine(xMax, yMax, &wordWrapLineMetrics, subtractDescent);
						wordWrapLineMetrics.removeTrailingWhitespace(); // Not trivial!!

						
						// print the line of text
						char *pChar = (char *) wordWrapLineMetrics.getTextStart();
						int i = 0;          // i = actual position in string
						INT16 horTextOffset = 0;
						switch ( form->getHorizontalJustify() )
						{
							case CenterJustify:
							{
								horTextOffset = (xMax - wordWrapLineMetrics.getLineLengthInPixels()) / 2;

								if ( horTextOffset < 0 )
									horTextOffset = 0;

								break;
							}
							case RightJustify:
							{
								horTextOffset = xMax - wordWrapLineMetrics.getLineLengthInPixels();

								if ( horTextOffset < 0 )
									horTextOffset = 0;

								break;
							}
							case LeftJustify:
							default:
								break;
						}

						int lineHeight = wordWrapLineMetrics.getMaxLineHeightInPixels();
						if ( lineHeight == 0 && wordWrapLineMetrics.getLineLengthInChars() <= 1 )
						{
							TEXTMETRIC tm;
#ifdef PAL_WIN32_RUN
							GetAndFixTextMetrics(fontDrvId, &tm);
#else
							GetAndFixTextMetrics(fontId, &tm);
#endif
							lineHeight = tm.tmHeight - tm.tmExternalLeading;
						}

						/* Here is where we now decide whether a line fits in the area allocated. */
//						if ( firstLine )
//						{
//							yTop = yTop - (wordWrapLineMetrics.getMaxLineAscentInPixels() - wordWrapLineMetrics.getMaxLineAscentWithoutLeadingInPixels());
//						}

						if ( yTop + lineHeight - (subtractDescent ? wordWrapLineMetrics.getMaxLineDescentInPixels() : 0) <= imageRect.getRectangleBottom() - border )
						{
							UINT16 lastCharLoc = 0;
							UINT16 fontLocation = 0;
							UINT16 fontAscent = 0;
							UINT16 ii = 0;
							UINT16 maxLineAscent = wordWrapLineMetrics.getMaxLineAscentInPixels();

							Debug::trace("Renderer.text", "renderField():  Rendering first line with top line at " + 
									HobString(Integer::toString(yTop + wordWrapLineMetrics.getMaxLineHeightInPixels())));

							while (ii < wordWrapLineMetrics.getNumFonts())
							{
								// check if  FontChangeSequence
								if (pChar[i] == ESC && pChar[i+1] == ESC)
								{
									i += skipFontChangeSequence((const char *) &pChar[i]);
								}

								// just print the entire line with the first font!!
								Util::CSmartFontHandle fontIdentifier = wordWrapLineMetrics.getFontId(fontLocation, lastCharLoc, fontAscent);
								UINT16 charBlockSize = lastCharLoc - fontLocation + 1;

								Debug::trace("Renderer.text", "renderField():  Going to SelectFont for fontLocation = " +
										uinttostring(fontLocation) + ", lastCharLoc = " + uinttostring(lastCharLoc) +
										", fontID = " + ptrtostring(fontIdentifier.Get()));

								Debug::trace("Renderer.text", "renderField():  Going to draw text \"" + string(&pChar[i]) +
										"\" (count: " + inttostring(charBlockSize) + ")");

                            #if defined(PAL_WIN32_RUN)
								Assert(SelectFont(fontDrvId, fontIdentifier.Get()) != NULL, Util::CNTException());
                            #endif


								if ( charBlockSize > 0 )
								{
									UINT16 lineOffset = maxLineAscent - fontAscent;

									// FT-885 fix crash
									if ( fontAscent > maxLineAscent )
									{
										lineOffset = 0;
									}

									/* We pass the baseline information to the Font Engine */
									if ( pChar[i + charBlockSize - 1] == '\n' || pChar[i + charBlockSize - 1] == '\t' )
									{
										if ( charBlockSize > 1 )
                                        {
	                            		#if defined(PAL_WIN32_RUN)
											Assert(TextOutA(fontDrvId, imageRect.getRectangleLeft() + border + horTextOffset + blockOffset, yTop + lineOffset, &pChar[i], charBlockSize - 1) != 0, Util::CNTException());
                                        #else
										{
											if (TextOut(fontIdentifier, imageRect.getRectangleLeft() + border + 
												horTextOffset + blockOffset, yTop + lineOffset, 
															&pChar[i], charBlockSize - 1, txt_bmp) != 0)
												Debug::trace("Renderer.Error","TextOut failed");
										}
                                        #endif
                                        }
									}
									else
                                    {
                            		#if defined(PAL_WIN32_RUN)
										Assert(TextOutA(fontDrvId, imageRect.getRectangleLeft() + border + horTextOffset + blockOffset, yTop + lineOffset, &pChar[i], charBlockSize) != 0, Util::CNTException());
	                                #else
									{
										if (TextOut(fontIdentifier, imageRect.getRectangleLeft() + border + 
											horTextOffset + blockOffset, yTop + lineOffset, 
														&pChar[i], charBlockSize, txt_bmp) != 0)
											Debug::trace("Renderer.Error","TextOut failed");
									}
                            		#endif
                                    }
								}

								i += charBlockSize;
								blockOffset = blockOffset + wordWrapLineMetrics.getBlockLengthInPixels(lastCharLoc);

								fontLocation = lastCharLoc + 1;
								ii++;

								/********** TO DO -- the code below at some point seg faults. 
								At this point, we have a meemory leak with this object. **********

								Debug::trace("Renderer.text", "fontIdentifier to Destroy:" +  util_string::ptrtostring(fontIdentifier.Get()));					
								PAL_FT_HANDLE ftHandle = fontIdentifier.Get();
								Debug::trace("Renderer.text", "Destroying face");
								PAL_absclass::PAL_sleep(50);									
								if(ftHandle->face != NULL)
								{
									FT_Done_Face(ftHandle->face);
								}

								Debug::trace("Renderer.text", "Destroying library");										
								PAL_absclass::PAL_sleep(50);							
								if(ftHandle->lib != NULL)
								{
									FT_Done_FreeType(ftHandle->lib);
								}
								**********/
							}

							yTop = yTop + wordWrapLineMetrics.getMaxLineHeightInPixels();
							yMax = imageRect.getRectangleBottom() - border - yTop;

							if (wordWrapLineMetrics.isTextAreaFull() == true)
							{
								done = true;
								if ( form->getFieldType() == REPORT_STREAM )
								{
									Debug::trace("Renderer.stream", 20, "Resetting REPORT_STREAM's stream to\n" +
											HobString(&pChar[i]));

									form->setStream((UINT8*)&pChar[i]);
								}
							}
							else
							{
								wordWrapLineMetrics.startNextLine();
								wordWrapLineMetrics.setTextStart((const char *) &pChar[i]);
							}
						}
						else
						{
							done = true;
							if ( form->getFieldType() == REPORT_STREAM )
							{
								Debug::trace("Renderer.stream", 20, "Resetting REPORT_STREAM's stream to\n" +
										HobString(&pChar[i]));

								form->setStream((UINT8*)&pChar[i]);
							}
						}

						firstLine = false;
					}
	
#if defined(PAL_WIN32_RUN)
					SelectBitmap(fontDrvId, winBitmapOld);
					Hobart::CRendererUtil::Draw(pBitmap, winBitmap);
#else

					Hobart::CRendererUtil::Draw(pBitmap, &txt_bmp);
#endif
					rotateImage(pBitmap, &imageRect, pDestBitMap, &saveRect, form->getFieldRotation());
					tempBitMap.freeBitMap();
					imageRect = saveRect;

#if defined(PAL_WIN32_RUN)
					DeleteDC(fontDrvId);
					DeleteObject(winBitmap);
#else
					txt_bmp.freeBitMap();
#endif
				}
				else
				{
					Debug::trace("Renderer.renderField", "renderField():  text field does not have data set");

					// SOF-2863
					// allow fields with no data to shrink
					yMax = form->getSizeVertical() - 1;
				}

				// Update the offset table if this text field is set as VARIABLE
				if (( form->isFormFlagVariable() ) && ( form->isFormPreImage() ))
				{
					const UINT16 delta( form->getSizeVertical() - yMax );

					Debug::trace("Renderer.renderField", "renderField(): variable field with"
							" origin " + inttostring(form->getOriginVertical()) +
							" top " + inttostring(yTop) +
							" max " + inttostring(yMax) +
							" delta " + inttostring(delta) +
							" voff " + inttostring(verticalOffset));

					if ( delta )
					{
						verticalOffset += yMax;

						// everything *AFTER* this variable control should be adjusted (hence the '+delta'...)
						const_cast<ImageControl*>( ic )->updateOffsetTable( form->getOriginVertical() + delta, verticalOffset );
					}
				}
			}

			if ((form->isFormFlagBarCodeEntity()) && (form->isDataPresent() == true))
			{
				Hobart::Rectangle saveRect = imageRect;
				BitMap *pBitmap = NULL;
				BitMap tempBitMap;
				BitMap txt_bmp; // FT-911

#if defined(PAL_WIN32_RUN)
				HBITMAP winBitmap = NULL;
				HDC fontDrvId;
#endif
				Util::CSmartFontHandle fontId;
				TEXTMETRIC tm;

            #if defined(PAL_WIN32_RUN)
				fontDrvId = CreateCompatibleDC(NULL);
				Assert(fontDrvId != NULL, Util::CNTException());
				Assert(GetMapMode(fontDrvId) == MM_TEXT, Util::CNTException());
            #endif

				if (form->getFieldRotation() == 0)
				{
					pBitmap = pDestBitMap;

					//this is the destination bitmap
		        #if defined(PAL_WIN32_RUN)
					winBitmap = CreateBitmap(ic->getImageBitMapWidth(), ic->getImageBitMapHeight(), 1, 1, NULL);
					Assert(winBitmap != NULL, Util::CNTException());
				#else
					//
					//Allocate buffer to hold the string bitmap
					//
					INT16 pitch = ((ic->getImageBitMapWidth()-1) / BMP_IDX_WIDTH)+1;
					txt_bmp.allocateBitMap(pitch*ic->getImageBitMapHeight());
					txt_bmp.setBitMapBounds(0, ic->getImageBitMapWidth(), 0, ic->getImageBitMapHeight());
					txt_bmp.setBitMapPitch(pitch);
                #endif
				}
				else
				{
					if (form->getFieldRotation() == 180)
					{
						imageRect = Hobart::Rectangle(0,
											  imageRect.getRectangleBottom() - imageRect.getRectangleTop(),
											  imageRect.getRectangleRight() - imageRect.getRectangleLeft(),
											  0);
					}
					else // Rotation == 90 || Rotation == 270
					{
						imageRect = Hobart::Rectangle(0,
											  imageRect.getRectangleRight() - imageRect.getRectangleLeft(),
											  imageRect.getRectangleBottom() - imageRect.getRectangleTop(),
											  0);
					}

					// set up bitmap attributes
					INT16 height = imageRect.getRectangleBottom() - imageRect.getRectangleTop();
					INT16 width = imageRect.getRectangleRight() - imageRect.getRectangleLeft();
					INT16 bitMapPitch = ((width - 1) / 16 + 1);

					tempBitMap.allocateBitMap(height * bitMapPitch);
					tempBitMap.setBitMapBounds(imageRect.getRectangleLeft(),
												imageRect.getRectangleRight(),
												imageRect.getRectangleTop(),
												imageRect.getRectangleBottom());
					tempBitMap.setBitMapPitch(bitMapPitch);

					pBitmap = &tempBitMap;
#if 0
Debug::trace("Renderer.TextOut", "pBitmap right is " + inttostring(pBitmap->getBitMapBoundsRight()) + " pBitmap's left " + 
	inttostring(pBitmap->getBitMapBoundsLeft()) + " pBitmap top is " + inttostring(pBitmap->getBitMapBoundsTop()) + " pBitmap bottom is " +
	inttostring(pBitmap->getBitMapBoundsBottom()));

Debug::trace("Renderer.TextOut", "imageRect right is " + inttostring(imageRect.getRectangleRight()) + " imageRect's left " + 
	inttostring(imageRect.getRectangleLeft()) + " imageRect top is " + inttostring(imageRect.getRectangleTop()) + " imageRect bottom is " +
	inttostring(imageRect.getRectangleBottom()));
#endif

/* //sjjc					renderMode.pFramBuffer = (UGL_UINT8 *) pBitmap->getBitMapBase();
					renderMode.uglMode.height = imageRect.getRectangleBottom() - imageRect.getRectangleTop();
					renderMode.uglMode.width =  bitMapPitch * 16; */
	        #if defined(PAL_WIN32_RUN)
					winBitmap = CreateBitmap(bitMapPitch * 16, height, 1, 1, /* //sjjc pBitmap->getBitMapBase()*/NULL);
					Assert(winBitmap != NULL, Util::CPlatformException());
			#else
					//
					//Allocate buffer to hold the string bitmap
					//
#ifdef ORIG_TXT_BMP
//
//This section of the ifdef should be deleted after 
//QA.
//
					INT16 pitch = ((ic->getImageBitMapWidth()-1) / 16)+1;
					txt_bmp.allocateBitMap(pitch*ic->getImageBitMapHeight());
					txt_bmp.setBitMapBounds(0, ic->getImageBitMapWidth(), 0, ic->getImageBitMapHeight());
					txt_bmp.setBitMapPitch(pitch);
#else
					txt_bmp.allocateBitMap(bitMapPitch*height);
					txt_bmp.setBitMapBounds(0, bitMapPitch*16, 0, height);
					txt_bmp.setBitMapPitch(bitMapPitch);
#endif
                #endif
				}

	    		#if defined(PAL_WIN32_RUN)
				
				HBITMAP winBitmapOld = SelectBitmap(fontDrvId, winBitmap);
				Assert( winBitmapOld != NULL, Util::CPlatformException());

				SetTextColor(fontDrvId, COLOR_BLACK);
				SetBkMode(fontDrvId, TRANSPARENT);
//				SetBkColor(fontDrvId, COLOR_WHITE);
				fontId = Util::CSmartFontHandle(fontDrvId, form->getFamilyName(), form->getPointSize());
	    		
			#else

				fontId = Util::CSmartFontHandle(form->getFamilyName(), form->getPointSize());

           		#endif

				if ( fontId.null() )
				{
					Debug::trace("Renderer.Error", "font not found");
					Debug::trace("Renderer.renderField", "exiting 1 ...");
					return ic->getImageLines();
				}
				else
					Debug::trace("Renderer.renderField", "set font to be faceName=" + HobString(form->getFamilyName())
							+ ";pixelSize=" + HobString(Integer::toString(form->getPointSize())));

	    #if defined(PAL_WIN32_RUN)
				SelectFont(fontDrvId, fontId.Get());
				GetAndFixTextMetrics(fontDrvId, &tm);
	    #else
				GetAndFixTextMetrics(fontId, &tm);
	    #endif

				Debug::trace("Renderer.renderField", "BarCode SubType=" + Integer::toString(form->getFieldSubType()));

				//what type of barcode is to be rendered
				switch ( form->getFieldSubType() )
				{
					case QR_LEVELM:
						// pass in the QRCode data to get rendered
						Debug::trace("Renderer.renderField", "QR(M) BarCode Data=" + HobString((char*)form->getData()));
						QRBarcodeAdapter::render( *pBitmap, imageRect, QRBarcodeAdapter::LevelM, (char*)form->getData() );
						break;
					case QR_LEVELL:
						// pass in the QRCode data to get rendered
						Debug::trace("Renderer.renderField", "QR(L) BarCode Data=" + HobString((char*)form->getData()));
						QRBarcodeAdapter::render( *pBitmap, imageRect, QRBarcodeAdapter::LevelL, (char*)form->getData() );
						break;
					case QR_LEVELQ:
						// pass in the QRCode data to get rendered
						Debug::trace("Renderer.renderField", "QR(Q) BarCode Data=" + HobString((char*)form->getData()));
						QRBarcodeAdapter::render( *pBitmap, imageRect, QRBarcodeAdapter::LevelQ, (char*)form->getData() );
						break;
					case QR_LEVELH:
						// pass in the QRCode data to get rendered
						Debug::trace("Renderer.renderField", "QR(H) BarCode Data=" + HobString((char*)form->getData()));
						QRBarcodeAdapter::render( *pBitmap, imageRect, QRBarcodeAdapter::LevelH, (char*)form->getData() );
						break;

					case DATAMATRIX:
						Debug::trace("Renderer.renderField", "DataMatrix BarCode Data=" + HobString((char*)form->getData()));
						DataMatrixBarcodeAdapter::render( *pBitmap, imageRect, (char*)form->getData() );
						break;

					case UPCA:
					{
						// PJR-143 RANDY TESTING REMOVED

						//get the font information for the barcode
						UpcBarcode *upcBarcode = new UpcBarcode(&imageRect,
																0,
																0, //form->getFieldRotation(),
																(INT16)tm.tmHeight, //sjjc fontInfo.height,
																(INT16)tm.tmExternalLeading, //sjjc fontInfo.leading,
																(INT16)tm.tmMaxCharWidth, //sjjc fontInfo.maxAdvance,
																(INT16)tm.tmAscent); //sjjc fontInfo.maxAscent);
						//draw the barcode
						if(! upcBarcode->isBarcodeBad())
						{
							Debug::trace("Renderer.BarCode", "Module size used is " + inttostring(upcBarcode->getModuleSize()));

							drawGuard(pBitmap, (GenericBarcode *)upcBarcode);
							upcBarcode->setCurrentTableUPCA();
#ifdef PAL_WIN32_RUN
							drawSymbols(pBitmap, (GenericBarcode *)upcBarcode, (UINT8 *)form->getData(), 6, 0, fontDrvId);
#else
							//
							//draw a symbol based on the fontId in linux.
							//
							drawSymbols(pBitmap, (GenericBarcode *)upcBarcode, (UINT8 *)form->getData(), 6, 0, fontId, txt_bmp);
#endif
							drawCenter(pBitmap, (GenericBarcode *)upcBarcode);
 							upcBarcode->setCurrentTableUPCB();
#ifdef PAL_WIN32_RUN
 							drawSymbols(pBitmap, (GenericBarcode *)upcBarcode, (UINT8 *)form->getData(), 6, 6, fontDrvId);
#else
 							drawSymbols(pBitmap, (GenericBarcode *)upcBarcode, (UINT8 *)form->getData(), 6, 6, fontId, txt_bmp);
#endif
 							drawGuard(pBitmap, (GenericBarcode *)upcBarcode);
						}
						else
						{
							Debug::trace("Renderer.Error", "Bad barcode");
						}

						delete upcBarcode;

						break;
					}

					case UPCD3:
					{
						/* we are not supporting UPCD3 stye barcode. This has not been tested ats 06262002 */
						UpcD3Barcode *upcD3Barcode = new UpcD3Barcode(&imageRect, 0, 0);//form->getFieldRotation());

						//create the hi side of the barcode	and draw hi side
						upcD3Barcode->createHiSide(0, form->getFieldRotation());

						if(! upcD3Barcode->isBarcodeBad())
						{
							Debug::trace("Renderer.BarCode", "Module size used is " + inttostring(upcD3Barcode->getModuleSize()));

							drawGuard(pBitmap, upcD3Barcode);
							upcD3Barcode->setCurrentTableUPCA();	//use 'A' table for the first three symbols
#ifdef PAL_WIN32_RUN
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 3, 0, fontDrvId);
#else
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 3, 0, fontId, txt_bmp);
#endif
							upcD3Barcode->setCurrentTableUPCC();	//change tables
#ifdef PAL_WIN32_RUN
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 3, 3, fontDrvId);
#else
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 3, 3, fontId, txt_bmp);
#endif
							drawCenter(pBitmap, (GenericBarcode *)upcD3Barcode);
							upcD3Barcode->setCurrentTableUPCB();	//change tables
#ifdef PAL_WIN32_RUN
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 6, 6, fontDrvId);
#else
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 6, 6, fontId, txt_bmp);
#endif
							drawGuard(pBitmap, upcD3Barcode);
						}
						else
						{
							Debug::trace("Renderer.Error", "Bad barcode");
						}
						//create and draw the low side of the barcode
						upcD3Barcode->createLowSide(0, form->getFieldRotation());
						if(! upcD3Barcode->isBarcodeBad())
						{
							drawGuard(pBitmap, upcD3Barcode);
							upcD3Barcode->setCurrentTableUPCC();	//change tables
#ifdef PAL_WIN32_RUN
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 1, 0, fontDrvId);
#else
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 1, 0, fontId, txt_bmp);
#endif
							upcD3Barcode->setCurrentTableUPCA();
#ifdef PAL_WIN32_RUN
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 2, 1, fontDrvId);
#else
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 2, 1, fontId, txt_bmp);
#endif
							upcD3Barcode->setCurrentTableUPCC();
#ifdef PAL_WIN32_RUN
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 1, 3, fontDrvId);
#else
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 1, 3, fontId, txt_bmp);
#endif
							drawCenter(pBitmap, (GenericBarcode *)upcD3Barcode);
							drawMargin(pBitmap, (GenericBarcode *)upcD3Barcode, form->getFieldRotation());
							drawCenter(pBitmap, (GenericBarcode *)upcD3Barcode);
							upcD3Barcode->setCurrentTableUPCA();
#ifdef PAL_WIN32_RUN
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 1, 4, fontDrvId);
#else
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 1, 4, fontId, txt_bmp);
#endif
							upcD3Barcode->setCurrentTableUPCC();
#ifdef PAL_WIN32_RUN
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 1, 5, fontDrvId);
#else
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 1, 5, fontId, txt_bmp);
#endif
							upcD3Barcode->setCurrentTableUPCA();
#ifdef PAL_WIN32_RUN
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 1, 6, fontDrvId);
#else
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 1, 6, fontId, txt_bmp);
#endif
							upcD3Barcode->setCurrentTableUPCC();
#ifdef PAL_WIN32_RUN
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 1, 7, fontDrvId);
#else
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 1, 7, fontId, txt_bmp);
#endif
							drawCenter(pBitmap, (GenericBarcode *)upcD3Barcode);
#ifdef PAL_WIN32_RUN
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 4, 8, fontDrvId);
#else
							drawSymbols(pBitmap, upcD3Barcode, (UINT8 *)form->getData(), 4, 8, fontId, txt_bmp);
#endif
							drawGuard(pBitmap, upcD3Barcode);
						}
						else
						{
							Debug::trace("Renderer.Error", "Bad barcode");
						}

						delete upcD3Barcode;

						break;
					}

					case EAN13:
					{
						EanBarcode *eanBarcode = new EanBarcode(&imageRect,
																0,
																0, //form->getFieldRotation(),
																(INT16)tm.tmHeight, //sjjc fontInfo.height,
																(INT16)tm.tmExternalLeading, //sjjc fontInfo.leading,
																(INT16)tm.tmMaxCharWidth, //sjjc fontInfo.maxAdvance,
																(INT16)tm.tmAscent); //sjjc fontInfo.maxAscent);
						//draw the barcode
						if(! eanBarcode->isBarcodeBad())
						{
							Debug::trace("Renderer.BarCode", "Module size used is " + inttostring(eanBarcode->getModuleSize()));

							UINT8 *symbolData = (UINT8 *) form->getData();
							INT16 tableRow = *symbolData - '0';


							//draw the first EAN symbol character
							Debug::trace("Renderer.BarCode", "EAN 13: rendering first symbol character '" + std::string((char*)symbolData, 1) +
								"' at (" + util_string::inttostring(eanBarcode->getBarcodePenHorizontal() - 7 * eanBarcode->getModuleSize()) + "," +
								util_string::inttostring(eanBarcode->getBarcodePenVertical() + eanBarcode->getSymbolHeight() + /*eanBarcode->getAscent() +*/ 1) + ")");


                        #if defined(PAL_WIN32_RUN)
							Assert(TextOutA(fontDrvId, eanBarcode->getBarcodePenHorizontal() - 7 * eanBarcode->getModuleSize(), eanBarcode->getBarcodePenVertical() + eanBarcode->getSymbolHeight() + /*eanBarcode->getAscent() +*/ 1, (char*)symbolData, 1) != 0, Util::CNTException());
                        #else
							if (TextOut(fontId, eanBarcode->getBarcodePenHorizontal() - 7 * eanBarcode->getModuleSize(), 
								eanBarcode->getBarcodePenVertical() + eanBarcode->getSymbolHeight() + 1, (char*)symbolData, 
											1, txt_bmp) != 0)
							{
								Debug::trace("Renderer.Error","TextOut failed");
							}
                        #endif
//							eanBarcode->moveBarcodePen((INT16)(tm.tmMaxCharWidth / 2), 0);

							drawGuard(pBitmap, eanBarcode);

							for(INT16 index = 0; index < 6; index++)
							{
								if(eanBarcode->getValueFromEANTable(tableRow, index) == true)
								{
									eanBarcode->setCurrentTableUPCA();
#if defined(PAL_WIN32_RUN)
									drawSymbols(pBitmap, eanBarcode, symbolData, 1, index + 1, fontDrvId);
#else
									drawSymbols(pBitmap, eanBarcode, symbolData, 1, index + 1, fontId, txt_bmp);
#endif
									eanBarcode->setCurrentTableEAN();
								}
								else
								{
									eanBarcode->setCurrentTableUPCC();
#if defined(PAL_WIN32_RUN)
									drawSymbols(pBitmap, eanBarcode, symbolData, 1, index + 1, fontDrvId);
#else
									drawSymbols(pBitmap, eanBarcode, symbolData, 1, index + 1, fontId, txt_bmp);
#endif
									eanBarcode->setCurrentTableEAN();
								}

							}

							drawCenter(pBitmap, eanBarcode);
							eanBarcode->setCurrentTableUPCB();
#if defined(PAL_WIN32_RUN)
							drawSymbols(pBitmap, eanBarcode, symbolData, 6, 7, fontDrvId);
#else
							drawSymbols(pBitmap, eanBarcode, symbolData, 6, 7, fontId, txt_bmp);
#endif
							drawGuard(pBitmap, eanBarcode);
						}
						else
						{
							Debug::trace("Renderer.Error", "Bad barcode");
						}

						delete eanBarcode;

						break;
					}

					case EAN128C: // --- ALV 
					{
						Ean128CBarcode *ean128CBarcode = new Ean128CBarcode(&imageRect,
																0,
																0, //form->getFieldRotation(),
																(INT16)tm.tmHeight, //sjjc fontInfo.height,
																(INT16)tm.tmExternalLeading, //sjjc fontInfo.leading,
																(INT16)tm.tmMaxCharWidth, //sjjc fontInfo.maxAdvance,
																(INT16)tm.tmAscent); //sjjc fontInfo.maxAscent);
						//draw the barcode
						if(!ean128CBarcode->isBarcodeBad())
						{
							Debug::trace("Renderer.BarCode", "Module size used is " + inttostring(ean128CBarcode->getModuleSize()));

							UINT8 *symbolData = (UINT8 *) form->getData();
							Debug::trace("Renderer.BarCode", "Symbol Data: " + std::string((const char*)form->getData()));
							Debug::trace("Renderer.BarCode", "Symbol Data: " + std::string((const char*)symbolData));
							//draw the EAN128C symbols
							Debug::trace("Renderer.BarCode", "EAN 128C: rendering first symbol character '" + std::string((char*)symbolData, 1) +
								"' at (" + util_string::inttostring(ean128CBarcode->getBarcodePenHorizontal() - 7 * ean128CBarcode->getModuleSize()) + "," +
								util_string::inttostring(ean128CBarcode->getBarcodePenVertical() + ean128CBarcode->getSymbolHeight() + 1) + ")");
#if defined(PAL_WIN32_RUN)
							Assert(TextOutA(fontDrvId, ean128CBarcode->getBarcodePenHorizontal(), ean128CBarcode->getBarcodePenVertical() + ean128CBarcode->getSymbolHeight() + 1, (char*)symbolData, (int)strlen((char*)symbolData)) != 0, Util::CNTException());
							//drawEAN128CSymbols(pBitmap,ean128CBarcode,symbolData,18,0,fontDrvId);
							drawEAN128CSymbols(pBitmap, ean128CBarcode, symbolData, (INT16)strlen((char*)symbolData), 0, fontDrvId);
#else
							if (TextOut(fontId, ean128CBarcode->getBarcodePenHorizontal(), ean128CBarcode->getBarcodePenVertical() + ean128CBarcode->getSymbolHeight() + 1, (char*)symbolData, strlen((char*)symbolData), txt_bmp) != 0)
							{
								Debug::trace("Renderer.Error","EAN128C TextOut failed");
							}
							
							//drawEAN128CSymbols(pBitmap, ean128CBarcode, symbolData, 18, 0, fontId);
							drawEAN128CSymbols(pBitmap,ean128CBarcode,symbolData, (INT16)strlen((char*)symbolData),0,fontId);
#endif
						}
						else
						{
							Debug::trace("Renderer.Error", "Bad barcode");
						}

						delete ean128CBarcode;

						break;
					} // --- ALV

					case RSS14E:					/* RSS barcode /w GTIN-14 */
			        case RSS14E_GTIN_13:            /* RSS barcode /w GTIN-13 (EAN-13) */
			        case RSS14E_GTIN_12:            /* RSS barcode /w GTIN-12 (UPC-A, UPC-E) */
			        case RSS14E_GTIN_8:             /* RSS barcode /w GTIN-8 (EAN-8) */
					case GS1_DATABAR_EXPANDED:
					{
						UINT8 *symbolData = (UINT8 *) form->getData();
						Debug::trace("Renderer.RSS14", "create new rss14 barcode");
						Rss14 rss14(pBitmap, &imageRect, symbolData, (INT16)strlen((char*)symbolData), (INT16)tm.tmHeight, (INT16)tm.tmExternalLeading, (INT16)tm.tmAscent, (INT16)tm.tmMaxCharWidth,  fontId, 2, 0, txt_bmp, ((form->getFieldSubType() == GS1_DATABAR_EXPANDED) ? true : false) );
						break;
					}
					default:
					{
							Debug::trace("Renderer.Error", "Invalid barcode type");
							break;
					}
				}

#if defined(PAL_WIN32_RUN)
				SelectBitmap(fontDrvId, winBitmapOld);
				Hobart::CRendererUtil::Draw(pBitmap, winBitmap);
#else
				Hobart::CRendererUtil::Draw(pBitmap, &txt_bmp);
//.				Hobart::CRendererUtil::Draw(pDestBitMap, &txt_bmp);
#endif

				if (form->getFieldRotation() != 0)
				{
#ifdef PAL_WIN32_RUN
					std::string filename = "RotatedBarcode.bmp";

					Hobart::CRendererUtil::SaveBitmapToFile( pBitmap, filename);
#endif
					rotateImage(pBitmap, &imageRect, pDestBitMap, &saveRect, form->getFieldRotation());
//					rotateImage(pDestBitMap, &imageRect, pDestBitMap, &saveRect, form->getFieldRotation());
					tempBitMap.freeBitMap();
					imageRect = saveRect;
				}

#if defined(PAL_WIN32_RUN)
				DeleteDC(fontDrvId);
				DeleteObject(winBitmap);
#else
				txt_bmp.freeBitMap();
#endif
			}
			else
			{
				if(form->isDataPresent() != true)
					Debug::trace("Renderer.Error", "Form data is NULL, Barcode will not print!!");
			}

			if ( (form->isFormFlagGraphicEntity() || form->getFieldType() == INDIRECT_LABEL_REFERENCE ) &&
					(form->isDataPresent() == true) )
			{
				const bool isRotated(( form->getFieldRotation() == 90 ) || ( form->getFieldRotation() == 270 ));
				const bool isVariable(( form->isFormFlagVariable() ) && ( form->isFormPreImage() ) && ( INDIRECT_LABEL_REFERENCE != form->getFieldType() ));

				Hobart::Rectangle destinationRectangle( imageRect );

				BitMap *sourceBitMap( form->getBitMapData() );
				Hobart::Rectangle sourceRectangle;

				if (( form->getFieldType() == INDIRECT_LABEL_REFERENCE ) && ( embeddedLSD ))
				{
					int hpos( form->getOriginHorizontal() );
					int vpos( form->getOriginVertical() );

					Debug::trace("Renderer.embeddedLabels", "Going to render embedded label into parent label");
					Debug::trace("Renderer.embeddedLabels", "Original form rectangle ((" +
							inttostring(hpos) + "," + inttostring(vpos) + "),(" +
							inttostring(hpos + form->getSizeHorizontal()) + "," + inttostring(vpos + form->getSizeVertical()) + "))");

					Debug::trace("Renderer.embeddedLabels", "Embedded LSD (0,0),(" +
							inttostring(embeddedLSD->getOriginalWidthDots()) + "," + inttostring(embeddedLSD->getLength()) + ")");

					sourceRectangle.setRectangleTop(0);
					sourceRectangle.setRectangleBottom(embeddedLSD->getLength());
					sourceRectangle.setRectangleRight(embeddedLSD->getOriginalWidthDots());
					sourceRectangle.setRectangleLeft(0);
				}
				else
				{
					sourceRectangle = sourceBitMap->getBitMapBounds();

					// SOF-2135
					// Update the offset table if this text field is set as VARIABLE
					if ( isVariable )
					{
						sourceBitMap->show();

						if ( Debug::isTracing("Renderer.Rectangle") )
						{
							Debug::trace("Renderer.Rectangle", "==== Have variable graphic... adjusting margins...");
							Debug::trace("Renderer.Rectangle", "==== Before:");
							sourceRectangle.show();
						}

						// adjust margins and get new rect
						sourceBitMap->adjustMargins();

						sourceRectangle = sourceBitMap->getBitMapBounds();

						if ( Debug::isTracing("Renderer.Rectangle") )
						{
							Debug::trace("Renderer.Rectangle", "==== After:");
							sourceRectangle.show();
						}
					}
				}

				//determine the source rectangle width and height
				INT16 sourceWidth( sourceRectangle.getWidth() );
				INT16 sourceHeight( sourceRectangle.getHeight() );
				INT16 destinationWidth( destinationRectangle.getWidth() );
				INT16 destinationHeight( destinationRectangle.getHeight() );

				// modify the destination rectangle based on how the form is to be imaged i.e. 0,90,180,270
				// form is rotated 90 or 270, bottom and top becomes the rectangle's width, right and left becomes height
				if ( isRotated )
					std::swap( destinationWidth, destinationHeight );

				if ( Debug::isTracing("Renderer.Rectangle") )
				{
					Debug::trace("Renderer.Rectangle", "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
					Debug::trace("Renderer.Rectangle", "Before Shrinking:");
					sourceRectangle.show();
					destinationRectangle.show();
				}

				if ( form->getFieldType() == INDIRECT_LABEL_REFERENCE )
				{
					Debug::trace("Renderer.embeddedLabels", "Destination rectangle before shrinking ((" +
							inttostring(destinationRectangle.getRectangleLeft()) + "," +
							inttostring(destinationRectangle.getRectangleTop()) + "),(" +
							inttostring(destinationRectangle.getRectangleRight()) + "," +
							inttostring(destinationRectangle.getRectangleBottom()) + "))");
				}

				//if the destination is smaller than the source than shrink the source rectangle to fit
				const INT16 tempWidth( destinationWidth - sourceWidth );
				const INT16 tempHeight( destinationHeight - sourceHeight );

				//resize the destination rectangle
				if ( tempWidth < 0 )
					sourceRectangle.shrinkRectangle((INT16)(-(tempWidth - 1) / 2), 0);   // Subtracting 1 makes diff "bigger"
				else if ( isRotated )
				{
					if ( isVariable )
						destinationRectangle.addToRectangleBottom( -tempWidth );
					else
						destinationRectangle.shrinkRectangle(0, tempWidth / 2);
				}
				else
					destinationRectangle.shrinkRectangle(tempWidth / 2, 0);

				if (tempHeight < 0)
					sourceRectangle.shrinkRectangle(0, (INT16)(-(tempHeight - 1) / 2));
				else if ( isRotated )
					destinationRectangle.shrinkRectangle(tempHeight / 2, 0);
				else
				{
					if ( isVariable )
						destinationRectangle.addToRectangleBottom( -tempHeight );
					else
						destinationRectangle.shrinkRectangle(0, tempHeight / 2);
				}

				if ( Debug::isTracing("Renderer.Rectangle") )
				{
					Debug::trace("Renderer.Rectangle", "After Shrinking:");
					sourceRectangle.show();
					destinationRectangle.show();
					Debug::trace("Renderer.Rectangle", "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
				}

				if ( isVariable )
				{
					const INT16 adj( isRotated ? tempWidth : tempHeight );

					// update image control
					if ( 0 < adj )
					{
						verticalOffset += adj;

						// everything *AFTER* this variable control should be adjusted (hence the '+adj'...)
						const_cast<ImageControl*>( ic )->updateOffsetTable( destinationRectangle.getRectangleTop() + adj, verticalOffset );
					}
				}

				//need to make the drawing primitives a class and construct a drawing primitive for graphics
				rotateImage(sourceBitMap, &sourceRectangle, pDestBitMap, &destinationRectangle, form->getFieldRotation());
			}

			if(form->isFormFlagRectBorder())
			{
				if((form->isFormFlagStretch()) || (form->isFormFlagVariable()))
				{
					drawFrame(pDestBitMap, &fieldRect, InvertFrame, ic->getPen());
				}
				else
				{
					if ( form->getFieldType() != INDIRECT_LABEL_REFERENCE )
					{
						switch(form->getFieldRotation())
						{
							case 0:
								imageRect.subtractFromRectangleTop(form->getVerticalJustify());
								break;
							case 90:
								imageRect.subtractFromRectangleLeft(form->getVerticalJustify());
								break;
							case 180:
								imageRect.addToRectangleBottom(form->getVerticalJustify());
								break;
							case 270:
								imageRect.addToRectangleRight(form->getVerticalJustify());
								break;
						}
					}

					drawFrame(pDestBitMap, &imageRect, InvertFrame, ic->getPen());
				}
			}
		}
		else
		{
			Debug::trace("Renderer.Rectangle", "Rectangles don't overlap");
			Debug::trace("Renderer.renderField", "field does not overlap ...");
		}
	}
	else
	{
		Debug::trace("Renderer.renderField", "field will not be rendered ...");
	}

	Debug::trace("Renderer.renderField", "exiting normal ...");
	return(ic->getImageLines());
//sjjc #endif //lcg rev 0.1 
//sjjc	return 0;
}




#if !defined (PAL_WIN32_RUN)



//////////////////////////////////////////////////////////////////////////////////////
//RenderBuf
//
void RenderBuf(FT_GlyphSlot slot, FT_Int &off_set, int32_t str_x, int32_t str_y,
		int32_t face_height, uint8_t starting_bit, BitMap &txt_bmp)
{
	bool isTracingRC = Debug::isTracing("Render.Clare");
	UINT16 *buf = txt_bmp.getBitMapBase();
	UINT32 bufSize = txt_bmp.getBitMapBaseSize();

	FT_Int i, j, p, q, bit_loc=0;

	FT_Int x_max = slot->bitmap.pitch;
	FT_Int y_max = slot->bitmap.rows;

	const int32_t hob_width = txt_bmp.getBitMapBoundsRight() - txt_bmp.getBitMapBoundsLeft();
	const int32_t x_loc = str_x;


	//
	//bottom up rendering
	//
	int32_t y_loc = str_y+face_height-1;

	FT_Int b_off = 0;

	//
	//for loops starts at the start of the bottom
	//most row and makes it way up through each 
	//iteration. First loop iterates the rows of 
	//the glyph.
	//
	for (i=p=0;p<y_max;i+=x_max,p++,y_loc++)
	{
		//
		//figure out the word in the buffer corresponding to the 
		//pixel (off_set) and start from the bit location passed in.
		//

		off_set = (hob_width * y_loc) + x_loc;
		off_set /= BMP_IDX_WIDTH;
		b_off = off_set;
		bit_loc = starting_bit;


		//
		//second loop iterates the columnbs of the glyph
		//
        for (j=i,q=0;q<x_max;q++,j++)
		{
			//
			//this loop is used to retrieve bits byte per byte
			//
			for (int32_t k=0;k<NUM_BITS;k++, bit_loc++)
			{
				//
				//The bits are being placed swapped within the word. Oring
				//prevents any existing bits in the buffer from being altered.
				//
				int8_t bit_swap = bit_loc > BYTE_SWAP_ZERO_BASED ? bit_loc-BYTE_SWAP : bit_loc+BYTE_SWAP; 

				if (slot->bitmap.buffer[j] & (0x80>>k))
					if ( b_off < bufSize )
						buf[b_off] |= (0x8000>>bit_swap);
	
				//
				//if we are at bit 15 then we are at the end
				//of the word already, move to next word and
				//make sure we update the first bit.  For loop
				//would increment bit_loc to 0 from -1.
				//
				if ((0 != bit_loc) && (0 == bit_loc%15))
				{
					b_off++;
					bit_loc = -1;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
//TextOut
//
// Routine makes the assumption that the fonts used are Horizantal and scalable.
//
// FT-495 Re-wrote this function to perform text metrics as per WINE implementation. I did not
// redo the height (y-axis) computation only the width (x-axis).
//
// WARNING: If you modify this funtion you will also need to change GetTextExtentPoint32A()
//
int32_t TextOut(
	Util::CSmartFontHandle fhndl,
	int32_t str_x,
	int32_t str_y,
	const char *data,
	int32_t count,
	BitMap &txt_bmp )
{
	PAL_FT_HANDLE ft_hndl = fhndl.Get();

	FT_Int str_width = 0;
	FT_Int str_height = 0;

	if ( NULL == ft_hndl )
	{
		Debug::trace("Renderer.TextOut", "Invalid font handle.");
		return -1;
	}
	if ( NULL == txt_bmp.getBitMapBase() )
	{
		Debug::trace("Renderer.TextOut", "Invalid destination handle.");
		return -1;
	}

	bool isTracing = Debug::isTracing("Renderer.TextOut");

	FT_Face face = ft_hndl->face;

	//
	//calculate the word and bit location for rendering and
	//save a local copy of the starting x & y coordinates.
	//
	FT_Int offset = (((txt_bmp.getBitMapBoundsRight()-txt_bmp.getBitMapBoundsLeft()) * str_y) + str_x)/BMP_IDX_WIDTH;

	int32_t x_coord = str_x;
	int32_t y_coord = str_y;
	uint32_t label_width = (txt_bmp.getBitMapBoundsRight()-txt_bmp.getBitMapBoundsLeft());
	uint8_t bit_loc = (((label_width * y_coord) + x_coord)%BMP_IDX_WIDTH);

	FT_GlyphSlot slot;
	FT_Error error;

	int32_t n;


	slot = face->glyph;

	//	FT-1001:  June 29, 2015
	//				We will make adjustments for a font that has glyphs that are heigher than they should be,
	//				as is the case with the sp font. That font changed when new characters were added but we
	//				have no apparent control over how the glyphs are set. We do not have the proper software and/or knowledge
	//				to adjust the font properties. So, when the render_height calculates to a negative number,
	//				the character/glyph will be imaged that many pixels plus one above the field rect.
	//				That is what caused the artifacts printing on subsequent labels.
	int32_t base_line = (face->size->metrics.ascender>>6);
	int32_t base_line_adjust = 0;		//	This will keep track of how high the string will go above the field rect (if at all).

	//	Go through the string first and make sure we are not printing chars higher than our limit.
	for (n = 0;n<count;n++)
	{
		//load glyph image into the slot (erase previous one)
		error = FT_Load_Char(face, data[n], FT_LOAD_RENDER|FT_LOAD_MONOCHROME|FT_LOAD_TARGET_MONO);
		if (error)
		{
			if (isTracing)
				Debug::trace("Renderer.TextOut", "Unable to create new font face.");
			return -1;
		}
		if (' ' != data[n])
		{
			int32_t render_height = (base_line - (slot->metrics.horiBearingY>>6) - 1);
			if (isTracing)
			{
				Debug::trace("Renderer.TextOut", "render_height = base_line - (slot->metrics.horiBearingY>>6) - 1 = (" + inttostring(base_line) +
						" - (" + longtostring(slot->metrics.horiBearingY) + ">>6) - 1 = " + inttostring(render_height));
			}
			if ((render_height < 0) && (render_height < base_line_adjust))
			{
				base_line_adjust = render_height;
				if (isTracing)
					Debug::trace("Renderer.TextOut", "base_line_adjust = render_height = " + inttostring(render_height));
			}
		}
	}
	if (base_line_adjust < 0)
	{
		int32_t temp_base_line = base_line - base_line_adjust;		//	This will force the base_line down the appropriate number of pixels.
		if (isTracing)
			Debug::trace("Renderer.TextOut", "base_line = base_line - base_line_adjust = " + inttostring(base_line) + " - " + inttostring(base_line_adjust) + " = " + inttostring(temp_base_line));
		base_line = temp_base_line;
	}

	for (n = 0;n<count;n++)
	{
		//load glyph image into the slot (erase previous one)
		error = FT_Load_Char(face, data[n], FT_LOAD_RENDER|FT_LOAD_MONOCHROME|FT_LOAD_TARGET_MONO);
		if (error)
		{
			if (isTracing)
				Debug::trace("Renderer.TextOut", "Unable to create new font face.");
			return -1;
		}

		string str;
		str = data[n];
		if (isTracing)
		{
			Debug::trace("Renderer.TextOut", "Src bmp Width " +
						inttostring(txt_bmp.getBitMapBoundsRight()-txt_bmp.getBitMapBoundsLeft()) +
						" Src bmp pitch " + inttostring(txt_bmp.getBitMapPitch()) + " Src bmp height " +
						inttostring(txt_bmp.getBitMapBoundsBottom()-txt_bmp.getBitMapBoundsTop()));
		}

		//
		//no rendering required if ' ' char encountered.
		//Only the width and coords need to be updated.
		//
		if (' ' != data[n])
		{
			//
			//we'll specify different depth for a letter part of which
			//renders below the base line as appose to the one which
			//only renders above it.
			//

			//
			//base_line is the line the pen is writing on.
			//Ascender is the heighest point above the base_line
			//that a glyph can render till for this font.
			//horiBearingY is the heighest point above the base_line
			//for this particular glyph.
			//render_height is calculated to be the point from
			//which we would start rendering this glyph (top down).
			//

			int32_t render_height = (base_line - (slot->metrics.horiBearingY>>6));

			//
			//add the x offset (distance between the pen pos
			//and the start of the char glyph).
			//
			int32_t char_x_bearing = (slot->metrics.horiBearingX>>6);

			RenderBuf(
				slot,
				offset,
				x_coord+char_x_bearing,
				y_coord,
				render_height,
				(bit_loc+char_x_bearing)%BMP_IDX_WIDTH,
				txt_bmp );
		}


		//
		//advance.x is the font specified location for the next
		//char glyph.  We'll calculate the new bit, the new x_coord
		//(to be used to determine the word to which the bit belongs)
		//and the string width. Also we are in font units and must
		//calculate the pixel equivalent hence the (>>6).
		//
		int32_t pen_advance=(slot->advance.x>>6);
		if (isTracing)
		{
			Debug::trace("Renderer.TextOut", "********************************************************");
			Debug::trace("Renderer.TextOut", "slot->advance.x: " + inttostring((int)slot->advance.x));
			Debug::trace("Renderer.TextOut", "pen_advance: " + inttostring((int)pen_advance));
		}

		x_coord += pen_advance;
		bit_loc += pen_advance;
		bit_loc %= BMP_IDX_WIDTH;

		str_width += pen_advance;

		str_height = (slot->bitmap.rows > str_height) ? slot->bitmap.rows : str_height;
		if (isTracing)
		{
			Debug::trace("Renderer.TextOut", "x_coord: " + inttostring((int)x_coord));
			Debug::trace("Renderer.TextOut", "bit_loc: " + inttostring((int)bit_loc));
			Debug::trace("Renderer.TextOut", "str_width: " + inttostring((int)str_width));
			Debug::trace("Renderer.TextOut", "str_height: " + inttostring((int)str_height));
			Debug::trace("Renderer.TextOut", "********************************************************");
		}

		if (isTracing)
		{
			Debug::trace("Renderer.TextOut", "About to print char [" +
					str + "]  width = " + inttostring((int) slot->advance.x>>6) + ", height = " +
					inttostring(slot->bitmap.rows) + ", off_set = " + inttostring(offset) + ", x offset passed = " +
					inttostring(str_x) + ", y offset passed = " + inttostring(str_y) + ". Total String Width = " +
					inttostring(str_width) + ", total string height " + inttostring(str_height));
		}
	}
	
	return 0;
}


#endif      //!PAL_WIN32_RUN
