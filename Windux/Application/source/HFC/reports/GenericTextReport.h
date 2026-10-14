#ifndef GENERICTEXTREPORT_H
#define GENERICTEXTREPORT_H

#include "AbstractReport.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Generic text report.
class GenericTextReport : public AbstractReport
{
	typedef GenericTextReport _Myt;
	typedef AbstractReport _Mybase;

public:

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] reportManager  manager
	 * @param[in] type  printer type
	 */
	GenericTextReport( ReportManager *reportManager, PrinterType type = FirstPrinter );

	/// Constructor.
	/**
	 * @param[in] name  report name
	 * @param[in] content  report content
	 * @param[in] reportManager  manager
	 * @param[in] type  printer type
	 */
	GenericTextReport( const std::string &name, const std::string &content, ReportManager *reportManager, PrinterType type = FirstPrinter );

	/// Destructor.
	~GenericTextReport();

	// ========================================================================
	// Properties
	// ========================================================================

	/// Retrieve report name.
	/**
	 * @return  name
	 */
	const std::string &getName() const {return name_;}

	/// Set report name.
	/**
	 * @param[in] value  name
	 */
	void setName( const std::string &value ) {name_ = value;}

	/// Retrieve content font family.
	/**
	 * @return  font family
	 */
	const std::string &getContentFontFamily() const {return _contentFontFamily_;}

	/// Retrieve content font size.
	/**
	 * @return  font size
	 */
	int getContentFontSize() const {return _contentFontSize_;}

protected:

	// ========================================================================
	// Methods
	// ========================================================================

	/// Initialize.
	void doInitialize();

private:

	// --- Object Data Members ---
	std::string name_;


	// --- Class Data Members ---
	static const std::string _contentFontFamily_;
	static const int _contentFontSize_;


	#ifdef USE_ACCOUNTANT
	static int dataType;
	static std::string dataTypeName;
#endif

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // GENERICTEXTREPORT_H

