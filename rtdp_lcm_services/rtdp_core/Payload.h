/*
 * UASParameters.h
 *
 *  Created on: 02/07/2014
 *      Author: icarus
 */

#ifndef PAYLOAD_H_
#define PAYLOAD_H_

#include <map>

#include <opencv2/core.hpp>

#include "XmlParser.h"
#include "Utils.h"

using namespace std;
using namespace cv;

class Camera {

	string cameraName;

	// Image parameters
	int imageWidth;			// Horizontal (image) resolution in pixels
	int imageHeight;		// Vertical (image) resolution in pixels

	// Internal parameters
	int sensorWidth;		// Horizontal (sensor) resolution in pixels
	int sensorHeight; 		// Vertical (sensor) resolution in pixels
	double pixelPitch; 		// Distance between pixels in the sensor (m)
	double focalLength;		// Focal length

	Mat intrinsicMatrix;
	Mat distortionCoeffs;

	// External parameters
	Vec3d camBODY;     	// Coordinates of the optical centre in the body frame
	Vec3d camAngles;    // Orientation: -pan/yaw/azimut: 0-to the nose, 90-to the right wing
						//   			-tilt/pitch/elevation: 0-vertical down, 90-horizontal
						//   			-roll

public:

    string GetName() const;

	int GetImageHeight() const;
	int GetImageWidth() const;

	int GetSensorHeight() const;
	int GetSensorWidth() const;
	double GetPixelPitch() const;
	double GetFocalLength() const;

	Mat GetIntrinsicMatrix() const;
	Mat GetDistortionCoeffs() const;

	Vec3d GetCamAngles() const;
	Vec3d GetCamBODY() const;

	int ReadXML(XMLNode xNode);
};

class GPS {

	Vec3d gpsBODY;     // Coordinates of the GPS in the body frame

public:
	Vec3d getGpsBODY() const;

	int readXML(XMLNode xNode);
};

class Payload {

	map<string, Camera> nameToCameraMap; // camera name -> camera parameters map
	GPS gps;

public:

	Camera* GetCamera(string cameraName);
	GPS* GetGps();

	int ReadXML(string xmlName);
};

#endif /* PAYLOAD_H_ */
