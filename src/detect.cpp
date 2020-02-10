#include<iostream>
#include<fstream>
#include<string>
#include<set>
#include<stdlib.h>

using namespace std;

#include"common.h"
#include"detect.h"
#include"Image.h"
#include"PixelRegion.h"

#define COLOR 1

int main(int argc, char **argv)
{
  if (argc != 6)
  {
    cerr << "Usage: detect <image.data> <delta> "
         << "<sur_bufsize(pixels)> <sur_width(pixels)> <outBaseStr>" << endl;
    exit(-1);
  }

  // parse parameters
  string imgFname = argv[1];
  double delta = atof(argv[2]);
  signed sur_bufsize = atoi(argv[3]);
  signed sur_width = atoi(argv[4]);
  string outBase = argv[5];

  // read and byte swap
  Image<float> im_f;
  im_f.readImage(imgFname.c_str());

  // run detection algorithm
  set< PixelRegion<float> > regions;
  detect(im_f, delta, sur_width, sur_bufsize, outBase, regions);

#if COLOR
  // write out regions to color image file
  ofstream outfile;
  outfile.open((outBase + "_colorRegions.ppm").c_str());

  // make color channels for image
  Image<uint8> regionImg_r(im_f.rows(), im_f.cols());
  Image<uint8> regionImg_g(im_f.rows(), im_f.cols());
  Image<uint8> regionImg_b(im_f.rows(), im_f.cols());
#endif

  Image<float> regionImg(im_f.rows(), im_f.cols());
  for(set<PixelRegion<float> >::iterator it = regions.begin();
      it != regions.end(); ++it)
  {
    #if COLOR
    // pick random color
    uint8 red = rand() % 255;
    uint8 green = rand() % 255;
    uint8 blue = rand() % 255;
    #endif

    for(set<Pixel<float> >::iterator jt = it->begin(); jt != it->end(); ++jt)
    {
      signed r = jt->getRow();
      signed c = jt->getCol();
      regionImg(r, c) = im_f(r, c);

      #if COLOR
      regionImg_r(r, c) = red;
      regionImg_g(r, c) = green;
      regionImg_b(r, c) = blue;
      #endif
    }
  }
  regionImg.writeImage((outBase + "_regions.pgm").c_str());

#if COLOR
  // write out ppm image
  outfile << "P3\n" << regionImg.cols() << " " << regionImg.rows() << "\n255\n";
  for(signed r = 0; r < regionImg.rows(); ++r)
  {
    for(signed c = 0; c < regionImg.cols(); ++c)
    {
      outfile << (int)regionImg_r(r, c) << " "
              << (int)regionImg_g(r, c) << " "
              << (int)regionImg_b(r, c) << " ";
    }
  }

  // close color image
  outfile.close();
#endif

  return(0);
}

