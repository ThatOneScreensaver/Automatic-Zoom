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
#include "Logger.hpp"
#include "Resource.h"
#pragma warning(disable:4477)

MEMORYSTATUSEX MemEx;
OSVERSIONINFOA OSVer;


extern int CxsWritten;
extern char *Inter;
extern char ToOutputLog[1024];

int month;
int day;
int year;


void
Debug::MemoryInformation(HWND hDlg)
{
    char CompileDate[24];

    //
    // Grab memory and system information.
    //

	OSVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    GetVersionExA(&OSVer);
    MemEx.dwLength = sizeof(MemEx);
    GlobalMemoryStatusEx(&MemEx);

    //
    // TODO: Update logging system to allow sprint-like parameters
    //

    CxsWritten = sprintf(ToOutputLog, "Debug Information\r\n");
    Inter = ToOutputLog + CxsWritten;

    // sscanf(__TIME__, "%d %d %d", &month, &day, &year);

    // CxsWritten = sprintf(Inter, "Automatic Zoom Compiled At %02d:%02d:%d\r\n", month, day, year);
    // Inter = Inter + CxsWritten;
    
    CxsWritten = sprintf(Inter, "\r\nWindows (Kernel) Version: %u.%u\r\n", OSVer.dwMajorVersion, OSVer.dwMinorVersion);
    Inter = Inter + CxsWritten;

    CxsWritten = sprintf(Inter, "Windows Build Number: %d\r\n", OSVer.dwBuildNumber);
    Inter = Inter + CxsWritten;

    CxsWritten = sprintf(Inter, "Memory Available:  %d MB\r\n", MemEx.ullAvailPhys / 1048576);
    Inter = Inter + CxsWritten;

    CxsWritten = sprintf(Inter, "Memory Installed:  %d MB\r\n", MemEx.ullTotalPhys / 1048576);
    Inter = Inter + CxsWritten;

    SetDlgItemTextA(hDlg, ZoomMTG_Input, "");

    Logger::LogToBox(hDlg, ToOutputLog, 1);
}