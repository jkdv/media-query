#pragma once
#ifndef __COLOR__HISTOGRAM__
#define __COLOR__HISTOGRAM__

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
class ColorHistogram
{
private:
	int histSize[3];
	float hranges[2];
	const float* ranges[3];
	int channels[3];
public:
	ColorHistogram();
	/* get histogram method */
	cv::MatND getHistogram(const cv::Mat &image);
	cv::SparseMat getSparseHistogram(const cv::Mat &image);
	/* 1-dimensional histogram with mask, BGR->HSV */
	/*cv::MatND getHueHistogram(const cv::Mat &image, int minSaturation = 0);*/

	cv::Mat& colorReduce(cv::Mat &image, int div);
};
#endif
