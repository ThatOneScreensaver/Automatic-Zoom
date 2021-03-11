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

#include "HUD.hpp"
#include "resource.h"
#include <CommCtrl.h>
#include <windows.h>
#include <stdio.h>

const wchar_t *TooltipsTxt[] = {
    L"In Dev Feature",
    L"Open Schedule File",
    L"Copy Log",
    L"About Automatic Zoom"
};

// 
// Output log Related
//

extern char *Inter; /* Intermediary Char Variable */
extern char ToOutputLog[1024]; /* Output Log */

extern DWORD Err;
extern int CxsWritten; /* Characters written to Buffer (return val from sprintf) */
extern SYSTEMTIME LocalTime; /* Local time stored here */
TBADDBITMAP addbitmap;
TBBUTTON Buttons[7];

HWND
HUD::CreateToolbar(HINSTANCE hInst, HWND hWnd)
{
    HWND ToolbarWindow = CreateWindowExA(0, TOOLBARCLASSNAMEA, "Automatic Zoom Toolbar", WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_LIST | TBSTYLE_FLAT, 0, 0, 0, 0, hWnd, (HMENU)MainToolbar, hInst, NULL);
    if (!ToolbarWindow)
    {
        Err = GetLastError();
        sprintf(ToOutputLog, "Failed to create toolbar window, error 0x%.8X ( %d )", Err, Err);
        OutputDebugStringA(ToOutputLog);
        return NULL;
    }

    SendMessageA(ToolbarWindow, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

    addbitmap.hInst = HINST_COMMCTRL;
    addbitmap.nID = IDB_STD_SMALL_COLOR;
    SendMessageA(ToolbarWindow, TB_ADDBITMAP, 0, (LPARAM)&addbitmap);


    memset(&Buttons, 0, sizeof(Buttons));

    //
    // ---------------------------------------------------------------- Buttons
    //

    // ----- Separator
    Buttons[0].fsState = TBSTATE_ENABLED;
    Buttons[0].fsStyle = BTNS_SEP;

    // ----- Open File
    Buttons[1].iBitmap = STD_FILEOPEN;
    #ifdef _DEBUG
    Buttons[1].fsState = TBSTATE_ENABLED;
    Buttons[1].idCommand = OpenFileToolbar;
    Buttons[1].iString = (INT_PTR)TooltipsTxt[1];
    #else
    Buttons[1].fsState = TBSTATE_INDETERMINATE;
    Buttons[1].idCommand = InDev;
    Buttons[1].iString = (INT_PTR)TooltipsTxt[0];
    #endif
    Buttons[1].fsStyle = TBSTYLE_BUTTON;

    // ----- Separator
    Buttons[2].fsState = TBSTATE_ENABLED;
    Buttons[2].fsStyle = BTNS_SEP;

    // ----- Save File
    Buttons[3].iBitmap = STD_FILESAVE;
    Buttons[3].fsState = TBSTATE_ENABLED;
    Buttons[3].fsStyle = TBSTYLE_BUTTON;
    Buttons[3].idCommand = InDev;
    Buttons[3].iString = (INT_PTR)TooltipsTxt[0];

    // ---- Separator
    Buttons[4].fsState = TBSTATE_ENABLED;
    Buttons[4].fsStyle = BTNS_SEP;

    // ---- Copy Log
    Buttons[5].iBitmap = STD_COPY;
    Buttons[5].fsState = TBSTATE_ENABLED;
    Buttons[5].fsStyle = TBSTYLE_BUTTON;
    Buttons[5].idCommand = Copy;
    Buttons[5].iString = (INT_PTR)TooltipsTxt[2];

    // ---- Help
    Buttons[6].iBitmap = STD_HELP;
    Buttons[6].fsState = TBSTATE_ENABLED;
    Buttons[6].fsStyle = TBSTYLE_BUTTON;
    Buttons[6].idCommand = AboutToolbar;
    Buttons[6].iString = (INT_PTR)TooltipsTxt[3];
    
    //
    // ---------------------------------------------------------------- The End
    //


    // Do this so text appears as a tooltip.
    SendMessageA(ToolbarWindow, TB_SETMAXTEXTROWS, 0, 0);

    // Window will autosize and Set Extended Style.
    SendMessageA(ToolbarWindow, TB_AUTOSIZE, 0, 0);
    SendMessageA(ToolbarWindow, TB_SETEXTENDEDSTYLE, 0, (LPARAM)TBSTYLE_EX_MIXEDBUTTONS);
    
    // We're finished here, send the buttons on their way.
    SendMessageA(ToolbarWindow, TB_ADDBUTTONS, sizeof(Buttons)/sizeof(TBBUTTON), (LPARAM)&Buttons);

    return ToolbarWindow;
}

HWND 
HUD::MakeStatusBar(HWND hWnd)
{
	return CreateStatusWindowA(WS_CHILD | WS_VISIBLE, "Ready", hWnd, StatusBarID);
}