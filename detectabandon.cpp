#include"detectabandon.h"
#include<cmath>
using std::sqrt;
std::vector<tracker> track;
bool firstframe = true;
int maxobjectnumber = 200;
float areaChangeFraction = 0.1;
float centroidChangeFraction = 0.1;
int maxConsecutiveMiss = 5;
int outcount = 0;
const int alarmcount = 20;
bool aaa = false;

void inittracker(std::vector<tracker> &track)
{
	for (int i = 0; i < maxobjectnumber; i++)
	{
		tracker track1;
		track.push_back(track1);
	}
}

/*�б�������
Input: �ͱ����������Ŀ�� invomerois
Output: ���ĵ������Ｏ��outbox
ǰ���������õ���track�ṹ����Ҫ����Ƶ��ʼǰ����inittracker��������һ�γ�ʼ����
*/

std::vector <cv::Rect> peopleedge;
void ObjTracker(std::vector<incomeroi> &incomerois, std::vector<Rect> &outbox)
{
	outcount = 0;

	outbox.resize(maxobjectnumber);
	for (int i = 0; i < maxobjectnumber; i++)
	{
		outbox[i] = Rect(1, 1, 0, 0);
	}
	if (incomerois.size() != 0)
	{
		for (int incomeroisi = 0; incomeroisi < incomerois.size(); incomeroisi++)
		{
			Point incentroid = incomerois[incomeroisi].centroid / 2;
			double inarea = incomerois[incomeroisi].area / 2;
			bool found = false;
			for (int j = 0; j < maxobjectnumber; j++)
			{
				if (track[j].isTrackActive)
				{
					double trackarea = track[j].area;
					Point2f trackcentroid = track[j].centroid;
					float arearatio = abs(trackarea - inarea) / (trackarea + 1);
					Point cendiff = Point(incentroid.x - trackcentroid.x, incentroid.y - trackcentroid.y);
					float cenratio = (cendiff.x*cendiff.x + cendiff.y*cendiff.y) / inarea;
					if (arearatio < areaChangeFraction&&cenratio < centroidChangeFraction)
					{
						found = true;
						track[j].justHit = true;
						track[j].hitCount++;
						break;
					}
				}
			}
			if (!found)
			{
				int k = 0;
				for (int m = 0; m < maxobjectnumber; m++)
				{
					if (!track[m].isTrackActive)
					{
						k = m;
						break;
					}
				}
				track[k].area = inarea;
				track[k].centroid = incentroid;
				track[k].bbox = incomerois[incomeroisi].box;
				track[k].age = 1;
				track[k].hitCount = 1;
				track[k].missCount = 0;
				track[k].justHit = true;
				track[k].isTrackActive = true;
			}
		}
	}
	for (int i = 0; i < maxobjectnumber; i++)
	{
		if (track[i].isTrackActive)
		{
			track[i].age++;
			if (track[i].justHit)
			{
				track[i].justHit = false;
				track[i].missCount = 0;
			}
			else
			{
				track[i].missCount++;
			}
			bool deleteMissFlag = bool(track[i].missCount > maxConsecutiveMiss);
			bool deleteRatioFlag = bool(track[i].hitCount < track[i].age*0.3);
			if (deleteMissFlag || deleteRatioFlag)
			{
				track[i].isTrackActive = false;
			}
		}
		if (track[i].hitCount >= alarmcount)
		{
			outcount++;
			outbox[outcount] = track[i].bbox;
			// cout << track[i].bbox << endl;
		}
	}
}
/*���ݵ�ǰ֡������ģ������仯��Ŀ��
Input:��ǰ֡frame������ģ��bg��
Output:��⵽�����б任��ͼ��顣
*/
void Detect(cv::Mat &frame, cv::Mat &bg, std::vector<incomeroi> &incomerois)
{
	assert(frame.channels() == 1 || frame.channels() == 3);

	cv::Mat segment = cv::Mat::zeros(frame.size(), frame.type());
	//get segment
	if (frame.channels() == 3)
	{
		for (int row = 0; row < frame.rows; row++)
		{
			uchar *prow = frame.ptr<uchar>(row);
			uchar *bgrow = bg.ptr<uchar>(row);
			uchar *segm = segment.ptr<uchar>(row);
			for (int col = 0; col < frame.cols; col++)
			{
				segm[col * 3] = abs(bgrow[col * 3] - prow[col * 3]);

				segm[col * 3 + 1] = abs(bgrow[col * 3 + 1] - prow[col * 3 + 1]);
				/*if (segm[col * 3+1] < 100)
				{
					segm[col * 3+1] = 0;
				}
				else
				{
					segm[col * 3+1] = 255;
				}*/
				segm[col * 3 + 2] = abs(bgrow[col * 3 + 2] - prow[col * 3 + 2]);
				/*	if (segm[col * 3+2] < 100)
					{
						segm[col * 3+2] = 0;
					}
					else
					{
						segm[col * 3 + 2] = 255;
					}*/
			}
		}
		vector<Mat> ycbcr;
		split(segment, ycbcr);

		threshold(ycbcr[0], ycbcr[0], 0, 255, THRESH_OTSU);
		threshold(ycbcr[1], ycbcr[1], 0, 255, THRESH_OTSU);
		threshold(ycbcr[2], ycbcr[2], 0, 255, THRESH_OTSU);

		/*threshold(ycbcr[0], ycbcr[0], 10, 255, THRESH_BINARY);
		threshold(ycbcr[1], ycbcr[1], 10, 255, THRESH_BINARY);
		threshold(ycbcr[2], ycbcr[2], 10, 255, THRESH_BINARY);*///��Ȼ�����ҳ�ǰ���������ֵ�޷�����Ӧ�ı䰡
		/*	for (int cbcri = 0; cbcri < ycbcr[1].rows; cbcri++)
			{
				uchar cbcr
			}*/
		cv::imshow("y", ycbcr[0]);
		cv::imshow("cr", ycbcr[1]);
		cv::imshow("cb", ycbcr[2]);
		segment = Mat::zeros(segment.size(), CV_8UC1);
		segment = ycbcr[0] + ycbcr[1]; //+ ycbcr[2];
		//segment = ycbcr[0];
	}
	else
	{
		for (int row = 0; row < frame.rows; row++)
		{
			uchar *prow = frame.ptr<uchar>(row);
			uchar *bgrow = bg.ptr<uchar>(row);
			uchar *segm = segment.ptr<uchar>(row);
			for (int col = 0; col < frame.cols; col++)
			{
				segm[col] = abs(bgrow[col] - prow[col]);
				if (segm[col] < 50)
				{
					segm[col] = 0;
				}
				else
				{
					segm[col] = 255;
				}
			}
		}
	}
	//open operator
	cv::Mat element = getStructuringElement(MORPH_RECT, cv::Size(5, 5));
	cv::erode(segment, segment, element);
	cv::dilate(segment, segment, element);
	namedWindow("seg", WINDOW_NORMAL);
	cv::imshow("seg", segment);
	cv::waitKey(1);
	//Blob Analysis
	std::vector<std::vector<cv::Point>> contours;
	incomeroi oneroi;
	cv::findContours(segment, contours, RETR_LIST, CHAIN_APPROX_NONE);
	//cv::drawContours(frame, contours, -1, Scalar(255, 0, 0), 3, 8);
	//cv::imshow("all_obj",frame);
	for (int i = 0; i < contours.size(); i++)
	{
		oneroi.area = contourArea(contours[i]);
		Moments mu;
		mu = moments(contours[i], false);
		//��������������
		oneroi.centroid = Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);
		minAreaRect(contours[i]);
		Rect rect = boundingRect(contours[i]);
		oneroi.box = rect;
		//rectangle(frame, oneroi.box, Scalar(255, 0, 0), 3, 3);
		//cv::imshow("all_obj", frame);
		incomerois.push_back(oneroi);
	}
}

