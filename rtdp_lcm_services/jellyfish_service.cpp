//============================================================================
// Name        : jellyfish_service.cpp
// Author      : CBM adapted from Javier Fuentes
// Version     : 1.0
// Copyright   : ICARUS-UPC
// Description : LCM service to detect jellyfish in an image
//============================================================================
// Input message:
//	int64_t command_id;
//	string photo_file;
//
// Output message:
// 	int64_t command_id;
// 	byte result;
// 	int32_t jellyfish_count;
// 	jellyfish_t jellyfish_list[jellyfish_count];
//============================================================================


#include <stdio.h>
#include <fstream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <iostream>
#include <list>
#include <lcm/lcm-cpp.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "rtdp_core/JellyfishFunctions.hpp"
#include "rtdp_core/HotSpot.h"
#include "rtdp_core/HotSpotList.h"
#include "cpp_messages/jellyfish_cmd_t.hpp"
#include "cpp_messages/jellyfish_ret_t.hpp"
#include "cpp_messages/hotspot_t.hpp"

#if !defined(LCM_NETWORK)
	#define LCM_NETWORK "udpm://224.0.0.1:7667?ttl=1"
#endif

using namespace std;
using namespace std::chrono; //timing


/////////////////////////////////////////////////////
// GLOBALS
//default RBG thresholds
unsigned  RMIN=120;
unsigned  RMAX=255;
unsigned  GMIN=0;
unsigned  GMAX=55;
unsigned  BMIN=100;
unsigned  BMAX=255;
bool markJelly=true;
bool patternMatching=false;
string outputFormat="jpeg";
string outputPath=".";
string filterJelly="ellipse";

//////////////////////////////////////////////////////


class JellyfishService 
{
    private:
	lcm::LCM* lcmp;

	std::ofstream time_log;
	high_resolution_clock::time_point time_begin, time_end;
	int n;

    public:
	JellyfishService(lcm::LCM* lcm) 
	{
		lcmp = lcm;
                 
		// Create output folder (if needed)
		if (outputPath != ".") if (createOutputFolder(outputPath)!=0)
		{
			cout << "Can't create output folder " << outputPath << endl;
			return;
		}

    		time_log.open(outputPath+"/jellyfish_time_log.txt", std::ofstream::out | std::ofstream::app);
		time_log << "Jellyfish execution time in seconds (" << getNumThreads() << " threads)"<< endl;
		time_log << fixed << setprecision(9);
		n = 1;
 	}

        ~JellyfishService() 
	{
	    	time_log.close();
	}

