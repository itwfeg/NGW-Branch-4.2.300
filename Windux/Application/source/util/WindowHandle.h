#ifndef WINDOWHANDLE_H
#define WINDOWHANDLE_H


#include <string>
#include "PAL_platformheader.h"

namespace Util
{
    struct _ValidateWindowParameters;

    typedef bool (ValidateWindowHandle)(HWND hwnd, struct _ValidateWindowParameters* parameters);

    typedef struct _ValidateWindowParameters
    {
        ValidateWindowHandle* validateFunction;
        char myClassName[256];
        char windowClassName[256];
        wchar_t  myScaleTitle[256]; 
        wchar_t  myBrowserTitle[256];
#if !defined ( __linux__ )
        WINDOWINFO windowInfo;
#endif
        HWND windowHandle;
    } ValidateWindowParameters;


    HWND findWindowHandle(ValidateWindowParameters* parameters);
    BOOL CALLBACK WindowEnumFindVirtualKeyboard(HWND hwnd, LPARAM lParam);

    HWND findScaleWindowHandle(ValidateWindowParameters* parameters);
    BOOL CALLBACK WindowEnumFindScaleTitle(HWND hwnd, LPARAM lParam);

    HWND findBrowserWindowHandle(ValidateWindowParameters* parameters);
    BOOL CALLBACK WindowEnumFindBrowserTitle(HWND hwnd, LPARAM lParam);

    std::string WindowStyleText(DWORD style);
    std::string WindowsExStyleText(DWORD style);
    std::string WindowShowingText(UINT show);
    void ShowWindowDebug(HWND hwnd);
}


#endif /* WINDOWHANDLE_H */
