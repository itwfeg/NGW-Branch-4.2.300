#ifndef RENDERERMANAGERUNITTESTER_H
#define RENDERERMANAGERUNITTESTER_H



class HobartLabel;
class ImageControl;



class RendererManagerUnitTester
{
public:
	// --- Types and subtypes ---

	// --- Constructors and destructor ---
	RendererManagerUnitTester();
	~RendererManagerUnitTester();


	// --- Processing function members ---
	bool runLineTests();
	bool runBarcodeTests();
	bool runGraphicTests();
	bool runFontTests();
	bool runTextTests();
	bool runLabelTests();


private:
	// --- Processing function members ---
	bool runTest1();
	bool runTest2();
	bool runTest11();
	bool runTest21();
	bool runTest31();
	bool runTest32();
	bool runTest41();
	bool runTest51();

	HobartLabel* createLabel1();
	HobartLabel* createLabel2();
	HobartLabel* createLabel11();
	HobartLabel* createLabel21();
	HobartLabel* createLabel29();
	HobartLabel* createLabel32();
	HobartLabel* createLabel51();

#ifdef LINUX_FIX
	ImageControl* renderLabel(HobartLabel* label, bool preImage = true);
#else
	ImageControl* RendererManagerUnitTester::renderLabel(HobartLabel* label, bool preImage = true);
#endif
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};



#endif //RENDERERMANAGERUNITTESTER_H
