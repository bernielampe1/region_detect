#include<iostream>
#include<fstream>
#include<stdlib.h>

using namespace std;

#include"Image.h"
#include"common.h"

int main(int argc, char **argv)
{
  if (argc != 3)
  {
    cerr << "Usage: linScaleImg <image.magn> <out.pgm>" << endl;
    exit(-1);
  }

  Image<float> im_f;
  im_f.readImage(argv[1]);
  im_f.writeImage(argv[2]); // write as linearly scaled pgm

  return(0);
}

