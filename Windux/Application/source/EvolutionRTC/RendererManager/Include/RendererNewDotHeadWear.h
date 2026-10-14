#ifndef RENDERERNEWDOTHEADWEAR_H
#define RENDERERNEWDOTHEADWEAR_H

#include "EvolMessage.h"

//class HobThread;
static const std::vector<UINT8> null_fill;

class RendererNewDotHeadWear : public EvolMessage {
	public:
		RendererNewDotHeadWear(Hobart::CEvolMsgReceiver *pHobThread = NULL, const std::vector<UINT8> pSource = null_fill);
		RendererNewDotHeadWear(Hobart::CEvolMsgReceiver *pHobThread = NULL, const UINT8 * pSource = NULL, int size = 0);
		RendererNewDotHeadWear(const char *buffer);
		~RendererNewDotHeadWear();
		 
		const char *serialize() const;

		void show();

		const std::vector<UINT8> getSource(){ return pSource; };
		UINT8 sourceAt(int i){return pSource[i];};
		void setSource(const UINT8 *pSource, int size){this->pSource.assign(pSource, pSource + size);};
		void setSource(const std::vector<UINT8>  pSource){ this->pSource = pSource; };

	private:
		std::vector<UINT8> pSource;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //RENDERERNEWDOTHEADWEAR_H