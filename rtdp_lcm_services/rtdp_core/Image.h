/*
 * Image.h
 *
 *  Created on: 25/02/2015
 *      Author: esther
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/highgui.hpp"

#include "BlurMetric.h"
#include "Histogram.h"

using namespace std;
using namespace cv;

class Image {

protected:
	Mat baseImage;

public:
	Image();
	virtual ~Image();

	Size GetSize();
	int Width();
	int Height();

    Mat GetBaseImage() const;
    void SetBaseImage(Mat mat);

    float Blurring();
    float Entropy();

    void Resize(double megapixels);

    virtual int LoadImage(string filename);
	virtual int SaveImage(string filename);
};

#endif /* IMAGE_H_ */
