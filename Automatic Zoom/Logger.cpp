#include "Debug.h"
#include "Logger.h"
#include "Resource.h"

const char *LogFilename = "automatic_zoommtg_log.txt";
char *InterOut;
char LogBox[2048];
char Out[256];
FILE *LogFile;

//
// Putting this here because it already
// exists in Automatic Zoom.cpp
//
extern char ToOutputLog[1024];
extern int CxsWritten; /* Characters written by sprintf */
extern SYSTEMTIME LocalTime;


void
Logger::Setup()
/*++

Routine Description:
    
    Erase log.txt from executable directory and reset log

Arguments:

    None.

Return Value:

    None.

--*/

{
    //
    // Delete File
    //

    DeleteFileA(LogFilename);

    //
    // Allocate Memory and Open File
    //

    malloc(sizeof(LogFile));
    LogFile = fopen(LogFilename, "a");
    
    if (LogFile == 0)
    {
        MessageBoxA(NULL, "Failed to create log file", "Error", MB_ICONERROR);

        if (LogFile)
            fclose(LogFile);
            
        free(LogFile);
        return;
    }
    
    fprintf(LogFile, "Automatic Zoom by ThatOneScreensaver\r\n");

    // Close file and free memory
    fclose(LogFile);
    free(LogFile);

    LogToFile("Starting Log");
}

void
Logger::CopyResults(HWND hDlg)
/*++

Routine Description:
    
    Open clipboard and erase previous data,
    Copy log to memory and then into clipboard.

Arguments:

    hDlg - Dialog handle in which the clipboard is stored.

Return Value:

    None.

--*/

{
    HGLOBAL hGlobal;

    //
    // Open clipboard and empty/wipe it out.
    //

    if (!OpenClipboard(hDlg))
    {
        Logger::LogToFile("Logger::CopyResults() : Failed to Open Clipboard");
        return;
    }

    EmptyClipboard();

    //
    // Get log from box and allocate memory for clipboard data.
    //
    
    GetDlgItemTextA(hDlg, OutputLog, LogBox, sizeof(LogBox));
    hGlobal = GlobalAlloc(GMEM_MOVEABLE, strlen(LogBox) + 1);

    if (!hGlobal)
    {
        LogToFile("Logger::CopyResults() : Failed to allocate memory for clipboard data");
        return;
    }

    //
    // Copy clpboard data to memory.
    //

    memcpy(GlobalLock(hGlobal), LogBox, strlen(LogBox) + 1);

    //
    // Release memory, set clipboard data
    // and close clipboard.
    //

    GlobalUnlock(hGlobal);
    SetClipboardData(CF_TEXT, hGlobal);
    CloseClipboard();
}

void
Logger::LogToFile(const char *ToFile)
/*++

Routine Description:
    
    Write input string to external log file

Arguments:

    ToFile - String to write

Return Value:

    None.

--*/

{
    //
    // Allocate Memory and Open File
    //

    malloc(sizeof(LogFile));
    LogFile = fopen(LogFilename, "a");

    if (LogFile == 0)
    {
        MessageBoxA(NULL, "Failed to write to log file", "Error", MB_ICONERROR);

        if (LogFile)
            fclose(LogFile);
            
        free(LogFile);
        return;
    }

    //
    // Get local time, store it.
    // Then write input argument to file.
    //

    GetLocalTime(&LocalTime);
    fseek(LogFile, 0, SEEK_SET);
    fprintf(LogFile,
              "%02d/%02d/%04d @ %02d:%02d:%02d (Local Time) : %s\r\n",
              LocalTime.wMonth,
              LocalTime.wDay,
              LocalTime.wYear,
              LocalTime.wHour,
              LocalTime.wMinute,
              LocalTime.wSecond,
              ToFile);

    //
    // Close file and free memory
    //
    
    fclose(LogFile);
    free(LogFile);
}

void
Logger::LogToBox(HWND hDlg, const char *ToLog, int Type)
/*++

Routine Description:
    
    Write input string to dialog output log

Arguments:

    hDlg - Dialog handle to display message in

    ToLog - String to write

    Type - Type of message to display
        3 = Write timestamped log to blank page
        2 = Write timestamped log to existing page
        1 = Write log to blank page
        0 = Write log to existing page

Return Value:

    None.

--*/

{
    if (Type == 3)
    {
        GetLocalTime(&LocalTime);

        CxsWritten = sprintf(Out,
                             "%02d/%02d/%04d @ %02d:%02d:%02d (Local Time): %s\r\n",
                             LocalTime.wMonth,
                             LocalTime.wDay,
                             LocalTime.wYear,
                             LocalTime.wHour,
                             LocalTime.wMinute,
                             LocalTime.wSecond,
                             ToLog);
        InterOut = Out + CxsWritten;       
    }

    if (Type == 2)
    {
        GetLocalTime(&LocalTime);

        CxsWritten = sprintf(InterOut,
                             "\r\n%02d/%02d/%04d @ %02d:%02d:%02d (Local Time): %s\r\n",
                             LocalTime.wMonth,
                             LocalTime.wDay,
                             LocalTime.wYear,
                             LocalTime.wHour,
                             LocalTime.wMinute,
                             LocalTime.wSecond,
                             ToLog);
        InterOut = InterOut + CxsWritten;
    }

    if (Type == 1)
    {
        CxsWritten = sprintf(Out, "%s\r\n", ToLog);
        InterOut = Out + CxsWritten;
    }

    else if (Type == 0)
    {
        CxsWritten = sprintf(InterOut, "\r\n%s\r\n", ToLog);
        InterOut = InterOut + CxsWritten;
    }
    
    SetDlgItemTextA(hDlg, OutputLog, Out);
}