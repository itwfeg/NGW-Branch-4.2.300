#ifndef WRAPBOX_H
#define WRAPBOX_H

#include "PAL_platformheader.h"
//#include "BaseNT.h"

#include <string>
#include <list>
#include <vector>
#include <functional>
#include <algorithm>


class WrapBox
{
	public:
		// --- Types and subtypes ---
		static const unsigned int _MaxNameLength_ = 10;
		static const unsigned int _MaxHeight_ = 60;
		static const unsigned int _HeightBoxSizeCan_ = 15;
		static const unsigned int _WrapBoxOffset_ = 28;
		static const unsigned int _WrapBoxSizeCan_ = 19;
		
		typedef enum
		{
			PERCENT_STRETCH_PARAMETER,		 // 0
			KNIFE_HEIGHT_PARAMETER,
			CLAMP_HEIGHT_PARAMETER,
			ELEVATOR_PLACEMENT_PARAMETER,
			ADDED_LENGTH_PARAMETER,
			GRIPPER_ADJUST_PARAMETER,		// 5
			FUF_ADJUST_PARAMETER,
			WRAP_SPEED_PARAMETER,
			DISCHARGE_SPEED_PARAMETER,
			SUF_ADJUST_PARAMETER,
			HEIGHT_RESTRICT_PARAMETER,		// 10
			SUF_RETURN_PARAMETER,			// 0 - no longer used
			GRIPPER_SHORT_PULL_PARAMETER,	// 0 - no longer used
			SIDE_CLAMP_GAP_PARAMETER,		// 0 - no longer used
			FILM_ROLL_USED_PARAMETER,
			RUF_TRIP_DELAY_PARAMETER,		// 15
			BRAKE_TIMING_PARAMETER,
			GRIPPER_RELEASE_PARAMETER,		// 0 - no longer used
			GRIPPER_RELAX_HOME_PARAMETER,	// 0 - no longer used
            // TODO VERTICAL OFFSETS - THIS NEEDS FIXED for 4.2.300	- THE TIME HAS ARRIVED - FIXED 01-10-2023		
			// FIX for: SOF-5475,5476 was to put the LAST_PARAMETER here - had to move it to the end for 2P and added 2 new params
			// LAST_PARAMETER

			PRIMARY_LABEL_VERT_OFFSET,
			SECONDARY_LABEL_VERT_OFFSET,	// 20
            
            LAST_PARAMETER
		} WrapParameter;

		// SOF-6077: The LAST_PARAMETER of this data structure is NOW 21.  WITH 1P it was 19
		// This is throwing off all of the for loops that depend on this LAST_PARAMETER as a loop to max.
		// We need to test for 1P or 2P, or maybe a boardtype, and then set the size 
		// We cant just add buffer += sizeOf(EvolWrHeightBox) because we have to consider 1P vs 2P
		// 
		static const unsigned int _Use1P_LAST_PARAM_ = 19;
		static const unsigned int _Use2P_LAST_PARAM_ = 21;

		class HeightBox
		{
			public:
				typedef std::vector<int> WrapParameterContainer_type;

				HeightBox() : height_(0), parameters_(LAST_PARAMETER) {}	//	To eliminate a compiler warning.
				HeightBox(unsigned int height) : height_(height), parameters_(LAST_PARAMETER) {}
				HeightBox(unsigned int height, const HeightBox& refBox)
						: height_(height), parameters_(refBox.parameters_) {}
				~HeightBox() {}

				unsigned int getHeight() const { return height_; }

				int getParameter(int parameter) const { return parameters_[parameter]; }
				void setParameter(int parameter, int value)
						{ if ( 0 <= parameter && parameter < LAST_PARAMETER ) parameters_[parameter] = value; }

				void show(const std::string& debugCategory = "") const;

			private:
				unsigned int height_;
				WrapParameterContainer_type parameters_;
		};

		typedef std::list<HeightBox> HeightBoxContainer_type;
		typedef HeightBoxContainer_type::iterator iterator;
		typedef HeightBoxContainer_type::const_iterator const_iterator;

		class isSameHeight : public std::binary_function<HeightBox, unsigned int, bool>
		{
			public:
				bool operator() (const HeightBox& heightBox, unsigned int height) const
						{ return heightBox.getHeight() == height; }
		};

		class containsHeight : public std::binary_function<HeightBox, unsigned int, bool>
		{
			public:
				bool operator() (const HeightBox& heightBox, unsigned int height) const
						{ return heightBox.getHeight() >= height; }
		};


		// --- Constructors and destructor ---
		WrapBox();
		WrapBox(const char* name, unsigned int lengthLow, unsigned int lengthHigh,
				unsigned int widthLow, unsigned int widthHigh, unsigned int lip);
		WrapBox(const char* name, unsigned int plu, unsigned int lengthLow, unsigned int lengthHigh,
				unsigned int widthLow, unsigned int widthHigh, unsigned int lip);
		~WrapBox() {}


		// --- Getters and setters ---
		const std::string& getName() const { return name_; }
		void setName(const std::string& name) { name_ = name; validateName(); }

		int getPLUException() const { return pluException_; }
		void setPLUException(int plu) { pluException_ = plu; }

		UINT16 getLengthLow() const { return lengthLow_; }
		void setLengthLow(UINT16 lengthLow) { lengthLow_ = lengthLow; }

		UINT16 getLengthHigh() const { return lengthHigh_; }
		void setLengthHigh(UINT16 lengthHigh) { lengthHigh_ = lengthHigh; }

		UINT16 getWidthLow() const { return widthLow_; }
		void setWidthLow(UINT16 widthLow) { widthLow_ = widthLow; }

		UINT16 getWidthHigh() const { return widthHigh_; }
		void setWidthHigh(UINT16 widthHigh) { widthHigh_ = widthHigh; }

		UINT16 getLip() const { return lip_; }
		void setLip(UINT16 lip) { lip_ = lip; }

		int getNumberHeightBoxes() const { return (int)heightBoxes_.size(); }

		HeightBox* getHeightBox(unsigned int height);
		const HeightBox* getHeightBox(unsigned int height) const;
		const HeightBox* getContainingHeightBox(unsigned int height) const;
		HeightBox* getHeightBoxNumber(int n);
		const HeightBox* getHeightBoxNumber(int n) const;
		int getHeightBoxHeight(int n);


		// --- Processing methods ---
		bool insertHeightBox(const HeightBox& newHeightBox);
		bool renameHeightBox(unsigned int oldHeight, unsigned int newHeight);
		bool removeHeightBox(unsigned int height);

		iterator begin() { return heightBoxes_.begin(); }
		const_iterator begin() const { return heightBoxes_.begin(); }

		iterator end() { return heightBoxes_.end(); }
		const_iterator end() const { return heightBoxes_.end(); }

		void initHeightBoxesDummy(unsigned int num = 1, int baseNumber = 0);
		void show(const std::string& debugCategory = "") const;


	private:
		std::string name_;
		UINT32 pluException_;
		UINT16 lengthLow_, lengthHigh_;
		UINT16 widthLow_, widthHigh_;
		UINT16 lip_;
		HeightBoxContainer_type heightBoxes_;
//		HeightBoxPointerContainer_type heightBoxPointers_;


		// --- Processing methods ---
		void validateName();
};



#endif /* WRAPBOX_H */
