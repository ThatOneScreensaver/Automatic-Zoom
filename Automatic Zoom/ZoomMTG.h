#include <windows.h>
#include <wininet.h>
#include <ShellAPI.h>
#include <stdio.h>

#pragma comment(lib, "wininet")
#pragma warning(disable:4996)



class ZoomMTG{ /* Class name */
public:


//-----------------Function Prototypes-----------------//

/* 
 * NAME: ZoomMTG_Resolve
 * 
 * PURPOSE: Determine whether to concatenate or to open in web
 *          Returns -1 if failed; 0 if web; 1 if local.
 *
 * ADVANTAGES: None
 * 
 * DISADVANTAGES: None
 */
static UINT ZoomMTG_Resolve(HWND hDlg);

/* 
 * NAME: ZoomMTG_Send
 * 
 * PURPOSE: Shell-Execute A Single Concatenated "zoommtg://" URL
 * 
 * ADVANTAGES: User can specify MeetingID and Meeting Passcode
 * 
 * DISADVANTAGES: Zoom client has to be installed on user's system
 */
static void ZoomMTG_Send(HWND hDlg);

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
static void ZoomMTG_Web(HWND hDlg);

};
