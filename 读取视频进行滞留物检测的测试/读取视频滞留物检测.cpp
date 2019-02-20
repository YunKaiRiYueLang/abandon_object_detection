/*
��ȡһ������������Ƶ������������
*/

#include<opencv2/opencv.hpp>
#include<time.h>
#include<thread>
#include<queue>
#define NOMINMAX/*Ϊ�˽��min max�Ķ����ͻ*/
#include<Windows.h>
#include"detectabandon.h"
#include"C4_people_detect.h"
#include"package_bgs/LBFuzzyGaussian.h"

using namespace ibgs;
IBGS* bgs = new LBFuzzyGaussian;/*�������ģ�Ͷ���*/
IntImage<double> original;
std::vector<cv::Point> centerpoints;

using cv::Mat;
using std::cout;
using std::endl;
using std::cerr;

/*
Description:���˼��,�����ĵ������ĵ�洢��ȫ�ֱ���centerpoints��.
Input:�����ÿ֡ͼ��
�ú�������ProcessImage�������ý������˼��
*/
void PeopleDetect(Mat pImg)
{
	original.Load(pImg);
	std::vector<CRect> results;
	scanner.FastScan(original, results, 2);
	PostProcess(results, 2);
	PostProcess(results, 0);
	RemoveCoveredRectangles(results);//�Ƴ��ڲ��ķ���(��Ϊnms��������ԭ��.��Ҫ���Ƿ������ڲ�)

	for (size_t i = 0; i < results.size(); i++)
	{
		centerpoints.push_back(cv::Point((results[i].left + results[i].right) / 2, (results[i].top + results[i].bottom) / 2));
		cv::rectangle(pImg, cv::Point2d(results[i].left, results[i].top), cv::Point2d(results[i].right, results[i].bottom), cv::Scalar(0, 255, 0), 2);
	}
	cv::imshow("result", pImg);
}

/*
description:������Ƶ��ÿһ֡ͼ�������������
Input:һ֡Matͼ��
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
	bgs->process(img_input, cv_fg, cv_bg); //�����õ�ǰ��ͼ�ͱ���ͼ.
	//cv::cvtColor(img_input, img_input, COLOR_BGR2GRAY);
	cv::Mat output = img_input.clone();
	//centerpoints.clear();// ������������.
	ObjectDetect(cv_fg, output, centerpoints);
	imshow("ǰ��", cv_fg);
	imshow("������", output);
	//cout << clock() - t << endl;
	//src = output;
	cv::waitKey(1);
}

#if 1
/*�������,��Ҫָ��һ����Ƶ·��*/
int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		cout << "ȱ�ٲ���" << endl;
		Sleep(2000);
		return 0;
	}
	/*��ʼ������ģ��*/
	inittracker(track);//��ʼ����������ģ�顣
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
/*�����������˼�⹦��*/
void PeopleDetect1(Mat pImg)
{
	original.Load(pImg);
	std::vector<CRect> results;
	scanner.FastScan(original, results, 2);
	PostProcess(results, 2);
	PostProcess(results, 0);
	RemoveCoveredRectangles(results);// resultsΪ��⵽�Ľ������

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
		cerr << "ȱ�ٲ���������Ƶ��·������" ;
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
/*���������������ж�*/
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
	bgs->process(img_input, cv_fg, cv_bg); //�����õ�ǰ��ͼ�ͱ���ͼ.
	//cv::cvtColor(img_input, img_input, COLOR_BGR2GRAY);
	cv::Mat output = img_input.clone();
	//centerpoints.clear();// ������������.
	ObjectDetect(cv_fg, output, centerpoints);
	imshow("ǰ��", cv_fg);
	imshow("������", output);
	//cout << clock() - t << endl;
	//src = output;
	cv::waitKey(1);
}

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		cout << "ȱ�ٲ���" << endl;
		Sleep(2000);
		return 0;
	}
	/*��ʼ������ģ��*/
	inittracker(track);//��ʼ����������ģ�顣
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