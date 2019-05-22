#pragma once
#include<opencv2\opencv.hpp>
#include <librealsense2/rs.hpp>
#include <OpenNI.h>
cv::Mat normDepth(const cv::Mat& depth, unsigned int& avg_dist, ushort minV = 400, ushort maxV = 1200);
class CameraConfig
{
protected:
	ushort maxDist, minDist;
public:
	CameraConfig(ushort minDist,ushort maxDist):minDist(minDist),maxDist(maxDist) {

	}
	virtual bool getFrames(cv::Mat& color, cv::Mat& depth) = 0;
	virtual cv::Mat normalizeDepth(const cv::Mat& depth, unsigned int& avg_dist)
	{
		return normDepth(depth, avg_dist, minDist, maxDist);
	}
	virtual void exit()
	{

	}
	virtual ~CameraConfig(){}
};

class RealsenseConfig: public CameraConfig
{
	rs2::pipeline pipe;
public:
	RealsenseConfig(ushort minDist = 300, ushort maxDist = 800) :CameraConfig(minDist, maxDist)
	{
		rs2::config cfg;
		cfg.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 30);
		cfg.enable_stream(RS2_STREAM_COLOR, RS2_FORMAT_RGB8);
		pipe.start(cfg);
	}
	bool getFrames(cv::Mat& color, cv::Mat& depth) override
	{
		rs2::frameset frameset = pipe.wait_for_frames();
		auto depth_r = frameset.get_depth_frame();
		auto color_r = frameset.get_color_frame();
		depth = RealsenseConfig::realsense2Mat(depth_r, false);
		color = RealsenseConfig::realsense2Mat(color_r, true);
		return true;
	}
	static cv::Mat realsense2Mat(rs2::frame& frame, bool color)
	{
		const int w = frame.as<rs2::video_frame>().get_width();
		const int h = frame.as<rs2::video_frame>().get_height();
		if (color)
		{
			cv::Mat image(cv::Size(w, h), CV_8UC3, (void*)frame.get_data(), cv::Mat::AUTO_STEP);
			cvtColor(image, image, cv::COLOR_RGB2BGR);
			return image;
		}
		else {
			cv::Mat image(cv::Size(w, h), CV_16U, (void*)frame.get_data(), cv::Mat::AUTO_STEP);
			return image;
		}
	}
	~RealsenseConfig()
	{
		pipe.stop();
	}
};
class ColorConfig : public CameraConfig
{
	cv::VideoCapture v;
public:
	ColorConfig() :CameraConfig(0, 0),v(cv::VideoCapture(0))
	{
	}
	bool getFrames(cv::Mat& color, cv::Mat& depth) override
	{
		return v.read(color);
	}
	~ColorConfig()
	{
		v.release();
	}
};
class ObbecConfig : public CameraConfig
{
	openni::VideoStream streamDepth;
	openni::Device device;
public:
	ObbecConfig(ushort minDist = 600, ushort maxDist = 1200) :CameraConfig(minDist, maxDist)
	{
		openni::Status rc = openni::OpenNI::initialize();
		rc = device.open(openni::ANY_DEVICE);
		if (device.getSensorInfo(openni::SENSOR_DEPTH) != NULL)
		{
			rc = streamDepth.create(device, openni::SENSOR_DEPTH);
			if (rc != openni::STATUS_OK)
			{
				MessageBoxW(0, L"Couldn't create depth stream!", L"Excption", MB_ICONEXCLAMATION | MB_OK);
			}
		}
		rc = streamDepth.start();
	}
	bool getFrames(cv::Mat& color, cv::Mat& depth) override
	{

		openni::VideoFrameRef  frameDepth;
		openni::Status rc = streamDepth.readFrame(&frameDepth);
		if (rc != openni::STATUS_OK)
		{
			return false;
		}
		depth = cv::Mat(frameDepth.getHeight(), frameDepth.getWidth(), CV_16UC1, (void*)frameDepth.getData());
		return true;
	}
	~ObbecConfig()
	{
		streamDepth.destroy();
		//streamColor.destroy();
		// 关闭设备
		device.close();
		// 最后关闭OpenNI
		openni::OpenNI::shutdown();
	}
};
class KinectConfig : public CameraConfig
{
	openni::VideoStream streamDepth;
	openni::VideoStream streamColor;
	openni::Device device;
public:
	KinectConfig(ushort minDist = 600, ushort maxDist = 1200) :CameraConfig(minDist, maxDist)
	{
		openni::Status rc = openni::OpenNI::initialize();
		rc = device.open(openni::ANY_DEVICE);
		if (device.getSensorInfo(openni::SENSOR_DEPTH) != NULL)
		{
			rc = streamDepth.create(device, openni::SENSOR_DEPTH);
			if (rc != openni::STATUS_OK)
			{
				MessageBoxW(0, L"Couldn't create depth stream!", L"Excption", MB_ICONEXCLAMATION | MB_OK);
			}
		}
		if (device.getSensorInfo(openni::SENSOR_COLOR) != NULL)
		{
			rc = streamColor.create(device, openni::SENSOR_COLOR);
			if (rc != openni::STATUS_OK)
			{
				MessageBoxW(0, L"Couldn't create color stream!", L"Excption", MB_ICONEXCLAMATION | MB_OK);
			}
		}
		rc = streamDepth.start();
		if (rc != openni::STATUS_OK)
		{
			MessageBoxW(0, L"Couldn't create color stream!", L"Excption", MB_ICONEXCLAMATION | MB_OK);
			return;
		}
		rc = streamColor.start();
	}
	bool getFrames(cv::Mat& color, cv::Mat& depth) override
	{

		openni::VideoFrameRef  frameDepth;
		openni::Status rc = streamDepth.readFrame(&frameDepth);
		if (rc != openni::STATUS_OK)
		{
			return false;
		}
		depth = cv::Mat(frameDepth.getHeight(), frameDepth.getWidth(), CV_16UC1, (void*)frameDepth.getData());
		openni::VideoFrameRef  frameColor;
		rc = streamColor.readFrame(&frameColor);
		if (rc != openni::STATUS_OK)
		{
			return false;
		}
		color = cv::Mat(frameColor.getHeight(), frameColor.getWidth(), CV_8UC3, (void*)frameColor.getData());
		cv::cvtColor(color, color, CV_RGB2BGR);
		return true;
	}
	~KinectConfig()
	{
		streamDepth.destroy();
		streamColor.destroy();
		//streamColor.destroy();
		// 关闭设备
		device.close();
		// 最后关闭OpenNI
		openni::OpenNI::shutdown();
	}
};