#include "TouchScreenCalibration.h"
#include "ugl/ugl.h"
#include "ugl/uglevent.h"
#include "atmel8Comm.h"

#ifndef HOBART_DEVELOPMENT
#include "Session.h"
#include "Debug.h"
#include "Integer.h"
#include "ServiceSpeaker.h"
#endif


extern "C" TOUCHSCREEN_CAL_DATA touchCalData;


STATUS touchCal(ATMEL8_TS_PACKET * packet)
{
TouchScreenCalibration * pTouchCal = TouchScreenCalibration::getInstance();

	return pTouchCal->handleCalibrationTouch(packet);
}

STATUS loadCalibrationDataFromAtmel8()
{
TouchScreenCalibration * pTouchCal = TouchScreenCalibration::getInstance();

	return pTouchCal->loadCalibrationDataFromAtmel8();
}

TouchScreenCalibration *TouchScreenCalibration::instance = (TouchScreenCalibration *) NULL;

TouchScreenCalibration::TouchScreenCalibration():m_oneShot(true) 
{
}

TouchScreenCalibration::~TouchScreenCalibration()
{
}


TouchScreenCalibration *TouchScreenCalibration::getInstance()
{
	if (instance == 0)
	{
		instance = new TouchScreenCalibration();
	}

	return instance;
}


STATUS TouchScreenCalibration::loadCalibrationDataFromAtmel8 ()
{
CAL_DATA_MSG calMSG;

#ifndef HOBART_DEVELOPMENT
	Debug::trace("TouchScreen", "start TouchScreenCalibration::loadCalibrationDataFromAtmel8");
#endif

	calMSG.hdr.cmd = GetCalData;
	if (Atmel8Comm::SendMessage((GEN_MSG *)&calMSG) != ERROR)
	{

		if (Debug::isTracing("TouchScreen"))
		{

			string msg ("cal:received cal message via SMBUS:\nxMinRaw = ");
			msg += Integer::toString(calMSG.xminRaw.val);
			msg += " xMinPix = ";
			msg += Integer::toString(calMSG.xminPix.val);
			msg += " xMaxRaw = ";
			msg += Integer::toString(calMSG.xmaxRaw.val);
			msg += " xMaxPix = ";
			msg += Integer::toString(calMSG.xmaxPix.val);
			msg += " yMinRaw = ";
			msg += Integer::toString(calMSG.yminRaw.val);
			msg += " yMinPix = ";
			msg += Integer::toString(calMSG.yminPix.val);
			msg += " yMaxRaw = ";
			msg += Integer::toString(calMSG.ymaxRaw.val);
			msg += " yMaxPix = ";
			msg += Integer::toString(calMSG.ymaxPix.val);


			Debug::trace("TouchScreen", msg);
		}

		if ((calMSG.xminRaw.val > XMIN_MIN) && (calMSG.xminRaw.val < XMIN_MAX) &&
			(calMSG.xmaxRaw.val > XMAX_MIN) && (calMSG.xmaxRaw.val < XMAX_MAX) &&
			(calMSG.yminRaw.val > YMIN_MIN) && (calMSG.yminRaw.val < YMIN_MAX) &&
			(calMSG.ymaxRaw.val > YMAX_MIN) && (calMSG.ymaxRaw.val < YMAX_MAX) )
		{

			Debug::trace("TouchScreen", "Using stored calibration values from atmel8");

			touchCalData.raw_max_x = calMSG.xmaxRaw.val;
			touchCalData.raw_min_x = calMSG.xminRaw.val;
			touchCalData.pix_max_x = calMSG.xmaxPix.val;
			touchCalData.pix_min_x = calMSG.xminPix.val;
		
			touchCalData.raw_max_y = calMSG.ymaxRaw.val;
			touchCalData.raw_min_y = calMSG.yminRaw.val;				
			touchCalData.pix_max_y = calMSG.ymaxPix.val;
			touchCalData.pix_min_y = calMSG.yminPix.val;

		}
		else
		{

			Debug::trace("TouchScreen", "***ERROR*** invalid calibration vals in atmel8. using default values");

			touchCalData.raw_max_x = RAW_MAXX_DEFAULT;
			touchCalData.raw_min_x = RAW_MINX_DEFAULT;
			touchCalData.pix_max_x = PIX_MAXX_DEFAULT;
			touchCalData.pix_min_x = PIX_MINX_DEFAULT;
		
			touchCalData.raw_max_y = RAW_MAXY_DEFAULT;
			touchCalData.raw_min_y = RAW_MINY_DEFAULT;				
			touchCalData.pix_max_y = PIX_MAXY_DEFAULT;
			touchCalData.pix_min_y = PIX_MINY_DEFAULT;

		}

		touchCalData.pixXRange = (INT32)touchCalData.pix_max_x - 
			(INT32)touchCalData.pix_min_x;
		touchCalData.rawXRange = (INT32)touchCalData.raw_max_x - 
			(INT32)touchCalData.raw_min_x;

		touchCalData.pixYRange = (INT32)touchCalData.pix_max_y - 
			(INT32)touchCalData.pix_min_y;
		touchCalData.rawYRange = (INT32)touchCalData.raw_max_y - 
			(INT32)touchCalData.raw_min_y;


		return OK;

	}

	return ERROR;
}


