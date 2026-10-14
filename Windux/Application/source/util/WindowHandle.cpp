#include <string>


#include "WindowHandle.h"


#include "Debug.h"
#include "util/e_string/utilstring.h"



namespace Util
{
	HWND findWindowHandle(ValidateWindowParameters* parameters)
	{
		if ( parameters->windowHandle != NULL && parameters->validateFunction(parameters->windowHandle, parameters) )
			return parameters->windowHandle;

		parameters->windowHandle = NULL;
		if ( !EnumWindows(&WindowEnumFindVirtualKeyboard, (LPARAM)parameters) )
		{
			DWORD error = GetLastError();
			if ( error != ERROR_SUCCESS )
				Debug::trace("Session.virtualKeyboard", "EnumWindows failed (possible error = " +
						util_string::uinttostring(GetLastError()) + ")");
		}

		return parameters->windowHandle;
	}


	BOOL CALLBACK WindowEnumFindVirtualKeyboard(HWND hwnd, LPARAM lParam)
	{
		ValidateWindowParameters* parameters = (ValidateWindowParameters*)lParam;

		if ( parameters->validateFunction(hwnd, parameters) )
		{
			parameters->windowHandle = hwnd;

			SetLastError(ERROR_SUCCESS);

			return FALSE;
		}

		return TRUE;
	}

	HWND findScaleWindowHandle(ValidateWindowParameters* parameters)
	{
		if ( parameters->windowHandle != NULL && parameters->validateFunction(parameters->windowHandle, parameters) )
			return parameters->windowHandle;

		parameters->windowHandle = NULL;

		if ( !EnumWindows(&WindowEnumFindScaleTitle, (LPARAM)parameters) )
		{
			DWORD error = GetLastError();
			if ( error != ERROR_SUCCESS )
				Debug::trace("Session.virtualKeyboard", "EnumWindows Scale failed (possible error = " +
						util_string::uinttostring(GetLastError()) + ")");
		}
		return parameters->windowHandle;
	}

	HWND findBrowserWindowHandle(ValidateWindowParameters* parameters)
	{		
		if ( parameters->windowHandle != NULL && parameters->validateFunction(parameters->windowHandle, parameters) )
			return parameters->windowHandle;

		parameters->windowHandle = NULL;

		if ( !EnumWindows(&WindowEnumFindBrowserTitle, (LPARAM)parameters) )
		{
			DWORD error = GetLastError();
			if ( error != ERROR_SUCCESS )
				Debug::trace("Session.virtualKeyboard", "EnumWindows Browser failed (possible error = " +
						util_string::uinttostring(GetLastError()) + ")");
		}
		return parameters->windowHandle;
	}

	BOOL CALLBACK WindowEnumFindScaleTitle(HWND hwnd, LPARAM lParam)
	{
		ValidateWindowParameters* parameters = (ValidateWindowParameters*)lParam;

		if ( parameters->validateFunction(hwnd, parameters) )
		{
			parameters->windowHandle = hwnd;

			SetLastError(ERROR_SUCCESS);

			return FALSE;
		}

		return TRUE;
	}

	BOOL CALLBACK WindowEnumFindBrowserTitle(HWND hwnd, LPARAM lParam)
	{
		ValidateWindowParameters* parameters = (ValidateWindowParameters*)lParam;

		if ( parameters->validateFunction(hwnd, parameters) )
		{
			parameters->windowHandle = hwnd;

			SetLastError(ERROR_SUCCESS);

			return FALSE;
		}

		return TRUE;
	}

