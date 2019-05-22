#pragma once
#include<string>
#include<vector>
#include "my_flags.h"
#include "DataSaverSimple.h"
#include<thread>
#include"GestureOptions.h"
#include"CameraConfig.h"
using namespace std;
class CamStreaming
{
protected:
	string name;
	DataSaverSimple dataSaver;
	thread streamingThread,viewThread;
public:
	enum CAMWEA_TYPE
	{
		REALSENSE,
		ORBBEC,
		KINECT,
		COLOR
	} camera_type;
	CameraConfig* camerConfig;
	volatile StreamFlags flags;
	virtual void streaming(bool show = true)
	{
	//	streamingFunc(this, show);
		flags.STREAMING_TERMINATE = false;
	    streamingThread = thread(streamingFunc, this, show);
	}
	CamStreaming(){}
	CamStreaming(CAMWEA_TYPE camera_type,
	string saveFolder,bool color_stream=true,bool depth_stream=true,bool saveAsVieo = true):dataSaver(DataSaverSimple(saveFolder,depth_stream,color_stream, saveAsVieo)),camerConfig(nullptr)
	{
		setCamera(camera_type);
	}
	void setCamera(CAMWEA_TYPE camera_type)
	{
		if(!flags.STREAMING_TERMINATE)
			stopStreaming();
		this->camera_type = camera_type;
		if (camerConfig != nullptr)
		{
			delete camerConfig;
			camerConfig = nullptr;
		}
		switch (camera_type)
		{
		case CamStreaming::REALSENSE:
			name = "Realsense";
			dataSaver.saveColor = true;
			dataSaver.saveDepth = true;
			camerConfig = new RealsenseConfig();
			break;
		case CamStreaming::ORBBEC:
			name = "Orbbec";
			dataSaver.saveColor = false;
			dataSaver.saveDepth = true;
			camerConfig = new ObbecConfig();
			break;
		case CamStreaming::KINECT:
			name = "Kinect";
			dataSaver.saveColor = true;
			dataSaver.saveDepth = true;
			camerConfig = new KinectConfig();
			break;
		default:
			name = "Color";
			dataSaver.saveColor = true;
			dataSaver.saveDepth = false;
			camerConfig = new ColorConfig();
			break;
		}
	}
	bool config(int subjectId, int cAngle, int cHeight, const GestureOptions& gp)
	{
		return dataSaver.setAndCheck(name, subjectId, GestureOptions::ges_options_value[gp.ges_idx], cAngle, cHeight, GestureOptions::completion_options_value[gp.left_completion_idx], GestureOptions::completion_options_value[gp.right_completion_idx]);
	}
	void startSave()
	{
		dataSaver.start();
		flags.STREAMING_SAVING = true;
	}
	void endSave()
	{
		flags.STREAMING_SAVING = false;
		dataSaver.stop();
	}
	void deleteCurSaved()
	{
		dataSaver.deleteCurSaved();
	}
	void stopStreaming()
	{
		if(!flags.STREAMING_TERMINATE){
			flags.STREAMING_TERMINATE = true;
			streamingThread.join();
		}
	}
	void viewCurSaved()
	{
		viewThread = thread(viewFunc,&dataSaver);
		viewThread.join();
	}
	~CamStreaming()
	{
		stopStreaming();
		if (camerConfig != nullptr)
		{
			delete camerConfig;
			camerConfig = nullptr;
		}
	}
	static void streamingFunc(CamStreaming*, bool);
	//static void colorStreaming(CamStreaming* cs, bool show);
	//static void RealsenseStreaming(CamStreaming* cs, bool show);
	static void viewFunc(const DataSaverSimple* dataSaver);
};

