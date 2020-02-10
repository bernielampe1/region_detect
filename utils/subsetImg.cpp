#include<iostream>
#include<fstream>
#include<string>

using namespace std;

#include"Image.h"

int main(int argc, char **argv)
{
  if (argc != 6)
  {
    cerr << "Usage: subsetImg <in.magn> <out.magn> <row> <col> <size>" << endl;
    exit(-1);
  }

  // parse parameters
  string inFname = argv[1];
  string outFname = argv[2];
  int row = atoi(argv[3]);
  int col = atoi(argv[4]);
  int size = atoi(argv[5]);

  // read and byte swap
  Image<float> im_in;
  im_in.readImage(inFname.c_str());

  // subset
  Image<float> im_out(size, size);
  for(int r = 0; r < size; ++r) {
    for(int c = 0; c < size; ++c) im_out(r, c) = im_in(row + r, col + c);
  }

  // write image
  im_out.writeMagn(outFname.c_str());

  return(0);
}

