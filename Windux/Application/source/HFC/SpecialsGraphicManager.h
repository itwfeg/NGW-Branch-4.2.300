/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SPECIALSGRAPHICMANAGER_H
#define SPECIALSGRAPHICMANAGER_H

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


class SpecialsGraphicManager : public AbstractGraphicManager
{
	public:

		SpecialsGraphicManager();
		SpecialsGraphicManager(hfc::HFCManagerContainer *container);
		virtual ~SpecialsGraphicManager();

		/* DataManager virtual methods. */
		UINT8 getWriteEvent() const { return WriteSpecialsGraphic; }
		UINT8 getDeleteEvent() const { return DeleteSpecialsGraphic; }
		UINT8 getDeleteAllEvent() const { return DeleteAllSpecialsGraphics; }


		bool handleGraphicMessage(SessionMessage *msg);

		void handleDeleteAll();


	private:

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //SPECIALSGRAPHICMANAGER_H
