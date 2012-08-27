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
#include "otf22paje.h"

static char doc[] = "Converts an OTF2 archive to the Paje file format";
static char args_doc[] = "ANCHORFILE";
struct arguments arguments;

static struct argp_option options[] = {
  {"dummy", 'd', 0, OPTION_ARG_OPTIONAL, "Read input traces but won't translate (no output)"},
  {"ignore-errors", 'i', 0, OPTION_ARG_OPTIONAL, "Ignore errors"},
  {"no-links", 'l', 0, OPTION_ARG_OPTIONAL, "Don't convert links"},
  {"no-states", 's', 0, OPTION_ARG_OPTIONAL, "Don't convert states"},
  {"only-mpi", 'o', 0, OPTION_ARG_OPTIONAL, "Only convert MPI states"},
  {"normalize-mpi", 'z', 0, OPTION_ARG_OPTIONAL, "Try to normalize MPI state names"},
  {"basic", 'b', 0, OPTION_ARG_OPTIONAL, "Avoid extended events (impoverished trace file)"},
  {"comment", 'm', "COMMENT", 0, "Comment is echoed to output"},
  {"commentfile", 'n', "FILE", 0, "Comments (from file) echoed to output"},
  { 0 }
};

static int parse_options (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;
  switch (key){
  case 'd': arguments->dummy = 1; break;
  case 'i': arguments->ignore_errors = 1; break;
  case 'l': arguments->no_links = 1; break;
  case 's': arguments->no_states = 1; break;
  case 'o': arguments->only_mpi = 1; break;
  case 'z': arguments->normalize_mpi = 1; break;
  case 'b': arguments->basic = 1; break;
  case 'm': arguments->comment = arg; break;
  case 'n': arguments->comment_file = arg; break;
  case ARGP_KEY_ARG:
    if (arguments->input_size == AKY_INPUT_SIZE) {
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

struct argp argp = { options, parse_options, args_doc, doc };
