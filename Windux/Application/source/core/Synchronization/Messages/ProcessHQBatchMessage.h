#ifndef PROCESSHQBATCHMESSAGE_H
#define PROCESSHQBATCHMESSAGE_H



#include "EvolMessage.h"


namespace synchronization
{


	class ProcessHQBatchMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		ProcessHQBatchMessage(int number);
		ProcessHQBatchMessage(const char* buffer);
		ProcessHQBatchMessage(const ProcessHQBatchMessage& sm);
		virtual ~ProcessHQBatchMessage();


		/* --- Getters and setters --- */
		int getBatchNumber() const { return batchNumber_; }
		void setBatchNumber(int batchNumber) { batchNumber_ = batchNumber; }


		/* --- Function members --- */
		/* Operators*/
		ProcessHQBatchMessage& operator = (const ProcessHQBatchMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Data members --- */
		int batchNumber_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
		/* --- Constructors and destructor --- */


		/* --- Function members --- */
	};


}



#endif /* PROCESSHQBATCHMESSAGE_H */
