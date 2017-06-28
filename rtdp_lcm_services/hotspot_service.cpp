//============================================================================
// Name        : hotspot_service.cpp
// Author      : ESS
// Version     : 1.0
// Copyright   : ICARUS-UPC
// Description : LCM service to detect hotspost in a thermal image
//============================================================================
// Input message:
//	int64_t command_id;
//	string photo_file;
//
// Output message:
// 	int64_t command_id;
// 	byte result;
// 	int32_t hotspot_count;
// 	hotspot_t hotspot_list[hotspot_count];
//============================================================================


#include <stdio.h>
#include <fstream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <lcm/lcm-cpp.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "rtdp_core/Image.h"
#include "rtdp_core/IrImage.h"
#include "rtdp_core/HotSpot.h"
#include "rtdp_core/HotSpotList.h"
#include "rtdp_core/Utils.h"
#include "cpp_messages/hotspot_cmd_t.hpp"
#include "cpp_messages/hotspot_ret_t.hpp"
#include "cpp_messages/hotspot_t.hpp"

#if !defined(LCM_NETWORK)
	#define LCM_NETWORK "udpm://224.0.0.1:7667?ttl=1"
#endif

using namespace std;
using namespace std::chrono; //timing


/////////////////////////////////////////////////////
// COMMAND LINE ARGUMENTS - DEFAULT VALUES

float thresholdTemp = 333;     // kelvin (333K=60C)

//////////////////////////////////////////////////////


class HotspotService 
{
    private:
	lcm::LCM* lcmp;

	std::ofstream time_log;
	high_resolution_clock::time_point time_begin, time_end;
	int n;

    public:
	HotspotService(lcm::LCM* lcm) 
	{
		lcmp = lcm;
    		time_log.open("hotspot_time_log.txt");
		time_log << "hotspot execution time in seconds (" << getNumThreads() << " threads)"<< endl;
		time_log << fixed << setprecision(6);
		n = 1;
 	}

        ~HotspotService() 
	{
	    	time_log.close();
	}

