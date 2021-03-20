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

/*++

--*/

//
// ------------------------------------------------------------------- Includes
//

// #include "stdafx.h"
#include "About.hpp"
#include "Automatic Zoom.hpp"
#include "HUD.hpp"
#include "Debug.hpp"
#include "FileInterface.hpp"
#include "Logger.hpp"
#include "ZoomMTG.hpp"

#include <assert.h>
#include <commctrl.h>
#include <process.h>
#include <ShellAPI.h>
#include <stdio.h>
#include <time.h>
#include <WinInet.h>

/* Link-Time Libraries */
#pragma comment(lib, "comctl32")
#pragma comment(lib, "WinInet")

/* Warnings Disabled */
#pragma warning(disable:4101)
#pragma warning(disable:4715)

//
// ---------------------------------------------------------------- Definitions
//

#define MAX_LOADSTRING 100

//
// -------------------------------------------------------------------- Globals
//

const char *AppVersion = "Automatic Zoom, version 1.2b";

BOOL UsingMTG_URL;

double duration;

int Resolve;

// 
// Output log Related
//

char *Inter; /* Intermediary Char Variable */
char ToOutputLog[1024]; /* Output Log */
DWORD Err;
int CxsWritten; /* Characters written to Buffer (return val from sprintf) */
SYSTEMTIME LocalTime; /* Local time stored here */


HINSTANCE hInst;								// current instance

HANDLE CountdownThread;
HWND Toolbar;
HWND StatusBar;
HWND DbgCopyResultsBtn;
HWND hDlg;
HWND hWnd;
HWND StrtTmrBtn;


int wait; /* Time in minutes, multiply by 60 to get minutes in seconds */


TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name


//
// ------------------------------------------------------------ Prototypes
//

INT_PTR
CALLBACK
MainWindow (
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	);


//
// -------------------------------------------------- Function Definitions
//

int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPTSTR    lpCmdLine,
                      int       nCmdShow)
{
	hInst = hInstance;

	InitCommonControls();
	Logger::Logger();
	Logger::LogToFile("Entry Point: WinMain()");
	#ifdef _DEBUG
			sprintf(ToOutputLog, "This is a debug version of %s, stability is not guaranteed.", AppVersion);
			MessageBoxA(hWnd, ToOutputLog, "Debug Version Warning", MB_ICONWARNING);
	#endif
	DialogBoxParamA(hInstance, MAKEINTRESOURCEA(MAIN), hWnd, MainWindow, 0);
}




