#ifndef _detect_h_
#define _detect_h_

#include<map>
#include<string>
#include<set>
#include<sstream>
#include<iomanip>

using namespace std;

#include"Histogram.h"
#include"Image.h"
#include"PixelRegion.h"
#include"DisjointSet.h"

#define DEBUG 0

void detect(const Image<float> &img, const double delta,
            const signed sur_width, const signed sur_bufsize,
            const string debugBase, set< PixelRegion<float> > &regions)
{
  signed nrows = img.rows();
  signed ncols = img.cols();
  signed npixels = nrows * ncols;
  signed offset_row[8] = {-1, -1, -1, 0, 0, 1, 1, 1 };
  signed offset_col[8] = { -1, 0, 1, -1, 1, -1, 0, 1};
  map< signed, PixelRegion<float> > regions_map;
  DisjointSet<unsigned> universe;
  signed n;

  // do 3x3 median filter in seperable fashion
  Image<float> img_filtered = img;
  img_filtered.median();

  // compute histogram
  Histogram<float> hist;
  hist.set(img_filtered.data(), npixels);

  // put pixels into a container and sort by value
  vector< Pixel< float > > pixels;
  pixels.reserve(npixels);
  for(signed r = 0; r < nrows; ++r)
  {
    for(signed c = 0; c < ncols; ++c)
      pixels.push_back(Pixel<float>(r, c, img_filtered(r * ncols + c)));
  }
  sort(pixels.rbegin(), pixels.rend());

  // create an image to keep track of set pixels
  Image<uint8> segmentedImage(nrows, ncols);

  // loop over the histogram bins
  vector< Pixel< float > >:: iterator cpix = pixels.begin();
  for(n = hist.getNumBins() - 1; n >= 0; --n)
  {
    if (!(n % 20))
    {
      cout << " * evaluating " << hist.getNumBins() - 1 - n << " of "
           << hist.getNumBins() << endl;
    }

    // loop over all pixels in this histogram bin
    for(signed i = 0; i < hist[n]; ++i, ++cpix)
    {
      // add pixel to disjoint set
      signed ind = cpix->getRow() * ncols + cpix->getCol();
      universe.make_set(ind);

      // set the pixel in the segmented image
      segmentedImage(cpix->getRow(), cpix->getCol()) = 0xFF;

      // check the 8 neighbors and merge if they are set
      for(signed k = 0; k < 8; ++k)
      {
        signed tr = cpix->getRow() + offset_row[k];
        signed tc = cpix->getCol() + offset_col[k];
        if (segmentedImage.inbounds(tr, tc) && segmentedImage(tr, tc))
        {
            signed a = universe.find(tr * ncols + tc);
            signed b = universe.find(ind);
            if (a != b) universe.join(a, b);
        }
      } // loop over pixel 4 connected neighborhood
    } // loop over pixels in bin

    #if DEBUG
    ostringstream oss;
    oss << "floodfill_" << setw(unsigned(ceil(log10(hist.getNumBins()-1))))
        << setfill('0') << hist.getNumBins() - n << ".pgm";
    segmentedImage.writeImage(oss.str().c_str());
    #endif

    // if the histogram had pixels in this bin
    if (hist[n] > 0)
    {
      // build a map of regions with universe index as first key and PixelRegion
      // as value
      regions_map.clear();
      for(signed r = 0; r < nrows; ++r)
      {
        for(signed c = 0; c < ncols; ++c)
        {
          if (segmentedImage(r, c))
          {
            signed a = universe.find(r * ncols + c);
            //signed s = universe.size(a);
            //if (s >= minsize && s <= maxsize) // TODO: removed for now
            //{
              Pixel<float> tpix(r, c, img_filtered(r, c));
              if (regions_map.find(a) == regions_map.end() ||
                  tpix > regions_map[a].getParent())
              {
                regions_map[a].setParent(tpix);
              }
              regions_map[a].insert(tpix);
            //}
          }
        }
      }

      // loop over all regions and see if they are greater than the background
      for(map<signed, PixelRegion<float> >::iterator it =
          regions_map.begin(); it != regions_map.end(); ++it)
      {
        // compute moments of region with respect to surround
        float region_mean = 0; float sur_mean = 0;
        float region_variance = 0; float sur_variance = 0;
        it->second.computeStats(region_mean, region_variance);
        it->second.computeSurroundingStats(img_filtered, sur_width, sur_bufsize,
                                           sur_mean, sur_variance);
        if (region_mean > sur_mean)
        {
          //float d = log(fabs(region_mean - sur_mean)); //TODO: try without log
          float d = fabs(region_mean - sur_mean);
          if (d > delta)// && regions.find(it->second) == regions.end())
          {
            it->second.setMetric(d);
            regions.insert(it->second);
          }
        }
      }
    } // if the hist[n] > 0
  } // loop over histogram bins
}

#endif // _detect_h_

