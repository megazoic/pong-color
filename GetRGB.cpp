#include <iostream>
#include <fstream>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_still_cv.h>

using namespace cv;
using namespace std;

const char color[] = {'b', 'g', 'r'};
const int xposit[4] = {261, 322, 386, 450};
const int yposit[4] = {145, 191, 242, 305};

int main(int argc, char** argv )
{
	ofstream rgbfile;
	raspicam::RaspiCam_Still_Cv Camera;
    Mat image;

    //set camera params
    Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 );

	//open camera
    cout<<"Opening Camera..."<<endl;
    if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
    //Start capture
    cout<<"sleeping for 1 secs"<<endl;
	sleep(1);
	Camera.grab();
    Camera.retrieve(image);
	
	Camera.release();
	
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
	rgbfile.open ("rgbfile.txt", ios::app);
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			for(int k = 0; k<3; k++)
			{
				Rect roi( xposit[i],yposit[j], 15, 15 );
				Mat image_roi = image( roi );
				Scalar avgPixelIntensity = mean( image_roi );
				//double rgbValue = image.at<cv::Vec3b>(pos[i].yvalue,pos[i].xvalue)[j];
				rgbfile << "Xpos " << i << " Ypos " << j << ' ' << color[k] << ' ' << avgPixelIntensity[k] << '\n';
			}
		}
	}
	rgbfile.close();
	//delete [] pos;

    return 0;
}