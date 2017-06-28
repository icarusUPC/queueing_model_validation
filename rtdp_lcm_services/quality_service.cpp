//============================================================================
// Name        : quality_service.cpp
// Author      : ESS
// Version     : 1.0
// Copyright   : ICARUS-UPC
// Description : LCM service to compute blur and entropy image quality metrics
//============================================================================
// Input message:
//    	int64_t command_id;
//    	string photo_file;
//
// Output message:
//	int64_t command_id;
//	byte result;
//	float entropy;
//	float blur;
//============================================================================

#include <stdio.h>
#include <fstream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <lcm/lcm-cpp.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "rtdp_core/Image.h"
#include "rtdp_core/Utils.h"
#include "cpp_messages/quality_cmd_t.hpp"
#include "cpp_messages/quality_ret_t.hpp"

#if !defined(LCM_NETWORK)
	#define LCM_NETWORK "udpm://224.0.0.1:7667?ttl=1"
#endif

using namespace std;
using namespace std::chrono; //timing


/////////////////////////////////////////////////////
// COMMAND LINE ARGUMENTS - DEFAULT VALUES


//////////////////////////////////////////////////////


class QualityService 
{
    private:
	lcm::LCM* lcmp;

	std::ofstream time_log;
	high_resolution_clock::time_point time_begin, time_end;
	int n;

    public:
	QualityService(lcm::LCM* lcm) {
		lcmp = lcm;
    		time_log.open("quality_time_log.txt");
		time_log << "quality execution time in seconds (" << getNumThreads() << " threads)"<< endl;
		time_log << fixed << setprecision(6);
		n = 1;
	}

        ~QualityService()
	{
	    	time_log.close();
	}

        void handleMessage(const lcm::ReceiveBuffer* rbuf,
                const std::string& chan, 
                const quality_cmd_t* msg)
        {
		//cout << "QUALITY_SRV: Received message on channel " << chan << endl;
		
  		quality_ret_t rmsg;
		rmsg.command_id = msg->command_id;
		rmsg.result = NO_ERR;

		try
		{

		// timing
     		time_begin = high_resolution_clock::now();

                string inputName = msg->photo_file;

		// Load image
		//cout << "QUALITY_SRV: loading image " << inputName << endl;
		Image image;
		int err = image.LoadImage(inputName);
	       	if (err != 0)
		{
			cout << "\terror reading image, error code " << err << endl;
			rmsg.result = LOAD_ERR;
			cout << "Sending message quality RET (id=" << rmsg.command_id << ")" << endl;
			lcmp->publish("quality_ret", &rmsg);
			return;
		}

		// Compute blurr and entropy
		rmsg.blur = image.Blurring();
     		rmsg.entropy = image.Entropy();
		//cout << "QUALITY_SRV: blur=" << rmsg.blur << "  entr=" << rmsg.entropy << endl;

		// timing     		
		time_end = high_resolution_clock::now();
		double time_span = duration_cast<duration<double>>(time_end - time_begin).count();
		time_log  << n++ << " \t#" << msg->command_id << "\t" << getRawBasename(inputName) << " \t" << time_span << endl;

		}
		catch( cv::Exception& e )
		{
		    	const char* err_msg = e.what();
		    	cout << "\texception caught: " << err_msg << endl;
			rmsg.result = OPENCV_ERR;
		}

		//cout << "QUALITY_SRV: Sending message quality RET (id=" << rmsg.command_id << ")" << endl;
		lcmp->publish("quality_ret", &rmsg);
        }
};


void printUsage()
{
    cout << endl;
    cout << "Quality - Determine Blur and Entropy in an image" << endl;
    cout << endl;
    cout << "Usage: quality [Options]" << endl;
    cout << endl;
    cout << "Options:" << endl;
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
	
    cout << "Quality Service over LCM network on" << endl;

    QualityService qualityService(&lcm);
    lcm.subscribe("quality_cmd", &QualityService::handleMessage, &qualityService);

    while(0 == lcm.handle());

    return 0;
}
