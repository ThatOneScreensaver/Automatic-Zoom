#include "Resource.h"
#include "ZoomMTG.h"

void ZoomMTG_Send(HWND hDlg, char *ZoomMTG, char *ZoomMeetingID, char *ZoomPasscode)
{
    /* Before we get started, clear out memory */
    memset(ZoomMTG, 0, sizeof(ZoomMTG));
    memset(ZoomMeetingID, 0, sizeof(ZoomMeetingID));
    memset(ZoomPasscode, 0, sizeof(ZoomPasscode));


    /* Get Zoom MeetingID and Passcode, store it */
    GetDlgItemTextA(hDlg, MeetingID, ZoomMeetingID, sizeof(ZoomMeetingID));
    GetDlgItemTextA(hDlg, MeetingPasscode, ZoomPasscode, sizeof(ZoomPasscode));


    /* "Combine" Strings Together */
    strcpy(ZoomMTG, "zoommtg://zoom.us/join?confno=");
    strcat(ZoomMTG, ZoomMeetingID);
    strcat(ZoomMTG, "&pwd=");
    strcat(ZoomMTG, ZoomPasscode);


    /* Shell-Execute Final Concatenated "zoommtg" URL */
    ShellExecuteA(ZoomMTG, "open", ZoomMTG, NULL, NULL, SW_SHOW);
    
}

void ZoomMTG_Web(HWND hDlg, char *ZoomURL)
{
    /* Before we get started, clear out memory */
    memset(ZoomURL, 0, sizeof(ZoomURL));


    /* Get Zoom URL */
    GetDlgItemTextA(hDlg, ZoomMTG_URL, ZoomURL, sizeof(ZoomURL));
    
}