#ifndef HOBART_DEVELOPMENT
STATUS TouchScreenCalibration::startCalibration (Session *pSession)
{
STATUS result;

	Debug::trace("TouchScreen", "begin TouchScreenCalibration::startCalibration");

	/*this is a long story but it is like this so that the calibration will work in both imgDevelopment and
          imgHBAll. The main problem is that in imgDevelopment, the windml code is statically linked with the os.
          all calls in windml must be resolved. Thus we have TouchScreenCalibration.cpp both in imgDevelopment and
          EvolAppRTC. This file is in imgDevelopment just to get it to compile. When EvolAppRTC.out is downloaded,
          the origional version of TouchScreenColibration.o is not unlinked. Thus we have two copies of TouchScreenCalibration.o
          loaded in memory. The windml touchscreen driver is linked to the first copy of TouchScreenCalibration.o.
          Im sure there is a better way of doing this, like making windml downloadable for imgDevelopment. but i 
          dont have the time...*/
	if (m_oneShot)
	{
		UGL_ATMEL8_TS_DEV * pointerDrv = (UGL_ATMEL8_TS_DEV *) uglRegistryFind(UGL_POINTER_TYPE, 0, 0, 0)->id;

		pointerDrv->touchCal = touchCal;
		
		m_oneShot = false;
	}

	m_pSession = pSession;


	tempTSData.raw_min_x = 0;
	tempTSData.raw_min_x_cnt = 0;

	tempTSData.raw_max_x = 0;
	tempTSData.raw_max_x_cnt = 0;

	tempTSData.raw_min_y = 0;
	tempTSData.raw_min_y_cnt = 0;

	tempTSData.raw_max_y = 0;
	tempTSData.raw_max_y_cnt = 0;

	touchCalData.calState = 1;

	Debug::trace("TouchScreen", "End TouchScreenCalibration::startCalibration");

	return result;

}
#endif

STATUS TouchScreenCalibration::abortCalibration ()
{

#ifndef HOBART_DEVELOPMENT
	Debug::trace("TouchScreen", "begin TouchScreenCalibration::abortCalibration");
#endif

	touchCalData.calState = 0;

	return OK;

}


STATUS TouchScreenCalibration::nextTargetDisplay (int x, int y)
{

	switch (touchCalData.calState)
	{
		case 2:
		case 4:
		case 6:
		case 8:
			break;
		case 1:
			touchCalData.pix_min_x = (UGL_UINT16) x;
			touchCalData.pix_min_y = (UGL_UINT16) y;

#ifndef HOBART_DEVELOPMENT
			Debug::trace("TouchScreen", string("TouchScreenCalibration::nextTargetDisplay state 1: x = ") + Integer::toString(touchCalData.pix_min_x) + string(", y = ") + Integer::toString(touchCalData.pix_min_y));
#endif

			touchCalData.calState = 2;
			break;
		case 3:
			touchCalData.pix_max_x = (UGL_UINT16) x;

#ifndef HOBART_DEVELOPMENT
			Debug::trace("TouchScreen", string("TouchScreenCalibration::nextTargetDisplay state 3: x = ") + Integer::toString(touchCalData.pix_max_x) + string(", y = ") + Integer::toString(touchCalData.pix_min_y));
#endif

			touchCalData.calState = 4;
			break;
		case 5:
			touchCalData.pix_max_y = (UGL_UINT16) y;

#ifndef HOBART_DEVELOPMENT
			Debug::trace("TouchScreen", string("TouchScreenCalibration::nextTargetDisplay state 5: x = ") + Integer::toString(touchCalData.pix_max_x) + string(", y = ") + Integer::toString(touchCalData.pix_max_y));
#endif

			touchCalData.calState = 6;
			break;
		case 7:
#ifndef HOBART_DEVELOPMENT
			Debug::trace("TouchScreen", string("TouchScreenCalibration::nextTargetDisplay state 7: finished steps "));
#endif
			touchCalData.calState = 8;
			break;
		default:
			break;
	}

	return OK;

}

