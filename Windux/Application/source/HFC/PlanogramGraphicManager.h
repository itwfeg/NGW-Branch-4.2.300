/* Fastrack Scale Project
 * COPYRIGHT (c) 2013.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PLANOGRAMGRAPHICMANAGER_H
#define PLANOGRAMGRAPHICMANAGER_H

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


class PlanogramGraphicManager : public AbstractGraphicManager
{
	public:

		PlanogramGraphicManager();
		PlanogramGraphicManager(hfc::HFCManagerContainer *container);
		virtual ~PlanogramGraphicManager();

		/* DataManager virtual methods. */
		UINT8 getWriteEvent() const { return WritePlanogramGraphic; }
		UINT8 getDeleteEvent() const { return DeletePlanogramGraphic; }
		UINT8 getDeleteAllEvent() const { return DeleteAllPlanogramGraphics; }

		bool handleGraphicMessage(SessionMessage *msg);

		void handleDeleteAll();

		// Overriding from AbstractGraphicManager
		const std::string getFullPathToImages();
		const std::string getRelativePathToImages();


	private:

		void handleCheckForNewPlanogramReceived();

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //PLANOGRAMGRAPHICMANAGER_H
