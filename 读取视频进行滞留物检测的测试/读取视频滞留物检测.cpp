/*
读取一个滞留物检测视频进行滞留物检测
*/

#include<opencv2/opencv.hpp>
#include<time.h>
#include<thread>
#include<queue>
#define NOMINMAX/*为了解决min max的定义冲突*/
#include<Windows.h>
#include"detectabandon.h"
#include"C4_people_detect.h"
#include"package_bgs/LBFuzzyGaussian.h"

using namespace ibgs;
IBGS* bgs = new LBFuzzyGaussian;/*背景检测模型对象*/
IntImage<double> original;
std::vector<cv::Point> centerpoints;

using cv::Mat;
using std::cout;
using std::endl;
using std::cerr;

/*
Description:行人检测,将检测的到的中心点存储在全局变量centerpoints中.
Input:待检测每帧图像
该函数将被ProcessImage函数调用进行行人检测
*/
void PeopleDetect(Mat pImg)
{
	original.Load(pImg);
	std::vector<CRect> results;
	scanner.FastScan(original, results, 2);
	PostProcess(results, 2);
	PostProcess(results, 0);
	RemoveCoveredRectangles(results);//移除内部的方框(因为nms方法特殊原因.需要考虑方框在内部)

	for (size_t i = 0; i < results.size(); i++)
	{
		centerpoints.push_back(cv::Point((results[i].left + results[i].right) / 2, (results[i].top + results[i].bottom) / 2));
		cv::rectangle(pImg, cv::Point2d(results[i].left, results[i].top), cv::Point2d(results[i].right, results[i].bottom), cv::Scalar(0, 255, 0), 2);
	}
	cv::imshow("result", pImg);
}

/*
description:处理视频的每一帧图像进行滞留物检测
Input:一帧Mat图像
*/
void ProecssImage(Mat &src)
{
	//double t = clock();
	if (src.data == NULL)
		return;
	cv::Mat img_input = src;
	cv::Mat cv_fg;
	cv::Mat cv_bg;
	PeopleDetect(src);
	//cout << clock() - t << endl;
	//t = clock();
	bgs->setShowOutput(true);
	bgs->process(img_input, cv_fg, cv_bg); //处理后得到前景图和背景图.
	//cv::cvtColor(img_input, img_input, COLOR_BGR2GRAY);
	cv::Mat output = img_input.clone();
	//centerpoints.clear();// 不考虑人体检测.
	ObjectDetect(cv_fg, output, centerpoints);
	imshow("前景", cv_fg);
	imshow("遗留物", output);
	//cout << clock() - t << endl;
	//src = output;
	cv::waitKey(1);
}

#if 1
/*程序入口,需要指定一个视频路径*/
int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		cout << "缺少参数" << endl;
		Sleep(2000);
		return 0;
	}
	/*初始化两个模块*/
	inittracker(track);//初始化遗留物检测模块。
	LoadCascade(scanner);
	cv::VideoCapture cap(argv[1]);
	cv::Mat frame;
	//double t = clock();
	if (cap.isOpened())
	{
		while (true)
		{
			cap >> frame;
			if (frame.empty())
				return 0;
			ProecssImage(frame);
			//cout << clock() - t << endl;
			//t = clock();
		}
	}
}
#endif

#if 0
/*单独测试行人检测功能*/
void PeopleDetect1(Mat pImg)
{
	original.Load(pImg);
	std::vector<CRect> results;
	scanner.FastScan(original, results, 2);
	PostProcess(results, 2);
	PostProcess(results, 0);
	RemoveCoveredRectangles(results);// results为检测到的结果方框

	for (size_t i = 0; i < results.size(); i++)
	{
		cout << results.size() << endl;
		/*centerpoints.push_back(cv::Point((results[i].left + results[i].right) / 2, (results[i].top + results[i].bottom) / 2));*/
		cv::rectangle(pImg, cv::Point2d(results[i].left, results[i].top), cv::Point2d(results[i].right, results[i].bottom), cv::Scalar(0, 255, 0), 2);
	}
	cv::imshow("result", pImg);
	cv::waitKey(1);
}

int main(int argc,char* argv[])
{
	if (argc == 1)
	{
		cerr << "缺少参数输入视频的路径参数" ;
		return 0;
	}
	LoadCascade(scanner);
	cv::VideoCapture cap(argv[1]);
	cv::Mat frame;
	if (cap.isOpened())
	{
		while (true)
		{
			cap >> frame;
			if (frame.empty())
				return 0;
			PeopleDetect1(frame);
			if (cv::waitKey(1) == 27)
				return 0;
		}
	}

}
#endif

#if 0
/*单独进行滞留物判断*/
void ProecssImage1(Mat &src)
{
	//double t = clock();
	if (src.data == NULL)
		return;
	cv::Mat img_input = src;
	cv::Mat cv_fg;
	cv::Mat cv_bg;
	//cout << clock() - t << endl;
	//t = clock();
	bgs->setShowOutput(true);
	bgs->process(img_input, cv_fg, cv_bg); //处理后得到前景图和背景图.
	//cv::cvtColor(img_input, img_input, COLOR_BGR2GRAY);
	cv::Mat output = img_input.clone();
	//centerpoints.clear();// 不考虑人体检测.
	ObjectDetect(cv_fg, output, centerpoints);
	imshow("前景", cv_fg);
	imshow("遗留物", output);
	//cout << clock() - t << endl;
	//src = output;
	cv::waitKey(1);
}

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		cout << "缺少参数" << endl;
		Sleep(2000);
		return 0;
	}
	/*初始化两个模块*/
	inittracker(track);//初始化遗留物检测模块。
	cv::VideoCapture cap(argv[1]);
	cv::Mat frame;
	//double t = clock();
	if (cap.isOpened())
	{
		while (true)
		{
			cap >> frame;
			if (frame.empty())
				return 0;
			ProecssImage1(frame);
			//cout << clock() - t << endl;
			//t = clock();
		}
	}
}
#endif