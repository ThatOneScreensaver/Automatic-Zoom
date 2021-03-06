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

#include "Debug.hpp"
#include "FileInterface.hpp"
#include "HUD.hpp"
#include "Logger.hpp"
#include "Resource.h"
#include "ZoomMTG.hpp"
#include <process.h>
#include <sstream>

/* 
 * ZoomMTG-Link Related
 */
char Input[1024];
char ZoomMTG_URL[2048]; /* Overall Concatenated URL to be sent to the Windows Shell */
char ZoomMeetingID[16]; /* 12-character buffer for MeetingID (Bumped to 16-characters just in-case) */
char ZoomPasscode[64]; /* 32-character buffer for Passcode (Bumped to 64-characters just in-case) */
char ZoomJoinName[128]; /* Zoom name to display upon joining */

/* 
 * ZoomURL Related
 */
char ZoomURL[128];

extern DWORD Err;

ZoomMTG *g_ZoomMTG;

//-----------------Function Definitions-----------------//

ZoomMTG::ZoomMTG()
{
    if (IsDebuggerPresent())
    {
        sprintf(ToOutputLog, "Successfully created ZoomMTG object! this -> %p\n", this);
        OutputDebugStringA(ToOutputLog);
    }
}

void
ZoomMTG::ClientCheck()
{
    HKEY hKey;

    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"Software\\Classes\\zoommtg\\shell\\open\\command", NULL, KEY_READ, &hKey) == ERROR_SUCCESS)
        g_ZoomMTG->m_IsZoomClientInstalled = TRUE;
    else
        g_ZoomMTG->m_IsZoomClientInstalled = FALSE;
}

int
ZoomMTG::ZoomMTG_Resolve(HWND hDlg)
/*++

Routine Description:

    Determine whether a Zoom Meeting Web URL
    or a Meeting ID was provided.

Arguments:

    hDlg - Dialog window handle.

Return Value:

    ZommMTG_ERR = Error.

    ZoomMTG_WEB = A meeting web URL was specified, ZoomMTG_Web() will be called.

    ZoomMTG_LOCAL = A meeting ID was specified, ZoomMTG_Send will be called.

--*/

{
    /* Before we get started, clear out memory */
    memset(Input, 0, sizeof(Input));
    
    //
    // Get input and check if there is a zoom meeting link/id specified
    //
    
    GetDlgItemTextA(hDlg, ZoomMTG_Input, Input, sizeof(Input));
	if (_stricmp(Input, "") == 0)
	{			
		g_Logger->LogToBox(hDlg, "ERROR: No specified Zoom Link/MeetingID!", 1);
		return ZoomMTG_ERR;
	}

    //
    // Change toolbar button states
    //

    SendMessageA(ToolbarWindow, TB_ENABLEBUTTON, StartTimerToolbar, 0);
    
    //
    // Check if it's a digit and if it's a valid url
    // 
    // If it's valid, return 0 
    //

    if (!isdigit(*Input))
    {
        if (_stricmp(Input, "System") == 0) /* System Information */
        {
            Debug::SystemInformation(hDlg);
            SendMessageA(ToolbarWindow, TB_ENABLEBUTTON, StartTimerToolbar, 1);
            return ZoomMTG_ERR; /* Not really an error, but just to reset things */
        }

        if (_stricmp(Input, "ScheduleData") == 0) /* ScheduleData */
        {
            _beginthreadex(0,0,(_beginthreadex_proc_type)Parser::ParseScheduleFile,(void *)hDlg,0,0);
            SendMessageA(ToolbarWindow, TB_ENABLEBUTTON, StartTimerToolbar, 1);
            return ZoomMTG_ERR; /* Not necessarily an error, but just return it */
        }

        else if (InternetCheckConnectionA(Input, FLAG_ICC_FORCE_CONNECTION, 0) == 0)
	    {
            sprintf(LogBox, "ERROR: Dead Link! Did you type it in correctly? ( %lu )", GetLastError());
		    g_Logger->LogToBox(hDlg, LogBox, 1);
            SendMessageA(ToolbarWindow, TB_ENABLEBUTTON, StartTimerToolbar, 1);
		    return ZoomMTG_ERR;
	    }
        return ZoomMTG_WEB;
    }


    //
    // Make sure Zoom client is installed
    //
    if (!g_ZoomMTG->m_IsZoomClientInstalled)
    {
        SendMessageA(ToolbarWindow, TB_ENABLEBUTTON, StartTimerToolbar, 1);
        if (MessageBoxA(hDlg, "Zoom client is not installed, do you want to open the download page?", "ZoomMTG_Resolve", MB_YESNO | MB_DEFBUTTON1 | MB_ICONINFORMATION) == 6)
            ShellExecuteA(hDlg, "open", "https://zoom.us/download", NULL, NULL, SW_SHOW);
        return ZoomMTG_ERR;
    }

    //
    // Return 1 if client is installed and if we're
    // [internally] going to be using a protocol URL
    //
    return ZoomMTG_LOCAL;
}

