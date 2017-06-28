/*
 * ErosionDilation.hpp
 *
 *  Created on: Oct 20, 2014
 *      Author: cristina
 */

#ifndef EROSIONDILATION_HPP_
#define EROSIONDILATION_HPP_

#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;

void Erosion(Mat src, Mat & erosion_dst, int mask);
void Dilation (Mat src, Mat & dilation_dst, int mask);


#endif /* EROSIONDILATION_HPP_ */
