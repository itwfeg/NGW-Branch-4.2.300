/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CMReaderWriterEvolUI.h"
#include "Integer.h"
#include "Hex.h"


CMReaderWriterEvolUI::CMReaderWriterEvolUI()
{
    readerWriterId = CMReaderWriter::EVOL_UI_ID;
    recordIdentifierLength = 0;
}

CMReaderWriterEvolUI::~CMReaderWriterEvolUI()
{
}

int CMReaderWriterEvolUI::getRecordHeaderLength() const
{
    return BUFFER_LENGTH_INDICATOR_LENGTH;
}

int CMReaderWriterEvolUI::getRecordFooterLength() const
{
    return 0;
}


int CMReaderWriterEvolUI::finalizeRecordForSending(char *buff, const char *msg, int length)
{
	const bool tracing( Debug::isTracing( "CMReaderWriter.EvolUI" ) );

    const int totalLength( BUFFER_LENGTH_INDICATOR_LENGTH + length );

    sprintf(buff, "%0*d", BUFFER_LENGTH_INDICATOR_LENGTH, totalLength);
    memcpy( &buff[BUFFER_LENGTH_INDICATOR_LENGTH], msg, length);
    buff[totalLength] = NULL;

    if ( tracing )
    	Debug::trace( "CMReaderWriter.EvolUI", HobString("finalizeRecordForSending - ") + Hex::toString(buff, totalLength) );

    return totalLength;
}


bool CMReaderWriterEvolUI::finishedReadingRecord(const char *msg, int length)
{
    if (length < BUFFER_LENGTH_INDICATOR_LENGTH)
    {
        totalRecordLength = 0;
        finishedReadingHeader = false;
        return false;
    }

    if (finishedReadingHeader)
    {
        if (length == totalRecordLength)
            return true;
    }
    else if (length == BUFFER_LENGTH_INDICATOR_LENGTH)
    {
        char *tempBuff = new char[BUFFER_LENGTH_INDICATOR_LENGTH + 1];
        memcpy(tempBuff, msg, BUFFER_LENGTH_INDICATOR_LENGTH);
        tempBuff[BUFFER_LENGTH_INDICATOR_LENGTH] = (char)NULL;

        totalRecordLength = Integer::valueOf(tempBuff);
        Debug::trace("CMReaderWriter.EvolUI", HobString("determined total record length: ") +
                HobString(Integer::toString(totalRecordLength)));
        delete[] tempBuff;
        finishedReadingHeader = true;
    }

    return false;
}

bool CMReaderWriterEvolUI::finishedReadingRecord(const HobString &msg)
{
#ifdef LINUX_FIX
    if (msg.size() < (size_t)BUFFER_LENGTH_INDICATOR_LENGTH)
#else
    if (msg.size() < BUFFER_LENGTH_INDICATOR_LENGTH)
#endif
    {
        totalRecordLength = 0;
        finishedReadingHeader = false;
        return false;
    }

    if (finishedReadingHeader)
    {
        if ((int)msg.size() == totalRecordLength)
            return true;
    }
#ifdef LINUX_FIX
    else if (msg.size() == (size_t)BUFFER_LENGTH_INDICATOR_LENGTH)
#else
    else if (msg.size() == BUFFER_LENGTH_INDICATOR_LENGTH)
#endif
    {
        char *tempBuff = new char[BUFFER_LENGTH_INDICATOR_LENGTH + 1];
        memcpy(tempBuff, msg.c_str(), BUFFER_LENGTH_INDICATOR_LENGTH);
        tempBuff[BUFFER_LENGTH_INDICATOR_LENGTH] = (char)NULL;

        totalRecordLength = Integer::valueOf(tempBuff);
        Debug::trace("CMReaderWriter.EvolUI", HobString("determined total record length: ") +
                HobString(Integer::toString(totalRecordLength)));
        delete[] tempBuff;
        finishedReadingHeader = true;
    }

    return false;
}


int CMReaderWriterEvolUI::formatFromReceive(char *msg, int length, bool UNUSED(isRecordBeginningBuffer))
{
    UNREFERENCED_PARAMETER(isRecordBeginningBuffer);

    if (length > BUFFER_LENGTH_INDICATOR_LENGTH)
    {
        memmove (msg, msg + BUFFER_LENGTH_INDICATOR_LENGTH, length - BUFFER_LENGTH_INDICATOR_LENGTH);
        return length - BUFFER_LENGTH_INDICATOR_LENGTH;
    }
    else if (length == BUFFER_LENGTH_INDICATOR_LENGTH)
    {
        *msg = (char)NULL;
        return 0;
    }
    else
        return -1;
}

int CMReaderWriterEvolUI::formatFromReceive(HobString &msg)
{
#ifdef LINUX_FIX
    if (msg.size() > (size_t)BUFFER_LENGTH_INDICATOR_LENGTH)
#else
    if (msg.size() > BUFFER_LENGTH_INDICATOR_LENGTH)
#endif
    {
        msg = msg.substr(BUFFER_LENGTH_INDICATOR_LENGTH);
        return (int) msg.size();
    }
#ifdef LINUX_FIX
    else if (msg.size() == (size_t)BUFFER_LENGTH_INDICATOR_LENGTH)
#else
    else if (msg.size() == BUFFER_LENGTH_INDICATOR_LENGTH)
#endif
    {
        msg = "";
        return 0;
    }
    else
        return -1;
}


/* static */
bool CMReaderWriterEvolUI::canIdentifyBuffer(const char *msg)
{
    for (int i = 0; i < BUFFER_LENGTH_INDICATOR_LENGTH; i++)
    {
        if (!isdigit(*(msg+i)))
            return false;
    }
    return true;
}

