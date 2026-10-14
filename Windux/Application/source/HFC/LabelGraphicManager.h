/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef LABELGRAPHICMANAGER_H
#define LABELGRAPHICMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "GraphicEvent.h"
#include "Graphic.h"
#include "GraphicFileNames.h"
#include "AbstractGraphicManager.h"

class SessionMessage;
class DBSession;

namespace hfc
{
	class HFCManagerContainer;
}


class LabelGraphicManager : public AbstractGraphicManager
{
	public:

		LabelGraphicManager();
		LabelGraphicManager(hfc::HFCManagerContainer *container);
		virtual ~LabelGraphicManager();

		/* DataManager virtual methods. */
		UINT8 getWriteEvent() const { return DtWrite; }
		UINT8 getDeleteEvent() const { return DtDelete; }
		UINT8 getDeleteAllEvent() const { return DtDeleteAll; }


		bool handleGraphicMessage(SessionMessage *msg);

		void handleDeleteAll();

		int countAllCustomGraphics();


	private:

		void init();

		void handleFirst();
		void handleLast();
		void handleNext(const Graphic *referenceGraphic);
		void handlePrevious(const Graphic *referenceGraphic);

		void handleReadAllNames();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //LABELGRAPHICMANAGER_H