STATUS TouchScreenCalibration::handleCalibrationTouch 
(
	ATMEL8_TS_PACKET * packet
)
{
STATUS result;
CAL_DATA_MSG calMSG;
SET_MODE_MSG modeMsg;
#ifndef HOBART_DEVELOPMENT
	Debug::trace("TouchScreen", "TouchScreenCalibration::handleCalibrationTouch");

	if (packet->penStatus == ATMEL8_PEN_DOWN)
	{
		ServiceSpeaker::getInstance()->beep(2);
	}

	switch (touchCalData.calState)
	{
		case 1:
		case 3:
		case 5:
		case 7:
			break;
		case 2: /* calibrate the upper right corner,  max x and min y */
			Debug::trace("TouchScreen", "TouchScreenCalibration::handleCalibrationTouch state 2");
			tempTSData.raw_min_x += packet->x;
			tempTSData.raw_min_x_cnt ++;
			tempTSData.raw_min_y += packet->y;
			tempTSData.raw_min_y_cnt ++;

			if (packet->penStatus == ATMEL8_PEN_UP)
			{
				m_pSession->nextTouchScreenCalibrationStep();
				touchCalData.calState = 3;
			}
			break;

		case 4: /* calibrate the upper right corner,  max x and min y */
			Debug::trace("TouchScreen", "TouchScreenCalibration::handleCalibrationTouch state 4");
			tempTSData.raw_max_x += packet->x;
			tempTSData.raw_max_x_cnt ++;
			tempTSData.raw_min_y += packet->y;
			tempTSData.raw_min_y_cnt ++;

			if (packet->penStatus == ATMEL8_PEN_UP)
			{
				m_pSession->nextTouchScreenCalibrationStep();
				touchCalData.calState = 5;
			}
			break;
		case 6: /* calibrate the lower right corner,  max x and max y */
			Debug::trace("TouchScreen", "TouchScreenCalibration::handleCalibrationTouch state 6");
			tempTSData.raw_max_x += packet->x;
			tempTSData.raw_max_x_cnt ++;
			tempTSData.raw_max_y += packet->y;
			tempTSData.raw_max_y_cnt ++;

			if (packet->penStatus == ATMEL8_PEN_UP)
			{
				m_pSession->nextTouchScreenCalibrationStep();
				touchCalData.calState = 7;
			}

			break;
		case 8: /* calibrate the lower left corner,  min x and max y */
			Debug::trace("TouchScreen", "TouchScreenCalibration::handleCalibrationTouch state 8");
			tempTSData.raw_min_x += packet->x;
			tempTSData.raw_min_x_cnt ++;
			tempTSData.raw_max_y += packet->y;
			tempTSData.raw_max_y_cnt ++;

			if (packet->penStatus == ATMEL8_PEN_UP)
			{

				if (tempTSData.raw_min_x_cnt > 0)
				{
					tempTSData.raw_min_x /= tempTSData.raw_min_x_cnt;
					tempTSData.raw_min_x_cnt = 0;
				}
				if (tempTSData.raw_max_x_cnt > 0)
				{
					tempTSData.raw_max_x /= tempTSData.raw_max_x_cnt;
					tempTSData.raw_max_x_cnt = 0;
				}
				if (tempTSData.raw_min_y_cnt > 0)
				{
					tempTSData.raw_min_y /= tempTSData.raw_min_y_cnt;
					tempTSData.raw_min_y_cnt = 0;
				}
				if (tempTSData.raw_max_y_cnt > 0)
				{
					tempTSData.raw_max_y /= tempTSData.raw_max_y_cnt;
					tempTSData.raw_max_y_cnt = 0;
				}

				if ((tempTSData.raw_min_x > XMIN_MIN) && (tempTSData.raw_min_x < XMIN_MAX) &&
					(tempTSData.raw_max_x > XMAX_MIN) && (tempTSData.raw_max_x < XMAX_MAX) &&
					(tempTSData.raw_min_y > YMIN_MIN) && (tempTSData.raw_min_y < YMIN_MAX) &&
					(tempTSData.raw_max_y > YMAX_MIN) && (tempTSData.raw_max_y < YMAX_MAX) )
				{

					touchCalData.raw_max_x = tempTSData.raw_max_x;
					touchCalData.raw_min_x = tempTSData.raw_min_x;
					touchCalData.raw_max_y = tempTSData.raw_max_y;
					touchCalData.raw_min_y = tempTSData.raw_min_y;

					touchCalData.pixXRange = (INT32)touchCalData.pix_max_x - 
						(INT32)touchCalData.pix_min_x;
					touchCalData.rawXRange = (INT32)touchCalData.raw_max_x - 
						(INT32)touchCalData.raw_min_x;

					touchCalData.pixYRange = (INT32)touchCalData.pix_max_y - 
						(INT32)touchCalData.pix_min_y;
					touchCalData.rawYRange = (INT32)touchCalData.raw_max_y - 
						(INT32)touchCalData.raw_min_y;



					/* send out the new calibration information to the atmel8 */
					/* first change the mode in the atmel8 to service */

					modeMsg.hdr.cmd = SetMode;
					modeMsg.mode = Service;

					if (Atmel8Comm::SendMessage((GEN_MSG *)&modeMsg) == ERROR)
						Debug::trace("TouchScreen", "TouchScreenCalibration::handleCalibrationTouch atmel8 failed to go into service mode");

					taskDelay (120);

					calMSG.hdr.cmd = SetCalData;
					calMSG.xmaxRaw.val = touchCalData.raw_max_x;
					calMSG.xminRaw.val = touchCalData.raw_min_x;
					calMSG.xmaxPix.val = touchCalData.pix_max_x;
					calMSG.xminPix.val = touchCalData.pix_min_x;

					calMSG.ymaxRaw.val = touchCalData.raw_max_y;
					calMSG.yminRaw.val = touchCalData.raw_min_y;
					calMSG.ymaxPix.val = touchCalData.pix_max_y;
					calMSG.yminPix.val = touchCalData.pix_min_y;


					/*send the new calibration data to the atmel8 */
					if (Atmel8Comm::SendMessage((GEN_MSG *)&calMSG) == ERROR)
						Debug::trace("TouchScreen", "TouchScreenCalibration::handleCalibrationTouch atmel8 failed to accept cal data");

					if (Debug::isTracing("TouchScreen"))
					{
						string msg ("cal:sending cal message via SMBUS:\nxMinRaw = ");
						msg += Integer::toString(calMSG.xminRaw.val);
						msg += " xMinPix = ";
						msg += Integer::toString(calMSG.xminPix.val);
						msg += " xMaxRaw = ";
						msg += Integer::toString(calMSG.xmaxRaw.val);
						msg += " xMaxPix = ";
						msg += Integer::toString(calMSG.xmaxPix.val);
						msg += " yMinRaw = ";
	 					msg += Integer::toString(calMSG.yminRaw.val);
						msg += " yMinPix = ";
						msg += Integer::toString(calMSG.yminPix.val);
						msg += " yMaxRaw = ";
						msg += Integer::toString(calMSG.ymaxRaw.val);
						msg += " yMaxPix = ";
						msg += Integer::toString(calMSG.ymaxPix.val);
			

						Debug::trace("TouchScreen", msg);
					}

					/*place the atmel 8 back into user mode*/

					modeMsg.hdr.cmd = SetMode;
					modeMsg.mode = User;


					taskDelay(120);
					while (Atmel8Comm::SendMessage((GEN_MSG *)&modeMsg) == ERROR)
					{
						Debug::trace("TouchScreen", "TouchScreenCalibration::handleCalibrationTouch atmel8 failed to go into user mode");
					}


					Debug::trace("TouchScreen", "TouchScreenCalibration::handleCalibrationTouch complete/saved");
				}
				else
				{
					Debug::trace("TouchScreen", "TouchScreenCalibration::handleCalibrationTouch complete/NOT saved");

				if (Debug::isTracing("TouchScreen"))
				{

					string msg ("cal:new calibration data \nxMinRaw = ");
					msg += Integer::toString(tempTSData.raw_min_x);
					msg += " xMinPix = ";
					msg += Integer::toString(touchCalData.pix_min_x);
					msg += " xMaxRaw = ";
					msg += Integer::toString(tempTSData.raw_max_x);
					msg += " xMaxPix = ";
					msg += Integer::toString(touchCalData.pix_max_x);
					msg += " yMinRaw = ";
					msg += Integer::toString(tempTSData.raw_min_y);
					msg += " yMinPix = ";
					msg += Integer::toString(touchCalData.pix_min_y);
					msg += " yMaxRaw = ";
					msg += Integer::toString(tempTSData.raw_max_y);
					msg += " yMaxPix = ";
					msg += Integer::toString(touchCalData.pix_max_y);


					Debug::trace("TouchScreen", msg);
				}

				}

				m_pSession->nextTouchScreenCalibrationStep();
				touchCalData.calState = 0;

			}

			break;

		default:
			break;

	}

	if (Debug::isTracing("TouchScreen"))
	{
		Debug::trace("TouchScreen", string("TouchScreenCalibration::handleCalibrationTouch next state = ") + Integer::toString(touchCalData.calState));
	}
#endif


	return result;

}
