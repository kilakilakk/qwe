#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;
using namespace std;

class LightDescriptor
{
public:
    RotatedRect rect;
    float angle;
    float length;
    Point2f center;

    LightDescriptor(const RotatedRect &rect) : rect(rect)
    {
        angle = rect.angle;
        length = max(rect.size.width, rect.size.height);
        center = rect.center;
    }
};

void getContours(Mat id, Mat img);

void ShowFPS_Method_One(Mat &dstImage,double t_front)
{
    char str[20];     
    double fps;            
    double  t_now; 
    t_now = ((double)getTickCount() - t_front) / getTickFrequency(); 
    fps = 1.0 / t_now;

    string FPSstring("FPS:");
    sprintf(str, "%.2f", fps);
    FPSstring += str;

    putText(dstImage, FPSstring, Point(20, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255));
}

void getContours(Mat id, Mat img)
{

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(id, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    drawContours(img, contours, -1, Scalar(255, 0, 255), 0);
    vector<LightDescriptor> lightInfos;
    for (int i = 0; i < contours.size(); i++)
    {

        double area = contourArea(contours[i]);
       if (area < 5 || contours[i].size() <= 5)
           continue;
        RotatedRect Light_Rec = fitEllipse(contours[i]);
        if (Light_Rec.size.width / Light_Rec.size.height > 4)
            continue;
        lightInfos.push_back(LightDescriptor(Light_Rec));
    }
    for (size_t i = 0; i < lightInfos.size(); i++)
    {
        for (size_t j = i + 1; (j < lightInfos.size()); j++)
        {
            LightDescriptor &leftLight = lightInfos[i];
            LightDescriptor &rightLight = lightInfos[j];
            float angleGap_ = abs(leftLight.angle - rightLight.angle);
            float LenGap_ratio = abs(leftLight.length - rightLight.length) / max(leftLight.length, rightLight.length);
            float dis = pow(pow((leftLight.center.x - rightLight.center.x), 2) + pow((leftLight.center.y - rightLight.center.y), 2), 0.5);
            float meanLen = (leftLight.length + rightLight.length) / 2;
            float lengap_ratio = abs(leftLight.length - rightLight.length) / meanLen;
            float yGap = abs(leftLight.center.y - rightLight.center.y);
            float yGap_ratio = yGap / meanLen;
            float xGap = abs(leftLight.center.x - rightLight.center.x);
            float xGap_ratio = xGap / meanLen;
            float ratio = dis / meanLen;
            if (angleGap_ > 15 ||
                LenGap_ratio > 1.0 ||
                lengap_ratio > 0.8 ||
                yGap_ratio > 1.5 ||
                xGap_ratio > 2.2 ||
                xGap_ratio < 0.8 ||
                ratio > 2 ||
                ratio < 0.4)
            {
                continue;
            }
            Point center = Point((leftLight.center.x + rightLight.center.x) / 2, (leftLight.center.y + rightLight.center.y) / 2);
            circle(img, center, 5, Scalar(255, 0, 255), 2, LINE_AA);
            RotatedRect rect = RotatedRect(center, Size(dis, meanLen), (leftLight.angle + rightLight.angle) / 2);
            Point2f vertices[4];
            rect.points(vertices);

            for (int i = 0; i < 4; i++)
            {
                line(img, vertices[i], vertices[(i + 2) % 4], Scalar(0, 255, 0), 2.2);
                line(img, vertices[i], vertices[(i + 1) % 4], Scalar(0, 255, 0), 2.2);
            }
        }
    }
}

int main(int, char **)
{
    string path = "/home/kilakila/Desktop/test/video/ood_red.mp4";
    Mat img, imggray, imgc, blur, mask, ie, id, imgHSV;
    VideoCapture cap(path);
    int hmin = 0, smin = 0, vmin = 121;
    int hmax = 100, smax = 255, vmax = 255;
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));

    while (true)
    {
        cap >> img;
         double t_front = (double)getTickCount(); 
        cvtColor(img,imggray,COLOR_BGR2GRAY);
        threshold(imggray, mask, 125, 255,THRESH_BINARY);
   
        dilate(mask, id, kernel);
        imshow("id", id);
        getContours(id, img);

        ShowFPS_Method_One(img,t_front);
        imshow("img", img);

        waitKey(35);
    }
}
