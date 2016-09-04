#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
struct Posit
{
	int xvalue;
	int yvalue;
};

const char color[] = {'b', 'g', 'r'};

const struct Posit pos[6] = {{352,672}, {940,1169}, {1612,1148}, {912,1656}, {1634,1680}, {2284,1684}};
int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    Mat image;
    image = imread( argv[1], 1 );

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
	//namedWindow("Display Image", WINDOW_AUTOSIZE );
    //imshow("Display Image", image);

    //waitKey(0);
	for(int i = 0; i < 6; i++)
	{
		for(int j = 0; j<3; j++)
		{
			double rgbValue = image.at<cv::Vec3b>(pos[i].yvalue,pos[i].xvalue)[j];
			std::cout << "position " << i << ' ' << color[j] << ' ' << rgbValue << '\n';
		}
	}
	//delete [] pos;

    return 0;
}