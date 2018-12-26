// img_proc_basic_GDI.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "img_proc_basic_GDI.h"

#include "image_c.h"
//#include "gdi_dblbuf.hpp" ... needs MFC, so -> nope!

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, int w, int h);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

HWND hWnd;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_IMG_PROC_BASIC_GDI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    image img = load_image("..\\data\\balloons_small.jpg");

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow, img.w, img.h))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IMG_PROC_BASIC_GDI));

    MSG msg;

    HDC hdc = GetDC(hWnd);
    image imgOut;

//    imgOut = copy_image(img);
    // a) remove red
/*    for (int y = 0; y < img.h; y++)
    {
      for (int x = 0; x < img.w; x++)
      {
        set_pixel(imgOut, x, y, 0, 0); // remove red part of RGB
      }
    }
    */

    // b) convert to grey
//    imgOut = rgb_to_grayscale(img);

    // c) shift
//    shift_image(imgOut, 0, 0.8f);
//    shift_image(imgOut, 1, 1.4f);
//    shift_image(imgOut, 2, 0.1f);
//    clamp_image(imgOut);
/*
    // d) HSV
    image imgTMP = rgb_to_hsv(img);
    scale_image(imgTMP, 1, 1.5f); // increase saturation
    clamp_image(imgTMP);
    imgOut = hsv_to_rgb(imgTMP);
    free_image(imgTMP);
*/

// heap allocation error ... was due to write over array borders in nn_resize
    imgOut = bilinear_resize(img, 1200, 600);

    for (int y = 0; y < imgOut.h; y++)
    {
      for (int x = 0; x < imgOut.w; x++)
      {
        float r,g, b;
        if (imgOut.chan == 1)
        {
          r = g = b = get_pixel(imgOut, x, y, 0);
        }
        if (imgOut.chan == 3)
        {
          r = get_pixel(imgOut, x, y, 0);
          g = get_pixel(imgOut, x, y, 1);
          b = get_pixel(imgOut, x, y, 2);
        }
        SetPixel(hdc, x, y, RGB(r*255.0f, g*255.0f, b*255.0f)); // draw to GDI canvas
      }
    }

//    save_image(imgOut, "d:\\output.png");
    free_image(img);
    free_image(imgOut);

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IMG_PROC_BASIC_GDI));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_IMG_PROC_BASIC_GDI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, int w, int h)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      0,
//      w, // x
//      h, // y
      CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
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
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
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
