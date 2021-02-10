#include "Debug.h"
#include "Resource.h"
#include "Logger.h"
#include "ZoomMTG.h"

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
    
    //
    // Check if zoom is installed by checking for
    // registry key
    //

    if (RegOpenKeyA(HKEY_CURRENT_USER, "Software\\Classes\\zoommtg\\shell\\open\\command", NULL) == ERROR_FILE_NOT_FOUND)
    {
        Logger::LogToBox(hDlg,
                         "Zoom client is not installed, download the Zoom client from here: https://zoom.us/client/latest/ZoomInstaller.exe", 0);
        return -1;
    }

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
    // Change dialog button text
    //

    SetDlgItemTextA(hDlg, StartTimer, "End Timer");
    
    //
    // Check if it's a digit and if it's a valid url
    // 
    // If it's valid, return 0 
    //

    if (!isdigit(*Input))
    {
        if (_stricmp(Input, "DebugInfo") == 0) /* Debug Information */
        {
            Debug::MemoryInformation(hDlg);
            SetDlgItemTextA(hDlg, StartTimer, "Start Timer");
            return -1; /* Not really an error, but just to reset things */
        }

        else if (InternetCheckConnectionA(ZoomURL, FLAG_ICC_FORCE_CONNECTION, 0) == 0)
	    {
		    Logger::LogToBox(hDlg, "ERROR: Dead Link! Did you type it in correctly?", 1);
            SetDlgItemTextA(hDlg, StartTimer, "Start Timer");
		    return -1;
	    }
        return 0;
    }

    //
    // Return 1 if it's not a URL
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