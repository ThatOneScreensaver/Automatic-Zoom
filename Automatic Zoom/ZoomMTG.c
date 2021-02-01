#include "Resource.h"
#include "ZoomMTG.h"

UINT ZoomMTG_Resolve(HWND hDlg/* , char *Input */)
{
    /* Before we get started, clear out memory */
    memset(Input, 0, sizeof(Input));
    

    /* Get Zoom Meeting URL or MeetingID and store it */
    GetDlgItemTextA(hDlg, ZoomMTG_Input, Input, sizeof(Input));


    /* No URL is stored in Buffer */
	if (_stricmp(Input, "") == 0)
	{			
		SetDlgItemTextA(hDlg, OutputLog, "ERROR: No specified Zoom Link/MeetingID!");

		return -1;
	}

    SetDlgItemTextA(hDlg, StartTimer, "End Timer");
    
    /* If it's not digit, return 0 */
    if (!isdigit(*Input))
        return 0;

    return 1;
}

void ZoomMTG_Send(HWND hDlg/* , char *ZoomMTG, char *ZoomMeetingID, char *ZoomPasscode */)
{
    /* Before we get started, clear out memory */
    memset(ZoomMTG, 0, sizeof(ZoomMTG));
    memset(ZoomMeetingID, 0, sizeof(ZoomMeetingID));
    memset(ZoomPasscode, 0, sizeof(ZoomPasscode));


    /* Get Zoom MeetingID and Passcode, store it */
    GetDlgItemTextA(hDlg, ZoomMTG_Input, ZoomMeetingID, sizeof(ZoomMeetingID));
    GetDlgItemTextA(hDlg, MeetingPasscode, ZoomPasscode, sizeof(ZoomPasscode));

    
    /* If there is no meeting ID or passcode specified, show error and return */
    if ( (_stricmp(ZoomMeetingID, "") || _stricmp(ZoomPasscode, "")) == 0) 
    {
        SetDlgItemTextA(hDlg, OutputLog, "ERROR: No MeetingID/Meeting Passcode Specified!");
        return;
    }

    /* "Combine" Strings Together */
    strcpy(ZoomMTG, "zoommtg://zoom.us/join?confno=");
    strcat(ZoomMTG, ZoomMeetingID);
    strcat(ZoomMTG, "&pwd=");
    strcat(ZoomMTG, ZoomPasscode);


    /* Shell-Execute Final Concatenated "zoommtg" URL */
    ShellExecuteA(hDlg, "open", ZoomMTG, NULL, NULL, SW_SHOW);
}

void ZoomMTG_Web(HWND hDlg/* , char *ZoomURL */)
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


    /* Shell-Execute URL using user's default browser*/
    ShellExecuteA(hDlg, "open", ZoomURL, NULL, NULL, SW_SHOW);
}