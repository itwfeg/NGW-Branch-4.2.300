//#include <vxWorks.h>

//#include "EventManager.h"
#include "Session.h"
#include "SessionManager.h"
#include "EvolMessage.h"
#include "SessionEvent.h"
//#include "DispatchEvent.h"
#include "RealTimeController.h"
#include "AccessKeys.h"
#include "AccessKeyActivation.h"
#include "Debug.h"

//#include "ugl/ugl.h"
//#include "ugl/uglinput.h"
//#include "ugl/uglucode.h"
//#include "../config/iacsfl440mx/atmel8.h"
//#include "../config/iacsfl440mx/serviceSpeaker.h"

LOCAL UGL_STATUS keyboardCallback(UGL_INPUT_SERVICE_ID inputServiceId,
	 UGL_MSG *pMsg, UGL_MSG_Q_ID *pQId, void *pParam);
LOCAL UGL_STATUS touchScreenCallback(UGL_INPUT_SERVICE_ID inputServiceId,
	UGL_MSG *pMsg, UGL_MSG_Q_ID *pQId, void *pParam);

LOCAL void sendPrintKeyPressed();
LOCAL void sendClearKeyPressed();

extern "C" void setupSpecialKeysCallBack()
{
	UGL_INPUT_SERVICE_ID inputServiceId = (UGL_INPUT_SERVICE_ID) uglRegistryFind(UGL_INPUT_SERVICE_TYPE, 0, 0, 0)->id;

	uglInputCbAdd(inputServiceId, MSG_KEYBOARD, 0, (UGL_INPUT_CB *) (keyboardCallback), NULL);

	uglInputCbAdd(inputServiceId, MSG_POINTER, 0, (UGL_INPUT_CB *) (touchScreenCallback), NULL);

}


UGL_STATUS touchScreenCallback(UGL_INPUT_SERVICE_ID inputServiceId,
													 UGL_MSG *pMsg,
													 UGL_MSG_Q_ID *pQId,
													 void *pParam)
{
UGL_STATUS result = UGL_STATUS_OK;

	if (pMsg->type == MSG_POINTER)
	{
		bool touch = (pMsg->data.pointer.buttonState & UGL_PTR_BUTTON1);

		if (touch)
		{
			if (!(pMsg->data.rawPtr.buttonState & UGL_PTR_BUTTON5))
				ServiceSpeaker::getInstance()->beep(2);
		}
	}

	return result;

}


UGL_STATUS keyboardCallback(UGL_INPUT_SERVICE_ID inputServiceId,
													 UGL_MSG *pMsg,
													 UGL_MSG_Q_ID *pQId,
													 void *pParam)
{
UGL_STATUS result = UGL_STATUS_OK;

	if (pMsg->type == MSG_KEYBOARD)
	{
		bool keyDown = (pMsg->data.keyboard.modifiers & UGL_KBD_KEYDOWN);

		if (keyDown)
		{
			if (pMsg->data.rawKbd.deviceId->deviceType == UGL_DEVICE_KEYBOARD)
				ServiceSpeaker::getInstance()->beep(2);
		}

		switch (pMsg->data.keyboard.key)
		{
			case UGL_UNI_PRINT:
			{
				Session* primary = (SessionManager::getInstance())->getPrimaryConsoleSession();
				OpModeEvent opMode = OpModeOperate;
				
				if (primary != 0)
					opMode = primary->getCurrentMode();

				if ((opMode == OpModeService) || (opMode == OpModeSupervisor))
				{
					pMsg->data.keyboard.key = UGL_UNI_CARRIAGE_RETURN;
				}
				else
				{ 
					if (keyDown)
						sendPrintKeyPressed();
					result = UGL_STATUS_FINISHED;
				}
				break;
			}
			case UGL_UNI_CLEAR:
			{
				if (keyDown)
					sendClearKeyPressed();
				result = UGL_STATUS_FINISHED;
				break;
			}
			case UGL_UNI_CONTRAST_UP:
			{
				if (keyDown)
					ServiceSpeaker::getInstance()->volumeUp();
				result = UGL_STATUS_FINISHED;
				break;
			}
			case UGL_UNI_CONTRAST_DOWN:
			{
				if (keyDown)
					ServiceSpeaker::getInstance()->volumeDown();
				result = UGL_STATUS_FINISHED;
				break;
			}
			case UGL_UNI_F10:
			{
				if ( !(pMsg->data.keyboard.modifiers & UGL_KBD_SHIFT) &&
						!(pMsg->data.keyboard.modifiers & UGL_KBD_CTRL) &&
						!(pMsg->data.keyboard.modifiers & UGL_KBD_ALT) &&
						!(pMsg->data.keyboard.modifiers & UGL_KBD_CAPS_LOCK))
				{
					if ( keyDown )
					{
						Debug::trace("USBKeyboard.HandleSpecialKeys", "F10 Key Pressed");

						EventManager * pEventMgr = EventManager::getInstance();
						AccessKeyActivation active(AccessKeys::SupervisorImmediate);
						DispatchEvent de(&active, pEventMgr);
						const char *pDisMsg = de.serialize();
						int mLength = de.getSerializedLength();
						pEventMgr->send(pDisMsg, mLength);

						Debug::trace("USBKeyboard.HandleSpecialKeys", "F10 Key Pressed message sent");

//						OperationModeSupervisor opSuper(pUsbManager);
//						pUsbManager->dispatchEvent(&opSuper, pUsbManager);
					}
				}

				result = UGL_STATUS_FINISHED;      //UGL_STATUS_OK;
				break;
			}
			default:
			{
				break;
			}
		}
	}


	return result;
}


void sendPrintKeyPressed()
{
int rv;
//IEventManager EvtMgr();
EventManager * pEventMgr = EventManager::getInstance();
EvolMessage PrintMsg(SessionDev, SnPrintKeyPressed);
DispatchEvent de(&PrintMsg, pEventMgr);
const char *pDisMsg = de.serialize();
int mLength = de.getSerializedLength();

//printf("\n*** sendPrinterEvent - %s ***\n", pDisMsg);
//	EvtMgr.dispatchEvent(&PrintMsg, NULL);

	rv = pEventMgr->send(pDisMsg, mLength);

//printf("\n*** sendPrinterEvent - message sent: %d ***\n", rv);

}

void sendClearKeyPressed()
{
int rv;
//IEventManager EvtMgr();
EventManager * pEventMgr = EventManager::getInstance();
EvolMessage PrintMsg(SessionDev, SnClearKeyPressed);
DispatchEvent de(&PrintMsg, pEventMgr);
const char *pDisMsg = de.serialize();
int mLength = de.getSerializedLength();

//printf("\n*** sendPrinterEvent - %s ***\n", pDisMsg);
//	EvtMgr.dispatchEvent(&PrintMsg, NULL);

	rv = pEventMgr->send(pDisMsg, mLength);

//printf("\n*** sendPrinterEvent - message sent: %d ***\n", rv);

}

extern "C" void startRTC()
{
	Debug::trace("Main", "Starting Real-Time Controller ...");
	RealTimeController::getInstance();
}
