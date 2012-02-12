#include <stdio.h>
#include <argp.h>
#include <rastro.h>

static char doc[] = "Converts aky trace files to the Paje file format";
static char args_doc[] = "{rastro-0-0.rst rastro-1-0.rst ...}";

static struct argp_option options[] = {
  {"sync", 's', "SYNC_FILE", 0, "Synchronization file (from rastro_timesync)"},
  { 0 }
};

struct arguments {
  char *input[RST_MAX_INPUT_SIZE];
  int input_size;
  char *synchronization_file;
};

static int parse_options (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;
  switch (key){
  case 'z': arguments->synchronization_file = arg; break;
  case ARGP_KEY_ARG:
    if (arguments->input_size == RST_MAX_INPUT_SIZE) {
      /* Too many arguments. */
      argp_usage (state);
    }
    arguments->input[state->arg_num] = arg;
    arguments->input_size++;
    break;
  case ARGP_KEY_END:
    if (state->arg_num < 1)
      /* Not enough arguments. */
      argp_usage (state);
    break;
  default: return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = { options, parse_options, args_doc, doc };

int main (int argc, char *argv[])
{
  struct arguments arguments;
  bzero (&arguments, sizeof(struct arguments));
  if (argp_parse (&argp, argc, argv, 0, 0, &arguments) == ARGP_KEY_ERROR){
    fprintf(stderr,
            "[rastro_read] at %s, "
            "error during the parsing of parameters\n",
            __FUNCTION__);
    return 1;
  }

  rst_file_t data;
  rst_event_t event;
  int i;

  /* opening rst files */
  for (i = 0; i < arguments.input_size; i++){
    int status = rst_open_file (arguments.input[i],
                                &data,
                                arguments.synchronization_file,
                                100000);
    if (status == RST_NOK){
      fprintf(stderr,
              "[rastro_read] at %s, "
              "trace file %s could not be opened\n",
              __FUNCTION__, arguments.input[i]);
      return 1;
    }
  }

  /* reading all the files */
  while (rst_decode_event (&data, &event)) {
    rst_print_event (&event);
  }

  /* closing everything */
  rst_close_file (&data);
  return 0;
}
