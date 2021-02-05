#include "Resource.h"
#include "FileParser.h"

const char *Filename = "Schedule.zmtg";
DWORD ErrNo;
FILE* inputfile;
char ScheduleData[512];

//-----------------Function Definitions-----------------//

UINT __stdcall
Parser::ParseScheduleFile(void *)
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
    fopen_s(&inputfile, Filename, "r");


    if (inputfile == 0)
    {
        MessageBoxA(NULL, "Failed to parse Schedule.zmtg", "Error", MB_ICONERROR);
        
        if (inputfile)
            fclose(inputfile);
        
        free(inputfile);
        return NULL;
    }

    /* Goto beginning of file, read it and store it */
    fseek(inputfile, 0, SEEK_SET);
    fread(ScheduleData, sizeof(char), sizeof(inputfile), inputfile);

    // ! Debug: display file data in Debug Console
    OutputDebugStringA(ScheduleData);

    /* Free up memory after parsing */
    fclose(inputfile);
    free(inputfile);

    return 1;
}