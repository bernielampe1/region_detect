#include<iostream>
#include<fstream>
#include<stdlib.h>

using namespace std;

#include"Image.h"
#include"common.h"

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    cerr << "Usage: linScaleImg <image.data>" << endl;
    exit(-1);
  }

  Image<float> im_f;
  im_f.readImage(argv[1]);
  im_f.median(); // perform seperable media filtering
  im_f.writeImage("test.pgm"); // write as linearly scaled pgm

  return(0);
}

