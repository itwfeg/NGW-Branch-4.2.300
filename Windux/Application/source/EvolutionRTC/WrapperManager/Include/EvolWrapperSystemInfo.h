#ifndef EVOLWRAPPAERSYSTEMINFO_H
#define EVOLWRAPPAERSYSTEMINFO_H

#include <string>
#include "Wrapper.h"
#include "WrapperGeneric.h"



class EvolWrapperSystemInfo : public WrapperGeneric
{
	public:
		EvolWrapperSystemInfo(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		EvolWrapperSystemInfo(const char *buffer);
		~EvolWrapperSystemInfo();

		const char *serialize()const;

		static const unsigned int _PartNumbSize_	= 10;
		static const unsigned int _RevisionSize_	= 2;
		static const unsigned int _SerialNumbSize_	= 12;
		static const unsigned int _DateSize_		= 6;
		static const unsigned int _CycleCountSize_	= 4;

		//static const unsigned int _SerialNumbSizeSerial_	= 11;	//used when serialized
		//static const unsigned int _PartNumbSizeSerial_		= 9;	//used when serialized

		typedef enum
		{
			NoModel,
			AWS1RL,
			AWS1LR,
			AWS1MLR,		//AWS manual machine left to right
			AWS1LR_I,		//AWS international left to right
			AWS1RL_I,		//AWS international right to left
			AWS1BWRL,		//AWS Lite or EWS (a defeatured AWS) right to left
			AWS1BWLR,		//AWS Lite or EWS (a defeatured AWS) left to right
			AWS2RL,			//AWS with a Wide Elevator only, right to left
			AWS2LR,			//AWS with a Wide Elevator only, left to right
			AWS3RL,			//AWS Enhanced, two roll, right to left
			AWS3LR,			//AWS Enhanced, two roll, left to right
			AWS3BRL,		//AWS Enhanced, single roll, right to left
			AWS3BLR,		//AWS Enhanced, single roll, left to right
			AWS4RL,			//AWS NGW Reduced footprint, two roll, right to left
			AWS4LR,			//AWS NGW Reduced footprint, two roll, left to right
			AWS4BRL,		//AWS NGW Reduced footprint, single roll, right to left
			AWS4BLR,		//AWS NGW Reduced footprint, single roll, left to right
			NGS1BRL,		//NOT USED?  DO NOT REMOVE ... NGS Small Footprint, Basic Wrapper, Single roll, right to left
			NGS1BLR,		//NOT USED?  DO NOT REMOVE ... NGS Small Footprint, Basic Wrapper, Single roll, left to right
			NGW3BRL,		//NGW3 Full CAN, Reduced footprint, Single roll, right to left
			NGW3BLR,		//NGW3 Full CAN, Reduced footprint, Single roll, left to right
			NGW3RL,			//NGW3 Full CAN, Reduced footprint, Dual roll, right to left
			NGW3LR,			//NGW3 Full CAN, Reduced footprint, Dual roll, left to right

		}EvolAWSModel;
		
		bool getBoardChecksumStatus()const {return boardChecksumStatus_;}
		void setBoardChecksumStatus(bool checkSumStatus) {boardChecksumStatus_ = checkSumStatus;}
		
		bool getAwsChecksumStatus() const {return awsChecksumStatus_;}
		void setAwsChecksumStatus(bool checkSumStatus)  {awsChecksumStatus_ = checkSumStatus;}
		
		std::string getMotorBoardPartNumber() const {return motorBoardPartNumber_;}
		void setMotorBoardPartNumber(std::string & partNumber){motorBoardPartNumber_ = partNumber;}

		std::string getMotorBoardRevision() const {return motorBoardRev_;}
		void setMotorBoardRevision(std::string & revision){motorBoardRev_ = revision;}
		bool isBoardCombo() const {return (motorBoardRev_ != "00") ? true : false;}

		std::string getMotorBoardDateOfManufacture() const {return motorBoardDateOfManufacture_;}
		void setMotorBoardDateOfManufacture(std::string & date){motorBoardDateOfManufacture_ = date;}



		std::string getAwsSerialNumber() const {return awsSerialNumber_;}
		void setAwsSerialNumber(std::string & serialNumber){awsSerialNumber_ = serialNumber;}

		std::string getAwsDateOfManufacture() const {return awsDateOfManufacture_;}
		void setAwsDateOfManufacture(std::string & date){awsDateOfManufacture_ = date;}

		std::string getAwsDateOfInstall() const {return awsDateOfInstall_;}
		void setAwsDateOfInstall(std::string & date){awsDateOfInstall_ = date;}

		EvolAWSModel getModelNumber() const {return modelNumber_;}
		void setModelNumber(unsigned int model) {modelNumber_ = (EvolAWSModel)model;}

		std::string getAwsModelNumber()const {return awsModelNumber_;}  		
		
		bool isEWSModelType() const {return ((modelNumber_ == AWS1BWRL) || (modelNumber_ == AWS1BWLR)) ? true : false;}
		bool isModelNumberBlank() const {return (modelNumber_ == NoModel) ? true : false;}

		ULONG getAwsCycleCount() const {return wrapperCycleCount_;}
		void setAwsCycleCount(ULONG count) {wrapperCycleCount_ = count;}

		int isLeftToRight();
		
		void show();
		
	private:
		
		void setAwsModelNumber(unsigned int model);	
		
		bool				boardChecksumStatus_;
		bool				awsChecksumStatus_;
		
		std::string			motorBoardPartNumber_;
		std::string			motorBoardRev_;
		std::string			motorBoardDateOfManufacture_;
		
		std::string			awsSerialNumber_;
		std::string			awsDateOfManufacture_;
		std::string			awsDateOfInstall_;
		
		std::string			awsModelNumber_;
		EvolAWSModel		modelNumber_;		
		ULONG				wrapperCycleCount_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif
