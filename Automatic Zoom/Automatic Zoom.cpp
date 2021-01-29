// Automatic Zoom.cpp : Defines the entry point for the application.
//

/* Local Headers */
#include "stdafx.h"
#include "Automatic Zoom.h"
#include "ZoomMTG.h"


/* Include Path Headers */
#include <commctrl.h>
#include <ShellAPI.h>
#include <stdio.h>
#include <WinInet.h>


/* Link-Time Libraries */
#pragma comment(lib, "comctl32")
#pragma comment(lib, "WinInet")


/* Warnings Disabled */
#pragma warning(disable:4101)


/* Defines */
#define MAX_LOADSTRING 100


//-----------------Global Variables-----------------//
BOOL Enabled;


/* 
 * ZoomMTG-Link Related
 */
char ZoomMTG[8192]; /* Overall Concatenated URL to be sent to the Windows Shell */
char ZoomMeetingID[16]; /* 12-character buffer for MeetingID (Bumped to 16-characters just in-case */
char ZoomPasscode[64]; /* 32-character buffer for Passcode (Bumped to 64-characters just in-case*/


/* 
 * ZoomURL Related
 */
char ZoomURL[128];


/* 
 * Output log Related
 */
char *Inter; /* Intermediary Char Variable */
char ToOutputLog[1024]; /* Output Log */
int CxsWritten; /* Characters written to Buffer (return val from sprintf) */

HINSTANCE hInst;								// current instance

HWND Button;
HWND hWnd;
HWND StatusBar;

int wait; /* Time in seconds, to be converted into minutes */

TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name



//-----------------Function Prototypes-----------------//
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
INT_PTR CALLBACK	MainWindow(HWND, UINT, WPARAM, LPARAM);


//-----------------Function Definitions-----------------//
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
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
	case WM_INITDIALOG:

		ZoomMTG[0] = '\0';
		ToOutputLog[0] = '\0';


		hWnd = GetDesktopWindow();
		GetWindowRect(hWnd, lpRect);
		GetWindowRect(hDlg, &Rect1);

		// Set Window Position To Center Of Screen
		SetWindowPos(hDlg,NULL,
                 	(Rect2.right + Rect2.left) / 2 - (Rect1.right - Rect1.left) / 2,
                 	(Rect2.top + Rect2.bottom) / 2 - (Rect1.bottom - Rect1.top) / 2,
					 0,
					 0,
					 1);


		Button = GetDlgItem(hDlg, StartTimer);

		GetWindowRect(StatusBar, &Rect);

		CxsWritten = sprintf(ToOutputLog, "Automatic Zoom Joiner, version 1.0b");
		Inter = ToOutputLog + CxsWritten;
		
		SetDlgItemTextA(hDlg, OutputLog, ToOutputLog);

		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == StartTimer)
		{
			
			if (Enabled == FALSE)
			{

				Enabled = TRUE;

				memset(ZoomMTG, 0, sizeof(ZoomMTG));

				GetDlgItemTextA(hDlg, MeetingID, ZoomMTG, sizeof(ZoomMTG));


				/* No URL is stored in Buffer */
				if (_stricmp(ZoomMTG, "") == 0)
				{
					sprintf(ToOutputLog, "ERROR: No specified Zoom URL!");
					
					SetDlgItemTextA(hDlg, OutputLog, ToOutputLog);

					Enabled = FALSE;
					return (INT_PTR)FALSE;
				}

				


				/* 
				 * Check if it's a valid URL
				 */
				if (InternetCheckConnectionA(ZoomMTG,
											 FLAG_ICC_FORCE_CONNECTION,
											 0) == 0)
				{
					sprintf(ToOutputLog, "ERROR: Dead Link! Did you type it in correctly?");
					SetDlgItemTextA(hDlg, OutputLog, ToOutputLog);
					return (INT_PTR)FALSE;
				}



				wait = GetDlgItemInt(hDlg, WaitTime, NULL, FALSE);

				if (wait == 0)
				{
					// MessageBoxA(hDlg, "No wait time specified, defaulting to 1 minute", "Warning", MB_ICONWARNING);
					wait = 1;
					sprintf(ToOutputLog, "WARNING: No wait time specified, defaulting to 1 minute.");
					SetDlgItemTextA(hDlg, OutputLog, ToOutputLog);
				}				

				SetTimer(hDlg, 400, wait * 60000, NULL);

				SetDlgItemTextA(hDlg, StartTimer, "End Timer");

			}

			else
			{

				Enabled = FALSE;

				KillTimer(hDlg, 1);

				SetDlgItemTextA(hDlg, StartTimer, "Start Timer");
			
				SetDlgItemTextA(hDlg, ZoomMTG_URL, "");

			}
			
		}
		break;
	
	case WM_TIMER:

		/* Show Progress */
		sprintf(ToOutputLog, "Opening Zoom URL in Browser...");
		SetDlgItemTextA(hDlg, OutputLog, ToOutputLog);


		/* Concatenation, etc */
		ZoomMTG_Send(hDlg, ZoomMTG, ZoomMeetingID, ZoomPasscode);
		

		/* Kill timer so all of this doesn't happen again */
		KillTimer(hDlg, 400);


		/* 
		 * Just reset the button text
		 */
		SetDlgItemTextA(hDlg, StartTimer, "Start Timer");


		/* 
		 * Not sure if this can get annoying over time
		 * But on a different perspective it could be useful
		 * because it could allow for a new URL to be placed
		 * without selecting the whole string, it just wipes 
		 * out the leftover URL.
		 */	
		SetDlgItemTextA(hDlg, ZoomMTG_URL, "");
		
		return (INT_PTR)TRUE;
	}
	return (INT_PTR)FALSE;
}



