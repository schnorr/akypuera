#include <stdio.h>
#include "rastro.h"

int main (int argc, char *argv[])
{
  rst_file_t data;
  rst_event_t ev;
  int i;
  if (argc < 3){
    fprintf (stderr, "libRastro: %s lists trace file content.\n"
                     "Usage: %s timesync_file trace_0.rst ... trace_N.rst\n",
        argv[0], argv[0]);
    return 0;
  }
  char *timesync_file = argv[1];
  for (i = 2; i < argc; i++) {
      if (rst_open_file (argv[i], &data, timesync_file, 100000) == -1) {
        fprintf (stderr, "Could not open rastro file %s, ignoring\n", argv[i]);
        continue;
      }
  }
  while (rst_decode_event (&data, &ev)) {
    rst_print_event (&ev);
  }
  rst_close_file (&data);
  return 0;
}
