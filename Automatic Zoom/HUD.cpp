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
#include "Logger.hpp"
#include "Resource.h"
#include <CommCtrl.h>
#include <windows.h>
#include <stdio.h>

HUD *g_HUD;

//
// Toolbar Related
//
HWND ToolbarWindow;
HIMAGELIST imagelist;
HBITMAP bitmap;
TBADDBITMAP addbitmap;

HUD::HUD()
{
    if (IsDebuggerPresent())
    {
        sprintf(ToOutputLog, "Successfully created HUD object! this -> %p\n", this);
        OutputDebugStringA(ToOutputLog);
    }
}


HWND
HUD::CreateToolbar(HINSTANCE g_hInst, HWND hWnd)

/*++

Routine Description:

    Create the toolbar window, add buttons
    and return handle to toolbar once finished.

Arguments:

    g_hInst - Supplies the running instance, used for imagelist
              creation and toolbar button bitmap loading.

    hWnd - Supplies handle to the current dialog, the owner-window
           of the toolbar.

Return Value:

    Returns a valid handle to the created toolbar window

--*/

{
    //
    // Create the toolbar window
    //
    ToolbarWindow = CreateWindowExA(0, TOOLBARCLASSNAMEA, "Automatic Zoom Toolbar", WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_LIST | TBSTYLE_FLAT, 0, 0, 0, 0, hWnd, (HMENU)MainToolbar, g_hInst, NULL);
    if (!ToolbarWindow)
    {
        sprintf(ToOutputLog, "Failed to create toolbar window, error %lu", GetLastError());
        OutputDebugStringA(ToOutputLog);
        exit(0);
    }

    SendMessageA(ToolbarWindow, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

    //
    // Create image list and load toolbar bitmap
    //
    imagelist = ImageList_Create(16,16,ILC_COLOR32 | ILC_MASK,7,0);
    bitmap = (HBITMAP)LoadImageA(g_hInst,
                                 MAKEINTRESOURCEA(ToolbarBitmap),
                                 IMAGE_BITMAP,
                                 0, /* Bitmap Image Width  (0 means automatically detected) */
                                 0, /* Bitmap Image Height (0 means automatically detected) */
                                 LR_CREATEDIBSECTION);
    

    //
    // Add loaded toolbar bitmap to image list (with color mask)
    // and set the toolbar image list
    //
    ImageList_AddMasked(imagelist, bitmap, RGB(192,192,192));
    SendMessageA(ToolbarWindow, TB_SETIMAGELIST, 0, (LPARAM)imagelist);

    //
    // ---------------------------------------------------------------- Buttons
    //

    TBBUTTON TBButtons[] = {
        
        // ----- Separator
        { NULL, NULL, TBSTATE_ENABLED, BTNS_SEP, {10}, 0, NULL },
        
        // ----- Start Timer
        { MAKELONG(4, 0), StartTimerToolbar, TBSTATE_ENABLED, BTNS_BUTTON, {10}, 0, (INT_PTR)L"Start Timer" },

        // ----- End Timer
        { MAKELONG(5, 0), EndTimerToolbar, TBSTATE_ENABLED, BTNS_BUTTON, {10}, 0, (INT_PTR)L"End Timer" },

        // ----- Separator
        { NULL, NULL, TBSTATE_ENABLED, BTNS_SEP, {10}, 0, NULL },

        // ----- Open File
#ifdef _DEBUG
        { MAKELONG(0, 0), OpenFileToolbar, TBSTATE_ENABLED, BTNS_BUTTON, {10}, 0, (INT_PTR)L"Open Schedule File" },
#else
        { MAKELONG(0, 0), InDev, TBSTATE_INDETERMINATE, BTNS_BUTTON, {10}, 0, (INT_PTR)L"In Dev Feature" },
#endif // _DEBUG

        // ----- Save File
#ifdef _DEBUG
        { MAKELONG(1, 0), SaveFileToolbar, TBSTATE_ENABLED, BTNS_BUTTON, {10}, 0, (INT_PTR)L"Save Schedule File" },
#else
        { MAKELONG(1, 0), InDev, TBSTATE_INDETERMINATE, BTNS_BUTTON, {10}, 0, (INT_PTR)L"In Dev Feature" },
#endif // _DEBUG

//         // ----- Separator
//         { NULL, NULL, TBSTATE_ENABLED, BTNS_SEP, {10}, 0, NULL },

//         // ---- Reload File
// #ifdef _DEBUG
//         { MAKELONG(6, 0), ReloadSchedToolbar, TBSTATE_ENABLED, BTNS_BUTTON, {10}, 0, (INT_PTR)L"Reload Schedule File" },
// #else
//         { MAKELONG(6, 0), InDev, TBSTATE_INDETERMINATE, BTNS_BUTTON, {10}, 0, (INT_PTR)L"In Dev Feature" },
// #endif
        
        // ----- Separator
        { NULL, NULL, TBSTATE_ENABLED, BTNS_SEP, {10}, 0, NULL },

        // ----- Copy Log
        { MAKELONG(2, 0), Copy, TBSTATE_ENABLED, BTNS_BUTTON, {10}, 0, (INT_PTR)L"Copy Log to Clipboard" },

        // ----- Save Log
        { MAKELONG(1, 0), SaveLogToolbar, TBSTATE_ENABLED, BTNS_BUTTON, {10}, 0, (INT_PTR)L"Save Log to File" },
    
        // ----- Separator
        { NULL, NULL, TBSTATE_ENABLED, BTNS_SEP, {10}, 0, NULL },
        
        // ----- Always On-Top
        { MAKELONG(0xFF, 0xFF), AlwaysOnTopToolbar, TBSTATE_ENABLED, BTNS_CHECK, {10}, 0, (INT_PTR)L"Always On-Top" },

        // ----- Separator
        { NULL, NULL, TBSTATE_ENABLED, BTNS_SEP, {10}, 0, NULL },

        // ----- About
        { MAKELONG(3, 0), AboutToolbar, TBSTATE_ENABLED, BTNS_BUTTON, {10}, 0, (INT_PTR)L"About Automatic Zoom" }
    };
    
    //
    // ---------------------------------------------------------------- The End
    //

    // Do this so text appears as a tooltip.
    SendMessageA(ToolbarWindow, TB_SETMAXTEXTROWS, 0, 0);

    // Window will autosize and Set Extended Style.
    SendMessageA(ToolbarWindow, TB_AUTOSIZE, 0, 0);
    SendMessageA(ToolbarWindow, TB_SETEXTENDEDSTYLE, 0, (LPARAM)TBSTYLE_EX_MIXEDBUTTONS);
    
    // We're finished here, send the buttons on their way.
    SendMessageA(ToolbarWindow, TB_ADDBUTTONS, ARRAYSIZE(TBButtons), (LPARAM)&TBButtons);

    // Grey out end timer button
    SendMessageA(ToolbarWindow, TB_ENABLEBUTTON, EndTimerToolbar, 0);

    // Return toolbar window handle
    return ToolbarWindow;
}

HWND 
HUD::MakeStatusBar(HWND hWnd)
{
    const int StatusBarParts[] = { 400, -1 };

    // Create the status bar window
	HWND StatusBarWindow = CreateStatusWindowA(WS_CHILD | WS_VISIBLE, "Ready", hWnd, StatusBarID);

    // Setup the status bar
    SendMessageA(StatusBarWindow, SB_SETPARTS, 2, (LPARAM)StatusBarParts); // Set the separate status bar parts
	SendMessageA(StatusBarWindow, SB_SETTEXTA, 0, (LPARAM)"Waiting for user input"); // Set the initial text
	SendMessageA(StatusBarWindow, SB_SETTEXTA, 1, (LPARAM)""); // Set the initial text

    //
    // Note: None of these actually work whenever we get
    // manifested to use version 6 of comctl32
    //
    SendMessageA(StatusBarWindow, SB_SETBKCOLOR, 0, RGB(105, 155, 247)); // Set background colour
    SendMessageA(StatusBarWindow, SB_SETBKCOLOR, 1, RGB(63, 135, 252)); // Set background colour for the right most part (zoom colour)

    // Return a handle to the status bar
    return StatusBarWindow;
}

void
HUD::Countdown(void * time)
{
    extern HWND StatusBar;
    
    // Convert the void variable into an unsigned integer
    UINT remaining = (UINT)time * 60;

    // While seconds remaining is greater than zero
    while(remaining > 0)
    {
        sprintf(ToOutputLog, "Time Remaining: %d:%.2d", remaining / 60, remaining % 60);
        remaining--;
        SendMessageA(StatusBar, SB_SETTEXTA, 1, (LPARAM)ToOutputLog);
        Sleep(1000);
    }
    SendMessageA(StatusBar, SB_SETTEXTA, 1, (LPARAM)"");
    return;
}