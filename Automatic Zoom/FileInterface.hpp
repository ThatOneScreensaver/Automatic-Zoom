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

#include <stdio.h>
#include <windows.h>

#define ClassesAllowed 10
extern bool isSchedFileRefresh;
extern UINT64 SchedMeetingID[ClassesAllowed];
extern UINT SchedHour[ClassesAllowed], SchedMinute[ClassesAllowed], SchedSecond[ClassesAllowed];

class FileInterface {
	public:
		/* static */ int OpenFile(HWND hDlg);
		/* static */ int SaveLogFile(HWND hDlg);
};

extern FileInterface *g_FileIO;

class Parser{

public:


//-----------------Function Prototypes-----------------//

/* 
 * NAME: ParseScheduleFile
 * 
 * PURPOSE: Parse schedule from an external provided file
 *          with the extension of .zmtg for zoom meeting
 * 
 * ADVANTAGES: TBD
 * 
 * DISADVANTAGES: TBD
 */
static UINT __stdcall ParseScheduleFile(void *);

static int ParseScheduleFileFromPath();

};

extern Parser *g_ParserObj;
