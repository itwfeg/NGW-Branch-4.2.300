/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SOFTKEYGRAPHICMANAGER_H
#define SOFTKEYGRAPHICMANAGER_H

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


class SoftkeyGraphicManager : public AbstractGraphicManager
{
	public:

		SoftkeyGraphicManager();
		SoftkeyGraphicManager(hfc::HFCManagerContainer *container);
		virtual ~SoftkeyGraphicManager();

		/* DataManager virtual methods. */
		UINT8 getWriteEvent() const { return WriteSoftkeyGraphic; }
		UINT8 getDeleteEvent() const { return DeleteSoftkeyGraphic; }
		UINT8 getDeleteAllEvent() const { return DeleteAllSoftkeyGraphics; }


		bool handleGraphicMessage(SessionMessage *msg);

		void handleDeleteAll();


	private:

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //SOFTKEYGRAPHICMANAGER_H
