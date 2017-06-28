/*
 * HotSpot.h
 *
 *  Created on: 17/06/2014
 *      Author: esther
 */

#ifndef HOTSPOT_H_
#define HOTSPOT_H_

#include <iostream>
#include <iomanip>
#include <list>

#include <opencv2/core.hpp>

using namespace std;
using namespace cv;

class HotSpot
{
    static int next_id;

    int id;

    // Information computed from the IR image by the SEGMENTATION module

    list<Point> pixels;  // Pixels in the hotspot

    Rect boundingBox;	// Bounding box
    Point center;      // Center of mass

    float max_t;		// Max temperature
    float sum_t;
    float sum_xt;
    float sum_yt;

    // float magnitude;	// Hot spot magnitude (as defined in JACIC2013)

	friend ostream& operator<<(ostream& stream, const HotSpot&);

public:
	HotSpot(int column, int row, float temp);

	void AddPixel(int column, int row, float temp);
	void MergeHotSpot(HotSpot* h);
	bool Intersects(HotSpot* h);

	int Id() const { return id; }
    int Size() const { return pixels.size(); }
    Point Center() const { return center; }
    Rect BoundingBox() const { return boundingBox; }
    float MaxTemp() const { return max_t; } // kelvin
    float AvgTemp() const { return sum_t/pixels.size(); } // Kelvin
};

#endif /* HOTSPOT_H_ */
