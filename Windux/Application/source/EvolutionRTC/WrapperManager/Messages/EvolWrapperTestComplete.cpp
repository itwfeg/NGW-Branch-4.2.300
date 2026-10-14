#include "WrapperEvent.h"
#include "EvolWrapperTestComplete.h"

#include "Debug.h"
#include "Integer.h"
#include "Bool.h"
#include "HobString.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperTestComplete::_dataType_ = 0;
std::string EvolWrapperTestComplete::_dataTypeName_("EvolWrapperTestComplete"); 
#endif


EvolWrapperTestComplete::EvolWrapperTestComplete(Hobart::CEvolMsgReceiver *pHobThread, EvolWrapperTest::WrEvolTestCode wrTestCode)
	: EvolMessage(Wrapper, WrEvolTestComplete, pHobThread)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    testCode = wrTestCode;
    testPassed = false;
}

EvolWrapperTestComplete::EvolWrapperTestComplete(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    EvolMessage::serialize();

    int j = getSerializedLength();

    // Now, for the derived class items
    char *pChar = (char *) &testCode;
    for (unsigned int i = 0; i < sizeof(testCode); i++)
    {
        *pChar++ = buffer[j++];
    }

    pChar = (char *) &data1;
    for (unsigned int i = 0; i < sizeof(data1); i++)
    {
        *pChar++ = buffer[j++];
    }

    pChar = (char *) &data2;
    for (unsigned int i = 0; i < sizeof(data2); i++)
    {
        *pChar++ = buffer[j++];
    }

    pChar = (char *) &testPassed;
    for (unsigned int i = 0; i < sizeof(testPassed); i++)
    {
        *pChar++ = buffer[j++];
    }
}

EvolWrapperTestComplete::~EvolWrapperTestComplete()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif
}

const char *EvolWrapperTestComplete::serialize() const
{
    bool traceOn = Debug::isTracing("EvolWrapperTestComplete");
    HobString msgDebug = "serialize(): ";

    EvolMessage::serialize();

    // Now, for the derived class items
    char *pChar = (char *) &testCode;
    for (unsigned int i = 0; i < sizeof(testCode); i++)
    {
        serializedData.append(1, pChar[i]);
    }
    if (traceOn)
    {
        HobString msgDebug2 = msgDebug;
        msgDebug2.append("  sizeof(testCode) = ").append(Integer::toString(sizeof(testCode)));
        Debug::trace("EvolWrapperTestComplete", msgDebug2);
    }

    pChar = (char *) &data1;
    for (unsigned int i = 0; i < sizeof(data1); i++)
    {
        serializedData.append(1, pChar[i]);
    }
    if (traceOn)
    {
        HobString msgDebug2 = msgDebug;
        msgDebug2.append("  sizeof(data1) = ").append(Integer::toString(sizeof(data1)));
        Debug::trace("EvolWrapperTestComplete", msgDebug2);
    }

    pChar = (char *) &data2;
    for (unsigned int i = 0; i < sizeof(data2); i++)
    {
        serializedData.append(1, pChar[i]);
    }
    if (traceOn)
    {
        HobString msgDebug2 = msgDebug;
        msgDebug2.append("  sizeof(data2) = ").append(Integer::toString(sizeof(data2)));
        Debug::trace("EvolWrapperTestComplete", msgDebug2);
    }

    pChar = (char *) &testPassed;
    for (unsigned int i = 0; i < sizeof(testPassed); i++)
    {
        serializedData.append(1, pChar[i]);
    }
    if (traceOn)
    {
        HobString msgDebug2 = msgDebug;
        msgDebug2.append("  sizeof(testPassed) = ").append(Integer::toString(sizeof(testPassed)));
        Debug::trace("EvolWrapperTestComplete", msgDebug2);
    }

    return serializedData.data();
}

void EvolWrapperTestComplete::show()
{
    if (Debug::isTracing("EvolWrapperTestComplete"))
    {
        HobString msgDebug = "Test: ";

        EvolMessage::show();

        HobString testName = "";
        EvolWrapperTest::getTestName(testCode, testName);
        msgDebug.append(testName);

        msgDebug.append("  data1: ").append(Integer::toString(data1));
        msgDebug.append("  data2: ").append(Integer::toString(data2));

        msgDebug.append("  testPassed = ").append(Bool::toString(testPassed));
        Debug::trace("EvolWrapperTestComplete", msgDebug);
    }
}

EvolWrapperTest::WrEvolTestCode EvolWrapperTestComplete::getTestCode()
{
    return testCode;
}

EvolWrapperTest::WrEvolTestCode EvolWrapperTestComplete::getTestCode() const
{
    return testCode;
}

void EvolWrapperTestComplete::setTestCode(EvolWrapperTest::WrEvolTestCode wrTestCode)
{
    testCode = wrTestCode;
}

UINT16 EvolWrapperTestComplete::getData1() const
{
    return data1;
}
UINT16 EvolWrapperTestComplete::getData1()
{
    return data1;
}
void EvolWrapperTestComplete::setData1(UINT16 value)
{
    data1 = value;
}

UINT16 EvolWrapperTestComplete::getData2() const
{
    return data2;
}
UINT16 EvolWrapperTestComplete::getData2()
{
    return data2;
}
void EvolWrapperTestComplete::setData2(UINT16 value)
{
    data2 = value;
}

bool EvolWrapperTestComplete::getTestPassed()
{
    return testPassed;
}
bool EvolWrapperTestComplete::getTestPassed() const
{
    return testPassed;
}

void EvolWrapperTestComplete::setTestPassed(bool value)
{
    testPassed = value;
}
