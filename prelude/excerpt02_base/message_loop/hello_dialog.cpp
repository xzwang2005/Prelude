// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

/*
 *  This sample is a classic windows program showing how to explicitly pumping
 *  messages to keep the system running.
 */

#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <ole2.h>
#include <shlwapi.h>
#include <windowsx.h>

HINSTANCE g_hinst;
HWND g_hwndChild;

void OnSize(HWND hwnd, UINT state, int cx, int cy) {
  if (g_hwndChild) {
    MoveWindow(g_hwndChild, 0, 0, cx, cy, TRUE);
  }
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpcs) { return TRUE; }

void OnDestroy(HWND hwnd) { PostQuitMessage(0); }

void PaintContent(HWND hwnd, PAINTSTRUCT *pps) {}

void OnPaint(HWND hwnd) {
  PAINTSTRUCT ps;
  BeginPaint(hwnd, &ps);
  PaintContent(hwnd, &ps);
  EndPaint(hwnd, &ps);
}

void OnPrintClient(HWND hwnd, HDC hdc) {
  PAINTSTRUCT ps;
  ps.hdc = hdc;
  GetClientRect(hwnd, &ps.rcPaint);
  ps.fErase = FALSE;
  PaintContent(hwnd, &ps);
}

void OnChar(HWND hwnd, TCHAR ch, int cRepeat) {
  switch (ch) {
  case ' ':
    // MessageBox(hwnd, TEXT("Message"), TEXT("Title"), MB_OK);
    break;
  }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) {
  switch (uiMsg) {
    HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
    HANDLE_MSG(hwnd, WM_SIZE, OnSize);
    HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
    HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
    HANDLE_MSG(hwnd, WM_CHAR, OnChar);
  case WM_PRINTCLIENT:
    OnPrintClient(hwnd, (HDC)wParam);
    return 0;
  case WM_CONTEXTMENU:
    if (lParam != -1 &&
        SendMessage(hwnd, WM_NCHITTEST, 0, lParam) == HTSYSMENU) {
      HMENU hmenu = CreatePopupMenu();
      if (hmenu) {
        AppendMenu(hmenu, MF_STRING, 1, TEXT("Custom menu"));
        TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON,
                       GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, hwnd,
                       NULL);
        DestroyMenu(hmenu);
      }
      return 0;
    }
    break;
  }
  return DefWindowProc(hwnd, uiMsg, wParam, lParam);
}

BOOL InitApp(void) {
  WNDCLASS wc;
  wc.style = 0;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = g_hinst;
  wc.hIcon = NULL;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = TEXT("Scratch");
  if (!RegisterClass(&wc))
    return FALSE;

  InitCommonControls();

  return TRUE;
}

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine,
                   int nShowCmd) {
  MSG msg;
  HWND hwnd;
  g_hinst = hinst;
  if (!InitApp())
    return 0;

  if (SUCCEEDED(CoInitialize(NULL))) {
    hwnd = CreateWindow(TEXT("Scratch"), TEXT("Scratch"), WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL,
                        hinst, 0);
    ShowWindow(hwnd, nShowCmd);

    while (GetMessage(&msg, NULL, 0, 0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  CoUninitialize();

  return 0;
}
