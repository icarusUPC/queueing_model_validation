/*
 * ImageProcessing.h
 *
 *  Created on: 17/07/2014
 *      Author: esther
 */

#ifndef GEOIMAGE_H_
#define GEOIMAGE_H_

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

#include <geotiff/geotiffio.h>
#include <geotiff/xtiffio.h>

#include "Image.h"
#include "Payload.h"
#include "Metadata.h"
#include "DirectGeoref.h"

using namespace std;
using namespace cv;

#define MAX_IMAGE_SIZE 50.0 * 1E6    // pixels
#define MIN_IMAGE_SIZE  5.0 * 1E3    // pixels

class GeoImage
{
	Image *image;
	Camera *camera;
	Metadata *metadata;
    DirectGeoref *directGeoref;

    Vec3d upperLeftLLA;
    Vec3d lowerLeftLLA;
    Vec3d upperRightLLA;
    Vec3d lowerRightLLA;
    Vec3d centerLLA;

    double latMin;
    double latMax;
    double lonMin;
    double lonMax;

    Mat warpedImage, warpedMask;

    double latPixelScale; // degrees/pixel
    double lonPixelScale; // degrees/pixel
    double imageSize; // megapixels

    bool fixedScale;
    bool fixedSize;

    Point LLAToPixel(Vec3d lla);
    Vec3d PixelToLLA(Point p);

    void CompensateLensDistortion();
    void GeoReference();
    void Rectify();

public:
    GeoImage(Image *image, Camera *camera, GPS *gps, Metadata *metadata);
    virtual ~GeoImage();

    Image* GetImage() const;
    Mat GetBaseImage() const;
    Mat GetWarpedImage() const;
    Mat GetWarpedMask() const;

    Vec3d GetCenterLLA() const;
    double GetLatMax() const;
    double GetLatMin() const;
    double GetLonMax() const;
    double GetLonMin() const;
    double GetLatPixelScale() const;
    double GetLonPixelScale() const;

    int FixSize(double megapixels);
    int FixMetScale(double metScale);
    int SetSize(double megapixels);
    int SetMetScale(double metScale);
    int SetDegScale(double lonScale, double latScale);
    int Scale(double xScale, double yScale);

    int Fusion(GeoImage *geo);
    int Add(GeoImage *geo);

    int WriteGeoTiff(string name);
};

#endif /* GEOIMAGE_H_ */
