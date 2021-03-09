#include "resource.h"
#include <windows.h>
#include <CommCtrl.h>


class HUD {
public:
    static HWND CreateToolbar(HINSTANCE hInst, HWND hWnd);
    static HWND MakeStatusBar(HWND hWnd);
};