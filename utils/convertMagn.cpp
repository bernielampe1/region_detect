#include<iostream>
#include<fstream>
#include<string>
#include<stdlib.h>

using namespace std;

int main(int argc, char **argv)
{
  if (argc != 5)
  {
    cerr << "Usage: convertMagn <in.magn> <out.magn> <rows> <cols>" << endl;
    exit(-1);
  }

  // parse parameters
  string inFname = argv[1];
  string outFname = argv[2];
  int rows = atoi(argv[3]);
  int cols = atoi(argv[4]);

  ifstream ifile;
  ifile.open(inFname.c_str(), ios::binary|ios::in|ios::ate);

  // get file size
  int size = ifile.tellg();
  ifile.seekg(ios::beg);

  // allocate buffer to read file
  char *buf = new char[size];
  ifile.read(buf, size);
  ifile.close();

  // write out sizes then data
  ofstream ofile;
  ofile.open(outFname.c_str(), ios::binary|ios::out);
  ofile.write((char*)&rows, sizeof(rows));
  ofile.write((char*)&cols, sizeof(cols));
  ofile.write(buf, size);
  ofile.close();

  return(0);
}

