#ifndef _IMAGE_H_
#define _IMAGE_H_

#include<iostream>
#include<fstream>
#include<string>
#include<string.h>

using namespace std;

#include"common.h"
#include"Histogram.h"

template<typename T> class Image
{
  private:
    // data members
    T *_data;
    signed _nrows, _ncols;

    // allocation functions
    void _alloc(const signed nr , const signed nc)
    { 
      _dealloc();
      _nrows = nr; _ncols = nc;
      _data = new T[nr * nc];
      memset(_data, 0, nr * nc * sizeof(T));
    } 

    void _dealloc()
    {
       if (_data != 0) delete [] _data; 
       _nrows = _ncols = 0;
       _data = 0;
    }

  public:
    Image(): _data(0), _nrows(0), _ncols(0) {}
    ~Image() { _dealloc(); }

    Image(signed nr, signed nc): _data(0), _nrows(nr), _ncols(nc)
    {
      _alloc(nr, nc);
    }

    Image(T *d, signed nr, signed nc): _data(0), _nrows(nr), _ncols(nc)
    {
      _alloc(nr, nc);
      memcpy(_data, d, sizeof(T) * nr * nc);
    }

    Image(const Image<T> &other): _data(0), _nrows(0), _ncols(0)
    {
      _alloc(other._nrows, other._ncols);
      memcpy(_data, other._data, sizeof(T) * other._nrows * other._ncols);
    }

    Image<T>& operator=(const Image<T> &other)
    {
      if (&other != this)
      {
        _alloc(other._nrows, other._ncols);
        memcpy(_data, other._data, sizeof(T) * other._nrows * other._ncols);
      }

      return(*this);
    }

    // accessors
    signed rows() const { return(_nrows); }
    signed cols() const { return(_ncols); }
    T* data() const { return(_data); }

    T& operator()(const signed i) const
    { return _data[i]; }

    T& operator()(const signed r, const signed c) const
    { return _data[r * _ncols + c]; }

    // is index in bounds?
    bool inbounds(const int r, const int c) const
    { return(r >= 0 && r < _nrows && c >= 0 && c < _ncols); }

    // add 2 images of the same size
    Image<T>& operator+=(const Image<T> &other)
    {
      int c = _nrows * _ncols;
      for(int i = 0; i < c; i++)
        _data[i] += other._data[i];
      return(*this);
    }
 
    // subtract 2 images of the same size
    Image<T>& operator-=(const Image<T> &other)
    {
      int c = _nrows * _ncols;
      for(int i = 0; i < c; i++)
        _data[i] -= other._data[i];
      return(*this);
    }

    void readImage(const char *fname)
    {
      string fn = fname;
      string ext = fn.substr(fn.find_last_of(".")+1); // get file extension

      if (ext == "pgm") { this->readPgmImage(fname); }
      else if (ext == "magn") { this->readMagnImage(fname, true); }
      else
      {
        cerr << "Error: unknown image type: " << fname << endl;
        exit(1);
      }
    }

    // read 8-bit PGM image from file
    void readPgmImage(const char *fname)
    {
      // open file
      ifstream infile;
      infile.open(fname);

      // read image sizes
      string P5;
      int max;
      infile >> P5;
      infile >> _ncols;
      infile >> _nrows;
      infile >> max;

      // allocate
      _alloc(_nrows, _ncols);

      // read pixels on at a time
      signed npixels = _nrows * _ncols;
      uint8 pix;
      for(signed i = 0; i < npixels; ++i)
      {
        infile.read((char*)(&pix), sizeof(uint8));
        _data[i] = (T)pix;
      }

      infile.close();
    }

    // read CARABAS image from file
    void readMagnImage(const char *fname, const bool swap = false)
    {
      // open file
      ifstream infile;
      infile.open(fname, ios::binary);

      // read image sizes
      infile.read((char*)&(_nrows), sizeof(_nrows));
      infile.read((char*)&(_ncols), sizeof(_ncols));

      // allocate
      _alloc(_nrows, _ncols);

      // read pixels
      infile.read((char*)(_data), sizeof(T) * _nrows * _ncols);
      infile.close();

      // swap bytes
      if (swap) endianSwap();
    }

    // byte swap _data
    void endianSwap()
    {
      union pixel_t { T d; char bytes[4]; } p;
      signed npixels = _nrows * _ncols;
      for(signed i = 0; i < npixels; i++)
      {
        p.d = _data[i];
        SWAP(p.bytes[0], p.bytes[3]);
        SWAP(p.bytes[1], p.bytes[2]);
        _data[i] = p.d;
      }
    }

    void writeMagn(const char *fname) const
    {
      Image<T> I = *this;
      I.endianSwap();
      ofstream outfile;
      outfile.open(fname, ios::out | ios::binary);
      outfile.write((char*)&(I._nrows), sizeof(I._nrows));
      outfile.write((char*)&(I._ncols), sizeof(I._ncols));
      outfile.write((char*)(I._data), I._nrows * I._ncols * sizeof(T));
      outfile.close();
    }

    // write image as linearly scaled PGM
    void writeImage(const char *fname) const
    {
      // alloc new image
      Image<uint8> im_c = this->linear();

      // write to pgm
      ofstream outfile;
      outfile.open(fname);
      outfile << "P5\n" << im_c.cols() << " " << im_c.rows() << "\n255\n";
      outfile.write((char*)im_c.data(), im_c.rows() * im_c.cols());
      outfile.close();
    }

    // linear scale to 8 bytes
    Image<uint8> linear(const bool verbose = true) const
    {
      // find min and max pixels
      T min, max;
      min = max = _data[0];
      signed npixels = _nrows * _ncols;
      for(signed i = 1; i < npixels; i++)
      {
        if (_data[i] > max) max = _data[i];
        else if (_data[i] < min) min = _data[i];
      }

      if (verbose)
      {
        cout << " - Min pixel value = " << (int)min << endl;
        cout << " - Max pixel value = " << (int)max << endl;
      }

      // return blank image
      if (max <= min) { return(Image<uint8>(_nrows, _ncols)); }

      // scale pixels linearly between (0-255)
      Image<uint8> im_c(_nrows, _ncols);
      for(signed i = 0; i < npixels; i++)
        im_c(i) = (uint8)((((_data[i] - min) / (max - min)) * 255.0) + 0.5);

      return(im_c);
    }

    // median with 3x3 separable filter
    void median()
    {
      // embed into a larger image
      Image<T> embedded(_nrows+2, _ncols+2);
      for(signed r = 0; r < _nrows; ++r)
      {
        for(signed c = 0; c < _ncols; ++c)
          embedded(r+1, c+1) = (*this)(r, c);
      }

      // compute median filter in rows
      for(signed r = 1; r <= _nrows; ++r)
      {
        for(signed c = 1; c <= _ncols; ++c)
        {
          T v0 = embedded(r, c-1);
          T v1 = embedded(r, c);
          T v2 = embedded(r, c+1);

          T mid = v0;
          if ((v1 > v0 && v1 < v2) || (v1 > v2 && v1 < v0)) mid = v1;
          else if ((v2 > v0 && v2 < v1) || (v2 > v1 && v2 < v0)) mid = v2;
          embedded(r, c) = mid;
        }
      }

      // compute median filter in cols
      for(signed r = 1; r <= _nrows; ++r)
      {
        for(signed c = 1; c <= _ncols; ++c)
        {
          T v0 = embedded(r-1, c);
          T v1 = embedded(r, c);
          T v2 = embedded(r+1, c);

          T mid = v0;
          if ((v1 > v0 && v1 < v2) || (v1 > v2 && v1 < v0)) mid = v1;
          else if ((v2 > v0 && v2 < v1) || (v2 > v1 && v2 < v0)) mid = v2;
          embedded(r, c) = mid;
        }
      }

      // extract image from embedded to this
      for(signed r = 0; r < _nrows; ++r)
      {
        for(signed c = 0; c < _ncols; ++c)
          (*this)(r, c) = embedded(r+1, c+1);
      }
    }

    Histogram<T> histogram() const
    {
      Histogram<T> hist(_data, _nrows * _ncols);
      return(hist);
    }
};

#endif // _IMAGE_H_
