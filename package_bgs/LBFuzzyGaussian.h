
#pragma once

#include "IBGS.h"
#include "lb/BGModelFuzzyGauss.h"

using namespace lb_library;
using namespace lb_library::FuzzyGaussian;

namespace bgslibrary
{
	namespace algorithms
	{
		class LBFuzzyGaussian : public IBGS
		{
		private:
			BGModel* m_pBGModel;
			int sensitivity;
			int bgThreshold;
			int learningRate;
			int noiseVariance;

		public:
			LBFuzzyGaussian();
			~LBFuzzyGaussian();

			void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

		private:
			void saveConfig();
			void loadConfig();
		};
	}
}
