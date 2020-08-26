#include "bitmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  if (argc != 4) {
    fprintf (stderr, "No args!\nReadable-to-Raw bitmap converter\nUsage:\n  %s [input file] [output file]\n", argv[0]);
    exit (1);
  }

  if (strlen(argv[1]) == 1 && argv[1][0] == 'c') {
    bitmap_compress (argv[2], argv[3]);
  } else if (strlen(argv[1]) == 1 && argv[1][0] == 'd') {
    bitmap_decompress (argv[2], argv[3]);
  } else {
    fprintf (stderr, "Unknown command '%s'\n", argv[1]);
  }
}
