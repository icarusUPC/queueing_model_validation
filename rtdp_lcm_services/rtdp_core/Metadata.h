/*
 * Metadata.h
 *
 *  Created on: 04/07/2014
 *      Author: esther
 */

#ifndef METADATA_H_
#define METADATA_H_

#include <fstream>
#include <map>

#include <opencv2/core.hpp>

#include "XmlParser.h"
#include "Utils.h"

using namespace cv;
using namespace std;

class Metadata {

	Vec3d uavPosition; 		// Latitude (deg), Longitude (deg), altitude (m)
	Vec3d uavAngles;		// Yaw (deg), Pitch (deg), Roll (deg)

	friend ostream& operator<<(ostream& stream, const Metadata&);

public:

	Metadata();
    Metadata(double lat, double lon, double alt, double yaw, double pitch, double roll);

    Vec3d GetUavPosition() const;
    Vec3d GetUavAngles() const;

	int ReadXML(string xmlName);
};

class MetadataMap {
	map<string, Metadata*> metadataMap;

public:
	virtual ~MetadataMap();

	void SetMetadata(string id, Metadata *m);
	Metadata* GetMetadata(string id);

	int ReadTxt(string txtName);
};

#endif /* METADATA_H_ */
