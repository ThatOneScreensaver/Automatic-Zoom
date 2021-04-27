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
#pragma warning(disable:4996)

MEMORYSTATUSEX MemEx;
OSVERSIONINFOA OSVer;

void
Debug::SystemInformation(HWND hDlg)
{
    //
    // Grab memory and system information.
    //

	OSVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    GetVersionExA(&OSVer);
    MemEx.dwLength = sizeof(MemEx);
    GlobalMemoryStatusEx(&MemEx);


    CxsWritten = sprintf(ToOutputLog, "Debug Information\r\n");
    Inter = ToOutputLog + CxsWritten;

    // Compile times are going to be different from the one in the About box
    // since it will show the date/time that THIS file was compiled on
    CxsWritten = sprintf(Inter, "Automatic Zoom compiled on %s at %s\r\n", __DATE__, __TIME__);
    Inter = Inter + CxsWritten;
    
    // Windows Information
    CxsWritten = sprintf(Inter, "\r\nWindows (Kernel) Version: %lu.%lu\r\n", OSVer.dwMajorVersion, OSVer.dwMinorVersion);
    Inter = Inter + CxsWritten;
    CxsWritten = sprintf(Inter, "Windows Build Number: %ld\r\n", OSVer.dwBuildNumber);
    Inter = Inter + CxsWritten;

    // Memory Information
    CxsWritten = sprintf(Inter, "Memory Available:  %llu MB\r\n", MemEx.ullAvailPhys / 1048576);
    Inter = Inter + CxsWritten;
    CxsWritten = sprintf(Inter, "Memory Installed:  %llu MB\r\n", MemEx.ullTotalPhys / 1048576);
    Inter = Inter + CxsWritten;

    //
    // Reset the input edit box text
    //

    SetDlgItemTextA(hDlg, ZoomMTG_Input, "");

    g_Logger->LogToBox(hDlg, ToOutputLog, 1);
}