
#pragma once
/*������ʾ�����麯���ӿڵĻ���*/
#include <iostream>
#include <fstream>
#include <list>
#include <opencv2/opencv.hpp>
#include <cv.h>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

#ifndef CV_RGB
  #define  CV_RGB(r, g, b)   cv::Scalar((b), (g), (r), 0)
#endif

namespace bgslibrary
{
  namespace algorithms
  {
    class IBGS
    {
    public:
      void setShowOutput(const bool _showOutput) {
        showOutput = _showOutput;
      }
      cv::Mat apply(const cv::Mat &img_input) {
        setShowOutput(false);
        cv::Mat _img_foreground;
        cv::Mat _img_background;
        process(img_input, _img_foreground, _img_background);
                _img_background.copyTo(img_background);
        return _img_foreground;
      }
      cv::Mat getBackgroundModel() {
        return img_background;
      }
      virtual void process(const cv::Mat &img_input, cv::Mat &img_foreground, cv::Mat &img_background) = 0;
      virtual ~IBGS() {}
      //static IBGS* create(const std::string alg_name);
      //static std::list<std::string> get_algs_name();
    protected:
      bool firstTime = true;
      bool showOutput = true;
      cv::Mat img_background;
      cv::Mat img_foreground;
      std::string config_xml;
      void setup(const std::string _config_xml) {
        config_xml = _config_xml;
        if (!config_xml.empty()) {
          if (!std::ifstream(config_xml))
            saveConfig();
          loadConfig();
        }
      }
      void init(const cv::Mat &img_input, cv::Mat &img_outfg, cv::Mat &img_outbg) {
        assert(img_input.empty() == false);
        //img_outfg = cv::Mat::zeros(img_input.size(), img_input.type());
        //img_outbg = cv::Mat::zeros(img_input.size(), img_input.type());
        img_outfg = cv::Mat::zeros(img_input.size(), CV_8UC1);
        img_outbg = cv::Mat::zeros(img_input.size(), CV_8UC3);
      }
    
    private:
      virtual void saveConfig() = 0;
      virtual void loadConfig() = 0;
    };
  }
}

namespace ibgs = bgslibrary::algorithms;
