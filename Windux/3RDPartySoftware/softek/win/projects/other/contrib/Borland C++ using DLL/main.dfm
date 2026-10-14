object Form1: TForm1
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Barcode scanner'
  ClientHeight = 417
  ClientWidth = 568
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Image1: TImage
    Left = 240
    Top = 24
    Width = 305
    Height = 369
    Stretch = True
  end
  object Button_scan: TButton
    Left = 32
    Top = 72
    Width = 97
    Height = 25
    Caption = 'Scan image'
    TabOrder = 0
    OnClick = Button_scanClick
  end
  object Button_open: TButton
    Left = 32
    Top = 24
    Width = 97
    Height = 25
    Caption = 'Open image'
    TabOrder = 1
    OnClick = Button_openClick
  end
  object Memo1: TMemo
    Left = 32
    Top = 184
    Width = 169
    Height = 209
    ReadOnly = True
    TabOrder = 2
  end
  object OpenDialog: TOpenDialog
    DefaultExt = 'jpg'
    Filter = 'Bitmap [.bmp]|*.bmp'
    Left = 176
    Top = 24
  end
end
