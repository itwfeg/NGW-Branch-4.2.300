Dim objIE 
Dim iHeight, iWidth 

Set objIE = CreateObject("InternetExplorer.Application") 

With Createobject("internetexplorer.application") 
   .navigate "about:blank" 
   With .document.parentWindow.screen 
     iHeight = 1050 
     iWidth = 1680
   End With 
End With 

objIE.FullScreen = True 
objIE.StatusBar = False 
objIE.Visible = True 
objIE.AddressBar = False 
objIE.MenuBar = False 
objIE.ToolBar = False
objIE.Resizable = False 
objIE.Top = 0 
objIE.Left = 0 
objIE.Width = 1680 
objIE.Height = 1300
objIE.Navigate ("http://127.0.0.1/home.asp?consoletype=4") 