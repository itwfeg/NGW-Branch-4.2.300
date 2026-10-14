#ifndef LISTCAMERASYSTEMPREDICTIONSMESSAGE_H
#define LISTCAMERASYSTEMPREDICTIONSMESSAGE_H

#include "HfcObject.h"

#include "HobList.h"
#include "HobString.h"

namespace hfc
{
	class ListCameraSystemPredictionsMessage : public HfcObject
	{
		public:
			enum FieldOffsets
			{
				fieldOffsetResult = 1,
				fieldOffsetImage,
				fieldOffsetProducts = 4,
				fieldOffsetConfidenceLevels = 6,

				endFieldOffsets
			};

			ListCameraSystemPredictionsMessage();
			ListCameraSystemPredictionsMessage(const char * data, int len, int formatType);

			const char *serialize(int format) const;

			void setResult(int value) {result = value;}
			int getResult() const {return result;}

			void setImage(const HobString& value) {image = value;}
			HobString getImage() const {return image;}

			void setProducts(const HobList& value) {products = value;}
			HobList getProducts() const {return products;}

			void setConfidenceLevels(const HobList& value) {confidenceLevels = value;}
			HobList getConfidenceLevels() const {return confidenceLevels;}
	
		private:
			int result;
			HobString image;
			HobList products;
			HobList confidenceLevels;

			static void deserialize(const HobString& list, HobList& result);
	};
}

#endif /* LISTCAMERASYSTEMPREDICTIONSMESSAGE_H */
