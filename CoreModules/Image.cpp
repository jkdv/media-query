//*****************************************************************************
//
// Image.cpp : Defines the class operations on images
//
// Author - Parag Havaldar
// Code used by students as starter code to display and modify images
//
//*****************************************************************************

#include "Image.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace cv;

// Constructor and Desctructors
Image::Image()
{
    m_width = -1;
    m_height = -1;
}

Image::Image(int w, int h, const string& path)
{
    m_width = w;
    m_height = h;
    m_imagePath = path;
	
}

Image::~Image()
{
}

// Copy constructor

Image::Image(const Image& rhs)
{
    m_height = rhs.m_height;
    m_width = rhs.m_width;
    m_colorMat = rhs.m_colorMat;
    m_grayMat = rhs.m_grayMat;
    m_imagePath = rhs.m_imagePath;
}

// = operator overload
Image& Image::operator= (const Image& rhs)
{
    m_height = rhs.m_height;
    m_width = rhs.m_width;
    m_colorMat = rhs.m_colorMat;
    m_grayMat = rhs.m_grayMat;
    m_imagePath = rhs.m_imagePath;
    return *this;
}

// Image::ReadImage
// Function to read the image given a path
bool Image::read()
{
    // Verify ImagePath
    if (m_imagePath.empty() || m_width < 0 || m_height < 0) {
        cerr << "Image or Image properties not defined" << endl;
        return false;
    }
    
    // Create a valid output file pointer
    ifstream ifs(m_imagePath, ifstream::binary);
    
    // get pointer to associated buffer object
    filebuf* pbuf = ifs.rdbuf();
    if (!pbuf->is_open()) {
        cerr << "Error Opening File for Reading" << endl;
        return false;
    }
    
    // Create and populate RGB buffers
    const size_t sz = m_width * m_height;
    vector<char> r_buf(sz);
    vector<char> g_buf(sz);
    vector<char> b_buf(sz);
    
    pbuf->sgetn(&r_buf.front(), sz);
    pbuf->sgetn(&g_buf.front(), sz);
    pbuf->sgetn(&b_buf.front(), sz);

    // close file stream
    ifs.close();
    
    // Allocate Data structure and copy
    // Do not delete!
    char* bgrData = new char[sz * 3];
//    float* grayData = new float[sz];
    for (size_t i = 0; i < sz; i++) {
        bgrData[3*i]	= b_buf[i];
        bgrData[3*i+1]	= g_buf[i];
        bgrData[3*i+2]	= r_buf[i];
//        grayData[i] = (0.299f * (unsigned int)(unsigned char)(r_buf[i]) +
//            0.587f * (unsigned int)(unsigned char)(g_buf[i]) +
//            0.114f * (unsigned int)(unsigned char)(b_buf[i])) / 255.0f;
    }
    
    m_colorMat = Mat(m_height, m_width, CV_8UC3, bgrData);
//    m_grayMat = Mat(m_height, m_width, CV_32FC1, grayData);
    
    return true;
}

bool Image::readGrayscale()
{
    if (m_imagePath.empty() || m_width < 0 || m_height < 0) {
        cerr << "Image or Image properties not defined" << endl;
        return false;
    }
    
    size_t sz = m_height * m_width;
    float* grayData = new float[sz];
    for (int i = 0; i < m_height; ++i) {
        for (int j = 0; j < m_width; ++j) {
            Vec3b color = m_colorMat.at<Vec3b>(i, j);
            grayData[i * m_width + j] =
            (0.299f * (unsigned int) color[2] +
             0.587f * (unsigned int) color[1] +
             0.114f * (unsigned int) color[0]) / 255.0f;
        }
    }
    
    m_grayMat = Mat(m_height, m_width, CV_32FC1, grayData);
    return true;
}

/* Histogram Equalization */
void Image::equalize(){
cv::Mat result;
cv::equalizeHist(this->m_colorMat, result);
this->m_colorMat = result;
}

void Image::setColorReduction(int factor){
	div = factor;
}
int Image::getColorReduction(){
	return div;
}
/* get Histogram */
MatND Image::getHistogram(int factor){
	//this->equalize();
	/*setColorReduction(factor);
	Mat imageMtx = hist.colorReduce(m_colorMat, div);*/

	int imgCount = 1;
	int dims = 3;
	const int sizes[] = { 30, 30, 30 };
	const int channels[] = { 0, 1, 2 };
	float rRange[] = { 0, 256 };
	float gRange[] = { 0, 256 };
	float bRange[] = { 0, 256 };
	const float *ranges[] = { bRange, gRange, rRange };
	Mat mask = Mat();
	Mat mData = this->m_colorMat;
	MatND histogram;

	calcHist(&mData, imgCount, channels, cv::Mat(), histogram, 2, sizes, ranges, true, false);
	return histogram;
}

