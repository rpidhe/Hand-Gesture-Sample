#include "CamStreaming.h"
#include"utils.h"

cv::Mat catDepthColor(cv::Mat& depth, cv::Mat& color)
{
	cv::Mat cat;
	if (!depth.empty())
	{
		//cv::convertScaleAbs(depth, cat, 1.0 / 255, 0);
		cv::Mat gray;
		if (depth.channels() == 1) {
			cv::cvtColor(depth, gray, cv::COLOR_GRAY2BGR);
		}
		else {
			gray = depth;
		}
		if (!color.empty())
		{
			cv::hconcat(gray, color, gray);
		}
		return gray;
	}
	else {
		return color;
	}
}
void showFrame(cv::Mat&showMat, int fps,unsigned int avg_dist, string winName)
{
	string fps_s = string("FPS: ") + std::to_string((int)fps);
	string dis_s = string("Average depth: ") + std::to_string((int)avg_dist) + "mm";
	cv::putText(showMat, fps_s, cv::Point(20, 10), CV_FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar(0, 0, 255));
	cv::putText(showMat, dis_s, cv::Point(20, 50), CV_FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar(0,0, 255));
	cv::imshow(winName, showMat);
	cv::waitKey(1);
}

void CamStreaming::streamingFunc(CamStreaming* cs, bool show)
{
	cv::Mat depth, color,nDepth;
	unsigned int avg_dist = 0;
	auto start = cv::getTickCount();
	bool success = cs->camerConfig->getFrames(color, depth);
	nDepth = cs->camerConfig->normalizeDepth(depth, avg_dist);
	int t = 0;
	double fps = 0;
	if (success)
	{
		do
		{
			if (cs->flags.STREAMING_SAVING)
			{
				if(cs->dataSaver.saveColor)
					cs->dataSaver.saveColorFrame(color);
				if (cs->dataSaver.saveDepth)
					cs->dataSaver.saveDepthFrame(depth,nDepth);
				cs->dataSaver.step();
			}
			if(t % 5 == 0)
				 fps = cv::getTickFrequency() / (cv::getTickCount() - start);
			start = cv::getTickCount();
			if (show)
			{
				auto showMat = catDepthColor(nDepth, color);
				showFrame(showMat, fps, avg_dist - avg_dist % 10, cs->name);
			}
			success = cs->camerConfig->getFrames(color, depth);
			nDepth = cs->camerConfig->normalizeDepth(depth, avg_dist);
			t ++;
		} while (success && !cs->flags.STREAMING_TERMINATE);
	}
}
//void CamStreaming::colorStreaming(CamStreaming* cs, bool show)
//{
//	cv::VideoCapture v(0);
//	cv::Mat frame;
//	if (!v.isOpened())
//	{
//		printf("Camera is not available");
//	}
//	if(show)
//		cv::namedWindow("color");
//	auto start = cv::getTickCount();
//	bool success = v.read(frame);
//	if (success)
//	{
//		do
//		{
//			if (cs->flags.STREAMING_SAVING)
//			{
//				cs->dataSaver.saveColorFrame(frame);
//				cs->dataSaver.step();
//			}
//			double fps = cv::getTickFrequency() / (cv::getTickCount() - start);
//			start = cv::getTickCount();
//			if (show)
//			{
//				auto showMat = frame.clone();
//				showFrame(showMat, fps, "color");
//			}
//			success = v.read(frame);
//		} while (success && !cs->flags.STREAMING_TERMINATE);
//	}
//	if(show)
//		cv::destroyWindow("color");
//	v.release();
//	return;
//}

//void CamStreaming::RealsenseStreaming(CamStreaming* cs, bool show)
//{
//
//	auto start = cv::getTickCount();
//	while (!cs->flags.STREAMING_TERMINATE) // Application still alive?
//	{
//		// Using the align object, we block the application until a frameset is available
//
//		if (cs->flags.STREAMING_SAVING)
//		{
//			cs->dataSaver.saveColorFrame(colorMat);
//			cs->dataSaver.saveDepthFrame(depthMat);
//			cs->dataSaver.step();
//		}
//		int fps = cv::getTickFrequency() / (cv::getTickCount() - start);
//		start = cv::getTickCount();
//		if (show) {
//			auto showMat = catDepthColor(depthMat, colorMat);
//			showFrame(showMat, fps, "Realsense");
//		}
//	}
//}
void viewImages(const DataSaverSimple* dataSaver)
{
	vector<string> colorFrameNames;
	vector<string> depthFrameNames;
	if (dataSaver->saveColor)
	{
		listDirFiles(dataSaver->colorDir, colorFrameNames, ".jpg");
	}
	if (dataSaver->saveDepth)
	{
		listDirFiles(dataSaver->depthDir, depthFrameNames, ".png");
	}
	int nFrame = max(colorFrameNames.size(), depthFrameNames.size());
	cv::namedWindow("view saved");
	for (int i = 0; i < nFrame&&cv::waitKey(30)!=27; i++)
	{
		cv::Mat color, depth, show;
		if (dataSaver->saveColor)
		{
			color = cv::imread(dataSaver->colorDir + "/" + colorFrameNames[i]);
		}
		if (dataSaver->saveDepth)
		{
			depth = cv::imread(dataSaver->depthDir + "/" + depthFrameNames[i], CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH);
			//depth = cv::imread(dataSaver->depthDir + "/" + depthFrameNames[i],cv::IMREAD_UNCHANGED);
		}
		show = catDepthColor(depth, color);
		cv::imshow("view saved", show);
	}
	cv::destroyWindow("view saved");
}
void viewVideos(const DataSaverSimple* dataSaver)
{
	cv::namedWindow("view saved");
	cv::VideoCapture colorV(dataSaver->curDir + "/color.avi");
	cv::VideoCapture depthV(dataSaver->curDir + "/depth.avi");
	int nFrame = 0;
	if (dataSaver->saveColor)
	{
		int cFrames = colorV.get(CV_CAP_PROP_FRAME_COUNT);
		nFrame = cFrames;
	}
	if (dataSaver->saveDepth)
	{
		int dFrames = depthV.get(CV_CAP_PROP_FRAME_COUNT);
		nFrame = dFrames;
	}
	for (int i = 0; i < nFrame && cv::waitKey(30) != 27; i++)
	{
		cv::Mat color, depth, show;
		if (dataSaver->saveColor)
		{
			colorV >> color;
		}
		if (dataSaver->saveDepth)
		{
			depthV >> depth;
			//depth = cv::imread(dataSaver->depthDir + "/" + depthFrameNames[i],cv::IMREAD_UNCHANGED);
		}
		show = catDepthColor(depth, color);
		cv::imshow("view saved", show);
	}
	colorV.release();
	depthV.release();
	cv::destroyWindow("view saved");
}
void CamStreaming::viewFunc(const DataSaverSimple* dataSaver)
{
	if (!dataSaver->saveAsVideo) {
		viewImages(dataSaver);
	}
	else {
		viewVideos(dataSaver);
	}
}