	std::string WindowStyleText(DWORD style)
	{
		char buffer[2048];

		buffer[0] = '\0';

		if ( (style & WS_OVERLAPPEDWINDOW) == WS_OVERLAPPEDWINDOW )
			strcat(buffer, "OVERLAPPEDWINDOW");
		if ( (style & WS_POPUPWINDOW) == WS_POPUPWINDOW )
			strcat(buffer, "POPUPWINDOW");
		if ( (style & WS_CAPTION) == WS_CAPTION )
			strcat(buffer, "CAPTION | ");

		if ( style & WS_POPUP )
			strcat(buffer, "POPUP | ");
		if ( style & WS_CHILD )
			strcat(buffer, "CHILD | ");
		if ( style & WS_MINIMIZE )
			strcat(buffer, "MINIMIZE(ICONIC) | ");
		if ( style & WS_VISIBLE )
			strcat(buffer, "VISIBLE | ");

		if ( style & WS_DISABLED )
			strcat(buffer, "DISABLED | ");
		if ( style & WS_CLIPSIBLINGS )
			strcat(buffer, "CLIPSIBLINGS | ");
		if ( style & WS_CLIPCHILDREN )
			strcat(buffer, "CLIPCHILDREN | ");
		if ( style & WS_MAXIMIZE )
			strcat(buffer, "MAXIMIZE | ");

		if ( style & WS_BORDER )
			strcat(buffer, "BORDER | ");
		if ( style & WS_DLGFRAME )
			strcat(buffer, "DLGFRAME | ");
		if ( style & WS_VSCROLL )
			strcat(buffer, "VSCROLL | ");
		if ( style & WS_HSCROLL )
			strcat(buffer, "HSCROLL | ");

		if ( style & WS_SYSMENU )
			strcat(buffer, "SYSMENU | ");
		if ( style & WS_THICKFRAME )
			strcat(buffer, "THICKFRAME(SIZEBOX) | ");
		if ( style & WS_MINIMIZEBOX )
			strcat(buffer, "MINIMIZEBOX | ");
		if ( style & WS_MAXIMIZEBOX )
			strcat(buffer, "MAXIMIZEBOX | ");

		return buffer;
	}

	std::string WindowsExStyleText(DWORD style)
	{
		char buffer[2048];

		buffer[0] = '\0';

		if ( (style & WS_EX_PALETTEWINDOW) == WS_EX_PALETTEWINDOW )
			strcat(buffer, "PALETTEWINDOW");
		if ( (style & WS_EX_OVERLAPPEDWINDOW) == WS_EX_OVERLAPPEDWINDOW )
			strcat(buffer, "OVERLAPPEDWINDOW");

		if ( style & WS_EX_DLGMODALFRAME )
			strcat(buffer, "DLGMODALFRAME | ");
		if ( style & WS_EX_NOPARENTNOTIFY )
			strcat(buffer, "NOPARENTNOTIFY | ");
		if ( style & WS_EX_TOPMOST )
			strcat(buffer, "TOPMOST | ");

		if ( style & WS_EX_ACCEPTFILES )
			strcat(buffer, "ACCEPTFILES | ");
		if ( style & WS_EX_TRANSPARENT )
			strcat(buffer, "TRANSPARENT | ");
		if ( style & WS_EX_MDICHILD )
			strcat(buffer, "MDICHILD | ");
		if ( style & WS_EX_TOOLWINDOW )
			strcat(buffer, "TOOLWINDOW | ");

		if ( style & WS_EX_WINDOWEDGE )
			strcat(buffer, "WINDOWEDGE | ");
		if ( style & WS_EX_CLIENTEDGE )
			strcat(buffer, "CLIENTEDGE | ");
		if ( style & WS_EX_CONTEXTHELP )
			strcat(buffer, "CONTEXTHELP | ");

		if ( style & WS_EX_RIGHT )
			strcat(buffer, "RIGHT | ");
		if ( style & WS_EX_RTLREADING )
			strcat(buffer, "RTLREADING | ");
		if ( style & WS_EX_LEFTSCROLLBAR )
			strcat(buffer, "LEFTSCROLLBAR | ");

		if ( style & WS_EX_CONTROLPARENT )
			strcat(buffer, "CONTROLPARENT | ");
		if ( style & WS_EX_STATICEDGE )
			strcat(buffer, "STATICEDGE | ");
		if ( style & WS_EX_APPWINDOW )
			strcat(buffer, "APPWINDOW | ");
		if ( style & WS_EX_LAYERED )
			strcat(buffer, "LAYERED | ");

		if ( style & WS_EX_NOINHERITLAYOUT )
			strcat(buffer, "NOINHERITLAYOUT | ");
		if ( style & WS_EX_LAYOUTRTL )
			strcat(buffer, "LAYOUTRTL | ");

		if ( style & WS_EX_COMPOSITED )
			strcat(buffer, "COMPOSITED | ");
		if ( style & WS_EX_NOACTIVATE )
			strcat(buffer, "NOACTIVATE | ");

		return buffer;
	}


	std::string WindowShowingText(UINT show)
	{
		char buffer[2048];

		buffer[0] = '\0';

		if ( show == SW_SHOWNORMAL )
			strcat(buffer, "NORMAL");
		else if ( show == SW_SHOWMINIMIZED )
			strcat(buffer, "MINIMIZED");
		else if ( show == SW_SHOWMAXIMIZED )
			strcat(buffer, "MAXIMIZED");
		else
		{
			char number[34];

			strcat(buffer, "unknown (");
			ultoa(show, number, 10);
			strcat(buffer, number);
			strcat(buffer, ")");
		}

		return buffer;
	}


