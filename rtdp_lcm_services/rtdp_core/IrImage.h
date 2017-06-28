/*
 * IRImage.h
 *
 *  Created on: 09/06/2014
 *      Author: icarus
 *
 *  This file defines the FLIR Public Format image structures
 *
 */

#ifndef FPFIMAGE_H_
#define FPFIMAGE_H_

#include <cstdio>
#include <fstream>
#include <list>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "Image.h"
#include "FPFHeader.h"
#include "HotSpot.h"
#include "HotSpotList.h"
#include "Utils.h"

using namespace std;
using namespace cv;

class IrImage: public Image
{

private:

	Mat grayImage;

    float rangeTmin;        // min image range temp
    float rangeTmax;        // max image range temp

    float dataTmin;        	// min existent fpf temp in data
    float dataTmax;        	// max existent fpf temp in data

    float gray2temp(unsigned char gray);
    unsigned char temp2gray(float temp);

    int ReadFPFImage(string filename);

public:
	IrImage();
	virtual ~IrImage();

	int LoadImage(string filename);

	HotSpotList* FindHotSpots(float thresholdTemp);
	void DrawHotSpots(HotSpotList* hotSpots);

	float DataTmax() const { return dataTmax; }
	float DataTmin() const { return dataTmin; }
};

#endif /* FPFIMAGE_H_ */
