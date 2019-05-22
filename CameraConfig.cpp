#include "CameraConfig.h"
cv::Mat normDepth(const cv::Mat& depth,unsigned int& avg_dist, ushort minV, ushort maxV)
{
	cv::Mat colorDepth(depth.size(), CV_8U);
	unsigned int tol_dist = 0;
	unsigned int t = 1;
#pragma omp parallel for
	for (int i = 0; i < depth.rows; ++i)
	{
		const ushort* r_d = (depth.ptr<ushort>(i));
		uchar* d = (colorDepth.ptr<uchar>(i));
		for (int j = 0; j < depth.cols; ++j)
		{
			if (r_d[j] >= maxV || r_d[j] <= minV)
			{
				d[j] = 255;
			}
			else {
				tol_dist += r_d[j];
				t += 1;
				d[j] = 255.0 * (r_d[j] - minV) / (maxV - minV);
			}
		}
	}
	avg_dist = tol_dist / t;
	return colorDepth;
}