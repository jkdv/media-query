//*****************************************************************************
//
// Image.h : Defines the class operations on images
//
// Author - Parag Havaldar
// Main Image class structure 
//
//*****************************************************************************

#ifndef __fuzzy_tribble__Image__
#define __fuzzy_tribble__Image__

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "ColorHistogram.h"


using namespace std;
using namespace cv;

// Class structure of Image
// Use to encapsulate an RGB image
class Image
{
private:
	int		m_width;		// Width of Image
	int		m_height;		// Height of Image
	string	m_imagePath;    // Image location
	Mat     m_colorMat;     // RGB Color image
    Mat     m_grayMat;      // grayscale data of the image


	ColorHistogram hist;
	int div;
	
	

public:
	// Constructor
	Image();
  	Image(int w, int h, const string& path);
	// Copy Constructor
	Image(const Image& rhs);
	// Destructor
	~Image();

	// operator overload
	Image& operator=(const Image & rhs);

	// Reader & Writer functions
	void	setWidth(const int w)  { m_width = w; }
	void	setHeight(const int h) { m_height = h; }
    void	setImagePath(const string& path) { m_imagePath = string(path); }
	int		getWidth() const { return m_width; }
	int		getHeight() const { return m_height; }
	Mat&    data() { return m_colorMat; }
    Mat&    grayscale() { return m_grayMat; }
	const string path() const { return m_imagePath.c_str(); }
	bool	isValud() const {
		return !m_imagePath.empty() && m_width > 0 && m_height > 0;
	}

	// Input Output operations
	bool	read();
    bool    readGrayscale();
    void    releaseGrayscale() { m_grayMat.release(); }
	
	void	setColorReduction(int factor);
	int		getColorReduction();
	
	/* Global Histogram Equalization */
	void    equalize();

	/* get Histogram */
	MatND getHistogram(int factor);
};

#endif /* defined(__fuzzy_tribble__Image__) */
