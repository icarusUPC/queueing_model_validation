/*
 * BlurrMetric.h
 *
 *  Created on: 20/01/2015
 *      Author: esther
 */

#ifndef BLURMETRIC_H_
#define BLURMETRIC_H_

#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;

class BlurMetric {

public:
	// OpenCV port of 'LAPM' algorithm (Nayar89)
	static float modifiedLaplacian(const Mat& src);

	// OpenCV port of 'LAPV' algorithm (Pech2000)
	static float varianceOfLaplacian(const Mat& src);

	// OpenCV port of 'TENG' algorithm (Krotkov86)
	static float tenengrad(const Mat& src, int ksize = 3);

	// OpenCV port of 'GLVN' algorithm (Santos97)
	static float normalizedGraylevelVariance(const Mat& src);

	// Using Canny Edge detection
	static float cannySharpness(const Mat& src, int threshold = 0, bool display = false);

};

#endif /* BLURMETRIC_H_ */
