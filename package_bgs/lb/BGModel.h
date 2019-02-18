
#pragma once
/*背景建模方法的基类,声明基本的函数接口*/
#include <opencv2/opencv.hpp>
#include <math.h>
#include <float.h>

#include "Types.h"

namespace lb_library
{
  class BGModel
  {
  public:

    BGModel(int width, int height);
    virtual ~BGModel();

    void InitModel(IplImage* image);
    void UpdateModel(IplImage* image);

    virtual void setBGModelParameter(int id, int value) {};

    virtual IplImage* GetSrc();
    virtual IplImage* GetFG();
    virtual IplImage* GetBG();

  protected:

    IplImage* m_SrcImage;
    IplImage* m_BGImage;
    IplImage* m_FGImage;

    const unsigned int m_width;
    const unsigned int m_height;

    virtual void Init() = 0;
    virtual void Update() = 0;
  };
}
