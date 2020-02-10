#ifndef _PIXELREGION_H_
#define _PIXELREGION_H_

#include<set>

using namespace std;

#include"Pixel.h"

template< typename T > class PixelRegion: public set< Pixel< float > >
{
  private:
    Pixel<T> parent;
    float metric;

  public:
    PixelRegion():metric(0) {}

    void setParent(const Pixel<T> &p) { this->parent = p; }
    Pixel<float>& getParent() { return(this->parent); }
    void setMetric(const float &f) { this->metric = f; }
    float getMetric() const { return(this->metric); }

    bool operator<(const PixelRegion &rhs) const
    { return(this->parent < rhs.parent); }

    bool operator>(const PixelRegion &rhs) const
    { return(this->parent > rhs.parent); }

    void computeStats(float &mean, float &sig) const
    {
      float acc = 0;
      float accsq = 0;
      int count = 0;
      for(typename set<Pixel<T> >::const_iterator it = this->begin();
          it != this->end(); it++)
      {
        acc += it->value();
        accsq += it->value() * it->value();
        ++count;
      }

      mean = acc / float(count);
      sig = accsq / float(count) - mean * mean;
    }

    void computeSurroundingStats(const Image<float> &I, const unsigned swidth,
                                 const unsigned sbuf, float &smean, float &ssig
                                ) const
    {
      float acc = 0;
      float accsq = 0;
      int count = 0;
      set<Pixel<float> > pixs;

      // get pixels
      this->getSurroundingPixels(I, swidth, sbuf, pixs);

      // compute moments
      for(typename set<Pixel<float> >::const_iterator it = pixs.begin();
          it != pixs.end(); ++it)
      {
        acc += it->value();
        accsq += it->value() * it->value();
        ++count;
      }

      smean = acc / float(count);
      ssig = accsq / float(count) - smean * smean;
    }

    void getSurroundingPixels(const Image<float> &I, const unsigned swidth,
                              const unsigned sbuf, set< Pixel<float> > &pixs
                             ) const
    {
      int max_col = 0;
      int min_col = I.cols();
      int max_row = 0;
      int min_row = I.rows();

      for(typename set<Pixel<T> >::const_iterator it = this->begin();
          it != this->end(); it++)
      {
        if (it->getCol() < min_col) min_col = it->getCol();
        if (it->getCol() > max_col) max_col = it->getCol();
        if (it->getRow() < min_row) min_row = it->getRow();
        if (it->getRow() > max_row) max_row = it->getRow();
      }

      min_col -= 2 * (swidth + sbuf) + 4;
      max_col += 2 * (swidth + sbuf) + 4;
      min_row -= 2 * (swidth + sbuf) + 4;
      max_row += 2 * (swidth + sbuf) + 4;

      // create a small image for the with rendered region pixels
      Image<uint8> region_image(max_row - min_row, max_col - min_col);
      for(typename set<Pixel<T> >::const_iterator it = this->begin();
          it != this->end(); it++)
      { region_image(it->getRow() - min_row, it->getCol() - min_col) = 0xFF; }

      // dilate region to cover surround buffer
      for(unsigned i = 0; i < sbuf; i++) this->dilate(region_image);

      // dilate surround image
      Image<uint8> sur_image = region_image;
      for(unsigned i = 0; i < sbuf + swidth; i++) this->dilate(sur_image);

      // subtract images
      sur_image -= region_image;

      // get pixels from image
      for(int r = 0; r < max_row - min_row; r++)
      {
        for(int c = 0; c < max_col - min_col; c++)
        {
          if (sur_image(r,c))
          {
            int rp = r + min_row;
            int cp = c + min_col;
            if (I.inbounds(rp, cp))
              pixs.insert(Pixel<float>(rp, cp, I(rp, cp)));
          }
        }
      }
    }

    // 4-connected dilate
    void dilate(Image<uint8> &I) const
    {
      // copy image, only dilate original that is returned
      Image<uint8> dimg(I);

      // optimization for looping and indexing
      int nrows = I.rows();
      int ncols = I.cols();
      int nrows_1 = nrows-1;
      int ncols_1 = ncols-1;

      // optimization get data ptrs
      unsigned char *data = I.data();
      unsigned char *ref_data= dimg.data();

      // first corner
      if (ref_data[0])
      {
        data[1] = 0xFF;
        data[ncols] = 0xFF;
      }

      // second corner
      if (ref_data[ncols_1])
      {
        data[ncols_1-1] = 0xFF;
        data[ncols+ncols_1] = 0xFF;
      }

      // third corner
      if (ref_data[nrows_1 * ncols])
      {
        data[nrows_1 * ncols + 1] = 0xFF;
        data[(nrows_1-1) * ncols] = 0xFF;
      }

      // fourth corner
      if (ref_data[nrows_1 * ncols + ncols_1])
      {
        data[nrows_1 * ncols + ncols_1-1] = 0xFF;
        data[(nrows_1-1) * ncols + ncols_1] = 0xFF;
      }

      // do first and last row
      for(int c = 1; c < ncols_1; ++c)
      {
        if (ref_data[c])
        {
          data[c-1] = 0xFF;
          data[c+1] = 0xFF;
          data[ncols + c] = 0xFF;
        }

        if (ref_data[nrows_1 * ncols + c])
        {
          data[nrows_1 * ncols + c - 1] = 0xFF;
          data[nrows_1 * ncols + c + 1] = 0xFF;
          data[(nrows_1-1) * ncols + c] = 0xFF;
        }
      }

      // do first and last col
      for(int r = 1; r < nrows_1; ++r)
      {
        if (ref_data[r * ncols])
        {
          data[(r-1) * ncols] = 0xFF;
          data[(r+1) * ncols] = 0xFF;
          data[r * ncols + 1] = 0xFF;
        }

        if (ref_data[r * ncols + ncols_1])
        {
          data[(r-1) * ncols + ncols_1] = 0xFF;
          data[(r+1) * ncols + ncols_1] = 0xFF;
          data[r * ncols + ncols_1 - 1] = 0xFF;
        }
      }

      // do interior pixels
      for(int r = 1; r < nrows_1; ++r)
      {
        for(int c = 1; c < ncols_1; ++c)
        {
          if (ref_data[r * ncols + c])
          {
            data[r * ncols + c-1] = 0xFF;
            data[r * ncols + c+1] = 0xFF;
            data[(r-1) * ncols + c] = 0xFF;
            data[(r+1) * ncols + c] = 0xFF;
          }
        }
      }
    }

    ostream& print(ostream &os) const
    {
      os << " R " << this->parent.getRow() << " C " << this->parent.getCol()
         << " V " << this->parent.value() << " M " << this->metric;

      return(os);
    }
};

template< typename T >
ostream& operator<<(ostream &os, const PixelRegion<T> &r) {return(r.print(os));}

#endif // _PIXELREGION_H_

