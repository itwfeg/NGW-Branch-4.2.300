#ifndef SCALETAGS_H
#define SCALETAGS_H



#include "Tags.h"



namespace hfc
{


	class ScaleTags : public Tags
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum ScaleFieldOffsets
			{
				fieldOffsetScaleNumber = 1,
				fieldOffsetScaleRole,
				fieldOffsetScaleHostName,
				fieldOffsetScaleIPAddress,
				fieldOffsetScaleCreationDateTime,
				fieldOffsetScaleState,
				fieldOffsetScaleStateChangedDateTime,
				fieldOffsetScaleRegistering,
				fieldOffsetScaleDeregistering,
				fieldOffsetScaleNotifiedBatches,
				fieldOffsetScaleNotifiedBatchesDateTime,
				fieldOffsetScaleNotifiedOnline,
				fieldOffsetScaleNotifiedOnlineDateTime,
				fieldOffsetScaleRetryCount,
				fieldOffsetScaleVersion,
				fieldOffsetScaleVersionReceived,
				fieldOffsetScaleVersionReceivedDateTime,
				fieldOffsetScaleLastCommSuccessful,
				fieldOffsetScaleLastSuccessfulCommDateTime,
				fieldOffsetScaleFirstFailedCommDateTime,
				fieldOffsetScaleTotalSuccessfulCommunications,
				fieldOffsetScaleTotalFailedCommunications,
				fieldOffsetScaleTotalBusyCommunications,

				endScaleFieldOffsets
			} ScaleFieldOffsets_t;


			/* --- Constructors and destructor --- */
			ScaleTags(FieldOffsetsList * pFieldList = 0);
			~ScaleTags();


			static const std::string _scaleNumber_;
			static const std::string _scaleRole_;
			static const std::string _scaleHostName_;
			static const std::string _scaleIPAddress_;
			static const std::string _scaleCreationDateTime_;
			static const std::string _scaleState_;
			static const std::string _scaleStateChangedDateTime_;
			static const std::string _scaleRegistering_;
			static const std::string _scaleDeregistering_;
			static const std::string _scaleNotifiedBatches_;
			static const std::string _scaleNotifiedBatchesDateTime_;
			static const std::string _scaleNotifiedOnline_;
			static const std::string _scaleNotifiedOnlineDateTime_;
			static const std::string _scaleRetryCount_;
			static const std::string _scaleVersion_;
			static const std::string _scaleVersionReceived_;
			static const std::string _scaleVersionReceivedDateTime_;
			static const std::string _scaleLastCommSuccessful_;
			static const std::string _scaleLastSuccessfulCommDateTime_;
			static const std::string _scaleFirstFailedCommDateTime_;
			static const std::string _scaleTotalSuccessfulCommunications_;
			static const std::string _scaleTotalFailedCommunications_;
			static const std::string _scaleTotalBusyCommunications_;


		protected:
			const TagsList & getMyTagsList() const { return _scaleTags_; }
			TagsMap & getMyTagsMap() const { return _fieldMap_; }


		private:
			/* --- Instance class members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif

			static bool _initialized_;
			static TagsList _scaleTags_;
			static TagsMap _fieldMap_;	//example pair: p#,fieldOffsetProductNum
	};

}



#endif /* SCALETAGS_H */
