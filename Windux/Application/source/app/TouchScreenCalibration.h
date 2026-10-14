#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include "vxWorks.h"
#include "atmel8.h"

typedef struct
{
	INT32 rawXRange;
	INT32 pixXRange;
	INT32 rawYRange;
	INT32 pixYRange;
	UINT16 scr_x;
	UINT16 scr_y;
	UINT32 raw_min_x;
	UINT32 raw_max_x;
	UINT32 raw_min_y;
	UINT32 raw_max_y;
	UINT16 pix_min_x;
	UINT16 pix_max_x;
	UINT16 pix_min_y;
	UINT16 pix_max_y;
	UINT8 calState;
} TOUCHSCREEN_CAL_DATA;

#ifdef __cplusplus

extern "C"
{
	STATUS touchCal(ATMEL8_TS_PACKET * packet);
	STATUS loadCalibrationDataFromAtmel8();
}

#ifndef HOBART_DEVELOPMENT
class Session;
#endif

class TouchScreenCalibration 
{
	public:
	 	~TouchScreenCalibration();

		static TouchScreenCalibration *getInstance();

#ifndef HOBART_DEVELOPMENT
		STATUS startCalibration (Session *pSession);
#endif
		STATUS abortCalibration ();
		STATUS nextTargetDisplay (int x, int y);
		STATUS handleCalibrationTouch (ATMEL8_TS_PACKET * packet);
		STATUS loadCalibrationDataFromAtmel8 ();


	private:

		typedef struct
		{
			UINT32 raw_min_x;
			UINT16 raw_min_x_cnt;
			UINT16 pix_min_x;
			UINT32 raw_max_x;
			UINT16 raw_max_x_cnt;
			UINT16 pix_max_x;
			UINT32 raw_min_y;
			UINT16 raw_min_y_cnt;
			UINT16 pix_min_y;
			UINT32 raw_max_y;
			UINT16 raw_max_y_cnt;
			UINT16 pix_max_y;

		} TEMP_TOUCHSCREEN_DATA;

		TouchScreenCalibration();

#ifndef HOBART_DEVELOPMENT
		Session *m_pSession;
#endif
		bool m_oneShot;
		TEMP_TOUCHSCREEN_DATA tempTSData;

		static TouchScreenCalibration* instance;

		const static UINT32 XMIN_MIN = 950;
		const static UINT32 XMIN_MAX = 1010;
		const static UINT32 XMAX_MIN = 20;
		const static UINT32 XMAX_MAX = 110;

		const static UINT32 YMIN_MIN = 950;
		const static UINT32 YMIN_MAX = 1010;
		const static UINT32 YMAX_MIN = 20;
		const static UINT32 YMAX_MAX = 110;		

#if 0
		const static UINT32 RAW_MINX_DEFAULT = 974;
		const static UINT32 PIX_MINX_DEFAULT = 30;
		const static UINT32 RAW_MAXX_DEFAULT = 58;
		const static UINT32 PIX_MAXX_DEFAULT = 609;
		const static UINT32 RAW_MINY_DEFAULT = 951;
		const static UINT32 PIX_MINY_DEFAULT = 30;
		const static UINT32 RAW_MAXY_DEFAULT = 81;
		const static UINT32 PIX_MAXY_DEFAULT = 448;
#else
		const static UINT32 RAW_MINX_DEFAULT = 990;
		const static UINT32 PIX_MINX_DEFAULT = 25;
		const static UINT32 RAW_MAXX_DEFAULT = 32;
		const static UINT32 PIX_MAXX_DEFAULT = 775;
		const static UINT32 RAW_MINY_DEFAULT = 980;
		const static UINT32 PIX_MINY_DEFAULT = 25;
		const static UINT32 RAW_MAXY_DEFAULT = 44;
		const static UINT32 PIX_MAXY_DEFAULT = 575;

#endif

};

#else
	STATUS touchCal(ATMEL8_TS_PACKET * packet);
	STATUS loadCalibrationDataFromAtmel8();
#endif

#endif //TOUCHSCREEN_H
