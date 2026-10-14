#include "PAL_platformheader.h"


#include <fstream>
#include <stdio.h>

#if defined(PAL_WIN32_RUN)
#include <tchar.h>
#include <windowsx.h>
#endif

#include "CRendererUtil.h"
#include "BitMap.h"
#include "Debug.h"
#include "util/e_string/utilstring.h"

#include "utilstring.h"

//#include "UltimaObjects.h"
#include "BitBlt.h"
#include "Pen.h"
#include "DrawingPrimitives.h"
#include "Hex.h"



extern void domRectToUltima(Hobart::Rectangle *domRect, UltimaRectangle &ultimaRect);
extern void domBitMapToUltima(BitMap *domBitMap, UltimaBitMap &ultBitMap);




namespace Hobart
{



#if defined(PAL_WIN32_RUN)
	//////////////////////////////////////////////////////////////////////////////////
	//Not required for Linux build
	//
	bool CRendererUtil::GetHobartBitmap( HBITMAP win_bitmap, BitMap * hobart_bitmap )
	{
		if ( win_bitmap == 0 )
			return false;

		hobart_bitmap->freeBitMap();

		BITMAP bm;
		GetObject( win_bitmap, sizeof( bm ), &bm ); 
		int pitch = ( bm.bmWidth - 1 ) / 16 + 1;
		int size = pitch * bm.bmHeight;

		UINT16 * base = new UINT16[size];
		hobart_bitmap->setBitMapBounds( 0, ( UINT16 )bm.bmWidth - 1, 0, ( UINT16 )bm.bmHeight -1 );
		hobart_bitmap->setBitMapBase( base, size );

		//copy data.
//		UINT16 * ptr = base;
		long bytes = GetBitmapBits( win_bitmap, size * 2, base );

		return ( bytes > 0 );
	}
#endif//PAL_WIN32_RUN


	//////////////////////////////////////////////////////////////////////////////////
	//Not required for Linux build
	//
#if defined(PAL_WIN32_RUN)
	HBITMAP CRendererUtil::GetWindowsBitmap( BitMap * hobart_bitmap )
#else
	PAL_HBITMAP CRendererUtil::GetWindowsBitmap(BitMap * UNUSED(hobart_bitmap))
#endif
	{
#if defined(PAL_WIN32_RUN)
		int width = hobart_bitmap->getBitMapBoundsRight() - hobart_bitmap->getBitMapBoundsLeft();
		int height = hobart_bitmap->getBitMapBoundsBottom() - hobart_bitmap->getBitMapBoundsTop();
		UINT16 * base = hobart_bitmap->getBitMapBase();

		return CreateBitmap( width, height, 1, 1, base );
#else
		return NULL;
#endif
	}


