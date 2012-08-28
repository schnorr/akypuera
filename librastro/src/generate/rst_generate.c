/*
    Copyright (c) 1998--2006 Benhur Stein
    
    This file is part of librastro.

    librastro is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    librastro is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
    for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with librastro; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02111 USA.
*/
#include <rastro.h>
#include <argp.h>

static char doc[] = "Implements rastro functions to be compiled with instrumented programs.";
static char args_doc[] = "{iii sfd ssii}";

static struct argp_option options[] = {
  {"header", 'h', "HEADER_FILE", 0, "Name of the new header file"},
  {"implementation", 'c', "CODE_FILE", 0, "Name of the new implementation file"},
  {"from-file", 'i', "INPUT_FILE", 0, "Implement functions from INPUT_FILE, one per line"},
  {0},
};

#define MY_INPUT_SIZE 200
struct arguments {
  char *input[MY_INPUT_SIZE];
  int input_size;
  char *header_file;
  char *implementation_file;
  char *from_file;
};

static int parse_options (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;
  switch (key){
  case 'h': arguments->header_file = arg; break;
  case 'c': arguments->implementation_file = arg; break;
  case 'i': arguments->from_file = arg; break;
  case ARGP_KEY_ARG:
    if (arguments->input_size == MY_INPUT_SIZE) {
      /* Too many arguments. */
      argp_usage (state);
    }
    arguments->input[state->arg_num] = arg;
    arguments->input_size++;
    break;
  case ARGP_KEY_END:
    if ((!arguments->header_file ||
         !arguments->implementation_file) &&
        !arguments->from_file){
      /* Not enough arguments. */
      argp_usage (state);
    }
    break;
  default: return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = { options, parse_options, args_doc, doc };

/* main */
int main(int argc, char *argv[])
{
  struct arguments arguments;
  bzero (&arguments, sizeof(struct arguments));
  if (argp_parse (&argp, argc, argv, 0, 0, &arguments) == ARGP_KEY_ERROR){
    fprintf(stderr,
            "[rastro_generate] at %s,"
            "error during the parsing of parameters\n",
            __FUNCTION__);
    return 1;
  }

  FILE *header = fopen (arguments.header_file, "w");
  if (!header){
    fprintf(stderr,
            "[rastro_generate] at %s,"
            "could not open file %s for writing\n",
            __FUNCTION__, arguments.header_file);
    return 1;
  }

  FILE *implem = fopen (arguments.implementation_file, "w");
  if (!implem){
    fprintf(stderr,
            "[rastro_generate] at %s,"
            "could not open file %s for writing\n",
            __FUNCTION__, arguments.implementation_file);
    fclose (header);
    return 1;
  }

  if (arguments.from_file){
    FILE *file = fopen (arguments.from_file, "r");
    if (!file){
      fprintf(stderr,
              "[rastro_generate] at %s,"
              "could not open file %s for reading\n",
              __FUNCTION__, arguments.from_file);
      fclose (header);
      fclose (implem);
      return 1;
    }
    while (feof(file)==0){
      char read_str[100];
      if (fscanf (file, "%s", read_str) != 1) break;
      if (feof(file)) break;

      arguments.input[arguments.input_size] = strdup (read_str);
      arguments.input_size++;
    }
    fclose (file);
  }

  rst_generate (arguments.input,
                arguments.input_size,
                header,
                implem,
                arguments.header_file);

  fclose (header);
  fclose (implem);
  return 0;
}
