// WindowsProject1.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "WindowsProject1.h"
#include "ArtiLife.h"
#include <vector>
#include <format>
#include <assert.h>
#include <windows.h>
#include <atltrace.h>
#include <thread>
#include <ppl.h>
using namespace concurrency;
using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;    // current instance
HWND hWindow;       // current window

WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

bool lifeAlive = false; // indicates lifeGrid has been initialized
bool lifePaused = false; // indicates "pause" has been selected by user
bool fullScreen = false; // indicates "full screen" has been selected by user
bool frameByFrame = false; // indicates "frame by frame" has been selected by user
bool exitApp = false;
long generations = 1;

int xCount=300; // temp row count
int yCount=160; // temp col count
vector<vector<LifeCell>> lifeGrid; // array of LifeCells

LARGE_INTEGER timeUpdate1, timeUpdate2, timeUpdateElapsed;
LARGE_INTEGER timeRender1, timeRender2, timeRenderElapsed;
LARGE_INTEGER timeTotal1, timeTotal2, timeTotalElapsed;
LARGE_INTEGER timeFrequency;

HDC         hdcMem;
HBITMAP     hbmMem;
HANDLE      hOld;
bool bufferInit = false;

HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0));
HBRUSH yellowBrush = CreateSolidBrush(RGB(255, 255, 0));
HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));

//may return 0 when not able to detect
const auto processor_count = std::thread::hardware_concurrency();
int threadCounter = 0;


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void UpdateAndRender();





/////////////////////////////////////////////
//
//              WIN MAIN
//
/////////////////////////////////////////////

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    ///////////////////////////////
    //
    // Initialize global strings
    // 
    ///////////////////////////////

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    // initialize LifeGrid
    
    lifeGrid.resize(xCount);
    for (int i = 0; i < xCount; ++i) lifeGrid[i].resize(yCount);

    bool border = false;
    bool alive = false;
    char color = 'b';
    int redCount = 0;
    int greenCount = 0;

    for (int x = 0; x < xCount; x++) {                                      
        for (int y = 0; y < yCount; y++) {
            if (x == 0 || y == 0 || x == xCount-1 || y == yCount-1) {   // initialize border
                border = true;
                color = 'b';
                alive = false;
                lifeGrid[x][y].init(x, y, color, border, alive, &lifeGrid[0][0], &lifeGrid[0][0], &lifeGrid[0][0], &lifeGrid[0][0], &lifeGrid[0][0], &lifeGrid[0][0], &lifeGrid[0][0], &lifeGrid[0][0]);
                continue;
            }
            
            if (x < (int)(xCount / 2))
                color = 'r';
            else
                color = 'g';

            alive = (rand() % 100 < 33);
            if (!alive)
                color = 'w';
            else switch (color)
            {
            case 'r': redCount++; break;
            case 'g': greenCount++; break;
            }
            border = false;
            lifeGrid[x][y].init(x, y, color, border, alive, &lifeGrid[x-1][y-1], &lifeGrid[x][y-1], &lifeGrid[x+1][y-1], &lifeGrid[x-1][y], &lifeGrid[x+1][y], &lifeGrid[x-1][y+1], &lifeGrid[x][y+1], &lifeGrid[x+1][y+1]);
        }
    }
    lifeAlive = true;
    generations = 1;


    // establish worker threads
    /*
    if (processor_count > 4)
    {
        // allocate 4 new threads for update
        // -- send 4 different indexes to the LifeGrid

    }

    if (processor_count > 5)
    {
        // allocate 1 new threads for renew

    }

    if (processor_count > 9)
    {
        // allocate 4 new threads for render
        // -- send 4 different indexes to the LifeGrid

    }
    */
    
    // prepare for MAIN LOOP

    InvalidateRect(hWindow, NULL, FALSE);
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));
    MSG msg;

    QueryPerformanceCounter(&timeTotal1);

    // Main message loop:
    while (!exitApp) 
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        UpdateAndRender();
        if (generations % 20 == 0)
        {   
            QueryPerformanceCounter(&timeTotal2);
            timeTotalElapsed.QuadPart = (timeTotal2.QuadPart - timeTotal1.QuadPart);

            ATLTRACE("Render Avg: %d\nUpdate Avg: %d\nTotal Avg: %d\n", (int)(timeRenderElapsed.QuadPart / generations), (int)(timeUpdateElapsed.QuadPart / generations), (int)(timeTotalElapsed.QuadPart / generations));
        }
    }

    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // destroy worker threads

    // destroy other objects
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
    DeleteObject(redBrush);
    DeleteObject(greenBrush);
    DeleteObject(yellowBrush);
    DeleteObject(whiteBrush);
    DeleteObject(blackBrush);

    
    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

