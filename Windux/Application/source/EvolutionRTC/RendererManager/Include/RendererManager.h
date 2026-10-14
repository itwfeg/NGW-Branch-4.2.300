#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H



#include <map>


#include "CEvolMsgReceiver.h"
#include "util/thread/mutex.h"


class ImageControl;



class RendererManager : public Hobart::CEvolMsgReceiver
{
	public:
		// --- Types and subtypes ---
		typedef struct
		{
			int printerType;			//printer type
			const UINT8* sourceId;		//source of the label image
			const UINT8* destinationId;	//destination of the label image
		} RendererDataLocation;

		typedef std::map<UINT16, RendererDataLocation*> dataLocationContainer;
		typedef dataLocationContainer::iterator dataLocationContainerIterator; 


		// --- Constructors and destructor ---
		static RendererManager* getInstance();
		~RendererManager();


		virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );


	private:
		bool subtractDescent_;           // whether to use descent when rendering text onto form
		UINT16 sessionCount;			//sessionId counter
		dataLocationContainer rendererMap;
		UINT8 *marginalBits;
		UINT16 printheadSize;

		static RendererManager* _instance_;
		static util_thread::Mutex<> _instanceMutex_;


		RendererManager();
		
		void handleOpenSession(const char* buffer);
		void handleSetSource(const char* buffer);
		void handleSetDestination(const char* buffer);
		void handleCloseSession(const char* buffer);
		void handleRendering(const char* buffer);
		void handleInitDotHeadWear(const char *buffer);
		void handleNewDotHeadWear(const char *buffer);
			
		void renderLabel(const ImageControl* ic, int printerType = -1);

		static bool writeImageToPGMFile(const char *Filename, int height, int width, int stride, const unsigned char *ImageData);

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif //RENDERERMANAGER_H
