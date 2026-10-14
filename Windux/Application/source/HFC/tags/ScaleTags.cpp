#include "ScaleTags.h"



#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int ScaleTags::_dataType_ = 0;
		std::string ScaleTags::_dataTypeName_("hfc::ScaleTags");
	#endif


	const std::string ScaleTags::_scaleNumber_                         = "S_";
	const std::string ScaleTags::_scaleRole_                           = "SR";
	const std::string ScaleTags::_scaleHostName_                       = "SH";
	const std::string ScaleTags::_scaleIPAddress_                      = "SP";
	const std::string ScaleTags::_scaleCreationDateTime_               = "Sc";
	const std::string ScaleTags::_scaleState_                          = "Ss";
	const std::string ScaleTags::_scaleStateChangedDateTime_           = "SG";
	const std::string ScaleTags::_scaleRegistering_                    = "Sr";
	const std::string ScaleTags::_scaleDeregistering_                  = "SE";
	const std::string ScaleTags::_scaleNotifiedBatches_                = "Sb";
	const std::string ScaleTags::_scaleNotifiedBatchesDateTime_        = "SO";
	const std::string ScaleTags::_scaleNotifiedOnline_                 = "So";
	const std::string ScaleTags::_scaleNotifiedOnlineDateTime_         = "SN";
	const std::string ScaleTags::_scaleRetryCount_                     = "SY";
	const std::string ScaleTags::_scaleVersion_                        = "SV";
	const std::string ScaleTags::_scaleVersionReceived_                = "Sv";
	const std::string ScaleTags::_scaleVersionReceivedDateTime_        = "Sz";
	const std::string ScaleTags::_scaleLastCommSuccessful_             = "SA";
	const std::string ScaleTags::_scaleLastSuccessfulCommDateTime_     = "Sa";
	const std::string ScaleTags::_scaleFirstFailedCommDateTime_        = "Sf";
	const std::string ScaleTags::_scaleTotalSuccessfulCommunications_  = "S1";
	const std::string ScaleTags::_scaleTotalFailedCommunications_      = "S2";
	const std::string ScaleTags::_scaleTotalBusyCommunications_        = "S3";

	bool     ScaleTags::_initialized_ = false;
	TagsList ScaleTags::_scaleTags_(endScaleFieldOffsets);
	TagsMap  ScaleTags::_fieldMap_;  //example pair: r#,fieldOffsetTextNum



	ScaleTags::ScaleTags(FieldOffsetsList * pFieldList)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif


		if ( !_initialized_ )
		{
			//initialize the the table of tags for the Scale class
			_scaleTags_[0]                                             = emptyTag;
			_scaleTags_[fieldOffsetScaleNumber]                        = _scaleNumber_;
			_scaleTags_[fieldOffsetScaleRole]                          = _scaleRole_;
			_scaleTags_[fieldOffsetScaleHostName]                      = _scaleHostName_;
			_scaleTags_[fieldOffsetScaleIPAddress]                     = _scaleIPAddress_;
			_scaleTags_[fieldOffsetScaleCreationDateTime]              = _scaleCreationDateTime_;
			_scaleTags_[fieldOffsetScaleState]                         = _scaleState_;
			_scaleTags_[fieldOffsetScaleStateChangedDateTime]          = _scaleStateChangedDateTime_;
			_scaleTags_[fieldOffsetScaleRegistering]                   = _scaleRegistering_;
			_scaleTags_[fieldOffsetScaleDeregistering]                 = _scaleDeregistering_;
			_scaleTags_[fieldOffsetScaleNotifiedBatches]               = _scaleNotifiedBatches_;
			_scaleTags_[fieldOffsetScaleNotifiedBatchesDateTime]       = _scaleNotifiedBatchesDateTime_;
			_scaleTags_[fieldOffsetScaleNotifiedOnline]                = _scaleNotifiedOnline_;
			_scaleTags_[fieldOffsetScaleNotifiedOnlineDateTime]        = _scaleNotifiedOnlineDateTime_;
			_scaleTags_[fieldOffsetScaleRetryCount]                    = _scaleRetryCount_;
			_scaleTags_[fieldOffsetScaleVersion]                       = _scaleVersion_;
			_scaleTags_[fieldOffsetScaleVersionReceived]               = _scaleVersionReceived_;
			_scaleTags_[fieldOffsetScaleVersionReceivedDateTime]       = _scaleVersionReceivedDateTime_;
			_scaleTags_[fieldOffsetScaleLastCommSuccessful]            = _scaleLastCommSuccessful_;
			_scaleTags_[fieldOffsetScaleLastSuccessfulCommDateTime]    = _scaleLastSuccessfulCommDateTime_;
			_scaleTags_[fieldOffsetScaleFirstFailedCommDateTime]       = _scaleFirstFailedCommDateTime_;
			_scaleTags_[fieldOffsetScaleTotalSuccessfulCommunications] = _scaleTotalSuccessfulCommunications_;
			_scaleTags_[fieldOffsetScaleTotalFailedCommunications]     = _scaleTotalFailedCommunications_;
			_scaleTags_[fieldOffsetScaleTotalBusyCommunications]       = _scaleTotalBusyCommunications_;

			_initialized_ = true;
		}


		if ( pFieldList )
			putTagsInMap(pFieldList);
	}


	ScaleTags::~ScaleTags()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


}