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
#include <process.h>
#include <time.h>

bool isSchedFileRefresh;
const char *InitFile = "Schedule.zmtg"; /* Filename to use */
char *LineData; /* Line data, data received by fgets() */
char ScheduleData[256]; /* Schedule File Data */
char PathToSchedFile[2048];
DWORD ErrNo; /* Error Number Storage */
FILE* inputfile; /* File Stream */

FileInterface *g_FileIO;
Parser *g_ParserObj;

UINT64 SchedMeetingID[ClassesAllowed];
UINT SchedHour[ClassesAllowed], SchedMinute[ClassesAllowed], SchedSecond[];

//-----------------Function Definitions-----------------//

int
FileInterface::OpenFile(HWND hDlg)
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
        strcpy(PathToSchedFile, FilePath);
        isSchedFileRefresh = TRUE;
        _beginthread((_beginthread_proc_type)g_ParserObj->ParseScheduleFile, 0, (void *) hDlg);
        return 1;
    }
    return 0;
}

int
FileInterface::SaveLogFile(HWND hDlg)
{
    // Path to save log file under
    char FilePath[1024];

    tagOFNA sfn;

    memset(FilePath, 0, sizeof(FilePath));
    memset(&sfn, 0, sizeof(sfn));
    sfn.lStructSize = sizeof(OPENFILENAMEA);
    sfn.hwndOwner = hDlg;
    sfn.lpstrTitle = "Save Log File";
    sfn.lpstrFilter = "Text Files (*.txt)\0*.txt";
    sfn.lpstrFile = FilePath;
    sfn.lpstrFile[0] = '\0';
    sfn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
    sfn.nMaxFile = sizeof(FilePath);
    
    if((GetSaveFileNameA(&sfn)) != 0)
    {
        g_Logger->SaveLogToFile(hDlg, FilePath);
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
    clock_t begin = clock();

    //
    // Allocate memory and open file with read access
    //
    #ifdef _DEBUG
        OutputDebugStringA("\r\nOpening Schedule File...\n");
    #endif

    if (strlen(PathToSchedFile) != 0)
    {
        inputfile = fopen(PathToSchedFile, "rt");
    }
    else
        inputfile = fopen(InitFile, "rt");

    //
    // Check if the file exists and if
    // it doesn't, exit the thread
    //
    if (inputfile == 0)
    {
        if (inputfile)
            fclose(inputfile);
        return NULL;
    }

    //
    // Seek to beginning of file and check
    // how many lines we have to read
    //
    int linesRead{}, totalLines{};
    char Intermediate[512];
    memset(Intermediate, 0, sizeof(Intermediate));

    //
    // Check if we are reloading the file
    // reset buffers if we are
    //
    if (isSchedFileRefresh == TRUE)
    {
        memset(SchedMeetingID, 0, sizeof(SchedMeetingID));
        memset(SchedHour, 0, sizeof(SchedHour));
        memset(SchedMinute, 0, sizeof(SchedMinute));
        memset(SchedSecond, 0, sizeof(SchedSecond));
        isSchedFileRefresh = FALSE;
    }

    //
    // Seek back to the beginning of file
    // and get the amount of total lines
    //
    fseek(inputfile, 0, SEEK_SET);
    
    for (int c = getc(inputfile); c != EOF; c = getc(inputfile))
        if (c == '\n')
            totalLines++;

    if (totalLines >= 10)
    {
        sprintf(ToOutputLog, "Total lines (classes) in schedule file exceeds the total amount of classes allowed!");
        goto error;
    }

    //
    // Read the lines from the file
    //
    fseek(inputfile, 0, SEEK_SET);
    for (linesRead = 0; linesRead <= totalLines; linesRead++)
    {
        fgets(Intermediate,
              512,
              inputfile);
        strcpy(ScheduleData, Intermediate);
        if (strlen(ScheduleData) != 0)
        {
            //
            // Get the schedule data from the file, 
            // and show it in the log box
            // Also check if the fields are correct and
            // if they aren't, abort file parsing.
            //
            if ((sscanf(ScheduleData, "%llu %u %u %u", &SchedMeetingID[linesRead], &SchedHour[linesRead], &SchedMinute[linesRead], &SchedSecond[linesRead]) != 4))
            {
                strcpy(ToOutputLog, "Schedule file error, expected"
                                    "\n'meetingID hour minute second'");
                goto error;
            }
            if (SchedSecond[linesRead] < 0 || SchedSecond[linesRead] > 59)
            {
                strcpy(ToOutputLog, "Seconds must be greater than 1 but less than 59!");
                goto error;
            }
            #ifdef _DEBUG
            sprintf(ToOutputLog, 
                    "MeetingID[%d] = %llu\r\n"
                    "Hour[%d] = %u\r\n"
                    "Minute[%d] = %u\r\n"
                    "Second[%d] = %u\r\n\n",
                    linesRead,
                    SchedMeetingID[linesRead],
                    linesRead,
                    SchedHour[linesRead],
                    linesRead,
                    SchedMinute[linesRead],
                    linesRead,
                    SchedSecond[linesRead]);
            strcat(LogBox, ToOutputLog);
            #endif
        }
    }

    //
    // Close file once we're done with everything
    //

    fclose(inputfile);

    clock_t end = clock();

    //
    // Show parsing time
    //
    sprintf(ToOutputLog, "Took %2.3f seconds to parse schedule file\n", ((double)(end - begin) / CLOCKS_PER_SEC));
    g_Logger->LogToFile(ToOutputLog);
    OutputDebugStringA(ToOutputLog);

    // ! Debug: display file data in a message box
    #ifdef _DEBUG
        memset(PathToSchedFile, 0, strlen(PathToSchedFile));
        __acrt_MessageBoxA((HWND) hDlg, LogBox, "Debug", MB_ICONINFORMATION);
        OutputDebugStringA(LogBox);
        memset(LogBox, 0, strlen(LogBox));
    #endif
    
    return 1;

error:
    //
    // Shouldn't get here unless we have an error!
    //
    fclose(inputfile);
    MessageBoxA((HWND) hDlg, ToOutputLog, "File Parser Error", MB_ICONSTOP);
    return 0;
}

int
Parser::ParseScheduleFileFromPath()
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
    _beginthread((_beginthread_proc_type)Parser::ParseScheduleFile,0,0);
    return 0;
}