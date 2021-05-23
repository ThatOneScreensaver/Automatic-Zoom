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

#include <commctrl.h>
#include <process.h>
#include <ShellAPI.h>
#include <stdio.h>
#include <time.h>
#include <WinInet.h>
#include <windowsx.h>
#include <fstream>

// Disable this because we purposefully
// check whether a bool is -1
#pragma warning(disable: 4806) 

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

const char *OptionsFilename = "automatic_zoom_options.txt"; 

BOOL UsingMTG_URL;
BOOL g_AlwaysOnTop = FALSE;

COptions g_Options;

DWORD g_idTrayThread;

double duration;

int Resolve;

HACCEL g_hAccelTable;
HANDLE CountdownThread;
HANDLE g_hTrayThread;
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

void COptions::SetDefaultOptions()
/*++

Routine Description:

	Sets default settings if previously loaded settings are corrupted
	or non-existant.

Arguments:

	None.

Return Value:

	None

 */
{
	//
	// Zero out any previous garbage
	// settings to overwrite them with
	// the new/defaults.
	//
	memset(this, 0, sizeof(COptions));

	//
	// These are the default values which get set
	//
	m_Debugging = FALSE;
	m_AlwaysOnTop = FALSE;
}

bool COptions::Load()
/*++

Routine Description:

	Loads previously stored options from an external text file.

Arguments:

	None.

Return Value:

	Non-zero if successful.

 */
{
	std::ifstream options_txt;

	options_txt.open(OptionsFilename, std::ifstream::binary | std::ifstream::in);
	if (options_txt.fail())
	{
		//
		// We failed to load the settings, so return to defaults
		//
		g_Options.SetDefaultOptions();
		return FALSE;
	}

	options_txt.read(reinterpret_cast<char*>(this), sizeof(COptions));
	options_txt.close();

	//
	// Check if any of the loaded options are invalid
	//
	if (m_Debugging == -1
		|| m_AlwaysOnTop == -1)
			SetDefaultOptions();

	return TRUE;
}

bool COptions::Save()
/*++

Routine Description:

	Saves current settings to an external txt file.

Arguments:

	None.

Return Value:

	Non-zero if successful.

 */
{
	std::ofstream options_txt;
	
	//
	// Open (or create) the options file as a binary
	//
	options_txt.open(OptionsFilename, std::ofstream::binary | std::ofstream::out);
	if (options_txt.fail())
	{
		MessageBox(g_hMainWnd, L"Failed to save options", L"Options Error", MB_ICONERROR);
		return FALSE;
	}

	//
	// If we were invoked with the debug command line param
	// return it back to FALSE so when we get ran again,
	// we don't run as debug
	//
	if (g_Options.m_Debugging)
		g_Options.m_Debugging = !g_Options.m_Debugging;

	//
	// Write options to file and close
	//
	options_txt.write(reinterpret_cast<char*>(this), sizeof(COptions));
	options_txt.close();
	return TRUE;
}


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

	//
	// Load settings from txt
	//
	g_Options.Load();

	//
	// Check if debug command line argument
	// was passed
	//
	if (!_stricmp(lpCmdLine, "debug"))
		g_Options.m_Debugging = !g_Options.m_Debugging;

	//
	// Create the tray worker thread
	//
	
	g_hTrayThread = CreateThread(nullptr, 0, &TrayThreadMsgLoop, nullptr, 0, &g_idTrayThread);
	if (!g_hTrayThread)
		dprintf("Failed to create tray worker, last error was %lu", GetLastError());

	g_hMainWnd = CreateDialog(g_hInst,
							  MAKEINTRESOURCE(MAIN), 
							  hWnd,
							  MainWindow);

	if(!g_hMainWnd)
	{
		exit(0);
	}

	// Add tray icon using the tray worker
	PostThreadMessage(g_idTrayThread, PM_INITIALIZE, 0, 0);

	//
	// Load accelerators, initialize comctrl
	// and show the main window
	//
	g_hAccelTable = LoadAccelerators(g_hInst, MAKEINTRESOURCE(IDC_AUTOMATICZOOM));
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

