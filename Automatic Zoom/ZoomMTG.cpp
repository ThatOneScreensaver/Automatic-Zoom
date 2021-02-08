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

UINT
ZoomMTG::ZoomMTG_Resolve(HWND hDlg)
/*++

Routine Description:

    TBD

Arguments:

    hDlg - Dialog window handle, to be used for getting

Return Value:

    -1 = Error
     
     0 = A meeting web URL was specified, ZoomMTG_Web() will be called

     1 = A meeting ID was specified, ZoomMTG_Send will be called

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
		Logger::LogToBox(hDlg, "ERROR: No specified Zoom Link/MeetingID!", 0);
		return -1;
	}

    //
    // Change dialog button text
    //

    SetDlgItemTextA(hDlg, StartTimer, "End Timer");
    
    //
    // If it's not a digit, return 0
    //
    // Else return 1
    //

    if (!isdigit(*Input))
        return 0;

    return 1;
}

void
ZoomMTG::ZoomMTG_Send(HWND hDlg)
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
    strcpy_s(ZoomMTG_URL, "zoommtg://zoom.us/join?confno=");
    strcat_s(ZoomMTG_URL, ZoomMeetingID);
    
    if (_stricmp(ZoomPasscode, "") != 0) /* Password Specified */
    {
        strcat_s(ZoomMTG_URL, "&pwd=");
        strcat_s(ZoomMTG_URL, ZoomPasscode);
    }

    strcat_s(ZoomMTG_URL, "&browser=Automatic_Zoom");
    
    if (_stricmp(ZoomJoinName, "") != 0) /* Join Name Specified */
    {
        strcat_s(ZoomMTG_URL, "&uname=");
        strcat_s(ZoomMTG_URL, ZoomJoinName);
    }

    Logger::LogToBox(hDlg, "Opening zoommtg link", 0);
    Logger::LogToFile("Opening zoommtg link");

    /* Shell-Execute Final Concatenated "zoommtg" URL */
    ShellExecuteA(hDlg, "open", ZoomMTG_URL, NULL, NULL, SW_SHOW);
}

void ZoomMTG::ZoomMTG_Web(HWND hDlg)
{
    /* Before we get started, clear out memory */
    memset(ZoomURL, 0, sizeof(ZoomURL));


    /* Get Zoom URL */
    GetDlgItemTextA(hDlg, ZoomMTG_Input, ZoomURL, sizeof(ZoomURL));
    

    /* 
	 * Check if it's a valid URL
	 */
	if (InternetCheckConnectionA(ZoomURL,
	        					 FLAG_ICC_FORCE_CONNECTION,
								 0) == 0)
	{
		SetDlgItemTextA(hDlg, OutputLog, "ERROR: Dead Link! Did you type it in correctly?");
		return;
	}

    Logger::LogToBox(hDlg, "Opening Zoom Meeting in Browser", 0);
    Logger::LogToFile("ZoomMTG_Web() Opening Zoom Meeting in Browser");

    /* Shell-Execute URL using user's default browser*/
    ShellExecuteA(hDlg, "open", ZoomURL, NULL, NULL, SW_SHOW);
}