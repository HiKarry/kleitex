#include <cstdio>
#include <iostream>
#include <string>
#include <squish.h>
#include "squishpng.h"

typedef struct kleitex_header {
  char magic[4];
  unsigned int header;
  unsigned short width;
  unsigned short height;
  unsigned short pitch;
  unsigned int size;
} kleitex_header_t;

int main(int argc, char **argv)
{

  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " <filename.tex>" << std::endl;
    return -1;
  }

  FILE* fp;
  kleitex_header_t head;
  DxtData dxt;
  Image* im=new Image();

  fp=fopen(argv[1],"rb");
  if (!fp)
  {
    std::cout << "Cannot open " << argv[1] << std::endl;
    return -1;
  }

  fseek(fp,0,SEEK_END);
  fseek(fp,0,SEEK_SET);
  fread(&head,sizeof(head),1,fp);

  int mipmaps = (head.header >> 13) & 31;
  std::cout << "MAGIC: " << head.magic << std::endl;
  std::cout << "MIPMAPS: " << mipmaps << std::endl;
  std::cout << "WIDTH: " << head.width << std::endl;
  std::cout << "HEIGHT: " << head.height << std::endl;

  int ignore_length = (mipmaps-1) * 10;
  char *ignore = (char *)std::malloc(ignore_length);
  fread(ignore, ignore_length, 1, fp);
  std::free(ignore);

  //load dxt data
  dxt.data.Reset(head.size);
  fread(dxt.data.Get(),head.size,1,fp);
  dxt.width=head.width;
  dxt.height=head.height;
  dxt.format=squish::kDxt5;
  dxt.isColour=true;
  dxt.isAlpha=true;

  //decompress
  im->Decompress(dxt);

  //save to png
  std::string filename=std::string(argv[1]);
  filename.replace(filename.end()-4,filename.end(),".png");
  im->SavePng(filename);
  std::cout << "Wrote " << filename << std::endl;
  return 0;
}
