#ifndef _cfar_h_ 
#define _cfar_h_

#include<string>
#include<set>

using namespace std;

#include"Image.h"
#include"PixelRegion.h"

void computeSurroundingStats(const Image<float> &img, const signed r,
                             const signed c, const signed b, const signed w,
                             float &mean, float &var)
{
  float acc = 0;
  float accsq = 0;
  float val = 0;
  signed count = 0;

  signed start = - b - w;
  signed end = b + w;
  for(signed i = start; i < end; ++i)
  {
    for(signed j = start; j < end; ++j)
    {
      // check to see if indexed pixel is in guard cells
      if (i >= -b && i <= b && j >= -b && j <= b) continue;

      // check to see if indexed pixel is in image
      if (img.inbounds(r + i, c + j))
      {
        val = img(r + i, c + j);
        acc += val;
        accsq += val * val;
        ++count;
      }
    }
  }

  mean = acc / float(count);
  var = accsq / float(count) - mean * mean;
}

void cfar(const Image<float> &img, const double delta,
          const signed sur_bufsize, const signed sur_width,
          const string debugBase, Image<float> &regionImg,
          Image<float> &deltaImg)
{
  signed nrows = img.rows();
  signed ncols = img.cols();

  // do 3x3 median filter in seperable fashion
  Image<float> img_filtered = img;
  img_filtered.median();

  // do cfar
  float sur_mean, sur_variance;
  for(int r = 0; r < nrows; ++r)
  {
    for(int c = 0; c < ncols; ++c)
    {
      // get stats from surrounding cells
      computeSurroundingStats(img, r, c, sur_bufsize, sur_width,
                              sur_mean, sur_variance);

      // compare with existing pixel and see if above threshold
      float d = fabs(img_filtered(r, c) - sur_mean);

      // record delta
      deltaImg(r, c) = d;

      // if delta exceeds threshold then detection
      if (d > delta) { regionImg(r, c) = img(r, c); }
    }
  }
}

#endif // _cfar_h_

