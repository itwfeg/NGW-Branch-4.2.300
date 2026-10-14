
THIS TEXT IS CONTAINED IN THE README.TXT FILE WHICH CAN BE FOUND AT THE TOP LEVEL OF THE INSTALLATION FOLDER.

The target folder includes the dll files for both the Softek Barcode Reader Toolkit for Windows and the PDF Extension to the Softek Barcode Reader Toolkit.

If you have purchased a license for the toolkit then you will have received a license key. This key should be applied at run-time in your code before you call the ScanBarCode command. For example:

barcode.LicenseKey = "MY LICENSE KEY"
barcode.ScanBarCode("input.tif")

There is no need to reinstall the toolkit after purchase.

Without a valid license key the toolkit will a display pop-up box when a barcode is read. An evaluation license key can requested from sales@bardecode.com.

No further steps are necessary unless you intend to use either the OCX or COM interfaces to the toolkit in which case please run either REGISTER.BAT or REGISTER64.BAT in the bin folder as the Administrator.



 