	//////////////////////////////////////////////////////////////////////////////////
	//Draw
	//
#if defined(PAL_WIN32_RUN)
	void CRendererUtil::Draw( BitMap * hobart_bitmap, HBITMAP win_bitmap, int left, int top, DWORD mode )
#else
	void CRendererUtil::Draw(BitMap * hobart_bitmap, BitMap *bmp_buf, int left, int top, DWORD UNUSED(mode))
#endif
	{
#ifdef PAL_WIN32_RUN
		int hobart_width = hobart_bitmap->getBitMapBoundsRight() - hobart_bitmap->getBitMapBoundsLeft();
		int hobart_height = hobart_bitmap->getBitMapBoundsBottom() - hobart_bitmap->getBitMapBoundsTop();
		int hobart_pitch = ( hobart_width + 15 ) / 16; 
		int hobart_size = hobart_pitch * hobart_height;
	
				
		HBITMAP dest_bitmap = GetWindowsBitmap( hobart_bitmap );
		if ( dest_bitmap != 0 )
		{
			HDC hdc_dest = CreateCompatibleDC(NULL);
			SelectBitmap( hdc_dest, dest_bitmap );

			HDC hdc_win = CreateCompatibleDC(NULL);
			SelectBitmap( hdc_win, win_bitmap );
			
			BITMAP bm;
			GetObject( win_bitmap, sizeof( bm ), &bm ); 
			
			/*DWORD r = */BitBlt( hdc_dest, left, top, bm.bmWidth, bm.bmHeight, hdc_win, 0, 0, mode );
			
			DeleteDC( hdc_dest );
			DeleteDC( hdc_win );
					
			UINT16* base = hobart_bitmap->getBitMapBase();
			/*long bytes = */GetBitmapBits( dest_bitmap, hobart_size * 2, base );	

			DeleteObject( dest_bitmap );
		}
#else
		if ((NULL == hobart_bitmap) || (NULL == bmp_buf) ||
			(NULL == hobart_bitmap->getBitMapBase()) || (NULL == bmp_buf->getBitMapBase()))
		{
	                Debug::trace("DrawingPrimitives", "CRendererUtil:Inavlid bitmap buffers passed in");
			return;
		}




#if 0
		UINT16 * minhaj= hobart_bitmap->getBitMapBase();
		UINT16 * src_min= bmp_buf->getBitMapBase();
		UINT16 min_pitch = bmp_buf->getBitMapPitch();
		UINT16 min_height = bmp_buf->getBitMapBoundsBottom()-bmp_buf->getBitMapBoundsTop();
		UINT16 min_width = bmp_buf->getBitMapBoundsRight()-bmp_buf->getBitMapBoundsLeft();
		int32_t hob_pitch = hobart_bitmap->getBitMapPitch();
//
		int32_t hob_width = hobart_bitmap->getBitMapBoundsRight()-hobart_bitmap->getBitMapBoundsLeft();
//
		int32_t off_set = (hobart_bitmap->getBitMapPitch() * top) + left;

		for ( int32_t ij = 0; ij < min_pitch*min_height; ij++, off_set++)
			minhaj[off_set] |= src_min[ij];
#else
		UINT16 * hob_base=hobart_bitmap->getBitMapBase();

		UINT16 *bmp_base= bmp_buf->getBitMapBase();
		UINT16 bmp_pitch = bmp_buf->getBitMapPitch();
		UINT16 bmp_height = bmp_buf->getBitMapBoundsBottom()-bmp_buf->getBitMapBoundsTop();

		//
		//calculate the Word where the rendering needs to occur.  Or in 
		//source buffer word by word so that original bits in the dest 
		//buffer are unaffected.
		//
		int32_t off_set = (hobart_bitmap->getBitMapPitch() * top) + left;

		for ( int32_t i=0; i<bmp_pitch*bmp_height; i++, off_set++)
			hob_base[off_set] |= bmp_base[i];


#endif//if 0


#endif//PAL_WIN32_RUN
	}		


	
#if defined(PAL_WIN32_RUN)
	void CRendererUtil::SaveBitmapToFile( HBITMAP win_bitmap, const std::wstring & file_name )
	{	
		if ( win_bitmap == 0 )
			return;

		// create output file
		std::ofstream bmpfile( file_name.c_str() );
		if(!bmpfile) return;

		BITMAP bm;
		GetObject( win_bitmap, sizeof( bm ), &bm ); 
		int bitmap_pitch = ( bm.bmWidth - 1 ) / 16 + 1;
		int bitmap_size = bitmap_pitch * bm.bmHeight;

		int file_pitch = ( bm.bmWidth - 1 ) / 32 + 1;
		int file_size = file_pitch * bm.bmHeight * 4;

		// save bitmap file header
		BITMAPFILEHEADER fileHeader;
		fileHeader.bfType = 0x4d42;		
		fileHeader.bfReserved1 = 0;
		fileHeader.bfReserved2 = 0;
		fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 2* sizeof(RGBQUAD);

		fileHeader.bfSize =  fileHeader.bfOffBits + file_size;

		if(!bmpfile.write((char*)&fileHeader, sizeof(fileHeader)))
		return;		

		// save bitmap info header
		BITMAPINFOHEADER  infoHeader;
		infoHeader.biSize = sizeof(infoHeader);
		infoHeader.biWidth = bm.bmWidth;
		infoHeader.biHeight = bm.bmHeight;
		infoHeader.biPlanes = 1;
		infoHeader.biBitCount = bm.bmBitsPixel;
		infoHeader.biCompression = BI_RGB;
		infoHeader.biSizeImage = 0;
		infoHeader.biXPelsPerMeter = 0;
		infoHeader.biYPelsPerMeter = 0;
		infoHeader.biClrUsed = 2;
		infoHeader.biClrImportant = 0;

		if(!bmpfile.write((char*)&infoHeader, sizeof(infoHeader)))
		return;

		// save color table (sample used here is 8-bit grayscale)
		if(infoHeader.biBitCount == 8) 
		{
			RGBQUAD table[256];
			for(size_t i = 0; i < 256; i++)
			table[i].rgbRed = table[i].rgbGreen = table[i].rgbBlue = (BYTE)i;
			if(!bmpfile.write((char*)table, 256*sizeof(RGBQUAD)))
				return;
		}
		else if(infoHeader.biBitCount == 4)// 16 colors
		{
		
		}
		else if(infoHeader.biBitCount == 1) 
		{
			//only two colors (doesn't necessarily have to be always black and white!)
			RGBQUAD table[2];
			table[0].rgbRed = table[0].rgbGreen = table[0].rgbBlue = (BYTE)255;
			table[1].rgbRed = table[1].rgbGreen = table[1].rgbBlue = (BYTE)0;

			table[0].rgbReserved = table[1].rgbReserved = 0;

			if(!bmpfile.write((char*)table, 2*sizeof(RGBQUAD)))
				return;
			
		}

		// save bitmap data
		UINT16 * win_buf = new UINT16[bitmap_size];
		UINT16 * file_buf = new UINT16[file_size /2];
		memset( file_buf, 0, file_size );

		/*long bytes = */GetBitmapBits( win_bitmap, bitmap_size * 2, win_buf );

		UINT16 * ps = &win_buf[( bm.bmHeight -1 ) * bitmap_pitch];
		UINT16 * pd = file_buf;
		
		for ( int i = 0; i < bm.bmHeight; i++ ) 
		{
			memcpy( pd, ps, bitmap_pitch * 2 );
			
			pd += file_pitch * 2;
			ps -= bitmap_pitch;
		}
		
		bmpfile.write( ( char * ) file_buf, file_size );
		delete []win_buf;
		delete []file_buf;
				
		bmpfile.close();

	}

#endif//PAL_WIN32_RUN


