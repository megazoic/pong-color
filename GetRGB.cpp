#include <iostream>
#include <fstream>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <raspicam/raspicam_still_cv.h>
#include <ctime>

using namespace cv;
using namespace std;

const int xposit[4] = {261, 322, 386, 450};
const int yposit[4] = {145, 191, 242, 305};

class ColorPicker
{
public:
	ColorPicker(unsigned int minValue, unsigned int minSaturation,
		unsigned int lowHCutoff, unsigned int highHCutoff);
	~ColorPicker();
	 //returns a color 0 - none, 1 - yellow, 2 - red, 3 - blue, 4 - white
	int whatColor(unsigned int Hue, unsigned int Sat, unsigned int Val);
private:
	unsigned int minVal;
	unsigned int minSat;
	unsigned int lowHCO;
	unsigned int highHCO;
};
ColorPicker::ColorPicker(unsigned int minValue, unsigned int minSaturation,
	unsigned int lowHCutoff, unsigned int highHCutoff)
{
	minVal = minValue;
	minSat = minSaturation;
	lowHCO = lowHCutoff;
	highHCO = highHCutoff;
}
ColorPicker::~ColorPicker()
{
}
int ColorPicker::whatColor(unsigned int Hue, unsigned int Sat, unsigned int Val)
{
	int color;
	if(Val < minVal)
		color = 0; //empty slot
	else
	{
		if(Sat < minSat)
			color = 4; //white
		else // use Hue to test for color
		{
			if(Hue < lowHCO)
				color = 1; //yellow
			if(Hue > highHCO)
				color = 3; //red
			else
				color = 2; //blue
		}
	}
	return color;
}
int main()
{
	ColorPicker cp(60, 60, 50, 120); //minVal, minSat, lowHueCut, highHueCut
	ofstream rgbfile;
	raspicam::RaspiCam_Still_Cv Camera;
	
	time_t now = time(0);
	char* dt = ctime(&now);
	
    Mat imgOriginal;
	Mat imgHSV;

    //set camera params
    Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 );

	//open camera
    cout<<"Opening Camera..."<<endl;
    if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
    //Start capture
    cout<<"sleeping for 1 secs"<<endl;
	sleep(1);
	Camera.grab();
    Camera.retrieve(imgOriginal);
	
	Camera.release();
	
    if ( !imgOriginal.data )
    {
        printf("No image data \n");
        return -1;
    }
	//Convert RBG to HSV for color recognition
	cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);
	
	rgbfile.open ("rgbfile.txt", ios::app);
	rgbfile << "Data collected on: " << dt << '\n';
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			unsigned int hsv[3];
			
			for(int k = 0; k<3; k++)
			{
				Rect roi( xposit[i],yposit[j], 15, 15 );
				Mat image_roi = imgHSV( roi );
				Scalar avgPixelIntensity = mean( image_roi );
				hsv[k] = (int) avgPixelIntensity[k];
				rgbfile << i << ',' << j << ',' << avgPixelIntensity[k] << '\n';
			}
			int wc = cp.whatColor(hsv[0], hsv[1], hsv[2]);
			rgbfile << "in this position " << wc << '\n';
		}
	}
	rgbfile.close();
	//delete [] pos;

    return 0;
}