	void ShowWindowDebug(HWND hwnd)
	{
		char* text = 0, className[1024];
		WINDOWINFO windowInfo = { sizeof(WINDOWINFO) };
		WINDOWPLACEMENT windowPlacement = { sizeof(WINDOWPLACEMENT) };
		int textSize = GetWindowTextLength(hwnd);

		if ( GetClassNameA(hwnd, className, 1024) )
		{
			Debug::trace("VirtualKeyboard.validate", "     Window Class: " + std::string(className));
		}
		else
			Debug::trace("VirtualKeyboard.validate", "     Error (" + util_string::uinttostring(GetLastError()) +
					") calling GetClassName Win32 API function");

		if ( textSize )
		{
			text = new char[textSize + 1];
			GetWindowTextA(hwnd, text, textSize + 1);
			Debug::trace("VirtualKeyboard.validate", "     Window Text: " + std::string(text));
		}
		else
			Debug::trace("VirtualKeyboard.validate", "     Window Text: ** NO NAME **");

		if ( GetWindowInfo(hwnd, &windowInfo) )
		{
			Debug::trace("VirtualKeyboard.validate", "     Window Style: " + WindowStyleText(windowInfo.dwStyle));
			Debug::trace("VirtualKeyboard.validate", "     Window ExStyle: " + WindowsExStyleText(windowInfo.dwExStyle));
			Debug::trace("VirtualKeyboard.validate", "     Window Status: " + util_string::uinttostring(windowInfo.dwWindowStatus) +
					" (" + std::string(windowInfo.dwWindowStatus == WS_ACTIVECAPTION ? "active" : "inactive") + ")");
			Debug::trace("VirtualKeyboard.validate", "     Window Type: " + util_string::inttostring(windowInfo.atomWindowType));
		}
		else
			Debug::trace("VirtualKeyboard.validate", "     Error (" + util_string::uinttostring(GetLastError()) +
					") calling GetWindowInfo Win32 API function");

		if ( GetWindowPlacement(hwnd, &windowPlacement) )
		{
			Debug::trace("VirtualKeyboard.validate", "     Window Showing: " + WindowShowingText(windowPlacement.showCmd));
			Debug::trace("VirtualKeyboard.validate", "     Window Minimized Position: " + util_string::inttostring(windowPlacement.ptMinPosition.x) +
					", " + util_string::inttostring(windowPlacement.ptMinPosition.y));
			Debug::trace("VirtualKeyboard.validate", "     Window Maximized Position: " + util_string::inttostring(windowPlacement.ptMaxPosition.x) +
					", " + util_string::inttostring(windowPlacement.ptMaxPosition.y));
			Debug::trace("VirtualKeyboard.validate", "     Window Normal Position: " + util_string::inttostring(windowPlacement.rcNormalPosition.left) +
					", " + util_string::inttostring(windowPlacement.rcNormalPosition.top));
			Debug::trace("VirtualKeyboard.validate", "     Window Normal Size: " + util_string::inttostring(windowPlacement.rcNormalPosition.right -
					windowPlacement.rcNormalPosition.left) + ", " + util_string::inttostring(windowPlacement.rcNormalPosition.bottom -
					windowPlacement.rcNormalPosition.top));
		}
		else
			Debug::trace("VirtualKeyboard.validate", "     Error (" + util_string::uinttostring(GetLastError()) +
					") calling GetWindowPlacement Win32 API function");

		Debug::trace("VirtualKeyboard.validate", "     Window is " + std::string(IsWindow(hwnd) ? "Window" : "not Window"));
		Debug::trace("VirtualKeyboard.validate", "     Window is " + std::string(IsWindowVisible(hwnd) ? "Visible" : "not Visible") + " Window");
		Debug::trace("VirtualKeyboard.validate", "     Window is " + std::string(IsWindowUnicode(hwnd) ? "Unicode" : "not Unicode") + " Window");
		Debug::trace("VirtualKeyboard.validate", "     Window is " + std::string(IsIconic(hwnd) ? "Iconic" : "not Iconic"));
		Debug::trace("VirtualKeyboard.validate", "     Window is " + std::string(IsZoomed(hwnd) ? "Zoomed" : "not Zoomed"));


		if ( text )
			delete[] text;
	}


}
