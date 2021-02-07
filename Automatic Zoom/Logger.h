#include <stdio.h>
#include <windows.h>

//
// Putting this here because it already
// exists in Automatic Zoom.cpp
//
extern SYSTEMTIME LocalTime;

class Logger{

public:

/* 
 * NAME: Setup
 * 
 * PURPOSE: Setup log writing
 *
 * ADVANTAGES: N/A
 * 
 * DISADVANTAGES: N/A
 */
static void Setup();

/* 
 * NAME: LogToFile
 * 
 * PURPOSE: Log input string to file
 *
 * ADVANTAGES: N/A
 * 
 * DISADVANTAGES: N/A
 */
static void LogToFile(const char *ToFile);

/* 
 * NAME: LogToBox
 * 
 * PURPOSE: Log input string to file
 *
 * ADVANTAGES: N/A
 * 
 * DISADVANTAGES: N/A
 * 
 * NOTE: Do NOT input type 0 prior to inputting type 1
 *       Doing so will cause a crash.
 */
static void LogToBox(HWND hDlg, const char *ToLog, int Type);

};