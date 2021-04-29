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
#include <fstream>
#include <stdexcept>

DWORD Err;
const char *LogFilename = "automatic_zoom_log.txt";
char *Inter;
char LogBox[2048];
char Out[256];
FILE *LogFile;
Logger *g_Logger;


char ToOutputLog[1024];
int CxsWritten; /* Characters written by sprintf */
SYSTEMTIME LocalTime;

Logger::Logger()
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
    // Open file Stream
    //
    LogFile = fopen(LogFilename, "a");
    
    if (LogFile == 0)
    {
        OutputDebugStringA("Failed to open log file");

        if (LogFile)
            fclose(LogFile);

        return;
    }
    
    fprintf(LogFile, "Automatic Zoom by ThatOneScreensaver\r\n");

    // Close file stream
    fclose(LogFile);

    LogToFile("Started log system");
}

Logger::~Logger()
/*++

Routine Description:
    
    Close the open file stream, and exit the logger system.

Arguments:

    None.

Return Value:

    None.

--*/
{
    LogFile = fopen(LogFilename, "a");
    LogToFile("Exiting Automatic Zoom");
    fclose(LogFile);
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
    // Get log from box
    //
    
    GetDlgItemTextA(hDlg, OutputLog, LogBox, sizeof(LogBox));

    try 
    {
        //
        // Allocate memory for clipboard data
        //
        hGlobal = GlobalAlloc(GMEM_MOVEABLE, strlen(LogBox) + 1);
        if (!hGlobal)
            throw std::exception("Failed to allocate memory for clipboard data");

        //
        // Copy log data to buffer which will eventually
        // be copied to the clipboard.
        //
        memcpy(GlobalLock(hGlobal), LogBox, strlen(LogBox) + 1);
        
        //
        // Unlock the memory
        //
        if(GlobalUnlock(hGlobal))
            throw std::exception("Failed to release memory");
    }

    catch (std::exception& excep) 
    {
        sprintf(ToOutputLog, "%p %s at line %d, error code %lu\n", this, excep.what(), __LINE__, GetLastError());

        if (IsDebuggerPresent())
            OutputDebugStringA(ToOutputLog);

        g_Logger->LogToFile(ToOutputLog);
    }


    //
    // Copy buffer to clipboard, and
    // close/release clipboard
    //
    SetClipboardData(CF_TEXT, hGlobal);
    CloseClipboard();
    
    if (IsDebuggerPresent())
        OutputDebugStringA(LogBox);
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
    // Open File
    //
    LogFile = fopen(LogFilename, "a");

    if (LogFile == 0)
    {
        MessageBoxA(NULL, "Failed to write to log file", "Error", MB_ICONERROR);

        if (LogFile)
            fclose(LogFile);
    
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
    switch (Type)
    {
        case LogToBox_Timestamped_BlankPage:
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
            Inter = Out + CxsWritten;
            break; 
        
        case LogToBox_Timestamped_ExistingPage:
            GetLocalTime(&LocalTime);

            CxsWritten = sprintf(Inter,
                                 "\r\n%02d/%02d/%04d @ %02d:%02d:%02d (Local Time): %s\r\n",
                                 LocalTime.wMonth,
                                 LocalTime.wDay,
                                 LocalTime.wYear,
                                 LocalTime.wHour,
                                 LocalTime.wMinute,
                                 LocalTime.wSecond,
                                 ToLog);
            Inter = Inter + CxsWritten;
            break;

        case LogToBox_BlankPage:
            CxsWritten = sprintf(Out, "%s\r\n", ToLog);
            Inter = Out + CxsWritten;
            break;
        
        case LogToBox_ExistingPage:
            CxsWritten = sprintf(Out, "%s\r\n", ToLog);
            Inter = Out + CxsWritten;
            break;

        default:
            MessageBoxA(hDlg, "No valid Logger type was provided", "Internal Error", MB_ICONHAND);
            return;
    }
    
    SetDlgItemTextA(hDlg, OutputLog, Out);
}

int
Logger::SaveLogToFile(HWND hDlg, char *PathToSaveTo)
{
    char LogContents[2048];
    FILE *f;
    strcat(PathToSaveTo, ".txt");
    f = fopen(PathToSaveTo, "w");

    if (f == NULL)
    {
        return 0;
    }

    GetDlgItemTextA(hDlg, OutputLog, LogContents, sizeof(LogContents));

    fwrite(LogContents, sizeof(char), sizeof(LogContents), f);
    fclose(f);
    return 1;
}