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

#include "FileInterface.hpp"
#include "Logger.hpp"
#include "Resource.h"
#pragma warning(disable:4172)

const char *InitFile = "Schedule.zmtg"; /* Filename to use */
char *LineData; /* Line data, data received by fgets() */
char ScheduleData[256]; /* Schedule File Data */
extern char Out[256];
extern HWND hDlg;
DWORD ErrNo; /* Error Number Storage */
FILE* inputfile; /* File Stream */

char Meeting[512];
int hour, minute, month, day, year;


//-----------------Function Definitions-----------------//

int
FileInterface::OpenFile(HINSTANCE hInst, HWND hDlg)
{   
    // Path to file
    char FilePath[1024];
 
    tagOFNA ofn;

    memset(FilePath, 0, sizeof(FilePath));
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = hDlg;
    ofn.lpstrTitle = "Select Schedule File";
    ofn.lpstrFilter = "Meeting Schedule File (*.zmtg)\0*.zmtg\0\0";
    ofn.lpstrFile = FilePath;
    ofn.lpstrFile[0] = '\0';
    ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
    ofn.nMaxFile = sizeof(FilePath);

    if ((GetOpenFileNameA(&ofn)) != 0)
    {
        sprintf(Out, "Schedule file path is %s", FilePath);
        OutputDebugStringA(Out);
        MessageBoxA(hDlg, Out, "Information", MB_ICONINFORMATION);
        Parser::ParseScheduleFileFromPath(FilePath);
        return 1;
    }
    return 0;
}

unsigned int __stdcall
Parser::ParseScheduleFile(void * hDlg)
/*++

Routine Description:
    
    Schedule file parser.

Arguments:

    None.

Return Value:

    1 - Successfully parsed file

    0 - Failed to parse file

--*/

{
    //
    // Allocate memory and open file with read access
    //
    #ifdef _DEBUG
        OutputDebugStringA("\r\nOpening Schedule File...\n");
    #endif

    fopen_s(&inputfile, InitFile, "rt");


    if (inputfile == 0)
    {
        MessageBoxA((HWND) hDlg, "Failed to parse Schedule.zmtg", "Error", MB_ICONERROR);
        
        if (inputfile)
            fclose(inputfile);
        
        // free(inputfile);
        return NULL;
    }

    //
    // Seek to beginning of file and check
    // how many lines we have to read
    //

    int linesRead = 0, totalLines = 0;
    char c, Intermediate[512];
    memset(Intermediate, 0, sizeof(Intermediate));

    //
    // Seek back to the beginning of file
    // and read the entire file
    //
    
    totalLines = 0;

    fseek(inputfile, 0, SEEK_SET);
    for (linesRead = 0; linesRead <= totalLines; linesRead++)
    {
        fgets(Intermediate,
              512,
              inputfile);
        strcat(ScheduleData, Intermediate);
        if (strlen(ScheduleData) != 0)
        {
            sscanf(ScheduleData, "%s %d %d %d %d %d", &Meeting, &hour, &minute, &month, &day, &year);
        }
    }

    //
    // Close file once we're done with everything
    //

    fclose(inputfile);

    // !
    // ! Debug: display file data in Debug Console
    // !

    #ifdef _DEBUG
        // Logger::LogToBox((HWND) hDlg, "Schedule Data Below", 0);
        sprintf(Out, "Meeting-> %s\r\nHour-> %d\r\nMinute-> %d\r\nDate-> %d/%d/%d", Meeting, hour, minute, month, day, year);
        // Logger::LogToBox((HWND) hDlg, Out, 0);
        // OutputDebugStringA("Schedule File Contents Below");
        // OutputDebugStringA(Out);
        MessageBoxA((HWND) hDlg, Out, "Debug", MB_ICONINFORMATION);
    #endif

    // Erase any previous schedule data
    memset(Intermediate, 0, 512);
    memset(ScheduleData, 0, sizeof(ScheduleData));

    return 1;
}

int
Parser::ParseScheduleFileFromPath(char *PathToFile)
/*++

Routine Description:
    
    Parses schedule file from provided path.

Arguments:

    PathToFile - Path to file that will be parsed.

Return Value:

    1 - Successfully parsed file

    0 - Failed to parse file

--*/

{
    //
    // Allocate memory and open file with read access
    //
    #ifdef _DEBUG
        OutputDebugStringA("Opening Schedule File...\n");
    #endif

    fopen_s(&inputfile, PathToFile, "rt");


    if (inputfile == 0)
    {
        sprintf(Out, "Failed to parse %s", PathToFile);
        MessageBoxA(hDlg, Out, "Error", MB_ICONERROR);
        
        if (inputfile)
            fclose(inputfile);

        return NULL;
    }

    //
    // Seek to beginning of file and check
    // how many lines we have to read
    //

    int linesRead = 0, totalLines = 0;
    char c, Intermediate[512];

    // Erase any previous traces
    memset(Intermediate, 0, sizeof(Intermediate));
    memset(ScheduleData, 0, sizeof(ScheduleData));

    //
    // Seek back to the beginning of file
    // and read the entire file
    //
    
    totalLines = 0;

    fseek(inputfile, 0, SEEK_SET);
    for (linesRead = 0; linesRead <= totalLines; linesRead++)
    {
        fgets(Intermediate,
              512,
              inputfile);
        strcat(ScheduleData, Intermediate);
        if (strlen(ScheduleData) != 0)
        {
            sscanf(ScheduleData, "%s %d %d %d %d %d", &Meeting, &hour, &minute, &month, &day, &year);
        }
    }

    //
    // Close file once we're done with everything
    //

    fclose(inputfile);

    // !
    // ! Debug: display file data in Debug Console
    // !

    #ifdef _DEBUG
        // Logger::LogToBox((HWND) hDlg, "Schedule Data Below", 0);
        sprintf(Out, "Meeting-> %s\r\nHour-> %d\r\nMinute-> %d\r\nDate-> %d/%d/%d", Meeting, hour, minute, month, day, year);
        // Logger::LogToBox((HWND) hDlg, Out, 0);
        // OutputDebugStringA("Schedule File Contents Below");
        // OutputDebugStringA(Out);
        MessageBoxA(hDlg, Out, "Debug", MB_ICONINFORMATION);
    #endif

    return NULL;
}