        void handleMessage(const lcm::ReceiveBuffer* rbuf,
                const std::string& chan, 
                const jellyfish_cmd_t* msg)
        {
		//cout << "Received message on channel " << chan << endl;

		jellyfish_ret_t rmsg;
		rmsg.command_id = msg->command_id;
		rmsg.result = NO_ERR;
		rmsg.jellyfish_count = 0;
                rmsg.jellyfish_list.resize(0);

		try
		{

		// timing
     		time_begin = high_resolution_clock::now();

                string imName = msg->photo_file;
		string name = getRawBasename(imName);

		// Load image
		cout << "\t...loading image " << imName << endl;
		Mat image;
	  	image=imread(imName, 1);
	  	//CV_Assert(!image.empty());
        	if (image.empty())
		{
			cout << "\t...!!!error reading image" << endl;
			rmsg.result = LOAD_ERR;
			lcmp->publish("jellyfish_ret", &rmsg);
			return;
		}

		// processing the image
		// 1-color enhancing
		cout << "\t...color enhancing" << endl;
	  	Mat image1(image.rows, image.cols, CV_8UC3);
	  	image1=color_enhancement(image, name);
		/*namedWindow( "Jellyfish Processed Image 1", CV_WINDOW_AUTOSIZE );
	  	imshow( "Jellyfish Processed Image 1", image1 );*/
	  	/*imwrite( "my"+name+"_f2_after_color.jpg", image1 );*/

		// 2-Segmentation
		cout << "\t...segmentation" << endl;
		Mat image2;
	  	image2=segmentation(image1, name, Scalar(BMIN, GMIN, RMIN), Scalar(BMAX,GMAX,RMAX), filterJelly=="ellipse");
	  	/*namedWindow( "Jellyfish Processed Image 2", CV_WINDOW_AUTOSIZE );
	  	imshow( "Jellyfish Processed Image 2", image2 );
	  	imwrite( "my"+name+"_f2_segments.jpg", image2 );*/

		//3-Detection
	  	cout << "\t...searching candidates ..." ;
	  	HotSpotList* jc = jellyfish_candidates(image2);
	  	cout << "\tnumber of jelly candidates is " << jc->Size() << endl;

		//4-Classification [Optional]
	  	if (patternMatching){
		  cout << "\t...checking of candidates ..." ;
		  check_candidates(image, jc);
		  cout << "\tnumber of jelly reduced to " << jc->Size() << endl;
	  	}

		//5-Output [Optional]
		if (markJelly)
		{
			paint_jellyfish(image, jc->HotSpots());
			writeImage(image, outputPath+"/"+name, "JF", outputFormat );
			//namedWindow( "Jellyfish search", CV_WINDOW_AUTOSIZE );
			//imshow( "Jellyfish search", image );
		}

		//return LCM message with results
		if (jc->Size()>0){
		  //paint_jellyfish(image, jc->HotSpots());
                  cout << "\t...jellyfish found!" << endl;
		  rmsg.jellyfish_count=jc->Size();
		  rmsg.jellyfish_list.resize(rmsg.jellyfish_count);
		  list<HotSpot*> hslist = jc->HotSpots();
		  list<HotSpot*>::iterator it;
		  int i = 0;
		  for (it=hslist.begin(); it != hslist.end(); i++,it++){
			HotSpot * hs=*it;
			rmsg.jellyfish_list[i].jellyfish_id = hs->Id();	 // id
			rmsg.jellyfish_list[i].num_pixels = hs->Size();	 // #pixels
			rmsg.jellyfish_list[i].center[0] = hs->Center().y; // row
			rmsg.jellyfish_list[i].center[1] = hs->Center().x; // column
			Rect bb=hs->BoundingBox();
			rmsg.jellyfish_list[i].bounding[0] = bb.height;	// #rows
			rmsg.jellyfish_list[i].bounding[1] = bb.width;	// #cols
			rmsg.jellyfish_list[i].bounding[2] = bb.y;		// upper_row
			rmsg.jellyfish_list[i].bounding[3] = bb.x; 	// leftmost_col			
		  }
		}
		delete jc;

		// timing     		
		time_end = high_resolution_clock::now();
		double time_span = duration_cast<duration<double>>(time_end - time_begin).count();
		time_log << msg->command_id << " \t";
		time_log << time_span << " seconds \t";
		time_log << getNumThreads() << " threads"<< endl;

		}
		catch( cv::Exception& e )
		{
		    	const char* err_msg = e.what();
		    	cout << "\texception caught: " << err_msg << endl;
			rmsg.result = OPENCV_ERR;
		}

		//cout << "Sending message jellyfish RET (id=" << rmsg.command_id << ")" << endl;
		lcmp->publish("jellyfish_ret", &rmsg);
        }
};

