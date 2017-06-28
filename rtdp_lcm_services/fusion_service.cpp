//============================================================================
// Name        : fusion_service.cpp
// Author      : ESS
// Version     : 1.0
// Copyright   : ICARUS-UPC
// Description : LCM service to overlay thermal and visual images
//============================================================================
// Input message:
//	int64_t command_id;
//	string photo_file[2];		// [0] Visual, [1] Thermal
//	geolocation_t photo_location[2];// [0] Visual, [1] Thermal
//	orientation_t photo_attitude[2];// [0] Visual, [1] Thermal
//
// Output message:
//	int64_t command_id;
//	byte result;
//	string photo_file;
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
#include "rtdp_core/Image.h"
#include "rtdp_core/IrImage.h"
#include "rtdp_core/GeoImage.h"
#include "cpp_messages/geolocation_t.hpp"
#include "cpp_messages/orientation_t.hpp"
#include "cpp_messages/fusion_cmd_t.hpp"
#include "cpp_messages/fusion_ret_t.hpp"


#if !defined(LCM_NETWORK)
	#define LCM_NETWORK "udpm://224.0.0.1:7667?ttl=1"
#endif

using namespace std;
using namespace std::chrono; //timing


/////////////////////////////////////////////////////
// COMMAND LINE ARGUMENTS - DEFAULT VALUES

string payloadXmlName = "cfg_files/payload.xml";
string viCameraName = "JAI-BB500GE"; 	// as coded in payload.xml
string irCameraName = "FLIR-A320";   	// as coded in payload.xml

string viImageName = "input/JAI/080.jpg";
string irImageName = "input/FLIR-A320/080.jpg";
string viTeleName = "input/JAI/PhotoLog.txt";
string irTeleName = "input/FLIR-A320/PhotoLog.txt";
string viTeleXmlPath = ""; // to support old XML format
string irTeleXmlPath = ""; // to support old XML format

string outputPath = "OUTPUT/FUSION";
double outputImageSize = 0.5;  		// output image size in MP
double outputPixelScale = 0.0; 		// meters/pixel (ignored by default)

//////////////////////////////////////////////////////


class FusionService 
{
    private:
	lcm::LCM* lcmp;

	Payload* payloadp;

	std::ofstream time_log;
	high_resolution_clock::time_point time_begin, time_end;
	int n;

    public:
	FusionService(lcm::LCM* lcm, Payload* payload) 
	{
		lcmp = lcm;
		payloadp = payload;
   		time_log.open("fusion_time_log.txt");
		time_log << "fusion execution time in seconds (" << getNumThreads() << " threads)"<< endl;
		time_log << fixed << setprecision(6);
		n = 1;
	}

        ~FusionService() 
	{
	    	time_log.close();
	}

