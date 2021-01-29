#include <windows.h>
#include <ShellAPI.h>
#include <stdio.h>

/* 
 * NAME: ZoomMTG_Send
 * 
 * PURPOSE: Shell-Execute A Single Concatenated "zoommtg://" URL
 * 
 * ADVANTAGES: User can specify MeetingID and Meeting Passcode
 * 
 * DISADVANTAGES: Zoom client has to be installed on user's system
 */
void ZoomMTG_Send(HWND hDlg, char *ZoomMTG, char *ZoomMeetingID, char *ZoomPasscode);

/* 
 * NAME: ZoomMTG_Web
 * 
 * PURPOSE: Shell-Execute a single user-provided Zoom meeting URL,
 *          Zoom meeting URL will open with the user's default
 *          browser (if no browser is set, a select browser dialog will pop up)
 * 
 * ADVANTAGES: Zoom client does not have to be installed on the user's system
 *             This is the most compatible method            
 * 
 * DISADVANTAGES: User cannot specify MeetingID and Meeting Passcode locally
 */
void ZoomMTG_Web(HWND hDlg, char *ZoomURL);

