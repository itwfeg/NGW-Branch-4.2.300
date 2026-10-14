#ifndef BOARDSITETESTMANAGER_H
#define BOARDSITETESTMANAGER_H
#include "CEvolMsgReceiver.h"
#include "CTLSThread.h"
#include "CHobartTimer.h"
#include <mutex>
#include "BoardSite.h"
#include "SystemStartManager.h"
#include "BoardSiteTestCompleteUI.h"
#include "BoardSitePercentComplete.h"

class BoardSiteTestManager : public Hobart::CEvolMsgReceiver, public Util::CRunnableClass, public Hobart::CHobartTimer
{
	public:

    typedef enum {
			_PHASE0,
			_PHASE1,
            _PHASE2,
            _LVDS_ONLY_PHASE0
		}BoardSitePhase;
		/**
		* @brief Gets an instance of the board site test manager
		*
		*/
		static BoardSiteTestManager *getInstance( BoardSitePhase phase );

        /**
         * @param msg the message received
         */
        virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );
        /**
         * @param timer_id the id of the timer that timed out
         */
        virtual void timeoutProcess( __int32 timer_id );

    protected:
        virtual DWORD Run();

	private:
        enum TestStates
        {
    	    _Init,
            _WaitingForTest,
            _ProcessTest,
            _SendTestResults,
            _StartUpKpc,
            _WaitingForKPC,
            _WaitForKPCModel,
            _SetupForDefaults,
            _WaitForDefaults,
            _WaitForWgStatus,
            _SendCutterStatus,
            _WaitForCutterStatus,
            _StartCutterTest,
            _WaitForCutterComplete,
            _WaitForCutterCut,
            _WaitForDotWear,
            _Exit
        };

        BoardSitePhase phase_;
        TestStates state_;
        
        /**
         * @brief Destructor
         * 
         */
		~BoardSiteTestManager();

        /**
         * @brief Constructor
         * 
         */
		BoardSiteTestManager( BoardSitePhase phase );        
		static std::mutex  instanceMutex_;
		static BoardSiteTestManager *pInstance_;
        
        //phase 0 test to run automatically
        std::vector< BoardSiteTests > autoTestsList_;
        
        Util::CMSGEvent m_ExitEvent;
        Util::CTLSThread *pMyRunThread_;   
        
        std::mutex mutex_;

        BoardSiteTestCompleteUI uiMsg;
        BoardSitePercentComplete cUiMsg;

        unsigned int autoIndex_;
        unsigned int wgStatusCntr_;
        
        #define MAX_STATUS_CNT          10
        #define WAIT_RETRY_CNT          300
        
        bool can0Operational_;
        bool kpcSerialFlashTest_;
        bool kpcModelTest_;
        bool kpcValueMax_;
        
        bool defaultsCmplt_;
        bool memTestOnce_;
        /**
         * @brief execute tests within auto test list.
         * 
         */
        void autoProcessTest( void );
        static const int waitTime = 20 * 1000;       //10 Seconds
        enum BoardSiteTimers
		{
			KPCTimeout_ = 1,
            KPCCutterTimeout_,
            KPCDotWearTimeOut_,		
		};

        enum CutterState
        {
            CHome_,
            CNotHome_,
            CReturnHome_
        };

        CutterState cState_;

        /**
         * @brief execute host requested board tests.
         * 
         */
        void processTest( const char *recvBuffer ); 
        bool busy_; 

        unsigned long waitCntr_;   
        void processError( BoardSiteTests test, int errNumber );
};
#endif