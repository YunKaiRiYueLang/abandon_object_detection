#include "LBFuzzyGaussian.h"

using namespace bgslibrary::algorithms;
/*列表初始化了一些变量,设置配置文件*/
LBFuzzyGaussian::LBFuzzyGaussian() :
	sensitivity(72), bgThreshold(162), learningRate(49), noiseVariance(195)
{
	std::cout << "LBFuzzyGaussian()" << std::endl;
	setup("./config/LBFuzzyGaussian.xml");
}

LBFuzzyGaussian::~LBFuzzyGaussian()
{
	delete m_pBGModel;
	std::cout << "~LBFuzzyGaussian()" << std::endl;
}

void LBFuzzyGaussian::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	init(img_input, img_output, img_bgmodel);

	IplImage *frame = new IplImage(img_input);

	if (firstTime)
	{
		int w = cvGetSize(frame).width;
		int h = cvGetSize(frame).height;

		m_pBGModel = new BGModelFuzzyGauss(w, h);
		m_pBGModel->InitModel(frame);
	}

	m_pBGModel->setBGModelParameter(0, sensitivity);
	m_pBGModel->setBGModelParameter(1, bgThreshold);
	m_pBGModel->setBGModelParameter(2, learningRate);
	m_pBGModel->setBGModelParameter(3, noiseVariance);

	m_pBGModel->UpdateModel(frame);

	img_foreground = cv::cvarrToMat(m_pBGModel->GetFG());
	img_background = cv::cvarrToMat(m_pBGModel->GetBG());

#ifndef MEX_COMPILE_FLAG
	if (showOutput)
	{
		cv::imshow("FG Mask", img_foreground);
		cv::imshow("FG Model", img_background);
	}
#endif

	img_foreground.copyTo(img_output);
	img_background.copyTo(img_bgmodel);

	delete frame;

	firstTime = false;
}

void LBFuzzyGaussian::saveConfig()
{
	CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

	cvWriteInt(fs, "sensitivity", sensitivity);
	cvWriteInt(fs, "bgThreshold", bgThreshold);
	cvWriteInt(fs, "learningRate", learningRate);
	cvWriteInt(fs, "noiseVariance", noiseVariance);
	cvWriteInt(fs, "showOutput", showOutput);

	cvReleaseFileStorage(&fs);
}

void LBFuzzyGaussian::loadConfig()
{
	CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

	sensitivity = cvReadIntByName(fs, nullptr, "sensitivity", 72);
	bgThreshold = cvReadIntByName(fs, nullptr, "bgThreshold", 162);
	learningRate = cvReadIntByName(fs, nullptr, "learningRate", 49);
	noiseVariance = cvReadIntByName(fs, nullptr, "noiseVariance", 195);
	showOutput = cvReadIntByName(fs, nullptr, "showOutput", true);

	cvReleaseFileStorage(&fs);
}