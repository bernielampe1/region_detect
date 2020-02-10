#include<iostream>
#include<fstream>
#include<stdlib.h>

using namespace std;

#include"Histogram.h"
#include"Image.h"
#include"common.h"

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    cerr << "Usage: histImg <image.data>" << endl;
    exit(-1);
  }

  Image<float> im_f;
  Histogram<float> hist;
  im_f.readImage(argv[1]);
  hist.set(im_f.data(), im_f.rows() * im_f.cols());
  cout << hist << endl; // print histogram

  return(0);
}

