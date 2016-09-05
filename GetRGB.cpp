#include <iostream>
#include <fstream>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <raspicam/raspicam_still_cv.h>
#include <ctime>
#include <string>

using namespace cv;
using namespace std;

const int xposit[4] = {261, 322, 386, 450};
const int yposit[4] = {145, 191, 242, 305};
const unsigned int fieldSize = 16;

class ColorPicker
{
public:
	ColorPicker(unsigned int minValue, unsigned int minSaturation,
		unsigned int lowHCutoff, unsigned int highHCutoff);
	~ColorPicker();
	 //returns a color 0 - none, 1 - yellow, 2 - red, 3 - blue, 4 - white
	void whatColor(unsigned int Hue, unsigned int Sat, unsigned int Val, unsigned int location);
	void setPattern(char c, int loc);
	string getPattern();
private:
	unsigned int minVal;
	unsigned int minSat;
	unsigned int lowHCO;
	unsigned int highHCO;
	string outPattern;
};
ColorPicker::ColorPicker(unsigned int minValue, unsigned int minSaturation,
	unsigned int lowHCutoff, unsigned int highHCutoff)
{
	minVal = minValue;
	minSat = minSaturation;
	lowHCO = lowHCutoff;
	highHCO = highHCutoff;
	outPattern = ""; //allow for commas and null char
}
ColorPicker::~ColorPicker()
{
}
void ColorPicker::setPattern(char c, int loc)
{
	outPattern += c;
	if(loc < fieldSize - 1)
		outPattern += ',';
	else
		outPattern += '\0';
}
string ColorPicker::getPattern()
{
	return outPattern;
}
void ColorPicker::whatColor(unsigned int Hue, unsigned int Sat, unsigned int Val, unsigned int location)
{
	char color;
	if(Val < minVal)
		color = '0'; //empty slot
	else
	{
		if(Sat < minSat)
			color = '4'; //white
		else // use Hue to test for color
		{
			if(Hue < lowHCO)
				color = '1'; //yellow
			else
			{
				if(Hue > highHCO)
					color = '3'; //red
				else
					color = '2'; //blue
			}
		}
	}
	setPattern(color, location);
}
int main()
{
	ColorPicker cp(68, 60, 50, 120); //minVal, minSat, lowHueCut, highHueCut
	ofstream outfile;
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
	
	outfile.open ("outfile.txt", ios::app);
	//outfile << "Data collected on: " << dt << '\n';
	int count = 0;
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
				//outfile << i << ',' << j << ',' << avgPixelIntensity[k] << '\n';
			}
			cp.whatColor(hsv[0], hsv[1], hsv[2], count);
			count = ++count;
			//outfile << "in this position " << wc << '\n';
		}
	}
	outfile << cp.getPattern();
	outfile.close();
	//delete [] pos;

    return 0;
}