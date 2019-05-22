/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2012-2015 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#include <Windows.h>
#include <WindowsX.h>
#include "resource1.h"
#include <CommCtrl.h>
#include <tchar.h>
#include <opencv2\opencv.hpp>
#include "my_gui.h"
using namespace cv;
MyGUI g;
static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM) {
	int t = HIWORD(wParam);
	switch (message) {
	case WM_INITDIALOG:
		g.OnInitDialog(hwndDlg);
		return TRUE;
	case WM_COMMAND:
		/* Check if any of the device/stream menu is pressed */
		/* Additional selections */
		switch (LOWORD(wParam)) {
		case IDCANCEL:
			g.OnCancel();
			return TRUE;
		case IDC_START:
			g.OnStart();
			return TRUE;
		case IDC_NEXT:
			g.OnNext();
			return TRUE;
		case IDC_PRE:
			g.OnPre();
			return TRUE;
		case IDC_END:
			g.OnEnd();
			return TRUE;
		case IDC_ABORT:
			g.OnAbort();
			return TRUE;
		case IDC_VIEW:
			g.OnView();
			return TRUE;
		case IDC_SHOW:
			g.OnShow();
			return TRUE;
		case IDC_SAVE_TO:
			g.OnSaveTo();
			return TRUE;
		case IDC_COMBO_CAM:
			return TRUE;
		case IDC_CHECK_SAVE_VIDEO:
			unsigned t = HIWORD(wParam);
			if (t == 0x0)
			{

			}
			else {

			}
			return TRUE;
		}
		/* If radio button is pressed, check it */
		//g.OnStreamButtonClick(wParam);
		switch (HIWORD(wParam)) {
		case IDC_COMBO_CAM:
			break;
		case BN_CLICKED:
			//g.OnButtonClick(wParam);
			break;
		}
		break;
	case WM_SIZE:
		return TRUE;
	}
	return FALSE;
}
#pragma warning(disable:4706) /* assignment within conditional */
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int) {
//	InitCommonControls();
	
	HWND hWnd = CreateDialogW(hInstance, MAKEINTRESOURCE(MAIN_FRAME), 0, DialogProc);
	ShowWindow(hWnd, SW_SHOW);
	if (!hWnd) {
		MessageBoxW(0, L"Failed to create a window", L"Raw Streams", MB_ICONEXCLAMATION | MB_OK);
		return 1;
	}

	UpdateWindow(hWnd);
	/*auto thread = CreateThread(0, 0, ThreadProc, hWnd, 0, 0);
	Sleep(0);*/
	MSG msg;

	for (int sts; (sts = GetMessageW(&msg, NULL, 0, 0));) {
		if (sts == -1) return sts;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
