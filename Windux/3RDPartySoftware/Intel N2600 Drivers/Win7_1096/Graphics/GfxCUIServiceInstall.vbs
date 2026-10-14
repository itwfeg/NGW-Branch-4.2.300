Dim cuiSh
Set cuiSh = WScript.CreateObject ("WScript.Shell")
cuiSh.run "cmd /c net start FontCache3.0.0.0",0
Set cuiSh = Nothing