void
ZoomMTG::ZoomMTG_Send(HWND hDlg)
/*++

Routine Description:

    Collect provided meeting information
    and concatenate it into a url.
    Then execute it.

Arguments:

    hDlg - Dialog window handle.

Return Value:

    None.

--*/

{
    /* Before we get started, clear out memory */
    memset(ZoomMTG_URL, 0, sizeof(ZoomMTG_URL));
    memset(ZoomMeetingID, 0, sizeof(ZoomMeetingID));
    memset(ZoomPasscode, 0, sizeof(ZoomPasscode));


    /* Get Zoom MeetingID and Passcode, store it */
    GetDlgItemTextA(hDlg, ZoomMTG_Input, ZoomMeetingID, sizeof(ZoomMeetingID));
    GetDlgItemTextA(hDlg, MeetingPasscode, ZoomPasscode, sizeof(ZoomPasscode));
    GetDlgItemTextA(hDlg, MeetingJoinName, ZoomJoinName, sizeof(ZoomJoinName));


    /* "Combine" Strings Together */
    strcpy(ZoomMTG_URL, "zoommtg://zoom.us/join?confno=");
    strcat(ZoomMTG_URL, ZoomMeetingID);

    if (_stricmp(ZoomPasscode, "") != 0) /* Password Specified */
    {
        strcat(ZoomMTG_URL, "&pwd=");
        strcat(ZoomMTG_URL, ZoomPasscode);
    }

    strcat(ZoomMTG_URL, "&browser=Automatic_Zoom");
    
    if (_stricmp(ZoomJoinName, "") != 0) /* Join Name Specified */
    {
        strcat(ZoomMTG_URL, "&uname=");
        strcat(ZoomMTG_URL, ZoomJoinName);
    }

    g_Logger->LogToBox(hDlg, "Opening zoommtg link", 2);
    if (IsDebuggerPresent())
    {
        sprintf(ToOutputLog, "Zoom Meeting URI is \"%s\"\r\n", ZoomMTG_URL);
        OutputDebugStringA(ToOutputLog);
    }

    /* Shell-Execute our produced protocol URL */
    ShellExecuteA(hDlg, "open", ZoomMTG_URL, NULL, NULL, SW_SHOW);
}

void ZoomMTG::ZoomMTG_Web(HWND hDlg)
/*++

Routine Description:

    Collect provided meeting url and
    execute it using user's default browser.

Arguments:

    hDlg - Dialog window handle.

Return Value:

    None.

--*/

{
    /* Before we get started, clear out memory */
    memset(ZoomURL, 0, sizeof(ZoomURL));

    //
    // Get URL and Log
    //

    GetDlgItemTextA(hDlg, ZoomMTG_Input, ZoomURL, sizeof(ZoomURL));

    g_Logger->LogToBox(hDlg, "Opening Zoom Meeting in Browser", 2);
    g_Logger->LogToFile("ZoomMTG_Web() Opening Zoom Meeting in Browser");

    /* Shell-Execute URL using user's default browser*/
    ShellExecuteA(hDlg, "open", ZoomURL, NULL, NULL, SW_SHOW);
}

int ZoomMTG::ZoomMTG_Scheduled_Send(void * hDlg)
{
    HWND Dlg = (HWND)hDlg;
    SYSTEMTIME lt;
    UINT classesOpened{};

    while(classesOpened <= ClassesAllowed)
    {
        GetLocalTime(&lt);
        if (lt.wHour == SchedHour[classesOpened]
            &&
            lt.wMinute == SchedMinute[classesOpened]
            &&
            lt.wSecond == SchedSecond[classesOpened])
        {
            sprintf(ZoomMTG_URL, "zoommtg://zoom.us/join?confno=%llu", SchedMeetingID[classesOpened]);
            strcat_s(ZoomMTG_URL, "&browser=Automatic_Zoom");
            
            sprintf(ToOutputLog, "Opening Class #%d", classesOpened+1);
            g_Logger->LogToBox(Dlg, ToOutputLog, 2);

            ShellExecuteA(NULL, "open", ZoomMTG_URL, NULL, NULL, SW_SHOW);
            classesOpened++;
        }
        Sleep(1000);
    }
    return 1;
}