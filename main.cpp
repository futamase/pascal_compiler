#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getsym.h>
#include <fstream>
#include "define.h"
#include "output.h"
#include "compiler.h"

TOKEN tok;
FILE *infile, *outfile;

int main(int argc, char*argv[])
{
  char outfile_name[255];
  fprintf(stderr, "Simple compiler: compile start.\n");

  if(argc < 2){
    fprintf(stderr, "USAGE: comp source-file-name\n");
    exit(1);
  }
  if((infile = fopen(argv[1], "r")) == NULL){
    fprintf(stderr, "Source file %s can't open.\n", argv[1]);
    exit(1);
  }

  if(argc == 3)
    strcpy(outfile_name, argv[2]);
  else
    strcpy(outfile_name, "a.asm");

  std::fstream out(outfile_name, std::ios::out);
  if(!out){
    fprintf(stderr, "Object file creation failed.\n");
    exit(1);
  }

  p1::Program::get_instance().compile();

  // output_all(outfile);
  p1::Commander::get_instance().output_all(out);

  fclose(infile);
  exit(0);
}
