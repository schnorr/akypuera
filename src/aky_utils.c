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
#include "aky_private.h"

void aky_paje_hierarchy (void)
{
  printf ("#\n"
          "# This is the type hierarchy for this trace file\n"
          "#\n");
  poti_DefineContainerType ("ROOT", "0", "ROOT");
  poti_DefineContainerType("PROCESS", "ROOT", "PROCESS");
  poti_DefineStateType("STATE", "PROCESS", "STATE");
  poti_DefineLinkType("LINK", "ROOT", "PROCESS", "PROCESS", "LINK");
  printf ("#\n"
          "# Let the timestamped events describe behavior\n"
          "#\n");
}

int aky_dump_version (const char *program, char **argv, int argc)
{
  printf ("#AKY_GIT_VERSION %s\n", GITVERSION);
  printf ("#AKY_GIT_DATE (date of the cmake configuration) %s\n", GITDATE);
  printf ("#This file was generated using %s (distributed with akypuera).\n", program);
  {
    printf ("#%s's command line: ", program);
    int i;
    for (i = 0; i < argc; i++){
      printf ("%s ", argv[i]);
    }
    printf ("\n");
  }
  return 0;
}

int aky_dump_comment (const char *program, const char *comment)
{
  printf ("# %s\n", comment);
  return 0;
}

int aky_dump_comment_file (const char *program, const char *filename)
{
  FILE *file = fopen (filename, "r");
  if (file == NULL){
    fprintf(stderr,
            "[%s] at %s, "
            "comment file %s could not be opened for reading\n",
            program, __func__, filename);
    return 1;
  }
  while (!feof(file)){
    char c;
    c = (char)fgetc(file);
    if (feof(file)) break;
    printf ("# ");
    while (c != '\n'){
      printf ("%c", c);
      c = (char)fgetc(file);
      if (feof(file)) break;
    }
    printf ("\n");
  }
  fclose(file);
  return 0;
}
