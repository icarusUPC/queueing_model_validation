/*
 * CameraFrame.h
 *
 *  Created on: 26/06/2014
 *      Author: esther
 */

#ifndef CAMERAFRAME_H_
#define CAMERAFRAME_H_

#include <math.h>

#include <opencv2/core.hpp>

#include "Payload.h"

using namespace cv;

class CameraFrame {
    Vec3d camBODY;  	  // Coordinates of the optical centre in the body frame
	Vec3d camAngles;      // Orientation (degrees respect to the body frame)
						  //   -pan/yaw/azimut: 0-to the nose, 90-to the right wing
						  //   -tilt/pitch/elevation: 0-vertical down, 90-horizontal
						  //   -roll

    double scaleX;        // Vertical meters/pixel relation in the image plane
    double scaleY;        // Horizontal meters/pixel relation in the image plane (usually equal to scaleX)
    double offsetX;       // Vertical offset (m) of the center of the upper-left pixel to the center in the image plane
    double offsetY;       // Horizontal offset (m) of the upper-left corner to the center in the image plane
    double focalLength;   // Focal length

    // Rotation matrices
    Matx33d R_BODY_CAM;
    Matx33d R_CAM_BODY;

public:
	CameraFrame(Camera *camera);
	virtual ~CameraFrame();

    Vec3d GetCamBODY() const;

	Vec3d BODYtoCAM(Vec3d BODY);
	Vec3d CAMtoBODY(Vec3d CAM);
	Vec3d PIXELtoCAM(Point p);
	Vec3d PIXELtoBODY(Point p);

};

#endif /* CAMERAFRAME_H_ */