        void handleMessage(const lcm::ReceiveBuffer* rbuf,
                const std::string& chan, 
                const hotspot_cmd_t* msg)
        {
		cout << "Received message on channel " << chan << endl;

		hotspot_ret_t rmsg;
		rmsg.command_id = msg->command_id;
		rmsg.hotspot_count = 0;
		rmsg.result = NO_ERR;

		try
		{

		// timing
     		time_begin = high_resolution_clock::now();

                string irName = msg->photo_file;

		// Load image
		cout << "\tloading image " << irName << endl;
		IrImage irImage;
		int err = irImage.LoadImage(irName);
        	if (err != 0)
		{
			cout << "\terror reading image, error code " << err << endl;
			rmsg.result = LOAD_ERR;
			cout << "Sending message hotspot RET (id=" << rmsg.command_id << ")" << endl;
			lcmp->publish("hotspot_ret", &rmsg);
			return;
		}

		// Segmentation
		cout << "\tstarting segmentation" << endl;
		HotSpotList* hotSpotList = irImage.FindHotSpots(thresholdTemp);
		if (hotSpotList->Size() > 0)
 		{
			cout << "\thotspot found!" << endl;
			rmsg.hotspot_count  = hotSpotList->Size();
			rmsg.hotspot_list.resize(rmsg.hotspot_count);
			list<HotSpot*> hslist = hotSpotList->HotSpots();
			list<HotSpot*>::iterator hsiter = hslist.begin();
			int i = 0;
			while(hsiter != hslist.end())
			{
				HotSpot *hs = *hsiter;
				rmsg.hotspot_list[i].hotspot_id = hs->Id();	 // id
				rmsg.hotspot_list[i].num_pixels = hs->Size();	 // #pixels
				rmsg.hotspot_list[i].center[0] = hs->Center().y; // row
				rmsg.hotspot_list[i].center[1] = hs->Center().x; // column
				rmsg.hotspot_list[i].bounding[0] = hs->BoundingBox().height;	// #rows
				rmsg.hotspot_list[i].bounding[1] = hs->BoundingBox().width;	// #cols
				rmsg.hotspot_list[i].bounding[2] = hs->BoundingBox().y;		// upper_row
				rmsg.hotspot_list[i].bounding[3] = hs->BoundingBox().x; 	// leftmost_col
				rmsg.hotspot_list[i].max_temp = hs->MaxTemp();
				rmsg.hotspot_list[i].avg_temp = hs->AvgTemp();
				hsiter++; i++;
			}

			// Create output folder (if needed)
			string outputPath = "OUTPUT/HOTSPOT";
			err = createOutputFolder(outputPath);
			if (err != 0)
			{
				cout << "\tunable to create output folder " << outputPath << ", error code " << err << endl;
				rmsg.result = MKDIR_ERR;
				cout << "Sending message hotspot RET (id=" << rmsg.command_id << ")" << endl;
				lcmp->publish("hotspot_ret", &rmsg);
				return;
			}

			// Write image
			string outputName = createName(outputPath, getRawBasename(irName), "HS", "jpeg");
			cout << "\twriting output image " << outputName << endl;
			irImage.DrawHotSpots(hotSpotList);
			err = irImage.SaveImage(outputName.c_str());
			if (err != 0)
			{
				cout << "\terror writing image, error code " << err << endl;
				rmsg.result = SAVE_ERR;
				cout << "Sending message hotspot RET (id=" << rmsg.command_id << ")" << endl;
				lcmp->publish("hotspot_ret", &rmsg);
				return;
			}
		}
		delete hotSpotList;

		// timing     		
		time_end = high_resolution_clock::now();
		double time_span = duration_cast<duration<double>>(time_end - time_begin).count();
		time_log  << n++ << " \t#" << msg->command_id << "\t" << getRawBasename(irName) << " \t" << time_span << endl;

		}
		catch( cv::Exception& e )
		{
		    	const char* err_msg = e.what();
		    	cout << "\texception caught: " << err_msg << endl;
			rmsg.result = OPENCV_ERR;
		}

		cout << "Sending message hotspot RET (id=" << rmsg.command_id << ")" << endl;
		lcmp->publish("hotspot_ret", &rmsg);
        }
};

void printUsage()
{
	    cout << endl;
	    cout << "HotSpots - Detect hotspots in a thermal (TIFF, PNG, JPEG or FLIR FPF format) image" << endl;
	    cout << endl;
	    cout << "Usage: hotspots [Options]" << endl;
	    cout << endl;
	    cout << "Options:" << endl;
	    cout << endl;
	    cout << "  -t <value>, --threshold <value>" << endl;
	    cout << "      temperature threshold in Celsius (default is " << CELSIUS(thresholdTemp) << "K)" << endl;
	cout << "  --num_threads <int>" << endl;
	cout << "      number of threads" << endl;
	cout << "      (default is " << getNumThreads() << ")" << endl;
	cout << endl;
	cout << "  -h, --help" << endl;
	cout << "      print usage" << endl;
	cout << endl;
}

// Parse command line arguments
int parseArguments(int argc, char* argv[])
{
    	try
	{
		for (int i = 1; i < argc; i++)
	    	{
            		if (string(argv[i]) == "--help" || string(argv[i]) == "-h" || string(argv[i]) == "/?")
			{
				return -1;
			}
			else if (string(argv[i]) == "--num_threads")
			{
				setNumThreads(atoi(argv[i+1]));
				i++;
			}
		    	else if (string(argv[i]) == "--threshold" || string(argv[i]) == "-t") 
		    	{
		    		thresholdTemp = KELVIN(atof(argv[i+1]));
		    		i++;
		    	}
			else
		    	{
		    	    return -1;
		    	}
	  	}
    	}
	catch (const Exception &e)
	{
		return -1;
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
	
    cout << "LCM network on" << endl;

    HotspotService hotspotService(&lcm);
    lcm.subscribe("hotspot_cmd", &HotspotService::handleMessage, &hotspotService);

    while(0 == lcm.handle());

    return 0;
}
