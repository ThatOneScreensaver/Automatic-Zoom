#include "Debug.h"
#include "Logger.h"
#include "Resource.h"

MEMORYSTATUSEX MemEx;
OSVERSIONINFOA OSVer;

extern int CxsWritten;
extern char *Inter;
extern char ToOutputLog[1024];

void
Debug::MemoryInformation(HWND hDlg)
{

    //
    // Grab memory and system information.
    //

	OSVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    GetVersionExA(&OSVer);
    MemEx.dwLength = sizeof(MemEx);
    GlobalMemoryStatusEx(&MemEx);

    //
    // HACK: sprintf-ing to display debug information
    // TODO: Update logging system to allow sprint-like parameters
    //

    CxsWritten = sprintf(ToOutputLog, "Debug Information\r\n");
    Inter = ToOutputLog + CxsWritten;

    CxsWritten = sprintf(Inter, "Windows (Kernel) Version: %u.%u\r\n", OSVer.dwMajorVersion, OSVer.dwMinorVersion);
    Inter = Inter + CxsWritten;

    CxsWritten = sprintf(Inter, "Memory Available:  %d MB\r\n", MemEx.ullAvailPhys / 1048576);
    Inter = Inter + CxsWritten;

    CxsWritten = sprintf(Inter, "Memory Installed:  %d MB\r\n", MemEx.ullTotalPhys / 1048576);
    Inter = Inter + CxsWritten;

    SetDlgItemTextA(hDlg, ZoomMTG_Input, "");

    Logger::LogToBox(hDlg, ToOutputLog, 1);
}