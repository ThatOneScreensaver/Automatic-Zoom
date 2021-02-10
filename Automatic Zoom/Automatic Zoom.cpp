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

#include "stdafx.h"
#include "Automatic Zoom.h"
#include "Debug.h"
#include "FileParser.h"
#include "Logger.h"
#include "ZoomMTG.h"

#include <commctrl.h>
#include <process.h>
#include <ShellAPI.h>
#include <stdio.h>
#include <WinInet.h>

/* Link-Time Libraries */
#pragma comment(lib, "comctl32")
#pragma comment(lib, "WinInet")

/* Warnings Disabled */
#pragma warning(disable:4101)

//
// ---------------------------------------------------------------- Definitions
//

#define MAX_LOADSTRING 100

//
// -------------------------------------------------------------------- Globals
//

const char *AppVersion = "Automatic Zoom, version 1.1";

BOOL Enabled;
BOOL UsingMTG_URL;

DWORD Err;

int Resolve;


/* 
 * Output log Related
 */
char *Inter; /* Intermediary Char Variable */
char ToOutputLog[1024]; /* Output Log */
int CxsWritten; /* Characters written to Buffer (return val from sprintf) */
SYSTEMTIME LocalTime; /* Local time stored here */


HINSTANCE hInst;								// current instance


HWND StrtTmrBtn;
HWND DbgCopyResultsBtn;
HWND hDlg;
HWND hWnd;
HWND StatusBar;


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

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
	HANDLE Mutex = CreateMutexA(0,0,"AutomaticZoom_Mutex");
	if (Err = GetLastError(), Err == ERROR_ALREADY_EXISTS)
	{
		MessageBoxA(NULL, "Another instance of this program is running\r\nPlease exit it before starting a new instance", "Initialization Error", MB_ICONERROR);
		return 0;
	}

	Logger::Setup();
	Logger::LogToFile("Entry Point: WinMain()");
	DialogBoxParamA(hInst, MAKEINTRESOURCEA(MAIN), NULL, MainWindow, 0);
}




INT_PTR CALLBACK MainWindow(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	tagRECT clientRect;
	tagRECT *lpRect;
	tagRECT Rect1;
	tagRECT Rect2;
	tagRECT Rect;
	hWnd = hDlg;
	lpRect = &Rect2;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG: // Dialog Initialization

		Logger::LogToFile("Initializing App");
		
		//
		// Start Parser Thread
		//

		Logger::LogToFile("Calling ParseScheduleFile()");
		_beginthreadex(0,0,Parser::ParseScheduleFile,(void *) hDlg,0,0);

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
		
		Logger::LogToBox(hDlg, AppVersion, 1);

		return (INT_PTR)TRUE;

	case WM_COMMAND: // Command Handler Section

		if (LOWORD(wParam) == Copy)
		{
			Logger::CopyResults(hDlg);
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			Logger::LogToFile("Exiting...");
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == StartTimer)
		{
			if (Enabled == FALSE)
			{

				Enabled = TRUE;
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
					Enabled = FALSE;
					return (INT_PTR)FALSE;
				}


				wait = GetDlgItemInt(hDlg, WaitTime, NULL, FALSE);

				if (wait == 0)
				{
					Logger::LogToBox(hDlg, "WARNING: No wait time specified", 1);
				}				

				CxsWritten = sprintf(ToOutputLog, "Starting %d Minute Timer...", wait);
				
				Logger::LogToBox(hDlg, ToOutputLog, 2);

				SetTimer(hDlg, /* Window handle to store time under */
						 400, /* Timer ID */
						 wait * 60000, /* Take wait time, convert it into seconds */
						 NULL); /* Function to execute when timer expires, WM_TIMER by default if set to NULL */
			}
			
			else
			{

				Enabled = FALSE;

				KillTimer(hDlg, 400);

				SetDlgItemTextA(hDlg, StartTimer, "Start Timer");
				SetDlgItemTextA(hDlg, OutputLog, "Ending Timer...");

				//
				// Erase all text in edit boxes
				//

				SetDlgItemTextA(hDlg, ZoomMTG_Input, "");
				SetDlgItemTextA(hDlg, MeetingPasscode, "");
				SetDlgItemTextA(hDlg, WaitTime, "");
				SetDlgItemTextA(hDlg, MeetingJoinName, "");

			}
			
		}
		break;
	
	case WM_TIMER: // Timer message, triggered by SetTimer()

		/* Show Progress */
		Logger::LogToBox(hDlg, "Timer Triggered", 1);

		/* Kill it from the get go */
		KillTimer(hDlg, 400);

		/* 
		 * Determine which one to 
		 * use from previous set value
		 */
		if (UsingMTG_URL == TRUE)
			ZoomMTG::ZoomMTG_Send(hDlg);
		else if (UsingMTG_URL == FALSE)
			ZoomMTG::ZoomMTG_Web(hDlg);
		
		/* 
		 * Just reset the button text
		 */
		SetDlgItemTextA(hDlg, StartTimer, "Start Timer");


		/* 
		 * Reset this so it doesn't break
		 */
		Enabled = FALSE;

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



