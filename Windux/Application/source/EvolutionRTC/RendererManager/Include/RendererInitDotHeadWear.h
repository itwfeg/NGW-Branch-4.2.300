#ifndef RENDERERINITDOTHEADWEAR_H
#define RENDERERINITDOTHEADWEAR_H

#include "EvolMessage.h"

//class HobThread;

class RendererInitDotHeadWear : public EvolMessage {
	public:
		RendererInitDotHeadWear(Hobart::CEvolMsgReceiver *pHobThread = NULL, UINT16 printheadSize = 0);
		RendererInitDotHeadWear(const char *buffer);
		~RendererInitDotHeadWear();
		 
		const char *serialize() const;

		void show() const;

		UINT16 getPrintheadSize() const;
		void setPrintheadSize(UINT16 printheadSize);

	private:
		UINT16 printheadSize;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //RENDERERINITDOTHEADWEAR_H