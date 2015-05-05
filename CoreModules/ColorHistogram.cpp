#include "ColorHistogram.h"


ColorHistogram::ColorHistogram(){
	// arguments for setting color histogram
	histSize[0] = histSize[1] = histSize[2] = 256;
	hranges[0] = 0.0;	//range for BGR
	hranges[1] = 255.0;
	ranges[0] = ranges[1] = ranges[2] = hranges;	//same range for all channels
	channels[0] = 0;
	channels[1] = 1;
	channels[2] = 2;
}
/* get histogram method */
cv::MatND ColorHistogram::getHistogram(const cv::Mat &image){
	cv::MatND hist;
	//calculate histogram
	cv::calcHist(&image,
		1,				// single image histogram
		channels,		// target channel
		cv::Mat(),		// without mask
		hist,			// result histogram
		3,				// 3-demensional histogram
		histSize,		// frequency
		ranges			// range for picture element
		);
	return hist;
}
cv::SparseMat ColorHistogram::getSparseHistogram(const cv::Mat &image){
	cv::SparseMat hist(3, histSize, CV_32F);
	//calculate histogram
	cv::calcHist(&image,
		1,				// single image histogram
		channels,		// target channel
		cv::Mat(),		// without mask
		hist,			// result histogram
		3,				// 3-demensional histogram
		histSize,		// frequency
		ranges			// range for picture element
		);
	return hist;
}


/* 1-dimensional histogram with mask, BGR->HSV */
/*cv::MatND ColorHistogram::getHueHistogram(const cv::Mat &image, int minSaturation = 0){
MatND hist;
// BGR -> HSV
Mat hsv;
cvtColor(image, hsv, CV_BGR2HSV);
// mask
Mat mask;
if (minSaturation > 0){
// divide 3 channels to 3 images
std::vector<cv::Mat> v;
cv::split(hsv, v);
// exclude low picture element
cv::threshold(v[1], mask, minSaturation, 255, cv::THRESH_BINARY);
}
//1-Dimensional color histogram arguments
hranges[0] = 0.0;
hranges[1] = 180.0;
channels[0] = 0;	// color channel
//calculate histogram
cv::calcHist(&hsv,
1,				// single image histogram
channels,		// target channel
mask,			// binary mask
hist,			// result histogram
1,				// 1-demensional histogram
histSize,		// frequency
ranges			// range for picture element
);
return hist;
}*/

cv::Mat& ColorHistogram::colorReduce(cv::Mat &image, int div){
	// rows and cols for the image
	int nl = image.rows;
	int nc = image.cols;

	//continuous??
	if (image.isContinuous()){
		nc = nc * nl;
		nl = 1;	//1-Dimensional array
	}
	int n = static_cast<int>(log(static_cast<double>(div)) / log(2.0));
	// mask for rounding off picture element
	uchar mask = 0xFF << n;		// div=16, mask = 0xF0
	// for every picture element
	for (int j = 0; j < nl; j++){
		uchar* data = image.ptr<uchar>(j);
		for (int i = 0; i < nc; i++){
			*data++ = *data&mask + div / 2;
			*data++ = *data&mask + div / 2;
			*data++ = *data&mask + div / 2;
		}//for each col
	}
	return image;
}