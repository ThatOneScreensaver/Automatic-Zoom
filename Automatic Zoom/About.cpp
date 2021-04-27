/*++
						The MIT License

Copyright (c) 2021 A Screensaver

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
--*/

#include "About.hpp"
#include "Logger.hpp"
#include "stdio.h"
#include "Resource.h"
#include <CommCtrl.h>

extern const char *AppVersion;

INT_PTR CALLBACK
About::AboutWndProc(HWND hDlg,
                    UINT msg,
                    WPARAM wParam,
                    LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    extern HWND Toolbar;
    int wmID = LOWORD(wParam);
    RECT Rect1, Rect2;
    HWND Wnd = GetDesktopWindow();
    
    // Switch for Window Messages
    switch(msg)
    {
        case WM_INITDIALOG:

            SendMessageA(Toolbar, TB_ENABLEBUTTON, AboutToolbar, 0);
            ShowWindow(hDlg, SW_SHOW);
            
            //
            // Center the about dialog box
            //

		    GetWindowRect(Wnd, &Rect2);
		    GetWindowRect(hDlg, &Rect1);
		    SetWindowPos(hDlg,
					     NULL,
                 	     (Rect2.right + Rect2.left) / 2 - (Rect1.right - Rect1.left) / 2,
                 	     (Rect2.top + Rect2.bottom) / 2 - (Rect1.bottom - Rect1.top) / 2,
					     0,
					     0,
					     1);

            SetDlgItemTextA(hDlg, AboutBoxText, AppVersion);
            sprintf(ToOutputLog, "Compiled on: %s at %s", __DATE__, __TIME__);
            SetDlgItemTextA(hDlg, AboutCompileDate, ToOutputLog);
            break;
        case WM_COMMAND:
            switch(wmID)
            {
                case IDOK:
                case IDCANCEL:
                    SendMessageA(Toolbar, TB_ENABLEBUTTON, AboutToolbar, 1);
                    EndDialog(hDlg, 0);
                    break;
            }
            break;
    }
    return (INT_PTR)FALSE;
}
