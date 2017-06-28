/*
 * DirectGeoref.h
 *
 *  Created on: 07/07/2014
 *      Author: icarus
 */

#ifndef DIRECTGEOREF_H_
#define DIRECTGEOREF_H_

#include <list>
#include <algorithm>

#include <opencv2/core.hpp>

#include "LNEDFrame.h"
#include "CameraFrame.h"
#include "Payload.h"
#include "Metadata.h"


using namespace cv;

#define MAX_ITERATIONS 10

class DirectGeoref
{
    CameraFrame camFrame;
    LNEDFrame nedFrame;

    Vec3d cameraNED;

	double GetDemAltitude(Vec3d lla);

public:
	DirectGeoref(Camera *camera, GPS *gps, Metadata *metadata);
	virtual ~DirectGeoref();

	Vec3d PixelGeolocation(Point p);
};


#endif /* DIRECTGEOREF_H_ */
