#include "Logger.h"

char Out;
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

    DeleteFileA("Log.txt");

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
    
    fprintf(LogFile, "Automatic Zoom by ThatOneScreensaver\r\n");

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
    fprintf(LogFile,
            "%02d/%02d/%04d @ %02d:%02d:%02d (Local Time) : %s\r\n",
            LocalTime.wMonth,
            LocalTime.wDay,
            LocalTime.wYear,
            LocalTime.wHour,
            LocalTime.wMinute,
            LocalTime.wSecond,
            ToFile);

    // Close file and free memory
    fclose(LogFile);
    free(LogFile);
}