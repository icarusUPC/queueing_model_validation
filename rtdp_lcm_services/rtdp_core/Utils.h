/*
 * Utils.h
 *
 *  Created on: 23/07/2014
 *      Author: esther
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <iostream>     // std::cout, std::fixed
#include <iomanip>      // std::setprecision
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <math.h>

#include "opencv2/core.hpp"

using namespace std;
using namespace cv;

// Error codes
enum error_code {
	NO_ERR = 0,
	LOAD_ERR,
	SAVE_ERR,
	MKDIR_ERR,
	RESIZE_ERR,
	SCALE_ERR,
	FUSION_ERR,
	STITCH_ERR,
	OPENCV_ERR
	};

// Math functions

#define MIN2(a,b) (((a) < (b)) ? (a) : (b))
#define MAX2(a,b) (((a) > (b)) ? (a) : (b))

#define MIN4(a, b, c, d) (MIN2((a), MIN2((b), MIN2((c), (d)))))
#define MAX4(a, b, c, d) (MAX2((a), MAX2((b), MAX2((c), (d)))))

#define RAD(a) ((a) * M_PI / 180.0)
#define DEG(a) ((a) * 180.0 / M_PI)

#define KELVIN(a) ((a) + 273.15)
#define CELSIUS(a) ((a) - 273.15)

// http://www.csgnetwork.com/degreelenllavcalc.html
// Alfes: LAT=41.549686, LON=0.651805,
// Length Of A Degree Of Latitude In Meters: 111064.51777928165
// Length Of A Degree Of Longitude In Meters: 83432.34510024716
#define M2LAT(a) ((a) / 111064.51777928165)
#define M2LON(a) ((a) / 83432.34510024716)
#define LAT2M(a) ((a) * 111064.51777928165)
#define LON2M(a) ((a) * 83432.34510024716)

// Coordinates functions

enum lla { LAT, LON, ALT };
enum ned { NORTH, EAST, DOWN };
enum xyz { X, Y, Z };
enum ypr { YAW, PITCH, ROLL };

double distance(Vec3d lla1, Vec3d lla2);
string LLAToString(Vec3d lla);

// Input/output functions

string getBasename(string filename);string getRawBasename(string filename);
string getExtension(string filename);
string getPath(string filename);
string getFormat(string filename);
string addSuffix(string filename, string suffix);
string createName(string path, string basename, string suffix, string format);
bool existsFile(string filename);
int readInputFolder(const string &inputFolder, vector<string>& inputNames);
int createFolder(const string &folder);
int createOutputFolder(const string &outputFolder);

// OpenCV related functions
string getImageType(int number);

#endif /* UTILS_H_ */
