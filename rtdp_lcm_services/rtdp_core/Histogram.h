/*
 * Histogram.h
 *
 *  Created on: 23/01/2015
 *      Author: esther
 */

#ifndef HISTOGRAM_H_
#define HISTOGRAM_H_

#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

class Histogram {

public:
	static Mat grayHistogram(Mat src, bool display = false);
	static vector<Mat> rgbHistogram(Mat src, bool display = false);
	static vector<Mat> hsvHistogram(Mat src, bool display = false);
	static float entropy(Mat src, bool display = false);
	static int median(Mat src);
};

#endif /* HISTOGRAM_H_ */
