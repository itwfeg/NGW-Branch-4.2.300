<html>
ASP Interface for the Softek Barcode Toolkit<br>
<br>
<%
Set barcode = Server.CreateObject("SoftekBarcodeCOM.Barcode")
' Set the license key here
barcode.LicenseKey = "MY LICENSE KEY"
barcode.ScanDirection = 15
' Select the barcode type to search for
barcode.ReadCode128 = True
barcode.ReadCode25 = True
barcode.ReadEAN13 = True
barcode.ReadEAN8 = True
barcode.ReadUPCA = True
barcode.ReadUPCE = True
barcode.ReadCode39 = True
' Only functions in the 2-D version of the toolkit
barcode.ReadPDF417 = True
' Scan every line of the image
barcode.LineJump = 1
FILE_NAME = "C:\Program Files\Softek Software\Softek Barcode Toolkit 30 Day Evaluation\Images\pen.jpg"
response.write "Scanning: " & FILE_NAME & "<BR>"
barcode.ScanBarCode(FILE_NAME)
nBarCode = barcode.BarCodeCount
response.write "Return value from ScanBarCode was " & nBarCode & "<br>"
If (nBarCode <= -6) Then
	Response.write "Evaluation expired or is not valid<br>"
ElseIf (nBarCode < 0) Then
	Response.write "Error reading barcode<br>"
ElseIf (nBarCode = 0) Then
	Response.write "No barcode found on this image<br>"
Else
	strBarcode = barcode.barstring(1)
	Response.write "Barcode found on this image has value " & strBarcode & "<br>"
End If
%>
</html>
