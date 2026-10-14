#ifndef PROPERTIESTREE_H
#define PROPERTIESTREE_H



#include <string>
#include <map>


#include "properties.h"



#ifndef NO_NAMESPACES
namespace util_env
{
#endif

	// The purpose of this class is help navigate the properties defined in a
	// properties file in a hierarchical fashion.
	// By calling 'getBeginIterator' passing the name of a parent property, you
	// obtain an iterator you can use to navigate all its child properties. For
	// example, given the following properties file:
	//
	// parent = value
	// parent.child1 = value-c1
	// parent.child2 = value-c2
	// parent.child4 = value-c4
	// parent.child1.grandchild1 = value-c1-gc1
	// parent.child1.grandchild2 = value-c1-gc2
	// parent.child3.grandchild1 = value-c3-gc1
	// parent.child4.grandchild1 = value-c4-gc1
	// parent.child4.grandchild2 = value-c4-gc2
	//
	// when you call 'getBeginInterator("parent")' your iterator will navigate
	// through the following children: 'parent.child1', 'parent.child2',
	// 'parent.child3', and 'parent.child4'.
	// Notice that 'parent.child3' is a child of 'parent', but if you try to
	// obtain the value associated with this property you'll get 0 (null) since
	// there is none. 
	// You may want to navigate the properties in a depth-first fashion: have a
	// method that first processes any value associated with the current property,
	// if any, and then, for each of the children call the same method.

	class PropertiesTree : public Properties
	{
		public:
			// --- Types and subtypes ---
			friend class PropertyIterator;	/* (SJJC port 6/19/06) */
			class PropertyIterator
			{
				public:
					PropertyIterator(const std::string& parent, PropertiesTree& props);
					PropertyIterator(const PropertyIterator& rhs)
							: parent_(rhs.parent_), child_(), props_(rhs.props_), iter_(rhs.iter_) {}
					PropertyIterator(PropertiesTree& props)
							: parent_(), child_(), props_(props), iter_(props.properties_.end()) {}
					virtual ~PropertyIterator() {}

					PropertyIterator& operator=(const PropertyIterator& rhs);
					const std::string& operator*() const { return child_; }
					const std::string* operator->() const { return &child_; }
					PropertyIterator& operator++();
//					const PropertyIterator& operator++() const;
					PropertyIterator operator++(int);
					const PropertyIterator operator++(int) const;
					bool operator==(const PropertyIterator& rhs) const;
					bool operator!=(const PropertyIterator& rhs) const { return !(*this == rhs); }

				private:
					std::string parent_;
					std::string child_;
					PropertiesTree& props_;
					properties_type::iterator iter_;

                #if defined(LINUX_FIX)
					void verifyFirstChild();
					void verifyNextChild();
                #else
					void PropertyIterator::verifyFirstChild();
					void PropertyIterator::verifyNextChild();
                #endif
			};
			typedef PropertyIterator iterator;
			typedef const PropertyIterator const_iterator;


			// --- Constructors and destructor ---
			PropertiesTree(const std::string& overrides, const std::string& defaults = "",
					bool alwaysAppend = false, bool autosave = false) 
					: Properties(overrides, defaults, alwaysAppend, autosave) {}
			virtual ~PropertiesTree() {}


			// --- Accessor methods ---
//			void setProperty(const std::string& key, const std::string& value);
			iterator getBeginIterator(const std::string& parent);
			iterator getEndIterator();
			const_iterator getEndIterator() const;


			// --- Processing methods ---


//		private:

	};

#ifndef NO_NAMESPACES
} // namespace util_env
#endif



#endif // PROPERTIESTREE_H
