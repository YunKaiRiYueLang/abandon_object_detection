#pragma once

#include<opencv2/opencv.hpp>;
using namespace std;
using namespace cv;
struct incomeroi
{
    double area = 0;
    Point2f centroid = Point2f(0, 0);
    Rect box = Rect(1, 1, 0, 0);

};
struct  tracker
{

    double  area = 0;
    Point centroid = Point(1, 1);
    Rect bbox = Rect(1, 1, 0, 0);
    int  age = 0;
    int hitCount = 0;
    int missCount = 0;
    bool justHit = false;
    bool isTrackActive = false;
};


void inittracker(std::vector<tracker> &track);
void ObjTracker(std::vector<incomeroi> &incomerois, std::vector<Rect> &outbox);
void Detect(cv::Mat &frame, cv::Mat &bg, std::vector<incomeroi> &incomerois);
void StoreBackground(cv::Mat &frame, cv::Mat &background);
extern std::vector<tracker> track;
bool ObjectDetect( Mat fg, cv::Mat &output,std::vector<cv::Point> &outrects);