///////////////////////////////////////////////////////////////////////////////////////
//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
///////////////////////////////////////////////////////////////////////////////////////
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   QueryPerformanceFrequency(&timeFrequency);

   hInst = hInstance; // Store instance handle in our global variable

   RECT r = RECT();
   r.top = 0;
   r.bottom = yCount*4; // each cell is reprented as a 4x4 rectangle
   r.left = 0;
   r.right = xCount*4;

   AdjustWindowRect(&r, WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, FALSE);
      
   hWindow = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, r.right-r.left, r.bottom-r.top, nullptr, nullptr, hInstance, nullptr);

   if (!hWindow)
   {
      return FALSE;
   }

   ShowWindow(hWindow, nCmdShow);
   UpdateWindow(hWindow);

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
///////////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                exitApp = true;
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            if (lifeAlive)
            {
                QueryPerformanceCounter(&timeRender1);

                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);

                // create buffer

                if (!bufferInit)
                {
                    hdcMem = CreateCompatibleDC(hdc);
                    hbmMem = CreateCompatibleBitmap(hdc, xCount*4, xCount*4);
                    bufferInit = true;
                }
                hOld = SelectObject(hdcMem, hbmMem);

                long top = 0;
                long left = 0;
                long bottom = 0;
                long right = 0;

                // write to buffer

                for (long x = 0; x < xCount; x++) {
                    for (long y = 0; y < yCount; y++) {
                        top = y * 4;
                        bottom = top + 4;
                        left = x * 4;
                        right = left + 4;

                        RECT cellRect = { left, top, right, bottom };

                        if (lifeGrid[x][y].getRender())
                        {
                            switch (lifeGrid[x][y].getColor()) {
                            case 'r': FillRect(hdcMem, &cellRect, redBrush);
                                break;
                            case 'g': FillRect(hdcMem, &cellRect, greenBrush);
                                break;
                            case 'y': FillRect(hdcMem, &cellRect, yellowBrush);
                                break;
                            case 'w': FillRect(hdcMem, &cellRect, whiteBrush);
                                break;
                            case 'b': FillRect(hdcMem, &cellRect, blackBrush);
                                break;
                            }
                        }
                    }
                }

                // Transfer the off-screen DC to the screen
                BitBlt(hdc, 0, 0, xCount*4, yCount*4, hdcMem, 0, 0, SRCCOPY);

                // Free-up the off-screen DC
                SelectObject(hdcMem, hOld);

                EndPaint(hWindow, &ps);

                QueryPerformanceCounter(&timeRender2);
                timeRenderElapsed.QuadPart += (timeRender2.QuadPart - timeRender1.QuadPart);
            }

        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        exitApp = true;
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}



/////////////////////////////////////////////
//
//           Update LifeGrid
//
/////////////////////////////////////////////


void UpdateAndRender() {

    QueryPerformanceCounter(&timeUpdate1);

    // make next generation

    for (int x = 0; x < xCount; x++) {
        for (int y = 0; y < yCount; y++) {
           lifeGrid[x][y].nextGen();
        }
    //    parallel_for(0, yCount, [&](int y)
    //        {
    //            lifeGrid[x][y].nextGen();
    //        });
    }

    // promote next generation

    for (int x = 0; x < xCount; x++) {
        for (int y = 0; y < yCount; y++) {
            lifeGrid[x][y].renew();
        }
    }

    // check for stable blocks (trusting cellular code will prevent out-of-range error)
    
    for (int x = 0; x < xCount; x++) {
        for (int y = 0; y < yCount; y++) {
            if (lifeGrid[x][y].couldBeBlock())  // checking the nw corner, essentially
            {
                if (lifeGrid[x + 1][y].couldBeBlock())
                    if (lifeGrid[x + 1][y + 1].couldBeBlock())
                        if (lifeGrid[x][y + 1].couldBeBlock())
                        {
                            lifeGrid[x][y].makeBlock();
                            lifeGrid[x+1][y].makeBlock();
                            lifeGrid[x][y+1].makeBlock();
                            lifeGrid[x+1][y+1].makeBlock();
                        }
            }
        }
    }
    
    generations++;
    if (generations >= 2400)
        DestroyWindow(hWindow);
    QueryPerformanceCounter(&timeUpdate2);
    timeUpdateElapsed.QuadPart += (timeUpdate2.QuadPart - timeUpdate1.QuadPart);

    // render the matrix
    InvalidateRect(hWindow, NULL, FALSE);
    //UpdateWindow(hWindow);
    return;
}