INT_PTR CALLBACK MainWindow(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	tagRECT *lpRect;
	clock_t start, end;
	tagRECT Rect1;
	tagRECT Rect2;
	tagRECT Rect;
	hWnd = hDlg;
	lpRect = &Rect2;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG: // Dialog Initialization

		start = clock();

		Logger::LogToFile("Initializing App");

		//
		// Set window position to the center
		//
		
		hWnd = GetDesktopWindow();
		GetWindowRect(hWnd, lpRect);
		GetWindowRect(hDlg, &Rect1);
		SetWindowPos(hDlg,
					 NULL,
                 	 (Rect2.right + Rect2.left) / 2 - (Rect1.right - Rect1.left) / 2,
                 	 (Rect2.top + Rect2.bottom) / 2 - (Rect1.bottom - Rect1.top) / 2,
					 0,
					 0,
					 1);


		StrtTmrBtn = GetDlgItem(hDlg, StartTimer);

		Toolbar = HUD::CreateToolbar(hInst, hDlg);
		SendMessageA(Toolbar, TB_ENABLEBUTTON, EndTimerToolbar, 0);
		StatusBar = HUD::MakeStatusBar(hDlg);

		Logger::LogToBox(hDlg, AppVersion, 1);

		end = clock();
		duration = (double)(end - start) / CLOCKS_PER_SEC;
		sprintf(ToOutputLog, "App initialization phase took %2.3f seconds", duration);

		Logger::LogToFile(ToOutputLog);

		OutputDebugStringA(ToOutputLog);

		//
		// Start Parser Thread
		//
		start = clock();
		Logger::LogToFile("Calling ParseScheduleFile()");
		_beginthreadex(0,0,Parser::ParseScheduleFile,(void *) hDlg,0,0);
		end = clock();
		duration = (double)(end - start) / CLOCKS_PER_SEC;
		
		sprintf(ToOutputLog, "Took %2.3f seconds to parse schedule file", duration);
		Logger::LogToFile(ToOutputLog);
		OutputDebugStringA(ToOutputLog);

		return (INT_PTR)TRUE;

	case WM_COMMAND: // Command Handler Section

	#ifdef _DEBUG
		if (LOWORD(wParam) == OpenFileToolbar)
		{
			SendMessageA(Toolbar, TB_ENABLEBUTTON, OpenFileToolbar, 0);
			FileInterface::OpenFile(hInst, hDlg);
			SendMessageA(Toolbar, TB_ENABLEBUTTON, OpenFileToolbar, 1);
			return 1;
		}
	#endif
		if (LOWORD(wParam) == InDev)
		{
			SendMessageA(Toolbar, TB_ENABLEBUTTON, InDev, 0);
			Logger::LogToBox(hDlg, "This is currently In Dev", 3);
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == Copy)
		{
			Logger::CopyResults(hDlg);
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == SaveLogToolbar)
		{
			FileInterface::SaveLogFile(hInst, hDlg);
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == AboutToolbar)
		{
			SendMessageA(Toolbar, TB_ENABLEBUTTON, AboutToolbar, 0);
			DialogBoxParamA(hInst, MAKEINTRESOURCEA(AboutBox), hDlg, About::AboutWndProc, NULL);
			SendMessageA(Toolbar, TB_ENABLEBUTTON, AboutToolbar, 1);
			return 1;
		}

		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			if (MessageBoxA(hDlg, "Exit Automatic Zoom?", "Automatic Zoom", MB_YESNO | MB_ICONQUESTION) == 6)
			{
				Logger::LogToFile("Exiting...");
				EndDialog(hDlg, LOWORD(wParam));
			}
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == StartTimerToolbar)
		{
			Resolve = ZoomMTG::ZoomMTG_Resolve(hDlg);
				
			if (Resolve == 1)
			{
				UsingMTG_URL = TRUE;
			}

			else if (Resolve == 0)
			{
				UsingMTG_URL = FALSE;
			}

			else if (Resolve == -1)
			{
				return (INT_PTR)FALSE;
			}

			SendMessageA(ToolbarWindow, TB_ENABLEBUTTON, EndTimerToolbar, 1);
			wait = GetDlgItemInt(hDlg, WaitTime, NULL, FALSE);

			if (wait <= 0)
			{
				Logger::LogToBox(hDlg, "WARNING: No wait time specified", 1);
			}
			else
			{
				//
				// Begin the countdown on a seperate thread or else
				// the app will lock up
				//

				CountdownThread = (HANDLE)_beginthread(HUD::CountdownStatusBar, 0, (void *)wait);
			}

			sprintf(ToOutputLog, "Starting %d Minute Timer...", wait);
			Logger::LogToBox(hDlg, ToOutputLog, 2);

			SetTimer(hDlg, /* Window handle to store time under */
					 400, /* Timer ID */
					 wait * 60000, /* Take wait time, convert it into seconds */
					 NULL); /* Function to execute when timer expires, WM_TIMER by default if set to NULL */
			
			SendMessageA(StatusBar, SB_SETTEXTA, 0, (LPARAM)"Waiting for timer to trigger");
		}
			
		if (LOWORD(wParam) == EndTimerToolbar)
		{
			//
			// Kill the timer and status bar countdown thread
			//

			KillTimer(hDlg, 400);
			TerminateThread(CountdownThread, 0);
			SendMessageA(StatusBar, SB_SETTEXTA, 1, (LPARAM)"");

			//
			// Return everything to normal
			//

			SendMessageA(ToolbarWindow, TB_ENABLEBUTTON, StartTimerToolbar, 1);
			SendMessageA(ToolbarWindow, TB_ENABLEBUTTON, EndTimerToolbar, 0);
			SetDlgItemTextA(hDlg, OutputLog, "Ending Timer...");
			SendMessageA(StatusBar, SB_SETTEXTA, 0, (LPARAM)"Waiting for user input");

			//
			// Only erase the wait time
			//
			
			SetDlgItemTextA(hDlg, WaitTime, "");
		}

		break;
	
	case WM_TIMER: // Timer message, triggered by SetTimer()

		/* Show Progress */
		Logger::LogToBox(hDlg, "Timer Triggered", 1);
		SendMessageA(StatusBar, SB_SETTEXTA, 0, (LPARAM)"Waiting for user input");

		//
		// Kill timer
		//
		KillTimer(hDlg, 400);

		/* 
		 * Determine which one to 
		 * use from previous set value
		 */
		if (UsingMTG_URL == TRUE)
			ZoomMTG::ZoomMTG_Send(hDlg);
		else
			ZoomMTG::ZoomMTG_Web(hDlg);

		//
		// Return toolbar to normal state
		//
		SendMessageA(ToolbarWindow, TB_ENABLEBUTTON, StartTimerToolbar, 1);
		SendMessageA(ToolbarWindow, TB_ENABLEBUTTON, EndTimerToolbar, 0);

		/* 
		 * Not sure if this can get annoying over time
		 * But on a different perspective it could be useful
		 * because it could allow for a new URL and passcode
		 * to be placed without selecting the whole string,
		 * it just wipes out the leftover URL and passcode.
		 */	
		SetDlgItemTextA(hDlg, ZoomMTG_Input, "");
		SetDlgItemTextA(hDlg, MeetingPasscode, "");
		SetDlgItemTextA(hDlg, WaitTime, "");
		SetDlgItemTextA(hDlg, MeetingJoinName, "");
		
		return (INT_PTR)TRUE;

	}
	return (INT_PTR)FALSE;
}

//
// end of file
//
