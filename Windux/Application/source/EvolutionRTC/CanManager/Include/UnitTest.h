/**
 * @file UnitTest.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Class for Unit Testing the CAN messaging for the peripherals
 * Created on: Jul 24, 2015
 * @addtogroup UnitTests
 * @{
 */
#ifndef UNITTEST_H_
#define UNITTEST_H_
#include "CEvolMsgReceiver.h"
#include "CTLSThread.h"
#include "CHobartTimer.h"
#include "Printer.h"
#include "WrapperEvent.h"

/**
 * @class UnitTest
 * @brief Unit Test Class for peripheral CAN messaging
 * 
 */
class UnitTest : public Hobart::CEvolMsgReceiver, public Util::CRunnableClass, public Hobart::CHobartTimer {
    public:
        
        typedef enum
        {
        	_UpgradeUI_Messaging,
        	_Printer__Messaging,
        	_Weigher__Messaging,
        	_NGWrapper_Messaging,
        	_AllWrapperPeriph_Messaging,
        	_AWS3Messaging,
        	_PPPMessaging,
        	_UnitTestEnd

        }DUTTestType;

        /**
         * @brief Gets an instance of the Unit Test
         *
         * @return An instance of the Unit test
         */
        static UnitTest *getInstance(DUTTestType test);

        /**
         * @brief Destructor
         * 
         */
        ~UnitTest();
        
        /**
         * @brief Tests the printer messaging
         * 
         * @param test the test number
         * @return true when the test is finished
         */
        bool testPrinterMessaging(unsigned short test);
        
        /**
         * @brief Tests the weigher messaging
         * 
         * @param test the test number
         * @return true when the test is finished
         */
        bool testWeigherMessaging(unsigned short test);
        
        /**
         * @brief Tests the wrapper messaging
         * 
         * @param test the test number
         * @return true when the test is finished
         */
        bool testWrapperMessaging(unsigned short test);
        
        /**
         * @brief Tests the prepack messaging
         * 
         * @param test the test number
         */
        void testPrepackMessaging(unsigned short test);
        
        /**
         * @brief Tests the printer messaging
         * 
         * @param test the test number
         * @return true when the test is finished
         */
        bool testNGWPrinterMessaging(unsigned short test);

        /**
         * @brief Tests the NGW upgrade message to and from UI
         *
         * @param test the test number
         * @return true when the test is finished
         */
        bool testNGWUpgradeMessaging(unsigned short test);

        /**
         * @brief Tests the AWS3 messaging to and from Wrapper peripheral
         *
         * @param test the test number
         * @return true when the test is finished
         */
        bool testAWS3Messaging(unsigned short test);
        
        /**
         * @brief Dispatches a printer request message to all of the printers
         * 
         * @param prReqCode the code for the message requested
         */
        void dispatchPrinterRequest(PrRequestCode prReqCode);
        
        /**
         * @brief Dispatches a printer execute message to all of the printers
         * 
         * @param prExecCode the code for the message to be executed
         */
        void dispatchPrinterExecute(PrExecuteCode prExecCode);
        
        /**
         * @brief Dispatches a printer configuration message to all of the printers
         * 
         */
        void dispatchPrinterConfiguration();
        
        /**
         * @brief Dispatches a printer label transfer message to all of the printers
         * 
         */
        void dispatchPrinterLabelTransfer();
        
        /**
         * @brief Dispatches a printer mask message to all of the printers
         * 
         */
        void dispatchPrinterMask();
        
        /**
         * @brief Dispatches a printer size message to all of the printers
         * 
         */
        void dispatchPrinterSize();
        
        /**
         * @brief Dispatches a printer test message to all of the printers
         * 
         */
        void dispatchPrinterTest();
        
        void handleWrappermessage( WrEvent wrEvent, const char *recvBuffer );
        /**
         * @brief Timer timeout handler
         * 
         * @param _id the timer id
         */
        virtual void timeoutProcess( __int32 _id );
        
        /**
         * @brief Event handler for the Unit Test
         * 
         * @param msg the message received by the Unit Test
         */
        virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );

    private:
        static UnitTest *pInstance;
        Util::CSmartPtr<Util::CTLSThread> _runThread;
        
        /**
         * @brief Constructor
         * 
         */
        UnitTest(DUTTestType test);

        DUTTestType _testType;
        bool _executeTest;
        unsigned long _testTime;
        unsigned short _testCount;
        bool _weigherTestComplete;
        bool _printerTestComplete;
        bool _wrapperTestComplete;
        bool _upgradeTestComplete;
        bool _aws3TestComplete;
        bool _pppTestComplete;

        bool _wait;

        static const unsigned short _UITestEnd = 20;
        static const unsigned short _UITestWrapperEnd = 21;
        static const unsigned short _UITestNGWPrinterEnd = 30;
        static const unsigned short _UITestNGWWeigher = 30;
        static const unsigned short _UITestNGWWrapper = 11;
        static const unsigned short _AWS3TestEnd = 7;
        /**
         * @enum 
         * @brief Contains the Unit Test timers
         * 
         */
        enum {
            _MessageTimer   /**<_MessageTimer. Sends out messages at a specified speed*/
        };
        
        /**
         * @brief Runs a unit message test
         * 
         * @return 0 when we finish our testing
         */
        virtual DWORD Run();
};
/** @} */
#endif /* UNITTEST_H_ */
