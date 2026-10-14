#include "PAL_platformheader.h"

#include <iostream>



#include "propertiestree.h"



#ifndef NO_NAMESPACES
namespace util_env
{
#endif


using namespace std;



PropertiesTree::iterator PropertiesTree::getBeginIterator(const std::string& parent)
{
	string search(parent);

	if ( search.size() && (search.at(search.size() - 1) != '.') )
		search.append(1, '.');

	return PropertyIterator(search, *this);
}


PropertiesTree::iterator PropertiesTree::getEndIterator()
{
	return PropertyIterator(*this);
}



PropertiesTree::PropertyIterator::PropertyIterator(const std::string& parent, PropertiesTree& props)
		: parent_(parent), child_(), props_(props), iter_(props.properties_.upper_bound(parent))
{
	verifyFirstChild();
}


PropertiesTree::PropertyIterator& PropertiesTree::PropertyIterator::operator=(const PropertyIterator& rhs)
{
	parent_ = rhs.parent_;
	child_ = rhs.child_;
	props_ = rhs.props_;
	iter_ = rhs.iter_;

	return *this;
}


PropertiesTree::PropertyIterator& PropertiesTree::PropertyIterator::operator++()
{
	++iter_;
	verifyNextChild();

	return *this;
}


PropertiesTree::PropertyIterator PropertiesTree::PropertyIterator::operator++(int)
{
	PropertyIterator iter(*this);

	++iter.iter_;
	iter.verifyNextChild();

	return *this;
}


const PropertiesTree::PropertyIterator PropertiesTree::PropertyIterator::operator++(int) const
{
	PropertyIterator iter(*this);

	++iter.iter_;
	iter.verifyNextChild();

	return *this;
}


bool PropertiesTree::PropertyIterator::operator==(const PropertiesTree::PropertyIterator& rhs) const
{
	return &props_ == &rhs.props_ && iter_ == rhs.iter_;
}


void PropertiesTree::PropertyIterator::verifyFirstChild()
{
#ifdef VXWORKS
	if ( iter_ != props_.properties_.end() && !iter_->first.compare(parent_, 0, parent_.size()) )
#else
	if ( iter_ != props_.properties_.end() && !iter_->first.compare(0, parent_.size(), parent_) )
#endif
	{
		string::size_type index = iter_->first.find('.', parent_.size());
		if ( index == string::npos )
			child_ = iter_->first;
		else
			child_ = iter_->first.substr(0, index);
	}
	else
		iter_ = props_.properties_.end();
}


void PropertiesTree::PropertyIterator::verifyNextChild()
{
	bool isNextChild = false;

#ifdef VXWORKS
	while ( iter_ != props_.properties_.end() && !iter_->first.compare(parent_, 0, parent_.size()) )
#else
	while ( iter_ != props_.properties_.end() && !iter_->first.compare(0, parent_.size(), parent_) )
#endif
	{
		string::size_type index = iter_->first.find('.', parent_.size());
		if ( index == string::npos )
		{
			child_ = iter_->first;
			isNextChild = true;
			break;
		}
#ifdef VXWORKS
		else if ( iter_->first.compare(child_, 0, child_.size()) )
#else
		else if ( iter_->first.compare(0, child_.size(), child_) )
#endif
		{
			child_ = iter_->first.substr(0, index);
			isNextChild = true;
			break;
		}

		++ iter_;
	}

	if ( !isNextChild )
		iter_ = props_.properties_.end();
}



#ifndef NO_NAMESPACES
} // namespace util_env
#endif

