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

//
// -------------------------------------------------------------------- Defines
//

#define _CRTDBG_MAP_ALLOC
#define MAX_LOADSTRING 100

//
// -------------------------------------------------------------------- Globals
//

#ifdef _DEBUG
const char *AppVersion = "Automatic Zoom version 1.4b";
#else
const char *AppVersion = "Automatic Zoom version 1.3";
#endif

BOOL UsingMTG_URL;
BOOL g_AlwaysOnTop = FALSE;

double duration;

int Resolve;

HACCEL g_hAccelTable;
HANDLE CountdownThread;
HINSTANCE g_hInst;								// current instance
HWND g_hMainWnd;
HWND hWnd;
HWND StatusBar;
HWND Toolbar;


int wait; /* Time in minutes, multiply by 60 to get minutes in seconds */

TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name


//
// ----------------------------------------------------------------- Prototypes
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
// ------------------------------------------------------------------ Functions
//

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nShowCmd)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(nShowCmd);

	g_hInst = hInstance;

	if (!_stricmp(lpCmdLine, "about"))
	{
		DialogBoxA(g_hInst, MAKEINTRESOURCEA(AboutBox), hWnd, About::AboutWndProc);
		exit(0);
	}

	//
	// Locate any existing instances and show it
	//
	WCHAR szAppTitle[MAX_PATH];
	if (LoadStringW(g_hInst, IDS_APP_TITLE, szAppTitle, ARRAYSIZE(szAppTitle)))
	{
		HWND prevWnd = FindWindow(WC_DIALOG, szAppTitle);
		if (prevWnd)
		{
			DWORD dwPid{};
			GetWindowThreadProcessId(prevWnd, &dwPid);
			AllowSetForegroundWindow(dwPid);

			ULONG_PTR dwResult;
			if (SendMessageTimeout(prevWnd,
								   PWM_ACTIVATE,
								   0, 0,
								   SMTO_ABORTIFHUNG,
								   10000,
								   &dwResult))
			{
				if (dwResult == PWM_ACTIVATE)
					return 0;
			}
		}
	}
	
	//
	// Initialize the objects and catch exceptions (if any)
	//
	try
	{
		if (g_Logger = new Logger, g_Logger == nullptr)
			throw std::exception("Failed to create Logger object");
		if (g_ZoomMTG = new ZoomMTG, g_ZoomMTG == nullptr)
			throw std::exception("Failed to create ZoomMTG object");
		if (g_HUD = new HUD, g_HUD == nullptr)
			throw std::exception("Failed to create HUD object");
		if (g_FileIO = new FileInterface, g_FileIO == nullptr)
			throw std::exception("Failed to create File Interface object");
		if (g_ParserObj = new Parser, g_ParserObj == nullptr)
			throw std::exception("Failed to create Parser object");
		
	}
	
	catch (std::exception& e)
	{
		char szErrorText[MAX_PATH];
		sprintf(szErrorText, "An exception has occurred!\n%s\n\nAutomatic Zoom will now exit", e.what());
		MessageBoxA(hWnd, szErrorText, "Automatic Zoom Error", MB_ICONHAND);
		exit(0xDEADBEEF);
	}

	#ifdef _DEBUG
			sprintf(ToOutputLog, "This is a debug version of %s, stability is not guaranteed.", AppVersion);
			MessageBoxA(hWnd, ToOutputLog, "Debug Version Warning", MB_ICONWARNING);
	#endif

	g_hMainWnd = CreateDialog(g_hInst,
							  MAKEINTRESOURCE(MAIN), 
							  hWnd,
							  MainWindow);

	if(!g_hMainWnd)
	{
		exit(0);
	}

	//
	// Load accelerators, initialize comctrl
	// and show the main window
	//
	g_hAccelTable = LoadAccelerators(g_hInst, MAKEINTRESOURCE(IDC_AUTOMATICZOOM));
	InitCommonControls();
	ShowWindow(g_hMainWnd, SW_SHOW);

	//
	// Accelerator translate and
	// dispatch loop
	//
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(g_hMainWnd, g_hAccelTable, &msg))
		{
			if (!IsDialogMessage(g_hMainWnd, &msg))
			{
				TranslateMessage(&msg); // Translates Accelerators
				DispatchMessage(&msg); // Dispatches translated messages to wndproc
			}
		}
	}

	//
	// Destroy the objects (bring down the entire app)
	//
	if (g_ParserObj)
		delete g_ParserObj;
	if (g_FileIO)
		delete g_FileIO;
	if (g_HUD)
		delete g_HUD;
	if (g_ZoomMTG)
		delete g_ZoomMTG;
	if (g_Logger)
		delete g_Logger;

	//
	// Detect any memory leaks when exiting 
	// and dump them to the debug console
	//
	#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
	#endif

	return (int)msg.wParam;
}




