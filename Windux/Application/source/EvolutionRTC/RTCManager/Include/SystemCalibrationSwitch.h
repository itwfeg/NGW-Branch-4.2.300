#ifndef SYSTEMCALIBRATIONSWITCH_H
#define SYSTEMCALIBRATIONSWITCH_H

#include "EvolMessage.h"
#include "System.h" // needed for part number size
#include "CEvolMsgReceiver.h"
//class HobThread;

class SystemCalibrationSwitch : public EvolMessage {
	public:
		SystemCalibrationSwitch(EvolDeviceType evolDevType, 
								Hobart::CEvolMsgReceiver *pHobThread = NULL,
								bool systemCalibrationSwitch = false);

		SystemCalibrationSwitch(const char *buffer);
		virtual ~SystemCalibrationSwitch();
		 
		const char *serialize() const;

		void show();

		void setSystemCalibrationSwitch();
		void clearSystemCalibrationSwitch();
		bool isSystemCalibrationSwitch() const;

	private:
	    bool systemCalibrationSwitch;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif //SYSTEMCALIBRATIONSWITCH_H
