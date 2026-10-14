object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'Form1'
  ClientHeight = 249
  ClientWidth = 554
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 16
    Height = 13
    Caption = 'File'
  end
  object Button1: TButton
    Left = 471
    Top = 216
    Width = 75
    Height = 25
    Caption = 'Read Barcode'
    TabOrder = 0
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 471
    Top = 8
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Browse'
    TabOrder = 1
    OnClick = Button2Click
  end
  object Results: TMemo
    Left = 8
    Top = 39
    Width = 538
    Height = 171
    Lines.Strings = (
      'Results')
    ReadOnly = True
    ScrollBars = ssBoth
    TabOrder = 2
  end
  object fileName: TEdit
    Left = 40
    Top = 8
    Width = 425
    Height = 21
    ReadOnly = True
    TabOrder = 3
    Text = 'Click on browse to select image file'
  end
  object Button3: TButton
    Left = 390
    Top = 216
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 4
    OnClick = Button3Click
  end
  object OpenDialog1: TOpenDialog
    Left = 296
    Top = 216
  end
end
