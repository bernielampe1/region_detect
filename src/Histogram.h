#ifndef _HISTOGRAM_H_
#define _HISTOGRAM_H_

#include<algorithm>
#include<map>
#include<vector>
#include<iostream>
#include<string.h>
#include<math.h>

using namespace std;

template<typename T> class Histogram
{
  private:
    signed *hist; // histogram
    signed numBins; // # of bins
    double binWidth; // width of bins
    double minVal, maxVal; // range

  public:
    Histogram(): hist(0), numBins(0), binWidth(0), minVal(0), maxVal(0) {}

    // accessors
    signed getNumBins() { return(numBins); }

    // get count in bin number
    signed operator[](const signed i) const { return(hist[i]); }

    // used freedman-diaconis rule for estimating optimal bin width
    void set(const T *v, const signed n)
    {
      // copy values into vector and sort
      vector<T> v_t;
      v_t.assign(v, v+n);
      sort(v_t.begin(), v_t.end());
      minVal = v_t[0];
      maxVal = *v_t.rbegin();

      // get the quartiles
      double qwidth = signed(n / 4.0 + 0.5);
      T q1 = v_t[qwidth - 1];
      T q3 = v_t[3 * qwidth - 1];

      double n13 = pow(double(n), 1.0/3.0);
      double r = maxVal - minVal;
      double d = 2.0 * (q3 - q1);

      if (d > 0 && r > 0)
      {
        numBins = signed(ceil(n13 * r / d));
        binWidth = r / numBins;
      }
      else
      {
        numBins = signed(r +1);
        binWidth = 1.0;
      }

      // allocate
      hist = new signed[numBins];
      memset(hist, 0, numBins);

      // assign
      for(int i = 0; i < n; ++i)
      {
        int b = int((v[i] - minVal) / binWidth);
        if (b >= numBins) b = numBins - 1;
        hist[b]++;
      }
    }

    ~Histogram()
    {
      if (hist != 0) { delete [] hist; }
      hist = 0;
      numBins = 0;
      binWidth = minVal = maxVal = 0;
    }

    ostream& print(ostream &os) const
    {
      os << "* Histogram:\n* Num Bins:" << numBins << "\n";
      for(int i = 0; i < numBins; ++i)
      {
        double v = minVal + i * binWidth;
        os << " " << i << " " << v << " " << hist[i] << endl;
      }

      return(os);
    }
};

template< typename T >
ostream& operator<<(ostream &os, const Histogram<T> &h) { return(h.print(os)); }

#endif // _HISTOGRAM_H_

