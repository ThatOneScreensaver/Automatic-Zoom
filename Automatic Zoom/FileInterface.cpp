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

const char *Filename = "Schedule.zmtg"; /* Filename to use */
char *LineData; /* Line data, data received by fgets() */
char ScheduleData[256]; /* Schedule File Data */
extern char Out[256];
DWORD ErrNo; /* Error Number Storage */
FILE* inputfile; /* File Stream */


//-----------------Function Definitions-----------------//

char *
FileInterface::OpenFile(HINSTANCE hInst, HWND hDlg)
{   
    // Path to file
    char Filename[1024];
 
    tagOFNA ofn;

    memset(Filename, 0, sizeof(Filename));
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = hDlg;
    ofn.lpstrFilter = "All Files (*.zmtg)\0*.zmtg\0\0";
    ofn.lpstrInitialDir = "Select ZoomMTG Schedule File";
    ofn.lpstrFile = Filename;
    ofn.lpstrFile[0] = '\0';
    ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
    ofn.nMaxFile = sizeof(Filename);

    if ((GetOpenFileNameA(&ofn)) != 0)
    {
        return Filename;
    }
    return NULL;
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
        OutputDebugStringA("Opening Schedule File...\n");
    #endif

    fopen_s(&inputfile, Filename, "rt");


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
    // fseek(inputfile, 0, SEEK_SET);
    // memset(Intermediate, 0, sizeof(Intermediate));

    // while(!feof(inputfile))
    // {
    //     c = fgetc(inputfile);
    //     if (c == '\n')
    //         totalLines = totalLines + 1;
    // }

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
    }
    // char Meeting[512];
    // int hour, minute, month, day, year;
    // sscanf(ScheduleData, "%s @ %i:%i %i/%i/%i", Meeting, hour, minute, month, day, year);

    //
    // Close file once we're done with everything
    //

    fclose(inputfile);

    // !
    // ! Debug: display file data in Debug Console
    // !

    #ifdef _DEBUG
        // Logger::LogToBox((HWND) hDlg, "Schedule Data Below", 0);
        // sprintf(Out, "Meeting-> %s\r\nHour-> %d\r\nMinute-> %d\r\nDate-> %d/%d/%d", Meeting, hour, minute, month, day, year);
        // Logger::LogToBox((HWND) hDlg, Out, 0);
        // OutputDebugStringA("Schedule File Contents Below");
        // OutputDebugStringA(Out);
        MessageBoxA((HWND) hDlg, ScheduleData, "Debug", MB_ICONINFORMATION);
    #endif

    return 1;
}