#include <stdio.h>
#include <windows.h>

#pragma warning(disable:4996)

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

};