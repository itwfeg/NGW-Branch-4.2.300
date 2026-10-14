Important information for Delphi 2009 or later....


How to set the License Key:

var sNum: string;
begin
 
sNum :='MY LICENSE KEY';
stSetLicenseKey(pChar(UTF8String(sNum)));


How to call the stScanBarCode function:

NumCodes:=stScanBarCode(pChar(UTF8String(OpenDialog1.FileName)));


How to get a barcode value:

pAnsiChar(stGetBarString(i)



Important information for previous versions of Delphi....


How to set the License Key:

var sNum: string;
begin
 
sNum :='MY LICENSE KEY';
stSetLicenseKey(pChar(sNum));


How to call the stScanBarCode function:

NumCodes:=stScanBarCode(pChar(OpenDialog1.FileName));

How to get a barcode value:

stGetBarString(i)



