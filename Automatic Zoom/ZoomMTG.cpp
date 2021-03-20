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

extern char LogBox[2048];
extern DWORD Err;

//-----------------Function Definitions-----------------//

int
ZoomMTG::ZoomMTG_Resolve(HWND hDlg)
/*++

Routine Description:

    Determine whether a Zoom Meeting Web URL
    or a Meeting ID was provided.

Arguments:

    hDlg - Dialog window handle.

Return Value:

    -1 = Error.

     0 = A meeting web URL was specified, ZoomMTG_Web() will be called.

     1 = A meeting ID was specified, ZoomMTG_Send will be called.

--*/

{
    /* Before we get started, clear out memory */
    memset(Input, 0, sizeof(Input));

    //
    // Allocate memory for input url
    //

    malloc(sizeof(Input));
    
    //
    // Get input and check if there is a zoom meeting link/id specified
    //
    
    GetDlgItemTextA(hDlg, ZoomMTG_Input, Input, sizeof(Input));
	if (_stricmp(Input, "") == 0)
	{			
		Logger::LogToBox(hDlg, "ERROR: No specified Zoom Link/MeetingID!", 1);
		return -1;
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
            return -1; /* Not really an error, but just to reset things */
        }

        if (_stricmp(Input, "ScheduleData") == 0) /* ScheduleData */
        {
            _beginthreadex(0,0,Parser::ParseScheduleFile,(void *)hDlg,0,0);
            SendMessageA(ToolbarWindow, TB_ENABLEBUTTON, StartTimerToolbar, 1);
            return -1; /* Not necessarily an error, but just return it */
        }

        else if (InternetCheckConnectionA(Input, FLAG_ICC_FORCE_CONNECTION, 0) == 0)
	    {
            Err = GetLastError();
            sprintf(LogBox, "ERROR: Dead Link! Did you type it in correctly? ( %d )", Err);
		    Logger::LogToBox(hDlg, LogBox, 1);
            SendMessageA(ToolbarWindow, TB_ENABLEBUTTON, StartTimerToolbar, 1);
		    return -1;
	    }
        return 0;
    }


    //
    // Make sure Zoom client is installed
    //

    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Classes\\zoommtg\\shell\\open\\command", NULL, KEY_ALL_ACCESS, &hKey) == ERROR_FILE_NOT_FOUND)
    {
        if (MessageBoxA(hDlg, "Zoom client is not installed, do you want to open the download page?", "ZoomMTG::", MB_YESNO | MB_DEFBUTTON1 | MB_ICONINFORMATION) == 6)
            ShellExecuteA(hDlg, "open", "https://zoom.us/download", NULL, NULL, SW_SHOW);
        return -1;
    }

    //
    // Return 1 if client is installed and if it's a URL
    //
    return 1;
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

    Logger::LogToBox(hDlg, "Opening zoommtg link", 2);
    Logger::LogToFile("Opening zoommtg link");

    /* Shell-Execute Final Concatenated "zoommtg" URL */
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

    Logger::LogToBox(hDlg, "Opening Zoom Meeting in Browser", 2);
    Logger::LogToFile("ZoomMTG_Web() Opening Zoom Meeting in Browser");

    /* Shell-Execute URL using user's default browser*/
    ShellExecuteA(hDlg, "open", ZoomURL, NULL, NULL, SW_SHOW);
}