INT_PTR CALLBACK MainWindow(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmID = LOWORD(wParam);
	RECT Rect1, Rect2;
	hWnd = GetDesktopWindow();
	clock_t start, end;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	
	case PWM_ACTIVATE:
		SetForegroundWindow(hDlg);

		SetWindowLongPtr(hDlg, DWLP_MSGRESULT, PWM_ACTIVATE);
		return (INT_PTR)TRUE;

	case WM_INITDIALOG: // Dialog Initialization

		g_Logger->LogToFile("Initializing App");

		start = clock();

		//
		// Set window position to the center
		//
		GetWindowRect(hDlg, &Rect1);
		GetWindowRect(hWnd, &Rect2);
		SetWindowPos(hDlg,
					 NULL,
                 	 (Rect2.right + Rect2.left) / 2 - (Rect1.right - Rect1.left) / 2,
                 	 (Rect2.top + Rect2.bottom) / 2 - (Rect1.bottom - Rect1.top) / 2,
					 0,
					 0,
					 1);

		//
		// Create the toolbar and status bar
		//
		Toolbar = g_HUD->CreateToolbar(g_hInst, hDlg);
		StatusBar = g_HUD->MakeStatusBar(hDlg);
		g_Logger->LogToBox(hDlg, AppVersion, 1);

		//
		// Check if zoom client is installed
		// on the local machine
		//
		g_ZoomMTG->ClientCheck();

		end = clock();
		sprintf(ToOutputLog, "App initialization phase took %2.3f seconds", (double)(end - start) / CLOCKS_PER_SEC);

		g_Logger->LogToFile(ToOutputLog);
		OutputDebugStringA(ToOutputLog);


		//
		// Start Parser Thread
		//
		g_Logger->LogToFile("Starting Schedule File Parser Thread");
		_beginthreadex(0,0,(_beginthreadex_proc_type)g_ParserObj->ParseScheduleFile,(void *) hDlg,0,0);


		//
		// Create a thread that will be used
		// to check the schedule data stored
		// in memory
		//
		_beginthreadex(0,0,(_beginthreadex_proc_type)g_ZoomMTG->ZoomMTG_Scheduled_Send,(void *) hDlg,0,0);

		return (INT_PTR)TRUE;

	case WM_COMMAND: // Command Handler Section

		switch(wmID)
		{
		#ifdef _DEBUG

			case OpenFileToolbar:
				SendMessageA(Toolbar, TB_ENABLEBUTTON, OpenFileToolbar, 0);
				g_FileIO->OpenFile(hDlg);
				SendMessageA(Toolbar, TB_ENABLEBUTTON, OpenFileToolbar, 1);
				break;
			
			case ReloadSchedToolbar:
				_beginthread((_beginthread_proc_type)g_ParserObj->ParseScheduleFile, 0, 0);
				break;
			
		#endif

			case InDev:
				SendMessageA(Toolbar, TB_ENABLEBUTTON, InDev, 0);
				g_Logger->LogToBox(hDlg, "This is currently In Dev", 3);
				break;

			case Copy:
				g_Logger->CopyResults(hDlg);
				break;

			case SaveLogToolbar:
				SendMessageA(Toolbar, TB_ENABLEBUTTON, SaveLogToolbar, 0);
				g_FileIO->SaveLogFile(hDlg);
				SendMessageA(Toolbar, TB_ENABLEBUTTON, SaveLogToolbar, 1);
				break;
			
			case AlwaysOnTopToolbar:
				g_AlwaysOnTop = !g_AlwaysOnTop;
				SetWindowPos(hDlg, g_AlwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST, 
							 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				break;

			case AboutToolbar:
				CreateDialogParam(g_hInst, MAKEINTRESOURCE(AboutBox), hDlg, About::AboutWndProc, 0);
				break;

			case IDCANCEL:
				if (MessageBoxA(hDlg, "Exit Automatic Zoom?", "Automatic Zoom", MB_YESNO | MB_ICONQUESTION) == IDYES)
					PostQuitMessage(0);
				break;
			
			case StartTimerToolbar:
				Resolve = g_ZoomMTG->ZoomMTG_Resolve(hDlg);
			
				switch(Resolve)
				{
					case ZoomMTG_LOCAL:
						UsingMTG_URL = TRUE;
						break;
				
					case ZoomMTG_WEB:
						UsingMTG_URL = FALSE;
						break;

					case ZoomMTG_ERR:
						return (INT_PTR)FALSE;
				}

				SendMessageA(ToolbarWindow, TB_ENABLEBUTTON, EndTimerToolbar, 1);
				wait = GetDlgItemInt(hDlg, WaitTime, NULL, FALSE);

				if (wait <= 0)
				{
					g_Logger->LogToBox(hDlg, "WARNING: No wait time specified", LogToBox_BlankPage);
				}
				else
				{
					//
					// Begin the countdown on a seperate thread or else
					// the app will lock up
					//
					CountdownThread = (HANDLE)_beginthread(HUD::Countdown, 0, (void *)wait);
				}

				sprintf(ToOutputLog, "Starting %d Minute Timer...", wait);
				g_Logger->LogToBox(hDlg, ToOutputLog, LogToBox_Timestamped_BlankPage);

				SetTimer(hDlg, /* Window handle to store time under */
						 400, /* Timer ID */
						 wait * 60000, /* Take wait time, convert it into seconds */
						 NULL); /* Function to execute when timer expires, WM_TIMER by default if set to NULL */
			
				SendMessageA(StatusBar, SB_SETTEXTA, 0, (LPARAM)"Waiting for timer to trigger");
				break;
			
			case EndTimerToolbar:

				//
				// Kill the timer and status bar countdown thread
				//

				KillTimer(hDlg, 400);
				TerminateThread(CountdownThread, 0);
				SendMessageA(StatusBar, SB_SETTEXTA, 1, (LPARAM)"");
				SetDlgItemTextA(hDlg, OutputLog, "");

				//
				// Return everything to normal
				//

				SendMessageA(ToolbarWindow, TB_ENABLEBUTTON, StartTimerToolbar, 1);
				SendMessageA(ToolbarWindow, TB_ENABLEBUTTON, EndTimerToolbar, 0);
				SendMessageA(StatusBar, SB_SETTEXTA, 0, (LPARAM)"Waiting for user input");

				//
				// Only erase the wait time
				//
				SetDlgItemTextA(hDlg, WaitTime, "");
				break;
		}
		break;
	case WM_TIMER: // Timer message, triggered by SetTimer()

		/* Show Progress */
		g_Logger->LogToBox(hDlg, "Timer Triggered", 1);
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
			g_ZoomMTG->ZoomMTG_Send(hDlg);
		else
			g_ZoomMTG->ZoomMTG_Web(hDlg);

		//
		// Return toolbar to normal state
		//
		SendMessageA(ToolbarWindow, TB_ENABLEBUTTON, StartTimerToolbar, 1);
		SendMessageA(ToolbarWindow, TB_ENABLEBUTTON, EndTimerToolbar, 0);

		/* 
		 * Not sure if this can get annoying over time
		 * But on a different perspective it could be useful
		 * because it allows for a new URL and passcode
		 * to be placed without selecting the whole string,
		 * it just wipes out the previous URL and passcode.
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
