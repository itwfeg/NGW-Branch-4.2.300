/**
 * @file WeigherValueMaxWriteAssembly.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max assembly number
 * Created on: Sept 20, 2017
 *
 **/
#ifndef WEIGHERVALUEMAXWRITEASSEMBLY_H
#define WEIGHERVALUEMAXWRITEASSEMBLY_H
#include <string>
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"

class WeigherValueMaxWriteAssembly : public EvolMessage
{
	public:
	WeigherValueMaxWriteAssembly( std::string str, Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherValueMaxWriteAssembly( const char *buffer );
	~WeigherValueMaxWriteAssembly();

	static const unsigned int _AssemblyNumbSize_	= 10;

	std::string getAssemblyNumber() { return assemblyNumber; }

	const char *serialize()const;

	private:
	std::string  assemblyNumber;

};






#endif
