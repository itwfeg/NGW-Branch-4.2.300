#ifndef EVOLWRAPPERTESTCOMPLETE_H
#define EVOLWRAPPERTESTCOMPLETE_H

#include "EvolMessage.h"
#include "EvolWrapperTest.h"

//class HobThread;

class EvolWrapperTestComplete : public EvolMessage
{
    public:
        static const short TEST_FAILED  = 0;
        static const short TEST_PASSED  = 1;

        EvolWrapperTestComplete(Hobart::CEvolMsgReceiver *pHobThread = NULL, EvolWrapperTest::WrEvolTestCode wrTestCode = EvolWrapperTest::NoServiceTest);
        EvolWrapperTestComplete(const char *buffer);
        virtual ~EvolWrapperTestComplete();

        const char *serialize()const;

        void show();

        EvolWrapperTest::WrEvolTestCode getTestCode();
        EvolWrapperTest::WrEvolTestCode getTestCode() const;		//new ats
        void setTestCode(EvolWrapperTest::WrEvolTestCode wrTestCode);

		//	Broke up INT32 data into two UINT16 values, as that's the way it is in the wrapper code [CCD 12/2010]
        UINT16 getData1() const;
        UINT16 getData1();
        void setData1(UINT16 value);

        UINT16 getData2() const;
        UINT16 getData2();
        void setData2(UINT16 value);

        bool getTestPassed();
        bool getTestPassed() const;
        void setTestPassed(bool value);


    private:
        EvolWrapperTest::WrEvolTestCode testCode;
	    UINT16							data1;
	    UINT16							data2;
        bool							testPassed;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif //EVOLWRAPPERTESTCOMPLETE_H
