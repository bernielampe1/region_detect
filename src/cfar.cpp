#include<iostream>
#include<fstream>
#include<string>
#include<set>
#include<stdlib.h>

using namespace std;

#include"common.h"
#include"cfar.h"
#include"Image.h"
#include"PixelRegion.h"

int main(int argc, char **argv)
{
  if (argc != 6)
  {
    cerr << "Usage: cfar <image.data> <delta> "
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
  Image<float> regionImg(im_f.rows(), im_f.cols());
  Image<float> deltaImg(im_f.rows(), im_f.cols());
  cfar(im_f, delta, sur_bufsize, sur_width, outBase, regionImg, deltaImg);

  // write out region image to file
  regionImg.writeImage((outBase + "_cfar.pgm").c_str());
  deltaImg.writeImage((outBase + "_deltacfar.pgm").c_str());

  return(0);
}

