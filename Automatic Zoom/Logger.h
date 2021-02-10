#include <stdio.h>
#include <windows.h>

class Logger{

public:

/* 
 * NAME: Setup
 * 
 * PURPOSE: Setup log writing
 *
 * ARGUMENTS:
 * 
 *      None
 * 
 */
static void Setup();

/* 
 * NAME: CopyResults
 * 
 * PURPOSE: Copy results from log to clipboard
 *
 * ARGUMENTS:
 * 
 *      hDlg - Dialog handle in which clipboard
 *             data is stored under.
 * 
 */
static void CopyResults(HWND hDlg);

/* 
 * NAME: LogToFile
 * 
 * PURPOSE: Log input string to file
 *
 * ARGUMENTS:
 * 
 *     ToFile - Input string to write
 * 
 */
static void LogToFile(const char *ToFile);

/* 
 * NAME: LogToBox
 * 
 * PURPOSE: Log input string to file
 *
 * ARGUMENTS:
 *      
 *      hDlg - Dialog handle to display message in
 *      ToLog - String to write
 *      Type - Type of message to display
 *      
 *
 *      3 = Write timestamped log to blank page
 *      2 = Write timestamped log to existing page
 *      1 = Write log to blank page
 *      0 = Write log to existing page
 * 
 * NOTE: Do NOT input type 0 prior to inputting type 1
 *       Doing so will cause a crash.
 */
static void LogToBox(HWND hDlg, const char *ToLog, int Type);

};