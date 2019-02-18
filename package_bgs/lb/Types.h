
#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/core/core_c.h>

namespace lb_library
{
template<class T> class Image
{
private:
  IplImage* imgp;

public:
  Image(IplImage* img=0) {imgp=img;}
  ~Image(){imgp=0;}
  
  void operator=(IplImage* img) {imgp=img;}
  
  inline T* operator[](const int rowIndx)
  {
    return ((T *)(imgp->imageData + rowIndx*imgp->widthStep));
  }
};

typedef struct{
  unsigned char b,g,r;
} RgbPixel;

typedef struct{
  unsigned char Blue,Green,Red;
} BYTERGB;

typedef struct{
  unsigned int Blue,Green,Red;
} INTRGB;

typedef struct{
  float b,g,r;
}RgbPixelFloat;

typedef struct{
  double Blue,Green,Red;
} DBLRGB;

typedef Image<RgbPixel>       RgbImage;
typedef Image<RgbPixelFloat>  RgbImageFloat;
typedef Image<unsigned char>  BwImage;
typedef Image<float>          BwImageFloat;

}
