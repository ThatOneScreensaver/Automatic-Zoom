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

#pragma once

#include <windows.h>
#include "Resource.h"
#define PWM_ACTIVATE    WM_USER + 11

class COptions {
	public:
		bool m_Debugging;
		bool m_AlwaysOnTop;
		void SetDefaultOptions();
		bool Load();
		bool Save();
};

DWORD WINAPI TrayThreadMsgLoop(LPVOID);
void TrayNotify(HWND hwnd, WPARAM wParam, LPARAM lParam);

#define PWM_TRAYICON		(WM_USER + 1)
#define PM_NOTIFYWAITING    (WM_USER + 2)
#define PM_QUITTRAYTHREAD   (WM_USER + 3)
#define PM_INITIALIZE		(WM_USER + 4)

extern COptions g_Options;