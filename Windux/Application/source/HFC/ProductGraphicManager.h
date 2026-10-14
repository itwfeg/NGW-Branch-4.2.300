/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTGRAPHICMANAGER_H
#define PRODUCTGRAPHICMANAGER_H

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


class ProductGraphicManager : public AbstractGraphicManager
{
	public:

		ProductGraphicManager();
		ProductGraphicManager(hfc::HFCManagerContainer *container);
		virtual ~ProductGraphicManager();

		/* DataManager virtual methods. */
		UINT8 getWriteEvent() const { return WriteProductGraphic; }
		UINT8 getDeleteEvent() const { return DeleteProductGraphic; }
		UINT8 getDeleteAllEvent() const { return DeleteAllProductGraphics; }


		bool handleGraphicMessage(SessionMessage *msg);

		void handleDeleteAll();


	private:

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //PRODUCTGRAPHICMANAGER_H
