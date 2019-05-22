#include "my_gui.h"
#include "resource1.h"
#include<atlimage.h>
#include <functional>
void static MatToCImage(const cv::Mat &mat, CImage &cImage)
{
	//create new CImage  
	int width = mat.cols;
	int height = mat.rows;
	int channels = mat.channels();

	cImage.Destroy(); //clear  
	cImage.Create(width,
		height, //positive: left-bottom-up   or negative: left-top-down  
		8 * channels); //numbers of bits per pixel  

					   //copy values  
	uchar* pimg = (uchar*)cImage.GetBits(); //A pointer to the bitmap buffer  

											//The pitch is the distance, in bytes. represent the beginning of   
											// one bitmap line and the beginning of the next bitmap line  
	int step = cImage.GetPitch();
#pragma omp parallel for
	for (int i = 0; i < height; ++i)
	{
		const uchar* ps = (mat.ptr<uchar>(i));
		for (int j = 0; j < width; ++j)
		{
			if (channels == 1) //gray  
			{
				*(pimg + i*step + j) = ps[j];
			}
			else if (channels == 3) //color  
			{
				for (int k = 0; k < 3; ++k)
				{
					*(pimg + i*step + j * 3 + k) = ps[j * 3 + k];
				}
			}
		}
	}
}
DWORD WINAPI MyGUI::ThreadProc(LPVOID arg) {
	cv::VideoCapture v(0);
	cv::Mat frame;
	auto gui = (MyGUI*)arg;
	if (!v.isOpened())
	{
		printf("Camera is not available");
	}
	bool success = v.read(frame);
	if (success)
	{
		do
		{
			gui->DrawBitmap(frame);
			success = v.read(frame);
		} while (success&& !gui->preview_terminated);
	}
	v.release();
	return 0;
}
void MyGUI::PreviewThread(MyGUI* gui)
{
	const GestureOptions& opt = gui->all_ges_options[gui->cur_ges_option_idx];
	string previewVidoePath = gui->preViewVideoFolder + "/" + opt.toString() + ".mp4";
	cv::VideoCapture v(previewVidoePath);
	cv::Mat frame;
	bool success = v.read(frame);
	while (success&&!gui->preview_terminated) {
		do
		{
			gui->DrawBitmap(frame);
			success = v.read(frame);
		} while (success && !gui->preview_terminated);
		v.set(CV_CAP_PROP_POS_FRAMES, 0);
		success = v.read(frame);
	}
	v.release();
}
void MyGUI::SetButtonEnable(bool e_next, bool e_pre, bool e_start, bool e_end, bool e_view, bool e_abort)
{
	Button_Enable(next_button, e_next);
	Button_Enable(pre_button, e_pre);
	Button_Enable(start_button, e_start);
	Button_Enable(end_button, e_end);
	Button_Enable(view_button, e_view);
	Button_Enable(abort_button, e_abort);

}
bool MyGUI::configCur()
{
	return true;
}
void MyGUI::OnStart()
{
	wchar_t e[5],h[8];
	Edit_GetText(subjectId_edit, e, 5);
	Edit_GetText(height_edit, h, 5);
	int subjectId;
	int cHeight;
	try {
		subjectId = std::stoi(e);
		cHeight = std::stoi(h);
	}
	catch (invalid_argument& ex)
	{
		MessageBoxW(0, L"Subject ID or Cam Height must be integer!", L"Excption", MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	if (camStreaming->config(subjectId, 90, cHeight, all_ges_options[cur_ges_option_idx]))
	{
		MessageBoxW(0, L"Current gesture already exists, please abort first", L"?", MB_ICONEXCLAMATION | MB_OK);
		SetButtonEnable(true, true, true, false, true, true);
		return;
	}
	colorCamStreaming->config(subjectId, 90, cHeight, all_ges_options[cur_ges_option_idx]);
	camStreaming->startSave();
	colorCamStreaming->startSave();
	SetButtonEnable(false, false, false, true, false, false);
	Edit_Enable(subjectId_edit, false);
	ComboBox_Enable(subjectId_edit, false);
}

void MyGUI::OnEnd()
{
	camStreaming->endSave();
	colorCamStreaming->endSave();
	SetButtonEnable(true, true, true, false, true, true);
	Edit_Enable(subjectId_edit, true);
	ComboBox_Enable(subjectId_edit, true);
}
void MyGUI::OnView()
{
	camStreaming->viewCurSaved();
}
void MyGUI::UpdatePreview()
{
	if (!preview_terminated) {
		preview_terminated = true;
		previewThr.join();
	}
	preview_terminated = false;
	//TerminateThread(previewThr, 0);
	ComboBox_SetCurSel(gesture_box, all_ges_options[cur_ges_option_idx].ges_idx);
	ComboBox_SetCurSel(left_completion_box, all_ges_options[cur_ges_option_idx].left_completion_idx);
	ComboBox_SetCurSel(right_completion_box, all_ges_options[cur_ges_option_idx].right_completion_idx);
	previewThr = thread(PreviewThread, this);
	//previewThr = CreateThread(0, 0, PreviewThread, this, 0, 0);
}
void MyGUI::OnNext()
{
	if (cur_ges_option_idx == all_ges_options.size()-1)
	{
		MessageBoxW(0, L"No more gestures", L"Finished", MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	cur_ges_option_idx += 1;
	UpdatePreview();
	SetButtonEnable(true, true, true, false, false, false);
}
void MyGUI::OnPre()
{
	if (cur_ges_option_idx ==0)
	{
		MessageBoxW(0, L"No more gestures", L"Finished", MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	cur_ges_option_idx -= 1;
	UpdatePreview();
	SetButtonEnable(true, true, true, false, false, false);
}
void MyGUI::OnAbort()
{
	camStreaming->deleteCurSaved();
	colorCamStreaming->deleteCurSaved();
	SetButtonEnable(true, true, true, false, false, false);
}

void MyGUI::OnSaveTo()
{

}
void MyGUI::OnChangeCam()
{
	int code = ComboBox_GetCurSel(camera_box);
	switch (code)
	{
	case 0:
		camStreaming->setCamera(CamStreaming::CAMWEA_TYPE::REALSENSE);
		break;
	case 1:
		camStreaming->setCamera(CamStreaming::CAMWEA_TYPE::ORBBEC);
		break;
	case 2:
		camStreaming->setCamera(CamStreaming::CAMWEA_TYPE::KINECT);
		break;
	default:
		camStreaming->setCamera(CamStreaming::CAMWEA_TYPE::COLOR);
		break;
	}
	camStreaming->streaming(true);
}
void MyGUI::OnShow()
{
	int code = ComboBox_GetCurSel(camera_box);
	switch (code)
	{
	case 0:
		camStreaming = new CamStreaming(CamStreaming::CAMWEA_TYPE::REALSENSE, defalutSaveFolder);
		//camStreaming->setCamera(CamStreaming::CAMWEA_TYPE::REALSENSE);
		break;
	case 1:
		camStreaming = new CamStreaming(CamStreaming::CAMWEA_TYPE::ORBBEC, defalutSaveFolder);

		//camStreaming->setCamera(CamStreaming::CAMWEA_TYPE::ORBBEC);
		break;
	case 2:
		camStreaming = new CamStreaming(CamStreaming::CAMWEA_TYPE::KINECT, defalutSaveFolder);

		//camStreaming->setCamera(CamStreaming::CAMWEA_TYPE::KINECT);
		break;
	default:
		camStreaming = new CamStreaming(CamStreaming::CAMWEA_TYPE::COLOR, defalutSaveFolder);
		//camStreaming->setCamera(CamStreaming::CAMWEA_TYPE::COLOR);
		break;
	}
	colorCamStreaming = new CamStreaming(CamStreaming::CAMWEA_TYPE::COLOR, defalutSaveFolder);
	Button_Enable(show_button, false);
	SetButtonEnable(true, true, true, false, false, false);
	camStreaming->streaming(true);
	colorCamStreaming->streaming(true);
}
void MyGUI::InitGestureOptions()
{
	for (int i = 0; i < GestureOptions::ges_options.size(); i++)
	{
		for (int j = 0; j < GestureOptions::completion_options.size(); j++)
		{
			for (int k = 0; k < GestureOptions::completion_options.size(); k++)
			{
				if (j == 0 && k == 0)
					continue;
				all_ges_options.push_back(GestureOptions(i, j, k));
			}
		}
	}
	cur_ges_option_idx = 0;
}
void MyGUI::OnInitDialog(HWND hwndDlg)
{
	preview_terminated = true;
	this->hwndDlg = hwndDlg;
	/*subjectId_edit = (CEdit*)GetDlgItem(hwndDlg, IDC_SUBJECTID);
	camera_box = (CComboBox*)GetDlgItem(hwndDlg, IDC_COMBO_CAM);
	gesture_box = (CComboBox*)GetDlgItem(hwndDlg, IDC_COMBO_GESTURE);
	left_completion_box = (CComboBox*)GetDlgItem(hwndDlg, IDC_COMBO_LEFT_COMPLETION);
	right_completion_box = (CComboBox*)GetDlgItem(hwndDlg, IDC_COMBO_RIGHT_COMPLETION);
	next_button = (CButton*)GetDlgItem(hwndDlg, IDC_NEXT);
	start_button = (CButton*)GetDlgItem(hwndDlg, IDC_START);
	end_button = (CButton*)GetDlgItem(hwndDlg, IDC_END);
	view_button = (CButton*)GetDlgItem(hwndDlg, IDC_VIEW);
	abort_button = (CButton*)GetDlgItem(hwndDlg, IDC_ABORT);
	save_button = (CButton*)GetDlgItem(hwndDlg, IDC_SAVE);
	save_to_button = (CButton*)GetDlgItem(hwndDlg, IDC_SAVE_TO);
	camera_box->AddString(CString("23"));*/
	subjectId_edit = GetDlgItem(hwndDlg, IDC_SUBJECTID);
	height_edit = GetDlgItem(hwndDlg, IDC_HEIGHT);
	Edit_SetText(height_edit, L"650");
	camera_box = GetDlgItem(hwndDlg, IDC_COMBO_CAM);
	gesture_box = GetDlgItem(hwndDlg, IDC_COMBO_GESTURE);
	left_completion_box = GetDlgItem(hwndDlg, IDC_COMBO_LEFT_COMPLETION);
	right_completion_box = GetDlgItem(hwndDlg, IDC_COMBO_RIGHT_COMPLETION);
	next_button = GetDlgItem(hwndDlg, IDC_NEXT);
	pre_button = GetDlgItem(hwndDlg, IDC_PRE);
	start_button = GetDlgItem(hwndDlg, IDC_START);
	end_button = GetDlgItem(hwndDlg, IDC_END);
	view_button = GetDlgItem(hwndDlg, IDC_VIEW);
	abort_button = GetDlgItem(hwndDlg, IDC_ABORT);
	save_to_button = GetDlgItem(hwndDlg, IDC_SAVE_TO);
	show_button = GetDlgItem(hwndDlg, IDC_SHOW);

	InitCombobox(camera_box, cam_options,true);
	InitCombobox(gesture_box, GestureOptions::ges_options);
	InitCombobox(left_completion_box, GestureOptions::completion_options);
	InitCombobox(right_completion_box, GestureOptions::completion_options);

	SetButtonEnable(true, true, false, false, false, false);
	InitGestureOptions();
	//auto thread = CreateThread(0, 0, ThreadProc, this, 0, 0);
	/*previewThr = CreateThread(0, 0, PreviewThread, this, 0, 0);
	Sleep(0);*/
	UpdatePreview();
	
	//camStreaming->streaming(true);
	/*ComboBox_AddString(camera_box, u"Realsense 400");
	ComboBox_AddString(camera_box, u"Orbbec");
	ComboBox_AddString(camera_box, u"Kinect");
	ComboBox_SetCurSel(camera_box,0);*/
}
void MyGUI::DrawBitmap(const cv::Mat& image) {
	HWND hwndPanel = GetDlgItem(hwndDlg, IDC_PANEL);
	CImage cImage;
	MatToCImage(image, cImage);
	CRect rect;
	GetClientRect(hwndPanel, &rect);
	float dx = rect.Width();
	float dy = rect.Height();//获得控件的宽高比
	float t = dy / dx;//获得控件的宽高比
	float k = image.rows * 1.0f / image.cols;
	if (k >= t)
	{
		rect.right = floor(rect.bottom / k);
		rect.left = (dx - rect.right) / 2;
		rect.right = floor(rect.bottom / k) + (dx - rect.right) / 2;
	}
	else
	{
		rect.bottom = floor(k*rect.right);
		rect.top = (dy - rect.bottom) / 2;
		rect.bottom = floor(k*rect.right) + (dy - rect.bottom) / 2;
	}
	//相关的计算为了让图片在绘图区居中按比例显示，原理很好懂，如果图片很宽但是不高，就上下留有空白区；如果图片很高而不宽就左右留有空白区，并且保持两边空白区一样大
	HDC dc = GetDC(hwndPanel); //获取picture control的DC，这是什么玩意我也不知道，百度就行
	int ModeOld = SetStretchBltMode(dc, STRETCH_HALFTONE);//设置指定设备环境中的位图拉伸模式
	cImage.StretchBlt(dc, rect, SRCCOPY);//显示函数
	SetStretchBltMode(dc, ModeOld);
	ReleaseDC(hwndPanel, dc);
}