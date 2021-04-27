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

#include <windows.h>
#include <wininet.h>
#include <ShellAPI.h>
#include <signal.h.>
#include <stdio.h>
#pragma comment(lib, "wininet")

#define ZoomMTG_LOCAL 1
#define ZoomMTG_WEB   0
#define ZoomMTG_ERR  -1

class ZoomMTG{ /* Class name */
private:
	bool m_IsZoomClientInstalled = FALSE;
public:


//-----------------Function Prototypes-----------------//

ZoomMTG();

void ClientCheck();
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
/* static */ int ZoomMTG_Resolve(HWND hDlg);

/* 
 * NAME: ZoomMTG_Send
 * 
 * PURPOSE: Shell-Execute A Single Concatenated "zoommtg://" URL
 * 
 * ADVANTAGES: User can specify MeetingID and Meeting Passcode
 * 
 * DISADVANTAGES: Zoom client has to be installed on user's system
 */
/* static */ void ZoomMTG_Send(HWND hDlg);

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
/* static */ void ZoomMTG_Web(HWND hDlg);

/* 
 * NAME: ZoomMTG_CreateZoomProtoURL
 *
 * PURPOSE: Create and Shell-Execute a concatenated Zoom meeting protocol link
 *
 * ADVANTAGES: This is fully automated
 */
static char *ZoomMTG_CreateZoomProtoURL();

static int __stdcall ZoomMTG_Scheduled_Send(void * hDlg);

};

extern ZoomMTG *g_ZoomMTG;
