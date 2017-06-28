/*
 * LNEDFrame.h
 *
 *  Created on: 03/07/2014
 *      Author: icarus
 */

#ifndef LNEDFRAME_H_
#define LNEDFRAME_H_

#include <math.h>

#include <opencv2/core.hpp>

#include "Payload.h"
#include "Metadata.h"

using namespace cv;

// This class provides support for Coordinate frames transformations
//
// LLA:  Latidude-Longitude-Altidude
// ECEF: Earth-Centered Earth-Fixed, with X pointing to the 0ºlat 0ºlon, Y pointing to the 0ºlat 90ºlon, Z pointing to the north (90ºlat)
// LNED: North-East-Down with origin at the GPS antenna
// BODY: origin at the vehicle center of mass, X pointing to the nose, Y to the right wing, and Z to the belly
//
// Units are meters for distances and radians for angles

#define _a (6378137.0)                     // WGS84 semi-major earth axis (m)
#define _b (6356752.31424518)              // WGS84 semi-major earth axis (m)
#define _f ((_a - _b) / _a)                   // flattening
#define _e2 (_f * (2 - _f))                  // first eccentricity squared
#define _ep2 ((_a * _a - _b * _b) / (_b * _b))   // second eccentricity squared

class LNEDFrame {

    // Datum parameters for a WGS84 geodetic model
    double N;                                       // radius of curvature
    double localGeoidHeight;                // geoid height in the area to scan

    // Origin of the LocalNED frame
    Vec3d gpsLLA;
    Vec3d gpsECEF;
    Vec3d gpsBODY;

    // Aircraft navigation angles
    Vec3d uavAngles;

    // Rotation matrices
    Matx33d R_ECEF_LNED;    // ECEF to LNED
    Matx33d R_LNED_ECEF;    // LNED to ECEF
    Matx33d R_LNED_BODY;    // LNED to BODY
    Matx33d R_BODY_LNED;    // BODY to LNED

public:
	LNEDFrame(GPS *gpsParam, Metadata *metadata);
	virtual ~LNEDFrame();

    double GetLocalGeoidHeight() const;
    Vec3d GetGpsLLA() const;

	Vec3d LLAtoECEF(Vec3d LLA);
	Vec3d ECEFtoLLA(Vec3d ECEF);
	Vec3d ECEFtoLNED(Vec3d ECEF);
	Vec3d LNEDtoECEF(Vec3d LNED);
	Vec3d LLAtoLNED(Vec3d LLA);
	Vec3d LNEDtoLLA(Vec3d LNED);
	Vec3d LNEDtoBODY(Vec3d LNED);
	Vec3d BODYtoLNED(Vec3d BODY);
};

#endif /* LNEDFRAME_H_ */
