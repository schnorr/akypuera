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
#ifndef __AKY2PAJE_H_
#define __AKY2PAJE_H_
#include <argp.h>
#include "aky_private.h"
#include "poti_private.h"

#define PROGRAM "aky_converter"

struct arguments {
  char *input[AKY_INPUT_SIZE];
  int input_size;
  int ignore_errors, no_links, no_states, basic, dummy;
  char *synchronization_file;
  char *comment;
  char *comment_file;
};

extern struct arguments arguments;
extern struct argp argp;

#endif