 //////////////////////////////////////////////////////////////////////////////////////////////////////////
//
    #if defined(PAL_WIN32_RUN)
        void CRendererUtil::SaveBitmapToFile( BitMap * hobart_bitmap, const std::string & file_name )
    #else
        void CRendererUtil::SaveBitmapToFile( BitMap * hobart_bitmap, const std::string & file_name )
    #endif
        {
			std::string localFilename = file_name.c_str();
			Debug::trace("RendererManager.saveLabel", "CRendererUtil:SaveBitmapToFile - begin - Output file name passed in: " + localFilename);

                // create output file
                std::ofstream bmpfile( file_name.c_str() );
                if(!bmpfile)
				{
					Debug::trace("RendererManager.saveLabel", "CRendererUtil:SaveBitmapToFile - Output file stream could not be created to : " + localFilename);
					return;
				}

                int width = hobart_bitmap->getBitMapBoundsRight() - hobart_bitmap->getBitMapBoundsLeft();
                int height = hobart_bitmap->getBitMapBoundsBottom() - hobart_bitmap->getBitMapBoundsTop();

                int bitmap_pitch = ( width - 1 ) / 16 + 1;
//              int bitmap_size = bitmap_pitch * height;

                int file_pitch = ( width - 1 ) / 32 + 1;
                int file_size = file_pitch * height * 4;

                // save bitmap file header
#ifdef __linux__
                PAL_BITMAP_FILE_HEADER fileHeader;

                fileHeader.bf_type = 0x4d42;
                fileHeader.bf_resrvd1 = 0;
                fileHeader.bf_resrvd2  = 0;
                fileHeader.bf_off_bits = sizeof(PAL_BITMAP_FILE_HEADER) + sizeof(PAL_BITMAP_INFO_HEADER) + 2* sizeof(PAL_RGB_QUAD);

                fileHeader.bf_size =  fileHeader.bf_off_bits + file_size;
#else
                BITMAPFILEHEADER fileHeader;
                fileHeader.bfType = 0x4d42;
                fileHeader.bfReserved1 = 0;
                fileHeader.bfReserved2 = 0;
                fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 2* sizeof(RGBQUAD);


                fileHeader.bfSize =  fileHeader.bfOffBits + file_size;
#endif

                if(!bmpfile.write((char*)&fileHeader, sizeof(fileHeader)))
				{
					Debug::trace("RendererManager.saveLabel", "CRendererUtil:SaveBitmapToFile - could not write fileHeader to output file: ");
					return;
				}

                // save bitmap info header
#ifdef __linux__
                PAL_BITMAP_INFO_HEADER  infoHeader;

                infoHeader.bi_size = sizeof(infoHeader);
                infoHeader.bi_width = width;
                infoHeader.bi_height = height;
                infoHeader.bi_planes = 1;
                infoHeader.bi_bit_cnt = 1;
                infoHeader.bi_compression = PAL_BI_RGB;
                infoHeader.bi_size_img = 0;
                infoHeader.bi_xpels_per_meter = 0;
                infoHeader.bi_ypels_per_meter = 0;
                infoHeader.bi_clr_used = 2;
                infoHeader.bi_clr_imp = 0;
#else
                BITMAPINFOHEADER  infoHeader;
                infoHeader.biSize = sizeof(infoHeader);
                infoHeader.biWidth = width;
                infoHeader.biHeight = height;
                infoHeader.biPlanes = 1;
                infoHeader.biBitCount = 1;
                infoHeader.biCompression = BI_RGB;
                infoHeader.biSizeImage = 0;
                infoHeader.biXPelsPerMeter = 0;
                infoHeader.biYPelsPerMeter = 0;
                infoHeader.biClrUsed = 2;
                infoHeader.biClrImportant = 0;
#endif

                if(!bmpfile.write((char*)&infoHeader, sizeof(infoHeader)))
				{
					Debug::trace("RendererManager.saveLabel", "CRendererUtil:SaveBitmapToFile - could not write infoHeader to output file: ");
					return;
				}

                // save color table (sample used here is 8-bit grayscale)
#ifdef __linux__
                if(infoHeader.bi_bit_cnt == 8)
                {
                        PAL_RGB_QUAD table[256];
                        for(size_t i = 0; i < 256; i++)
                        table[i].rgb_red = table[i].rgb_green = table[i].rgb_blue = (BYTE)i;
                        if(!bmpfile.write((char*)table, 256*sizeof(PAL_RGB_QUAD)))
						{
							Debug::trace("RendererManager.saveLabel", "CRendererUtil:SaveBitmapToFile - could not write 8bit color table to output file: ");
							return;
						}

                }
                else if(infoHeader.bi_bit_cnt == 4)// 16 colors
                {

                }
                else if(infoHeader.bi_bit_cnt == 1)
                {
                        //only two colors (doesn't necessarily have to be always black and white!)
                        PAL_RGB_QUAD table[2];
                        table[0].rgb_red = table[0].rgb_green = table[0].rgb_blue = (BYTE)255;
                        table[1].rgb_red = table[1].rgb_green = table[1].rgb_blue = (BYTE)0;

                        table[0].rgb_resrvd = table[1].rgb_resrvd = 0;

                        if(!bmpfile.write((char*)table, 2*sizeof(PAL_RGB_QUAD)))
						{
							Debug::trace("RendererManager.saveLabel", "CRendererUtil:SaveBitmapToFile - could not write 1bit color table to output file: ");
							return;
						}

                }
#else//__linux__
		if(infoHeader.biBitCount == 8) 
		{
			RGBQUAD table[256];
			for(size_t i = 0; i < 256; i++)
			table[i].rgbRed = table[i].rgbGreen = table[i].rgbBlue = (BYTE)i;
			if(!bmpfile.write((char*)table, 256*sizeof(RGBQUAD)))
			{
				Debug::trace("RendererManager.saveLabel", "CRendererUtil:SaveBitmapToFile - could not write 8bit color table to output file: ");
				return;
			}
		}
		else if(infoHeader.biBitCount == 4)// 16 colors
		{
		
		}
		else if(infoHeader.biBitCount == 1) 
		{
			//only two colors (doesn't necessarily have to be always black and white!)
			RGBQUAD table[2];
			table[0].rgbRed = table[0].rgbGreen = table[0].rgbBlue = (BYTE)255;
			table[1].rgbRed = table[1].rgbGreen = table[1].rgbBlue = (BYTE)0;

			table[0].rgbReserved = table[1].rgbReserved = 0;

			if(!bmpfile.write((char*)table, 2*sizeof(RGBQUAD)))
			{
				Debug::trace("RendererManager.saveLabel", "CRendererUtil:SaveBitmapToFile - could not write 1bit color table to output file: ");
				return;
			}
			
		}
#endif//__linux__

                // save bitmap data
                UINT16 * win_buf =  hobart_bitmap->getBitMapBase();
                UINT16 * file_buf = new UINT16[file_size /2];
                memset( file_buf, 0, file_size );

                UINT16 * ps = &win_buf[( height -1 ) * bitmap_pitch];
                UINT16 * pd = file_buf;

                for ( int i = 0; i < height; i++ )
                {
                        memcpy( pd, ps, bitmap_pitch * 2 );

                        pd += file_pitch * 2;
                        ps -= bitmap_pitch;
                }

				if (Debug::isTracing("RendererManager.debugSaveLabel"))
				{
					Debug::trace("RendererManager.saveLabel", " ");
					Debug::trace("RendererManager.saveLabel", " ");
					char *aaa = (char *)(file_buf);
					int len = file_size * 2;
					while (len > 0)
					{
						Debug::trace("RendererManager.saveLabel", "bmpbuf: " + Hex::toString(aaa, 56, true));
						len -= 56;
						aaa += 56;
					}
					Debug::trace("RendererManager.saveLabel", " ");
					Debug::trace("RendererManager.saveLabel", " ");
				}

				bmpfile.write( ( char * ) file_buf, file_size );

                delete []file_buf;

                bmpfile.close();

#if defined ( __linux__ )
				// sync disk
				sync();
#endif // __linux__
		}

//	}


}//end of namespace
