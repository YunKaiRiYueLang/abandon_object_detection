/*
This file is part of BGSLibrary.

BGSLibrary is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

BGSLibrary is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include "IBGS.h"
#include "BGModelFuzzyGauss.h"

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

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);/*调用m_pBGMmdel初始化和获取数据，更新模型得到结果*/

    private:/*读入写入配置文件的数据*/
      void saveConfig();
      void loadConfig();
    };
  }
}
