#pragma once
#include <Windows.h>
#include <windowsx.h>
#include <string>
#include <vector>
#include<codecvt>
#include "opencv2\opencv.hpp"
#include "DataSaverSimple.h"
#include "my_flags.h"
#include "GestureOptions.h"
#include "CamStreaming.h"
#include <thread>
using namespace std;
//struct FLAGES
//{
//	bool PROGRAM_TERMINATE = false;
//};
//volatile FLAGES flags;

class MyGUI
{
	HWND hwndDlg;
	//CEdit* subjectId_edit;
	//CComboBox* camera_box;
	//CComboBox* gesture_box;
	//CComboBox* left_completion_box;
	//CComboBox* right_completion_box;
	//CButton*next_button, *start_button, *end_button, *view_button, *abort_button, *save_button,*save_to_button;
	HWND subjectId_edit,height_edit;
	HWND camera_box;
	HWND gesture_box;
	HWND left_completion_box;
	HWND right_completion_box;
	HWND next_button,pre_button, start_button, end_button, view_button, abort_button, save_to_button, show_button;
	std::vector<std::wstring> cam_options = {L"Realsense",L"Orbbec" ,L"Kinect" };
	const string defalutSaveFolder = "Data/HandGestures";
	const string preViewVideoFolder = "Data/preview_gestures";
	vector<GestureOptions> all_ges_options;
	int cur_ges_option_idx;
	CamStreaming* camStreaming;
	CamStreaming* colorCamStreaming;
	void InitGestureOptions();
	void UpdatePreview();
	bool configCur();
public:
	volatile bool preview_terminated;
	MyGUI() :hwndDlg(0) {} 
	bool EnableButton(int ctrl, bool enable) {
		return Button_Enable(GetDlgItem(hwndDlg, ctrl), enable ? TRUE : FALSE) != 0;
	}
	void InitCombobox(HWND cId,const std::vector<std::wstring>& contents,bool enable=false)
	{
		for (int i = 0; i < contents.size(); i++)
		{
			ComboBox_InsertString(cId, i, contents[i].c_str());
		}
		ComboBox_SetCurSel(cId, 0);
		ComboBox_Enable(cId, enable);
	}
	bool IsButtonEnabled(int ctrl) {
		return IsWindowEnabled(GetDlgItem(hwndDlg, ctrl)) != 0;
	}
	void OnInitDialog(HWND hwndDlg);
	void OnStart();
	void OnEnd();
	void OnView();
	void OnNext();
	void OnPre();
	void OnAbort();
	void OnSaveTo();
	void OnChangeCam();
	void OnShow();
	static DWORD WINAPI ThreadProc(LPVOID arg);
	static void PreviewThread(MyGUI*);
	thread previewThr;
	void DrawBitmap(const cv::Mat& image);
	void SetButtonEnable(bool e_next, bool e_pre, bool e_start, bool e_end, bool e_view, bool e_abort);
	void OnCancel() {
		preview_terminated = true;
		previewThr.join();
		delete camStreaming;
		camStreaming = nullptr;
		if (hwndDlg) DestroyWindow(hwndDlg);
		PostQuitMessage(0);
	}
	~MyGUI() {
		if (camStreaming != nullptr)
			delete camStreaming;
	}
};

