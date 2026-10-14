#ifndef CLASYSTEMSTATUS_H
#define CLASYSTEMSTATUS_H

#include <string>
#include "EvolMessage.h"
#include "LabelApplierEvent.h"
#include "LabelApplier.h"
#include "System.h"



class CLASystemStatus : public EvolMessage
{
	public:
		CLASystemStatus(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		CLASystemStatus(const char *buffer);
		~CLASystemStatus();

		const char *serialize()const;

		/*	SETUP CLASysInfo	*/
		ULONG getConfigurationChecksum() const {return configurationChecksum_;}
		void setConfigurationChecksum(ULONG configurationChecksum) {configurationChecksum_ = configurationChecksum;}

		UINT16 getAppFlashMajor() const {return appFlashMajor_;}
		void setAppFlashMajor(UINT16 appFlashMajor) {appFlashMajor_ = appFlashMajor;}
		UINT16 getAppFlashMinor() const {return appFlashMinor_;}
		void setAppFlashMinor(UINT16 appFlashMinor) {appFlashMinor_ = appFlashMinor;}

		UINT16 getDataFlashMajor() const {return dataFlashMajor_;}
		void setDataFlashMajor(UINT16 dataFlashMajor) {dataFlashMajor_ = dataFlashMajor;}
		UINT16 getDataFlashMinor() const {return dataFlashMinor_;}
		void setDataFlashMinor(UINT16 dataFlashMinor) {dataFlashMinor_ = dataFlashMinor;}

		UINT16 getBootFlashMajor() const {return bootFlashMajor_;}
		void setBootFlashMajor(UINT16 bootFlashMajor) {bootFlashMajor_ = bootFlashMajor;}
		UINT16 getBootFlashMinor() const {return bootFlashMinor_;}
		void setBootFlashMinor(UINT16 bootFlashMinor) {bootFlashMinor_ = bootFlashMinor;}
		
		void show() const;
		
	private:
		
		/*	CLASysInfo	*/
		ULONG			configurationChecksum_;
		UINT16			appFlashMajor_;
		UINT16			appFlashMinor_;
		UINT16			dataFlashMajor_;
		UINT16			dataFlashMinor_;
		UINT16			bootFlashMajor_;
		UINT16			bootFlashMinor_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif