#include "Resource.h"
#include "FileParser.h"

const char *Filename = "Schedule.zmtg";
DWORD ErrNo;
FILE* inputfile;
char ScheduleData[512];

//-----------------Function Definitions-----------------//

void Parser::ParseScheduleFile(HWND hDlg)
/*++

Routine Description:
    
    Schedule file parser.

Arguments:

    hDlg - Handle to dialog window to be used by SetDlgItemTextA().

Return Value:

    None.

--*/

{
    /* Allocate Memory for File */
    malloc(sizeof(inputfile));

    /* Open file with read access */
    inputfile = fopen(Filename, "rt");

    if (inputfile == NULL)
    {
        MessageBoxA(NULL, "Failed to parse Schedule.zmtg", "Warning", MB_ICONWARNING);
        
        if (inputfile)
            fclose(inputfile);
        
        free(inputfile);
        return;
    }

    /* Goto beginning of file, read it and store it */
    // fseek(inputfile, 0, SEEK_SET);
    fgets(ScheduleData, 512, inputfile);
    // fread(ScheduleData, sizeof(char), 512, inputfile);

    // ! Debug: display file data in OutputLog
    SetDlgItemTextA(hDlg, OutputLog, ScheduleData);

    /* Free up memory after parsing */
    free(inputfile);
}