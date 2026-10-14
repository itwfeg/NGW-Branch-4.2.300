unit Unit1;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, SoftekATL_TLB;

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
  oBarcode: CBarcode;
  i: Integer;
  nDirection: Integer;
  nPage: Integer;
  topLeftX: Integer;
  topLeftY: Integer;
  bottomRightX: Integer;
  bottomRightY: Integer;
  openDialog : TOpenDialog;
 
implementation

{$R *.dfm}


procedure TForm1.FormCreate(Sender: TObject);
begin
  oBarcode := CoCBarcode.Create;
end;


procedure TForm1.Button1Click(Sender: TObject);

begin
  // Turn on the barcode types you want to read.
  // Turn off the barcode types you don't want to read (this will increase the speed of your application)
  oBarcode.ReadCode128 := 1;
  oBarcode.ReadCode39 := 1;
  oBarcode.ReadCode25 := 1;
  oBarcode.ReadEAN13 := 1;
  oBarcode.ReadEAN8 := 1;
  oBarcode.ReadUPCA := 1;
  oBarcode.ReadUPCE := 1;
  oBarcode.ReadCodabar := 0;
  oBarcode.ReadPDF417 := 0;
  oBarcode.ReadDataMatrix := 0;
  oBarcode.ReadDatabar := 0;
  oBarcode.ReadMicroPDF417 := 0;
  oBarcode.ReadQRCode := 1;

  // Databar Options is a mask that controls which type of databar barcodes will be read and whether or not
  // the software will look for a quiet zone around the barcode.
  // 1 = 2D-Linkage flag (handle micro-PDf417 barcodes as supplementary data - requires ReadMicroPDF417 to be true).
  // 2 = Read RSS14
  // 4 = Read RSS14 Stacked
  // 8 = Read Limited
  // 16 = Read Expanded
  // 32 = Read Expanded Stacked
  // 64 = Require quiet zone
  oBarcode.DatabarOptions := 255;

  // If you want to read more than one barcode then set Multiple Read to True
  // Setting MutlipleRead to False will make the recognition faster
  oBarcode.MultipleRead := 1;

  // Noise reduction takes longer but can make it possible to read some difficult barcodes
  // When using noise reduction a typical value is 10 - the smaller the value the more effect it has.
  // A zero value turns off noise reduction.
  // If you use NoiseReduction then the ScanDirection mask must be either only horizontal or only
  // vertical (i.e 1, 2, 4, 8, 5 or 10).
  // oBarcode.NoiseReduction := 0;

  // You may need to set a small quiet zone if your barcodes are close to text and pictures in the image.
  // A value of zero uses the default.
  oBarcode.QuietZoneSize := 0;

  // LineJump controls the frequency at which scan lines in an image are sampled.
  // The default is 1.
  oBarcode.LineJump := 1;

  // You can restrict your search to a particular area of the image if you wish.
  // retval = oBarcode.SetScanRect(TopLeftX, TopLeftY, BottomRightX, BottomRightY, 0)

  // Set the direction that the barcode reader should scan for barcodes
  // The value is a mask where 1 = Left to Right, 2 = Top to Bottom, 3 = Right To Left, 4 = Bottom to Top
  oBarcode.ScanDirection := 15;

  // Set the page number to read from in a multi-page TIF file. The default is 0, which will make the
  // toolkit check every page.
  // oBarcode.PageNo := 1;

  // SkewTolerance controls the angle of skew that the barcode toolkit will tolerate. By default
  // the toolkit checks for barcodes along horizontal and vertical lines in an image. This works
  // OK for most barcodes because even at an angle it is possible to pass a line through the entire
  // length. SkewTolerance can range from 0 to 5 and allows for barcodes skewed to an angle of 45
  // degrees.
  oBarcode.SkewTolerance := 0;

  // ColorProcessingLevel controls how much time the toolkit will searching a color image for a barcode.
  // The default value is 2 and the range of values is 0 to 5. If ColorThreshold is non-zero then
  // ColorProcessingLevel is effectively set to 0.
  oBarcode.ColorProcessingLevel := 2;

  // MaxLength and MinLength can be used to specify the number of characters you expect to
  // find in a barcode. This can be useful to increase accuracy or if you wish to ignore some
  // barcodes in an image.
  oBarcode.MinLength := 4;
  oBarcode.MaxLength := 999;

  // When the toolkit scans an image it records the number of hits it gets for each barcode that
  // MIGHT be in the image. If the hits recorded for any of the barcodes are >= PrefOccurrence
  // then only these barcodes are returned. Otherwise, any barcode whose hits are >= MinOccurrence
  // are reported. If you have a very poor quality image then try setting MinOccurrence to 1, but you
  // may find that some false positive results are returned.
  //oBarcode.MinOccurrence := 2;
  //oBarcode.PrefOccurrence := 4;

  // Read Code 39 barcodes in extended mode
  // oBarcode.ExtendedCode39 := 1;

  // Barcode string is numeric
  // oBarcode.ReadNumeric := 1;

  // Set a regular expression for the barcode
  // oBarcode.Pattern := '^[A-Z]{2}[0-9]{5}$';

  // If you are scanning at a high resolution and the spaces between bars are
  // larger than 1 pixel then set MinSpaceBarWidth to 2 and increase your read rate.
  // oBarcode.MinSpaceBarWidth := 2;

  // MedianFilter is a useful way to clean up higher resolution images where the black bars contain white dots
  // and the spaces contain black dots. It does not work if the space between bars is only 1 pixel wide.
  oBarcode.MedianFilter := 0;

  // Flags for handling PDF files
  // PdfImageOnly defaults to true and indicates that the PDF documents are simple images.
  oBarcode.PdfImageOnly := 1;

  // The PdfImageExtractOptions mask controls how images are removed from PDF documents (when PdfImageOnly is True)
  // 1 = Enable fast extraction
  // 2 = Auto-invert black and white images
  // 4 = Auto-merge strips
  // 8 = Auto-correct photometric values in black and white images
  oBarcode.PdfImageExtractOptions := 15;

  // The PdfImageRasterOptions mask controls how images are rasterized when PdfImageOnly is false or when image extraction fails
  // 1 = Use alternative pdf-to-tif conversion function
  // 2 = Always use pdf-to-tif conversion rather than loading the rasterized image directly into memory
  oBarcode.PdfImageRasterOptions := 0;

  // PdfDpi and PdfBpp control what sort of image the PDF document is rasterized into
  oBarcode.PdfDpi := 300;
  oBarcode.PdfBpp := 8;


  // Or you can load the settings from a xml format file
  // See the manual for more details.
  // oBarcode.LoadXMLSettings("settings.xml")
  // The result of any XML operation is stored in oBarcode.XMLRetval (1 = Success, 0 = Failure)

  oBarcode.ScanBarcode(imagePath);
  nBarCodes := oBarcode.BarCodeCount;

  Results.Text := '' ;

  if (nBarcodes > 0) then
  begin
    for i := 1 to nBarcodes do
    begin
      Results.Text := Results.Text + 'Barcode ' + IntToStr(i) + sLineBreak;
      Results.Text := Results.Text + 'Value: ' + oBarcode.BarString[i] + sLineBreak ;
      Results.Text := Results.Text + 'Type: ' + oBarcode.BarStringType[i] + sLineBreak ;

      nDirection := oBarcode.BarStringDirection[i];
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

      npage := oBarcode.BarStringPage[i];
      Results.Text := Results.Text + 'Page = ' + IntToStr(nPage) + sLineBreak;

      topLeftX := oBarcode.BarStringTopLeftX[i];
      topLeftY := oBarcode.BarStringTopLeftY[i];
      bottomRightX := oBarcode.BarStringBottomRightX[i];
      bottomRightY := oBarcode.BarStringBottomRightY[i];

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
