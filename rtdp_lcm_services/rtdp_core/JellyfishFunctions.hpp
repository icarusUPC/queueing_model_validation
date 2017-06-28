/*
 * functions.hpp
 *
 *  Created on: Jun 19, 2014
 *      Author: cristina
 */

#ifndef FUNCTIONS_HPP_
#define FUNCTIONS_HPP_

#include <list>
#include <iostream>
#include <fstream>
#include <math.h>
#include <assert.h>

#include <opencv2/core.hpp>
#include "ErosionDilation.hpp"
#include "HotSpot.h"
#include "HotSpotList.h"
#include "Utils.h"

using namespace cv;
using namespace std;

struct main_options{
	vector<string> inputNames;
	//default RBG thresholds
	unsigned  RMIN=120;
	unsigned  RMAX=255;
	unsigned  GMIN=0;
	unsigned  GMAX=55;
	unsigned  BMIN=100;
	unsigned  BMAX=255;
	unsigned  numThreads=1;
	unsigned  numbThreads=1;
	bool markJelly=true;
	bool patternMatching=false;
	string outputFormat="jpeg";
	string outputPath=".";
	string filterJelly="ellipse";
}; //main options

Mat color_separate (Mat a);
Mat color_normalize (Mat a);
Mat color_enhancement (Mat a, String);
Mat segmentation(Mat a, String s, Scalar rgb_min, Scalar rgb_max, int mask);
void discard_date(Mat& a);
HotSpotList* jellyfish_candidates(Mat a);
void calculateH_feature(Mat c, double H[]);
void show_mat(Mat m, int s);
int writeImage(Mat img, string name, string suffix, string format);
void check_candidates(Mat a, HotSpotList*  lis);
void paint_jellyfish(Mat& a, list<HotSpot*> jc);
string getBasename(string filename);
string getRawBasename(string filename);
string getExtension(string filename);
string getPath(string filename);

const int SIZE_NORM=64;
const int SIZE_TILE=18;
const int SIZE_BOUND=1512;

struct JellyModel{
	double alpha[3];
	int dimension[3];
	double threshold[3];
	double error[3];
	double boundary[SIZE_BOUND];
};

#endif /* FUNCTIONS_HPP_ */