void printUsage()
	{
	    cout << endl;
	    cout << "Jellyfish - Detects Jellyfish in a visual image" << endl;
	    cout << endl;
	    cout << "Usage: jelly [Options]" << endl;
	    cout << endl;
	    cout << "Options:" << endl;
	    cout << endl;
	    cout << "  -t values ..., --threshold values ..." << endl;
	    cout << "      range of RBG values: Rmin-Rmax Gmin-Gmax Bmin-Bmax (default is ";
	    cout << RMIN << "-" << RMAX << ", " << GMIN << "-" << GMAX << ", " << BMIN << "-" << BMAX << ")" << endl;
	    cout << endl;
	    cout << "  -m (yes|no), --mark (yes|no)" << endl;
	    cout << "      mark jellyfish in the image (default is " << (markJelly? "yes": "no") << ")" << endl;
	    cout << endl;
	    cout << "  -f (jpeg|png|bmp|tiff), --output_format (jpeg|png|bmp|tiff)" << endl;
	    cout << "      converted image output format (default is " << outputFormat << ")" << endl;
	    cout << endl;
	    cout << "  -o path, --output_path path" << endl;
	    cout << "      output path (default is .)" << endl;
	    cout << endl;
            cout << "  -n num_threads, --num_threads #" << endl;
	    cout << "      number of parallel (default is 8)" << endl;
	    cout << endl;
	    cout << "  -k (disk|ellipse), --kernel_filter (disk|ellipse)" << endl;
	    cout << "      set erode/dilate kernel filter (default is " << filterJelly << ")" << endl;
	    cout << endl;
	    cout << "  -r , --pattern_matching " << endl;
	    cout << "      to activate pattern matching filtering (default is no pattern matching)" << endl;
	    cout << endl;
	    cout << "  -h ,--help" << endl;
	    cout << "      print usage" << endl;
	    cout << endl;
}

// Parse command line arguments
int parseArguments(int argc, char* argv[])
{
	if (argc == 1)
	{
	    return 0;
	}

	for (int i = 1; i < argc; i++)
	{
	    if (string(argv[i]) == "--help" || string(argv[i]) == "-h" || string(argv[i]) == "/?")
	    {
	        return -1;
	    }
	    else if (string(argv[i]) == "--threshold" || string(argv[i]) == "-t")
	    {
	    	RMIN = atof(argv[i+1]); RMAX = atof(argv[i+2]);
	    	GMIN = atof(argv[i+3]); GMAX = atof(argv[i+4]);
	    	BMIN = atof(argv[i+5]); BMAX = atof(argv[i+6]);
	    	i+=6;
	    }
	    else if (string(argv[i]) == "--mark" || string(argv[i]) == "-m")
	    {
	    	if (string(argv[i+1]) == "yes")
	    	{
	    		markJelly = true;
	    	}
	    	else if (string(argv[i+1]) == "no")
	    	{
	    		markJelly = false;
	    	}
	    	else
	    	{
	    	    return -1;
	    	}
	    	i++;
	    }
	    else if (string(argv[i]) == "--output_format" || string(argv[i]) == "-f")
	    {
	    	outputFormat = argv[i+1];
	    	i++;
	    	if (outputFormat != "jpeg" && outputFormat != "bmp" && outputFormat != "png" && outputFormat != "tiff")
	    	{
	    	    return -1;
	    	}
	    }
            else if (string(argv[i]) == "-n" || string(argv[i]) == "--num_threads")
	    {
			setNumThreads(atoi(argv[i+1]));
			i++;
	    }
	    else if (string(argv[i]) == "--output_path" || string(argv[i]) == "-o")
	    {
	    	outputPath = argv[i+1];
	    	i++;
	    }
	    else if (string(argv[i]) == "--kernel_filter" || string(argv[i]) == "-k")
	   	{
	   	    	filterJelly = argv[i+1];
	   	    	i++;
	   	    	if (filterJelly != "disk" && filterJelly != "ellipse")
	   	    	{
	   	    	    return -1;
	   	    	}
	   	}
	    else if (string(argv[i]) == "--pattern_matching" || string(argv[i]) == "-r")
		{
	    	patternMatching = true;
		}
        else
        {
        	if (argv[i][0] == '-')
	    	{
	    	    return -1;
	    	}
        }
    }

    return 0;
}

int main(int argc, char** argv)
{

    // Parse command line arguments
    if (parseArguments(argc, argv) != 0)
    {
	printUsage();
	exit (-1);
    }
    
    lcm::LCM lcm = lcm::LCM(LCM_NETWORK);

    if(!lcm.good())
        return 1;
	
    cout << "Jellyfish Service over LCM network on" << endl;

    JellyfishService jellyfishService(&lcm);
    lcm.subscribe("jellyfish_cmd", &JellyfishService::handleMessage, &jellyfishService);

    while(0 == lcm.handle());

    cout << "Stopping Jellyfish Service!!!" << endl;
    return 0;
}
