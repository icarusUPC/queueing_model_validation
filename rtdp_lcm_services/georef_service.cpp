//============================================================================
// Name        : georef_service.cpp
// Author      : ESS
// Version     : 1.0
// Copyright   : ICARUS-UPC
// Description : LCM service to get geographic coordinates of a pixel in an image
//============================================================================
// Input message:
//	int64_t command_id;
//	int32_t pixel[2];
//	geolocation_t location;
//	orientation_t attitude;
//
// Output message:
//	int64_t command_id;
//	byte result;
//	geolocation_t pixel_loc;
//============================================================================

#include <stdio.h>
#include <fstream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <lcm/lcm-cpp.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "rtdp_core/Utils.h"
#include "rtdp_core/Payload.h"
#include "rtdp_core/Metadata.h"
#include "rtdp_core/DirectGeoref.h"
#include "cpp_messages/geolocation_t.hpp"
#include "cpp_messages/orientation_t.hpp"
#include "cpp_messages/georef_cmd_t.hpp"
#include "cpp_messages/georef_ret_t.hpp"

#if !defined(LCM_NETWORK)
	#define LCM_NETWORK "udpm://224.0.0.1:7667?ttl=1"
#endif

using namespace std;
using namespace std::chrono; //timing


/////////////////////////////////////////////////////
// COMMAND LINE ARGUMENTS - DEFAULT VALUES

string payloadXmlName = "cfg_files/payload.xml";
string cameraName = "FLIR-A320"; // as coded in payload.xml

//////////////////////////////////////////////////////


class GeorefService 
{
    private:
	lcm::LCM* lcmp;

	Payload* payloadp;

	std::ofstream time_log;
	high_resolution_clock::time_point time_begin, time_end;
	int n;

    public:
	GeorefService(lcm::LCM* lcm, Payload* payload) {
		lcmp = lcm;
		payloadp = payload;
    		time_log.open("georef_time_log.txt");
		time_log << "georef execution time in seconds (" << getNumThreads() << " threads)"<< endl;
		time_log << fixed << setprecision(6);
		n = 1;
	}

        ~GeorefService()
	{
	    	time_log.close();
	}

        void handleMessage(const lcm::ReceiveBuffer* rbuf,
                const std::string& chan, 
                const georef_cmd_t* msg)
        {
		cout << "Received message on channel " << chan << endl;

		georef_ret_t rmsg;
		rmsg.command_id = msg->command_id;
		rmsg.result = NO_ERR;

		try
		{

		// timing
     		time_begin = high_resolution_clock::now();

		int row = msg->pixel[0];
		int col = msg->pixel[1];
		double lat = msg->location.lat;
		double lon = msg->location.lon;
		double alt = msg->location.alt;
		double yaw = msg->attitude.yaw;
		double pitch = msg->attitude.pitch;
		double roll = msg->attitude.roll;
		Metadata *metadata = new Metadata(lat, lon, alt, yaw, pitch, roll);

		cout << "\tstarting georeferencing pixel (" << row << ", " << col << ")" << endl;
		DirectGeoref directGeoref(payloadp->GetCamera(cameraName), payloadp->GetGps(), metadata);
		Vec3d pixelLLA = directGeoref.PixelGeolocation(Point(col, row));
		cout << "\tLLA: " << LLAToString(pixelLLA) << endl;

		rmsg.pixel_loc.lat = pixelLLA[LAT];
		rmsg.pixel_loc.lon = pixelLLA[LON];
		rmsg.pixel_loc.alt = pixelLLA[ALT];

		// timing     		
		time_end = high_resolution_clock::now();
		double time_span = duration_cast<duration<double>>(time_end - time_begin).count();
		time_log  << n++ << " \t#" << msg->command_id << " \t" << row << " \t" << col  << " \t" << time_span << endl;

		}
		catch( cv::Exception& e )
		{
		    	const char* err_msg = e.what();
		    	cout << "\texception caught: " << err_msg << endl;
			rmsg.result = OPENCV_ERR;
		}

		cout << "Sending message georef RET (id=" << rmsg.command_id << ")" << endl;
		lcmp->publish("georef_ret", &rmsg);
        }
};

void printUsage()
{
    cout << endl;
    cout << "GeoRef - Determine geographic coordinates of a pixel in an image" << endl;
    cout << endl;
    cout << "Usage: georef [Options]" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << endl;
    cout << "  -p <payload_xml>, --payload_xml <payload_xml>" << endl;
    cout << "      payload xml file (contains the camera parameters)" << endl;
    cout << "      (default is " << payloadXmlName << ")" << endl;
    cout << endl;
    cout << "  -c <camera_name>, --camera_name <camera_name>" << endl;
    cout << "      camera name (as coded in the payload xml file)" << endl;
    cout << "      (default is " << cameraName << ")" << endl;
    cout << endl;
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
	    else if (string(argv[i]) == "--payload_xml" || string(argv[i]) == "-p")
            {
                payloadXmlName = argv[i+1];
                i++;
            }
            else if (string(argv[i]) == "--camera_name" || string(argv[i]) == "-c")
            {
                cameraName = argv[i+1];
                i++;
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
    
    // Load payload XML file
    Payload payload;
    if(payload.ReadXML(payloadXmlName) < 0)
    {
	cout << "ERROR loading payload file " << payloadXmlName << endl;
	exit (-1);
    }
    else if (payload.GetCamera(cameraName) == NULL)
    {
	cout << "ERROR: camera " << cameraName << " not found in " << payloadXmlName << endl;
	exit (-1);
    }

    lcm::LCM lcm = lcm::LCM(LCM_NETWORK);

    if(!lcm.good())
        return 1;
	
    cout << "LCM network on" << endl;

    GeorefService georefService(&lcm, &payload);
    lcm.subscribe("georef_cmd", &GeorefService::handleMessage, &georefService);

    while(0 == lcm.handle());

    return 0;
}
