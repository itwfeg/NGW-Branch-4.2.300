/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CATEGORYGRAPHICMANAGER_H
#define CATEGORYGRAPHICMANAGER_H

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


class CategoryGraphicManager : public AbstractGraphicManager
{
	public:

		CategoryGraphicManager();
		CategoryGraphicManager(hfc::HFCManagerContainer *container);
		virtual ~CategoryGraphicManager();

		/* DataManager virtual methods. */
		UINT8 getWriteEvent() const { return WriteCategoryGraphic; }
		UINT8 getDeleteEvent() const { return DeleteCategoryGraphic; }
		UINT8 getDeleteAllEvent() const { return DeleteAllCategoryGraphics; }


		bool handleGraphicMessage(SessionMessage *msg);

		void handleDeleteAll();


	private:

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //CATEGORYGRAPHICMANAGER_H
