#include "Logger.h"
#include "Resource.h"

char *InterOut;
char Out[256];
extern int CxsWritten; /* Characters written by sprintf */
FILE *LogFile;

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

    DeleteFileA("automatic_zoomtg_log.txt");

    //
    // Allocate Memory and Open File
    //

    malloc(sizeof(LogFile));
    fopen_s(&LogFile, "Log.txt", "a");
    
    if (LogFile == 0)
    {
        MessageBoxA(NULL, "Failed to create log file", "Error", MB_ICONERROR);

        if (LogFile)
            fclose(LogFile);
            
        free(LogFile);
        return;
    }
    
    fprintf_s(LogFile, "Automatic Zoom by ThatOneScreensaver\r\n");

    // Close file and free memory
    fclose(LogFile);
    free(LogFile);

    LogToFile("Starting Log");
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
    fopen_s(&LogFile, "Log.txt", "a");

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
    fprintf_s(LogFile,
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
        1 = Write log to entire blank page
        0 = Write log to existing page

Return Value:

    None.

--*/

{
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