BOOL MainWindow_OnInitDlg(HWND hwnd, HWND hwndFocus, LPARAM lparam)
{
	tagRECT MainWndRect, DesktopRect;
	HWND hDesktopWnd = GetDesktopWindow();
	UNREFERENCED_PARAMETER(hwndFocus);
	UNREFERENCED_PARAMETER(lparam);

	g_Logger->LogToFile("Initializing App");

	clock_t start = clock();

	//
	// Set window position to the center
	//
	GetWindowRect(hwnd, &MainWndRect);
	GetWindowRect(hDesktopWnd, &DesktopRect);
	SetWindowPos (hwnd,
				 NULL,
               	 (DesktopRect.right + DesktopRect.left) / 2 - (MainWndRect.right - MainWndRect.left) / 2,
               	 (DesktopRect.top + DesktopRect.bottom) / 2 - (MainWndRect.bottom - MainWndRect.top) / 2,
				 0,
				 0,
				 1);

	//
	// Create the toolbar and status bar
	//
	Toolbar = g_HUD->CreateToolbar(g_hInst, hwnd);
	StatusBar = g_HUD->MakeStatusBar(hwnd);
	g_Logger->LogToBox(hwnd, AppVersion, 1);

	//
	// Notify if running under debug
	//
	if(g_Options.m_Debugging)
	{
		wchar_t szWndTitle[MAX_PATH];
		wcscpy(szWndTitle, L"Automatic Zoom (Debug Release Version)");
		SetWindowText(hwnd, szWndTitle);

		g_Logger->LogToBox(hwnd, "*** Release version running under debug mode", LogToBox_ExistingPage);
	}

	//
	// Check if we're initially supposed to be top-most
	// and if we are, set the window as top-most
	//
	if (g_Options.m_AlwaysOnTop)
	{
		SendMessage(Toolbar, TB_SETSTATE, AlwaysOnTopToolbar, TBSTATE_CHECKED | TBSTATE_ENABLED);
		SetWindowPos(hwnd, HWND_TOPMOST,
					 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}

	//
	// Check if zoom client is installed
	// on the local machine
	//
	g_ZoomMTG->ClientCheck();

	clock_t end = clock();
	sprintf(ToOutputLog, "App initialization phase took %2.3f seconds", (double)(end - start) / CLOCKS_PER_SEC);

	g_Logger->LogToFile(ToOutputLog);
	OutputDebugStringA(ToOutputLog);


	//
	// Start Parser Thread
	//
	g_Logger->LogToFile("Starting Schedule File Parser Thread");
	_beginthreadex(0,0,(_beginthreadex_proc_type)g_ParserObj->ParseScheduleFile,(void *) hwnd,0,0);

	//
	// Create a thread that will be used
	// to check the schedule data stored
	// in memory
	//
	_beginthreadex(0,0,(_beginthreadex_proc_type)g_ZoomMTG->ZoomMTG_Scheduled_Send,(void *) hwnd,0,0);

	return TRUE;
}

/*++

Routine Description:

	Handles all window size/resize related messages.
	Resizes group boxes and controls.

Arguments:

	hwnd - owner window that is being resized.
	idState - resized window state.
	cx - width of new client area.
	cy - height of new client area.

Return Value:

	None.

--*/
void MainWindow_OnSize(HWND hwnd, UINT idState, int, int)
{
	// Hide window to the taskbar
	if (idState == SIZE_MINIMIZED)
		ShowWindow(hwnd, SW_HIDE);
}


INT_PTR CALLBACK MainWindow(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmID = LOWORD(wParam);
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
	
	HANDLE_MSG(hDlg, WM_INITDIALOG, MainWindow_OnInitDlg);
	HANDLE_MSG(hDlg, WM_SIZE, 		MainWindow_OnSize);

	case PWM_ACTIVATE:
		SetForegroundWindow(hDlg);

		SetWindowLongPtr(hDlg, DWLP_MSGRESULT, PWM_ACTIVATE);
		return (INT_PTR)TRUE;

	case PWM_TRAYICON:
		TrayNotify(hDlg, wParam, lParam);
		break;

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
				g_Options.m_AlwaysOnTop = !g_Options.m_AlwaysOnTop;
				SetWindowPos(hDlg, g_Options.m_AlwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST, 
							 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				break;

			case AboutToolbar:
				CreateDialogParam(g_hInst, MAKEINTRESOURCE(AboutBox), hDlg, About::AboutWndProc, 0);
				break;

			case IDM_EXIT:
			case WM_DESTROY:
				// If there's a tray worker, tell it to quit.

				if(g_idTrayThread)
					PostThreadMessage(g_idTrayThread, PM_QUITTRAYTHREAD, 0, 0);
				
				//
				// Stick around and wait for the tray thread to cleanup
				// and exit, if it doesn't do this in time we drop it.
				//

				if(g_hTrayThread)
				{
					WaitForSingleObject(g_hTrayThread, 3000); // 3 seconds *should* be enough
					CloseHandle(g_hTrayThread);
				}

				//
				// Save settings to text and quit.
				//

				g_Options.Save();
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