/*���㱳��ģ��
Input:��ǰ֡frame,����background
Output:background
*/
void StoreBackground(cv::Mat &frame, cv::Mat &background)
{
	CV_Assert(frame.channels() == 1 || frame.channels() == 3);
	assert(frame.channels() == 1 || frame.channels() == 3);
	if (firstframe)
	{
		firstframe = false;
		background = frame.clone();
	}
}

bool ObjectDetect(cv::Mat fg, cv::Mat &output, std::vector<cv::Point> &outrects)
{
	//outrects.clear();
	bool ret = false;
	if (output.data == NULL)
	{
		output = cv::Mat(fg.size(), fg.type());
		output = fg.clone();
	}
	if (fg.channels() != 3 && fg.channels() != 1)
	{
		return ret;
	};
	if (fg.data != NULL)
	{
		if (fg.channels() == 3)
		{
			cv::cvtColor(fg, fg, COLOR_BGR2GRAY);
		}
		/*cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
		cv::erode(fg, fg, element);*/
		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(fg, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
		vector<incomeroi> incomerois;
		incomeroi oneroi;
		for (int i = 0; i < contours.size(); i++)
		{
			/*for (int pointj = 0; outrects.size(); pointj++)
			{
				if (pointPolygonTest(contours[i], outrects[pointj], false) == 1)
				{
					continue;//���������ķ�ʽ��̫��. ��Ϊ�ж��������ĵ�֮��ľ���������϶���.
				}
			}*/
			
			oneroi.area = cv::contourArea(contours[i]);
			if (oneroi.area < 10) continue;/*С��10�����壬ֱ�ӿ����㣬�ų���*/
			cv::Moments mu;
			mu = cv::moments(contours[i], false);
			//��������������
			oneroi.centroid = cv::Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);
			bool abandon = false;
			for (int pointj = 0;pointj<outrects.size(); pointj++)
			{
				double distance = sqrt(std::pow(oneroi.centroid.x - outrects[pointj].x,2) + std::pow(oneroi.centroid.y - outrects[pointj].y,2));
				if (distance > 10)/*�ж�������������֮��Ĺ�ϵ*/
				{
					abandon = true;
				}
			}
			if (!abandon)
			{
				continue;
			}
			cv::minAreaRect(contours[i]);
			cv::Rect rect = cv::boundingRect(contours[i]);
			oneroi.box = rect;
			incomerois.push_back(oneroi);
		}
		std::vector<cv::Rect> rects;
		ObjTracker(incomerois, rects);

		for (int recti = 0; recti < rects.size(); recti++)
		{
			if (rects[recti].area() < 40)// ��С��Χ���ε����С��40Ҳ������.�����û����Ϊ�ⶨ
			{
				continue;
			}
			//outrects.push_back(rects[recti]);
			cv::Point p1 = cv::Point(rects[recti].tl().x, rects[recti].tl().y);
			cv::Point p2 = cv::Point(rects[recti].br().x, rects[recti].br().y);
			cv::rectangle(output, p1, p2, cv::Scalar(255, 0, 0), 3, 8);
		}
		ret = true;
	}

	return ret;
}