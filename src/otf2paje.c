/*
    This file is part of Akypuera

    Akypuera is free software: you can redistribute it and/or modify
    it under the terms of the GNU Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Akypuera is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Public License for more details.

    You should have received a copy of the GNU Public License
    along with Akypuera. If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <otf2/otf2.h>

/* Parameter handling */
static char doc[] = "Converts an OTF2 archive to the Paje file format";
static char args_doc[] = "ANCHORFILE";

#define OTF2PAJE_INPUT_SIZE 200
struct arguments {
  char *input[OTF2PAJE_INPUT_SIZE];
  int input_size;
  int ignore_errors, no_links, no_states, only_mpi, normalize_mpi;
};
static struct arguments arguments;

static struct argp_option options[] = {
  /* {"ignore-errors", 'i', 0, OPTION_ARG_OPTIONAL, "Ignore errors"}, */
  /* {"no-links", 'l', 0, OPTION_ARG_OPTIONAL, "Don't convert links"}, */
  /* {"no-states", 's', 0, OPTION_ARG_OPTIONAL, "Don't convert states"}, */
  /* {"only-mpi", 'm', 0, OPTION_ARG_OPTIONAL, "Only convert MPI states"}, */
  /* {"normalize-mpi", 'n', 0, OPTION_ARG_OPTIONAL, "Try to normalize MPI state names"}, */
  { 0 }
};

static int parse_options (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;
  switch (key){
  case 'i': arguments->ignore_errors = 1; break;
  case 'l': arguments->no_links = 1; break;
  case 's': arguments->no_states = 1; break;
  case 'm': arguments->only_mpi = 1; break;
  case 'n': arguments->normalize_mpi = 1; break;
  case ARGP_KEY_ARG:
    if (arguments->input_size == OTF2PAJE_INPUT_SIZE) {
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

int main (int argc, char **argv)
{
  bzero (&arguments, sizeof(struct arguments));
  if (argp_parse (&argp, argc, argv, 0, 0, &arguments) == ARGP_KEY_ERROR){
    fprintf(stderr,
            "[otf2paje] at %s, "
            "error during the parsing of parameters\n",
            __FUNCTION__);
    return 1;
  }

  OTF2_Reader* reader = OTF2_Reader_New (arguments.input[0]);
  if (reader == NULL){
    fprintf(stderr,
            "[otf2paje] at %s, "
            "creation of OTF2_Reader_New failed\n",
            __FUNCTION__);
    return 1;
  }


  OTF2_Reader_Delete (reader);
  return 0;
}
