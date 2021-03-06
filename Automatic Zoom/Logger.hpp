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
#include <string>
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

int WINAPI __acrt_MessageBoxA(HWND, LPCSTR, LPCSTR, int);
_CRTIMP void __cdecl _assert(
   char const* message,
   char const* filename,
   unsigned line
);

#ifdef __cplusplus
}
#endif

inline void dprintf(LPCSTR szFormat, ...)
{
   char szBuffer[MAX_PATH];

   va_list  vaList;
   va_start(vaList, szFormat);

   //
   // print var-args according to the provided
   // format parameter, and display it in the 
   // debug console if a debugger is present.
   //

   vsprintf(szBuffer, szFormat, vaList);
   if(IsDebuggerPresent())
      OutputDebugStringA(szBuffer);

   va_end  (vaList);
}

#define LogToBox_Timestamped_BlankPage 3
#define LogToBox_Timestamped_ExistingPage 2
#define LogToBox_BlankPage 1
#define LogToBox_ExistingPage 0

class Logger{

public:

/* 
 * NAME: Logger
 * 
 * PURPOSE: Constructor
 *
 * ARGUMENTS:
 * 
 *      None
 * 
 */
Logger();

/* 
 * NAME: Logger
 * 
 * PURPOSE: Deconstructor
 *
 * ARGUMENTS:
 * 
 *      None
 * 
 */
~Logger();

/* 
 * NAME: CopyResults
 * 
 * PURPOSE: Copy results from log to clipboard
 *
 * ARGUMENTS:
 * 
 *      hDlg - Dialog handle in which clipboard
 *             data is stored under.
 * 
 */
void CopyResults(HWND hDlg);

/* 
 * NAME: LogToFile
 * 
 * PURPOSE: Log input string to file
 *
 * ARGUMENTS:
 * 
 *     ToFile - Input string to write
 * 
 */
void LogToFile(const char *ToFile);

/* 
 * NAME: LogToBox
 * 
 * PURPOSE: Log input string to file
 *
 * ARGUMENTS:
 *      
 *      hDlg - Dialog handle to display message in
 *      ToLog - String to write
 *      Type - Type of message to display
 *      
 *
 *      3 = Write timestamped log to blank page
 *      2 = Write timestamped log to existing page
 *      1 = Write log to blank page
 *      0 = Write log to existing page
 * 
 * NOTE: Do NOT input type 0 prior to inputting type 1
 *       Doing so will cause a crash.
 */
void LogToBox(HWND hDlg, const char *ToLog, int Type);

/*
 * NAME: SaveLogToFile
 * 
 * PURPOSE: Save log to a user provided file path
 * 
 * ARGUMENTS:
 * 
 * 		hDlg - Dialog handle to get log content from
 * 		PathToSaveTo - User provided path to save log file to
 */
int SaveLogToFile(HWND hDlg, char *PathToSaveTo);
};

extern char *Inter;
extern char ToOutputLog[1024];
extern char LogBox[2048];
extern char Out[256];
extern DWORD Err;
extern int CxsWritten; /* Characters written by sprintf */
extern SYSTEMTIME LocalTime;
extern Logger *g_Logger;