#include "DataSaverSimple.h"
#include <cstdio>
#include "utils.h"
bool DataSaverSimple::setAndCheck(string camera, int subjectId, string gesture, int cAngle, int cHeight, int lc, int rc)
{
	char curDir[256];
	sprintf(curDir, "%s/%s/Subject%03d/%s/A%d_H%d_R%d_L%d", rootFolder.c_str(), camera.c_str(), subjectId, gesture.c_str(), cAngle, cHeight, rc, lc);
	this->curDir = curDir;
	return !makedirs(curDir);
}

void DataSaverSimple::start()
{
	frameCounter = 0;
	if (saveDepth)
	{
		if (!saveAsVideo) {
			depthDir = this->curDir + "/depth";
			makedirs(depthDir);
		}
		else
		{
			depthWriter.open(this->curDir + "/depth.avi", CV_FOURCC('M', 'P', '4', '2'), 30, cv::Size(640, 480), false);
		}
	}
	if (saveColor) {
		if (!saveAsVideo)
		{
			colorDir = this->curDir + "/color";
			makedirs(colorDir);
		}
		else
		{
			colorWriter.open(this->curDir + "/color.avi", CV_FOURCC('M', 'P', '4', '2'), 30, cv::Size(640, 480), true);
		}
	}
}
void DataSaverSimple::saveDepthFrame(const cv::Mat& depthFrame,const cv::Mat& normDepth)
{
	if (!saveDepth)
		return;
	if (saveAsVideo)
	{
		depthWriter.write(normDepth);
	}
	else
	{
		char image_name[15];
		sprintf(image_name, "%04d.png", frameCounter);
		cv::imwrite(depthDir + "/" + image_name, depthFrame);
	}
}
void DataSaverSimple::saveColorFrame(const cv::Mat& colorFrame)
{
	if (!saveColor)
		return;
	if (saveAsVideo)
	{
		colorWriter.write(colorFrame);
	}
	else
	{
		char image_name[15];
		sprintf(image_name, "%04d.jpg", frameCounter);
		cv::imwrite(colorDir + "/" + image_name, colorFrame);
	}
}
void DataSaverSimple::deleteCurSaved()
{
	rmDirs(this->curDir, true);
}
void DataSaverSimple::stop()
{
	if (saveAsVideo)
	{
		if (saveColor)
		{
			colorWriter.release();
		}
		if (saveDepth)
		{
			depthWriter.release();
		}
	}
}
