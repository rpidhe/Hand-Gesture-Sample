#pragma once
#include<string>
#include<opencv2\opencv.hpp>
using namespace std;
class DataSaverSimple
{
	string rootFolder;
	string camera;
	int frameCounter;
public:
	string curDir;
	bool saveDepth, saveColor;
	bool saveAsVideo;
	string depthDir;
	string colorDir;
	cv::VideoWriter depthWriter, colorWriter;
	DataSaverSimple() {}
	DataSaverSimple(const string& rootFolder, bool saveDepth = true, bool saveColor = true,bool saveAsVieo=true) :
		rootFolder(rootFolder), saveDepth(saveDepth), saveColor(saveColor), frameCounter(0),saveAsVideo(saveAsVideo)
	{}
	bool setAndCheck(string camera, int subjectId, string gesture, int cAngle, int cHeight, int lc, int rc);
	void start();
	void saveColorFrame(const cv::Mat& colorFrame);
	void saveDepthFrame(const cv::Mat& depthFrame, const cv::Mat& normDepth);
	void setRootFolder(const string& rootFolder)
	{
		this->rootFolder = rootFolder;
	}
	void SaveDepth(bool saveDepth)
	{
		this->saveDepth = saveDepth;
	}
	void SaveColor(bool saveColor)
	{
		this->saveColor = saveColor;
	}
	void step()
	{
		frameCounter++;
	}
	void stop();
	void deleteCurSaved();
};

