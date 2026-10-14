unit Unit1;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, SoftekBarcode;

type
  TForm1 = class(TForm)
    Button1: TButton;
    OpenDialog1: TOpenDialog;
    Button2: TButton;
    Results: TMemo;
    Label1: TLabel;
    fileName: TEdit;
    Button3: TButton;
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;
  imagePath: String;
  nBarCodes: Integer;
  i: Integer;
  nDirection: Integer;
  nPage: Integer;
  topLeftX: Long;
  topLeftY: Long;
  bottomRightX: Long;
  bottomRightY: Long;
  openDialog : TOpenDialog;
  libraryLoaded: boolean;
  hBarcode:Pointer;

implementation

{$R *.dfm}


procedure TForm1.FormCreate(Sender: TObject);
begin
  imagePath := '';
  libraryLoaded := InitSoftekLibrary('..\..\..\..\x86\softekbarcode.dll');
end;


procedure TForm1.Button1Click(Sender: TObject);

begin
  if (libraryLoaded = false) then
  begin
    Results.Text := 'Could not load SoftekBarcode.dll' ;
    exit;
  end;

  hBarcode:=mtCreateBarcodeInstance;

  // Turn on the barcode types you want to read.
  // Turn off the barcode types you don't want to read (this will increase the speed of your application)
  mtSetReadCode128(hBarcode, true);
  mtSetReadCode39(hBarcode, true);
  mtSetReadCode25(hBarcode, true);
  mtSetReadEAN13(hBarcode, true);
  mtSetReadEAN8(hBarcode, true);
  mtSetReadUPCA(hBarcode, true);
  mtSetReadUPCE(hBarcode, true);
  mtSetReadCodabar(hBarcode, false);
  mtSetReadPDF417(hBarcode, false);
  mtSetReadDataMatrix(hBarcode, false);
  mtSetReadDatabar(hBarcode, false);
  mtSetReadMicroPDF417(hBarcode, false);
  mtSetReadQRCode(hBarcode, true);

  // Databar Options is a mask that controls which type of databar barcodes will be read and whether or not
  // the software will look for a quiet zone around the barcode.
  // 1 = 2D-Linkage flag (handle micro-PDf417 barcodes as supplementary data - requires ReadMicroPDF417 to be true).
  // 2 = Read RSS14
  // 4 = Read RSS14 Stacked
  // 8 = Read Limited
  // 16 = Read Expanded
  // 32 = Read Expanded Stacked
  // 64 = Require quiet zone
  mtSetDatabarOptions(hBarcode, 255);

  // If you want to read more than one barcode then set Multiple Read to True
  // Setting MutlipleRead to False will make the recognition faster
  mtSetMultipleRead(hBarcode, true);

  // Noise reduction takes longer but can make it possible to read some difficult barcodes
  // When using noise reduction a typical value is 10 - the smaller the value the more effect it has.
  // A zero value turns off noise reduction.
  // If you use NoiseReduction then the ScanDirection mask must be either only horizontal or only
  // vertical (i.e 1, 2, 4, 8, 5 or 10).
  // mtSetNoiseReduction(hBarcode, 0);

  // You may need to set a small quiet zone if your barcodes are close to text and pictures in the image.
  // A value of zero uses the default.
  mtSetQuietZoneSize(hBarcode, 0);

  // LineJump controls the frequency at which scan lines in an image are sampled.
  // The default is 1.
  mtSetLineJump(hBarcode, 1);

  // You can restrict your search to a particular area of the image if you wish.
  // stSetScanRect(TopLeftX, TopLeftY, BottomRightX, BottomRightY, 0)

  // Set the direction that the barcode reader should scan for barcodes
  // The value is a mask where 1 = Left to Right, 2 = Top to Bottom, 3 = Right To Left, 4 = Bottom to Top
  mtSetScanDirection(hBarcode, 15);

  // Set the page number to read from in a multi-page TIF file. The default is 0, which will make the
  // toolkit check every page.
  // mtSetPageNo(hBarcode, 1);

  // SkewTolerance controls the angle of skew that the barcode toolkit will tolerate. By default
  // the toolkit checks for barcodes along horizontal and vertical lines in an image. This works
  // OK for most barcodes because even at an angle it is possible to pass a line through the entire
  // length. SkewTolerance can range from 0 to 5 and allows for barcodes skewed to an angle of 45
  // degrees.
  mtSetSkewTolerance(hBarcode, 0);

  // ColorProcessingLevel controls how much time the toolkit will searching a color image for a barcode.
  // The default value is 2 and the range of values is 0 to 5. If ColorThreshold is non-zero then
  // ColorProcessingLevel is effectively set to 0.
  mtSetColorProcessingLevel(hBarcode, 2);

  // MaxLength and MinLength can be used to specify the number of characters you expect to
  // find in a barcode. This can be useful to increase accuracy or if you wish to ignore some
  // barcodes in an image.
  mtSetMinLength(hBarcode, 4);
  mtSetMaxLength(hBarcode, 999);

  // When the toolkit scans an image it records the number of hits it gets for each barcode that
  // MIGHT be in the image. If the hits recorded for any of the barcodes are >= PrefOccurrence
  // then only these barcodes are returned. Otherwise, any barcode whose hits are >= MinOccurrence
  // are reported. If you have a very poor quality image then try setting MinOccurrence to 1, but you
  // may find that some false positive results are returned.
  mtSetMinOccurrence(hBarcode, 2);
  mtSetPrefOccurrence(hBarcode, 4);

  // Read Code 39 barcodes in extended mode
  // mtSetExtendedCode39(hBarcode, true);

  // Barcode string is numeric
  // mtSetReadNumeric(hBarcode, true);

  // Set a regular expression for the barcode
  // mtSetPattern(hBarcode, pChar(UTF8String('^[A-Z]{2}[0-9]{5}$')));

  // If you are scanning at a high resolution and the spaces between bars are
  // larger than 1 pixel then set MinSpaceBarWidth to 2 and increase your read rate.
  // mtSetMinSpaceBarWidth(hBarcode, 2);

  // MedianFilter is a useful way to clean up higher resolution images where the black bars contain white dots
  // and the spaces contain black dots. It does not work if the space between bars is only 1 pixel wide.
  // mtSetMedianFilter(hBarcode, false);

  // Flags for handling PDF files
  // PdfImageOnly defaults to true and indicates that the PDF documents are simple images.
  mtSetPdfImageOnly(hBarcode, true);

  // The PdfImageExtractOptions mask controls how images are removed from PDF documents (when PdfImageOnly is True)
  // 1 = Enable fast extraction
  // 2 = Auto-invert black and white images
  // 4 = Auto-merge strips
  // 8 = Auto-correct photometric values in black and white images
  mtSetPdfImageExtractOptions(hBarcode, 15);

  // The PdfImageRasterOptions mask controls how images are rasterized when PdfImageOnly is false or when image extraction fails
  // 1 = Use alternative pdf-to-tif conversion function
  // 2 = Always use pdf-to-tif conversion rather than loading the rasterized image directly into memory
  mtSetPdfImageRasterOptions(hBarcode, 0);

  // PdfDpi and PdfBpp control what sort of image the PDF document is rasterized into
  mtSetPdfDpi(hBarcode, 300);
  mtSetPdfBpp(hBarcode, 8);


  // Or you can load the settings from a xml format file
  // See the manual for more details.
  // stLoadXMLSettings(pChar(UTF8String('settings.xml')))

  nBarCodes := mtScanBarcode(hBarcode, pAnsiChar(UTF8String(imagePath)));

  Results.Text := '' ;

  if (nBarcodes > 0) then
  begin
    for i := 1 to nBarcodes do
    begin
      Results.Text := Results.Text + 'Barcode ' + IntToStr(i) + sLineBreak;
      Results.Text := Results.Text + 'Value: ' + System.Utf8ToUnicodeString(mtGetBarString(hBarcode, i)) + sLineBreak ;
      Results.Text := Results.Text + 'Type: ' + mtGetBarStringType(hBarcode, i) + sLineBreak ;

      nDirection := mtGetBarStringDirection(hBarcode, i);
      if (nDirection = 1) Then
          Results.Text := Results.Text + 'Direction = Left to Right' + sLineBreak
      else
      begin
          if (nDirection = 2) Then
            Results.Text := Results.Text + 'Direction = Top to Bottom' + sLineBreak
          else

          begin
            if (nDirection = 4) then
              Results.Text := Results.Text + 'Direction = Right to Left' + sLineBreak
            else
              Results.Text := Results.Text + 'Direction = Bottom to Top' + sLineBreak
          end;
      end;

      nPage := mtGetBarStringPos(hBarcode, i, topLeftX, topLeftY, bottomRightX, bottomRightY);
      Results.Text := Results.Text + 'Page: ' + IntToStr(nPage) + sLineBreak;
      Results.Text := Results.Text + 'Top Left = (' + IntToStr(topLeftX) + ',' + IntToStr(topLeftY) + ')' + sLineBreak;
      Results.Text := Results.Text + 'BottomRight = (' + IntToStr(bottomRightX) + ',' + IntToStr(bottomRightY) + ')' + sLineBreak;

      Results.Text := Results.Text + sLineBreak;

    end;
  end
  else
   begin
    if (nBarcodes < 0) then
      Results.Text := 'Error opening image file'
    else
      Results.Text := 'Sorry - no barcode found';
   end;



   mtDestroyBarcodeInstance(hBarcode);
end;

procedure TForm1.Button2Click(Sender: TObject);
begin
    openDialog := TOpenDialog.Create(self);

    openDialog.Filter := 'Image Files and PDF Documents|*.tif;*.tiff;*.jpg;*.jpeg;*.png;*.bmp;*.gif;*.pdf|';
    if openDialog.Execute
    then
    begin
          imagePath := openDialog.FileName;
          fileName.Text := imagePath;
    end;
    openDialog.Free;
end;



procedure TForm1.Button3Click(Sender: TObject);
begin
  close;
end;

end.