        void handleMessage(const lcm::ReceiveBuffer* rbuf,
                const std::string& chan, 
                const fusion_cmd_t* msg)
        {
		cout << "Received message on channel " << chan << endl;
		
		fusion_ret_t rmsg;
		rmsg.command_id = msg->command_id;
		rmsg.result = NO_ERR;

		try
		{

		// timing
     		time_begin = high_resolution_clock::now();

		Image viImage, irImage;
		string viName, irName;
		double lat, lon, alt, yaw, pitch, roll;

		viName = msg->photo_file[0];
		lat = msg->photo_location[0].lat;
		lon = msg->photo_location[0].lon;
		alt = msg->photo_location[0].alt;
		yaw = msg->photo_attitude[0].yaw;
		pitch = msg->photo_attitude[0].pitch;
		roll = msg->photo_attitude[0].roll;
		Metadata *viMetadata = new Metadata(lat, lon, alt, yaw, pitch, roll);

		irName = msg->photo_file[1];
		lat = msg->photo_location[1].lat;
		lon = msg->photo_location[1].lon;
		alt = msg->photo_location[1].alt;
		yaw = msg->photo_attitude[1].yaw;
		pitch = msg->photo_attitude[1].pitch;
		roll = msg->photo_attitude[1].roll;
		Metadata *irMetadata = new Metadata(lat, lon, alt, yaw, pitch, roll);

		// Load VI image
		cout << "\tloading visual image " << viName << endl;
		int err = viImage.LoadImage(viName);
        	if (err != 0)
		{
			cout << "\terror reading image, error code " << err << endl;
			rmsg.result = LOAD_ERR;
			cout << "Sending message fusion RET (id=" << rmsg.command_id << ")" << endl;
			lcmp->publish("fusion_ret", &rmsg);
			return;
		}
  		GeoImage viGeoImage(&viImage, payloadp->GetCamera(viCameraName), payloadp->GetGps(), viMetadata);

		// Load IR image
		cout << "\tloading thermal image " << irName << endl;
		err = irImage.LoadImage(irName);
        	if (err != 0)
		{
			cout << "\terror reading image, error code " << err << endl;
			rmsg.result = LOAD_ERR;
			cout << "Sending message fusion RET (id=" << rmsg.command_id << ")" << endl;
			lcmp->publish("fusion_ret", &rmsg);
			return;
		}
  		GeoImage irGeoImage(&irImage, payloadp->GetCamera(irCameraName), payloadp->GetGps(), irMetadata);

		// Fusion
		string suffix = "F";
     		if (outputPixelScale > 0)
   		{
    			err = viGeoImage.FixMetScale(outputPixelScale);
    			if (err < 0)
			{
   				cout << "\tunable to set output scale to " << outputPixelScale << " meters per pixel" << err << endl;
 				rmsg.result = SCALE_ERR;
				cout << "Sending message fusion RET (id=" << rmsg.command_id << ")" << endl;
				lcmp->publish("fusion_ret", &rmsg);
				return;
    			}
	    		suffix += to_string((int)(1000*outputPixelScale)) + "MM";
    		}
     		else if (outputImageSize > 0)
   		{
    			err = viGeoImage.FixSize(outputImageSize);
    			if (err < 0)
    			{
    				cout << "\tunable to set output size to " << outputImageSize << " megapixels, error code " << err << endl;
 				rmsg.result = RESIZE_ERR;
				cout << "Sending message fusion RET (id=" << rmsg.command_id << ")" << endl;
				lcmp->publish("fusion_ret", &rmsg);
				return;
    			}
 	    		suffix += to_string((int)outputImageSize) + (outputImageSize<1?to_string((int)(10*outputImageSize)): "") + "M";
   		}

  		err = viGeoImage.Fusion(&irGeoImage);
        	if (err != 0)
		{
			cout << "\terror overlaying images, error code " << err << endl;
			rmsg.result = FUSION_ERR;
			cout << "Sending message fusion RET (id=" << rmsg.command_id << ")" << endl;
			lcmp->publish("fusion_ret", &rmsg);
			return;
		}

		// Create output folder (if needed)
		err = createOutputFolder(outputPath);
		if (err != 0)
		{
			cout << "\tunable to create output folder " << outputPath << ", error code " << err << endl;
			rmsg.result = MKDIR_ERR;
			cout << "Sending message fusion RET (id=" << rmsg.command_id << ")" << endl;
			lcmp->publish("fusion_ret", &rmsg);
			return;
		}

		// Write GeoTiff
 		string outputName = createName(outputPath, getRawBasename(irName), suffix, "tiff");
		cout << "\twriting output image " << outputName << endl;
		err = viGeoImage.WriteGeoTiff(outputName);
		if (err != 0)
		{
			cout << "\terror writing image, error code " << err << endl;
			rmsg.result = SAVE_ERR;
			cout << "Sending message fusion RET (id=" << rmsg.command_id << ")" << endl;
			lcmp->publish("fusion_ret", &rmsg);
			return;
		}
		rmsg.photo_file = outputName;

		// timing     		
		time_end = high_resolution_clock::now();
		double time_span = duration_cast<duration<double>>(time_end - time_begin).count();
		time_log  << n++ << " \t#" << msg->command_id << "\t" << getRawBasename(viName) << " \t" << time_span << endl;

		}
		catch( cv::Exception& e )
		{
		    	const char* err_msg = e.what();
		    	cout << "\texception caught: " << err_msg << endl;
			rmsg.result = OPENCV_ERR;
		}

		cout << "Sending message fusion RET (id=" << rmsg.command_id << ")" << endl;
		lcmp->publish("fusion_ret", &rmsg);
        }
};

