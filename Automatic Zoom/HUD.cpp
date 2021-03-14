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
    L"Save Log",
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

//
// Toolbar Related
//
HIMAGELIST imagelist;
HBITMAP bitmap;
TBADDBITMAP addbitmap;
TBBUTTON Buttons[8];

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

    //
    // Create image list and load toolbar bitmap
    //
    imagelist = ImageList_Create(16,16,ILC_COLOR32 | ILC_MASK,3,0);
    bitmap = (HBITMAP)LoadImageA(hInst,
                                 MAKEINTRESOURCEA(ToolbarBitmap),
                                 IMAGE_BITMAP,
                                 0, /* Image Width  (0 means automatically detected) */
                                 0, /* Image Height (0 means automatically detected) */
                                 LR_CREATEDIBSECTION);
    

    //
    // Add loaded toolbar bitmap to image list (with color mask)
    // and set the toolbar image list
    //
    ImageList_AddMasked(imagelist, bitmap, RGB(192,192,192));
    SendMessageA(ToolbarWindow, TB_SETIMAGELIST, 0, (LPARAM)imagelist);
    

    memset(&Buttons, 0, sizeof(Buttons));

    //
    // ---------------------------------------------------------------- Buttons
    //

    // ----- Separator
    Buttons[0].fsState = TBSTATE_ENABLED;
    Buttons[0].fsStyle = BTNS_SEP;

    // ----- Open File
    Buttons[1].iBitmap = MAKELONG(0, 0);
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

    // ----- Save File
    Buttons[2].iBitmap = MAKELONG(1, 0);
    Buttons[2].fsState = TBSTATE_ENABLED;
    Buttons[2].fsStyle = TBSTYLE_BUTTON;
    Buttons[2].idCommand = InDev;
    Buttons[2].iString = (INT_PTR)TooltipsTxt[0];

    // // ---- Separator
    Buttons[3].fsState = TBSTATE_ENABLED;
    Buttons[3].fsStyle = BTNS_SEP;

    // ---- Copy Log
    Buttons[4].iBitmap = MAKELONG(2, 0);
    Buttons[4].fsState = TBSTATE_ENABLED;
    Buttons[4].fsStyle = TBSTYLE_BUTTON;
    Buttons[4].idCommand = Copy;
    Buttons[4].iString = (INT_PTR)TooltipsTxt[2];

    // ----- Save Log
    Buttons[5].iBitmap = MAKELONG(1, 0);
    Buttons[5].fsState = TBSTATE_ENABLED;
    Buttons[5].fsStyle = TBSTYLE_BUTTON;
    Buttons[5].idCommand = SaveLogToolbar;
    Buttons[5].iString = (INT_PTR)TooltipsTxt[3];

    // ----- Separator
    Buttons[6].fsState = TBSTATE_ENABLED;
    Buttons[6].fsStyle = BTNS_SEP;

    // ---- Help
    Buttons[7].iBitmap = MAKELONG(3, 0);
    Buttons[7].fsState = TBSTATE_ENABLED;
    Buttons[7].fsStyle = TBSTYLE_BUTTON;
    Buttons[7].idCommand = AboutToolbar;
    Buttons[7].iString = (INT_PTR)TooltipsTxt[4];
    
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