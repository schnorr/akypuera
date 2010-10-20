#include <stdio.h>
#include <cuia.h>
#include <rastro.h>

int main (int argc, char **argv)
{
  if (argc == 1){
    printf ("%s {rastro-0-0.rst rastro-1-0.rst ...}\n", argv[0]);
    return 1;
  }

  rst_file_t data;
  rst_event_t event;
  int i;

  for (i = 1; i < argc; i++){
    int ret = rst_open_file(argv[i], &data, NULL, 100000);
    if (ret == -1){
      printf ("%s: trace %s could not be opened\n", argv[0], argv[i]);
      return 1;
    }
  }

  while (rst_decode_event (&data, &event)){
    fprintf (stderr, "%ld ", event.id1);
    rst_print_event(&event);
  }

  rst_close_file(&data);
  return 0;
}