void printUsage()
{
	cout << endl;
	cout << "Fusion - Overlay visual and thermal images" << endl;
	cout << endl;
	cout << "Usage: fusion [Options]" << endl;
	cout << endl;
	cout << "Options:" << endl;
	cout << endl;
	cout << "  -vt <file>, --visual_telemetry <file>" << endl;
	cout << "      text file containing the telemetry of all visual images" << endl;
	cout << "      (default is " << viTeleName << ", ignored if -vx option set)" << endl;
	cout << endl;
	cout << "  -it <file>, --thermal_telemetry <file>" << endl;
	cout << "      text file containing the telemetry of all thermal images" << endl;
	cout << "      (default is " << irTeleName << ", ignored if -ix option set)" << endl;
	cout << endl;
	cout << "  -vx <path>, --visual_xml_telemetry <path>" << endl;
	cout << "      path to old XML format telemetry files for visual images (img_id.XML)" << endl;
	cout << "      (ignored by default)" << endl;
	cout << endl;
	cout << "  -ix <path>, --thermal_xml_telemetry <path>" << endl;
	cout << "      path to old XML format telemetry files for thermal images (img_id.XML)" << endl;
	cout << "      (ignored by default)" << endl;
	cout << endl;
	cout << "  -p <file>, --payload_xml <file>" << endl;
	cout << "      payload xml file (contains the camera parameters)" << endl;
	cout << "      (default is " << payloadXmlName << ")" << endl;
	cout << endl;
	cout << "  -vc <name>, --visual_camera <name>" << endl;
	cout << "      visual camera name (as coded in the payload xml file)" << endl;
	cout << "      (default is " << viCameraName << ")" << endl;
	cout << endl;
	cout << "  -ic <name>, --thermal_camera <name>" << endl;
	cout << "      thermal camera name (as coded in the payload xml file)" << endl;
	cout << "      (default is " << irCameraName << ")" << endl;
	cout << endl;
	cout << "  -o <path>, --output_path <path>" << endl;
	cout << "      output path" << endl;
	cout << "      (default is " << outputPath << ")" << endl;
	cout << endl;
	cout << "  -r <double>, --compose_resolution <double>" << endl;
	cout << "      pixel resolution (in meters per pixel) of the output tiff image" << endl;
	cout << "      (ignored by default)" << endl;
	cout << endl;
	cout << "  -s <double>, --output_size <double>" << endl;
	cout << "      output image size in MP (ignored if pixel resolution is set)" << endl;
	cout << "      (default is " << outputImageSize << ")" << endl;
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
		else if (string(argv[i]) == "--visual_telemetry" || string(argv[i]) == "-vt")
		{
			viTeleName = argv[i+1];
			i++;
		}
		else if (string(argv[i]) == "--thermal_telemetry" || string(argv[i]) == "-it")
		{
			irTeleName = argv[i+1];
			i++;
		}
		else if (string(argv[i]) == "--visual_xml_telemetry" || string(argv[i]) == "-vx")
		{
			viTeleXmlPath = argv[i+1];
			i++;
		}
		else if (string(argv[i]) == "--thermal_xml_telemetry" || string(argv[i]) == "-ix")
		{
			irTeleXmlPath = argv[i+1];
			i++;
		}
		else if (string(argv[i]) == "--payload_xml" || string(argv[i]) == "-p")
		{
			payloadXmlName = argv[i+1];
			i++;
		}
		else if (string(argv[i]) == "--visual_camera" || string(argv[i]) == "-vc")
		{
			viCameraName = argv[i+1];
			i++;
		}
		else if (string(argv[i]) == "--thermal_camera" || string(argv[i]) == "-ic")
		{
			irCameraName = argv[i+1];
			i++;
		}
		else if (string(argv[i]) == "--output_path" || string(argv[i]) == "-o")
		{
			outputPath = argv[i+1];
			i++;
		}
		else if (string(argv[i]) == "--compose_resolution" || string(argv[i]) == "-r")
		{
			outputPixelScale = atof(argv[i+1]);
			outputImageSize = 0.0;
			i++;
		}
		else if (string(argv[i]) == "--output_size" || string(argv[i]) == "-s")
		{
			if (outputPixelScale == 0)
				outputImageSize = atof(argv[i+1]);
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
    else if (payload.GetCamera(viCameraName) == NULL)
    {
	cout << "ERROR: visual camera " << viCameraName << " not found in " << payloadXmlName << endl;
	exit (-1);
    }
    else if (payload.GetCamera(irCameraName) == NULL)
    {
	cout << "ERROR: thermal camera " << irCameraName << " not found in " << payloadXmlName << endl;
	exit (-1);
    }

    lcm::LCM lcm = lcm::LCM(LCM_NETWORK);

    if(!lcm.good())
        return 1;
	
    cout << "LCM network on" << endl;

    FusionService fusionService(&lcm, &payload);
    lcm.subscribe("fusion_cmd", &FusionService::handleMessage, &fusionService);

    while(0 == lcm.handle());

    return 0;
}
