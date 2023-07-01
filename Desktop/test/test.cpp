#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat imgHSV, mask;
int hmin = 0, smin = 110, vmin = 153;
int hmax = 19, smax = 240, vmax = 255;

int main()
{
    string path = "/home/kilakila/Desktop/test/0.mp4";
    VideoCapture cap(path);
    string path2="/home/kilakila/Desktop/test/imgage/kk.jpg";
    Mat img=imread(path2);
    Mat frame;
    Mat imgGray, imgBlur, imgCanny, imgDil;

    cvtColor(img,imgHSV,COLOR_BGR2HSV);
        namedWindow("Trackbars", (640, 200));
	createTrackbar("Hue Min", "Trackbars", &hmin, 179);
	createTrackbar("Hue Max", "Trackbars", &hmax, 179);
	createTrackbar("Sat Min", "Trackbars", &smin, 255);
	createTrackbar("Sat Max", "Trackbars", &smax, 255);
	createTrackbar("Val Min", "Trackbars", &vmin, 255);
	createTrackbar("Val Max", "Trackbars", &vmax, 2555);
    while (cap.read(frame))
    {
        
    while(true){
  
        GaussianBlur(imgGray, imgBlur, Size(3, 3), 5, 0);
        Canny(imgBlur, imgCanny, 25, 75);

        Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
        dilate(imgCanny, imgDil, kernel);

        Mat imgThreshold;
        threshold(imgDil, imgThreshold, 127, 255, THRESH_BINARY);

        vector<vector<Point> > contours;
        findContours(imgThreshold, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        for (int i = 0; i < contours.size(); i++)
        {
            Rect boundingRects = boundingRect(contours[i]);
            if (boundingRects.width > 50 && boundingRects.height > 50)
            {
                rectangle(frame, boundingRects.tl(), boundingRects.br(), Scalar(0, 255, 0), 2);
            }
        }

        cv::imshow("frame",frame);
        cv::waitKey(30);}
    
    }

    cap.release();

    return 0;
}

