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

#include "Resource.h"
#include "FileParser.h"
#include "Logger.h"

const char *Filename = "Schedule.zmtg"; /* Filename to use */
char *LineData; /* Line data, data received by fgets() */
char ScheduleData[256]; /* Schedule File Data */
DWORD ErrNo; /* Error Number Storage */
FILE* inputfile; /* File Stream */


//-----------------Function Definitions-----------------//

UINT __stdcall
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
    
    OutputDebugStringA("Allocating Memory and Opening File...\n");
    malloc(sizeof(inputfile));
    fopen_s(&inputfile, Filename, "rt");


    if (inputfile == 0)
    {
        MessageBoxA((HWND) hDlg, "Failed to parse Schedule.zmtg", "Error", MB_ICONERROR);
        
        if (inputfile)
            fclose(inputfile);
        
        free(inputfile);
        return NULL;
    }

    //
    // Seek to beginning of file and read data
    //

    fseek(inputfile, 0, SEEK_SET);

    // HACKHACK: Use fread to view file data
    fread(ScheduleData, sizeof(char), 512, inputfile);

    //
    // ! Debug: display file data in Debug Console
    //

    #ifdef _DEBUG

    OutputDebugStringA(ScheduleData);
    MessageBoxA((HWND) hDlg, ScheduleData, "Debug", MB_ICONINFORMATION);
    
    #endif

    /* Free up memory after parsing */
    fclose(inputfile);
    free(inputfile);

    return 1;
}