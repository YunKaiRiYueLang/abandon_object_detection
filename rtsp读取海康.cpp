/*�����֧��rtspЭ��,���ú�����sdkҲ���Ի�ȡ����ͼ��*/
/*ʹ��queue��֤ͼ��Ĵ���˳��FIFO,������ٶ�������Ҫ�ȴ��͵��ٶȿ�*/

#include<opencv2/opencv.hpp>
#include<time.h>
#include<thread>
#include<queue>

#include"detectabandon.h"
#include"C4_people_detect.h"
#include"package_bgs/LBFuzzyGaussian.h"

using cv::Mat;
using std::queue;
using namespace ibgs;

cv::Mat frame;
queue<cv::Mat> queuesrc;
IBGS* bgs = new LBFuzzyGaussian;
IntImage<double> original;

void proecssimage(Mat &src)
{
	std::vector<cv::Point> centerpoints;
	cv::Mat img_input = src;
	cv::Mat cv_fg;
	cv::Mat cv_bg;
	bgs->setShowOutput(true);
	bgs->process(img_input, cv_fg, cv_bg); //�������ǵĵ���ǰ��ͼƬ
	//cv::cvtColor(img_input, img_input, COLOR_BGR2GRAY);//�õ���ԭͼ�ĻҶ�ͼ��
	cv::Mat output = img_input.clone();
	ObjectDetect(cv_fg, output, centerpoints);// ��ʱ��õ����Ѿ��������ﻭ���ͼ�ˡ�
	imshow("ǰ��", cv_fg);
	imshow("������", output);
	//src = output;
	//waitKey(1);
}

void secondthread()
{
	for(;;)
	{

		if (queuesrc.empty())		{continue;}		/*���stack�����Ƿ�Ϊ��*/
		Mat image = queuesrc.front();
		queuesrc.pop();
		if (image.data == NULL) { continue; }		/*���ͼ���Ƿ�Ϊ��*/

		cv::resize(image, image, cv::Size(320, 240));
		proecssimage(image);
		cv::imshow("test", image);
		if (cv::waitKey(1)==27)
		{
			exit(0);
		}
		std::cout << "test" << std::endl;
	}
}

int main()
{
	cv::VideoCapture HkCamera("rtsp://admin:a1234567@192.168.8.6/h256/ch1/sub/av_stream");
	double t1 = clock();
	int num = 100;
	int index = 1;
	bool firststart = true;
	inittracker(track);//��ʼ����������ģ�顣

	while (true)
	{
		HkCamera >> frame;
		queuesrc.push(frame);
		cv::imshow("hkcamera", frame);
		if (firststart)
		{
			firststart = false;
			std::thread process(secondthread);
			process.detach();
		}
		cv::waitKey(10);
		index++;
		if (index == 100)
		{
			std::cout << "��ȡ100֡��ʱ" << clock() - t1 << std::endl;
			std::cout << frame.size() << std::endl;
			t1 = clock();
			index = 0;
		}
	}
}