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

#include "Common.hpp"

NOTIFYICONDATAA stNotifyIconData{};

/*++ TrayThreadMsgLoop (Worker Thread)

Routine Description:

    Processes and dispatches creation/deletion messages to 
    the tray.

--*/
DWORD WINAPI TrayThreadMsgLoop(LPVOID)
{
    //
    // Tray message loop
    //
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        switch(msg.message)
        {
            case PM_INITIALIZE:

                //
                // Specify the tray notification icon, owner window, and callback message
                //

                memset(&stNotifyIconData, 0, sizeof(stNotifyIconData));
                stNotifyIconData.cbSize =  sizeof(stNotifyIconData);
                stNotifyIconData.hWnd = g_hMainWnd;
                stNotifyIconData.uID = 0;
                stNotifyIconData.uCallbackMessage = PWM_TRAYICON;
                stNotifyIconData.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_AUTOMATICZOOM));
                stNotifyIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_STATE;

                //
                // Now set the tip text and display the tray icon
                //

                LoadStringA(g_hInst, IDS_APP_TITLE, stNotifyIconData.szTip, ARRAYSIZE(stNotifyIconData.szTip));
                Shell_NotifyIconA(NIM_ADD, &stNotifyIconData);
                break;

            case PM_QUITTRAYTHREAD:

                //
                // Delete icon from tray and exit thread
                //
                
                memset(&stNotifyIconData, 0, sizeof(stNotifyIconData));
                stNotifyIconData.cbSize = sizeof(stNotifyIconData);
                stNotifyIconData.hWnd = g_hMainWnd;
                stNotifyIconData.uID = 0;
                Shell_NotifyIconA(NIM_DELETE, &stNotifyIconData);
                PostQuitMessage(0);
                break;
            
            default:
                dprintf("Tray worker thread got unexpected message, msg = %u", msg.message);
                break;
        }
    }
    return 0;
}

/*++ TrayNotify

Routine Description:

    Handler for tray notifications.

--*/
void TrayNotify(HWND, WPARAM, LPARAM lParam)
{
    switch(lParam)
    {
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
            OpenIcon(g_hMainWnd);
            SetForegroundWindow(g_hMainWnd);
            SetWindowPos(g_hMainWnd, g_Options.m_AlwaysOnTop ? HWND_TOPMOST : HWND_TOP,
                         0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
            break;
    }
}