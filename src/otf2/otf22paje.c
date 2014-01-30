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

int main (int argc, char **argv)
{
  bzero (&arguments, sizeof(struct arguments));
  if (argp_parse (&argp, argc, argv, 0, 0, &arguments) == ARGP_KEY_ERROR){
    fprintf(stderr,
            "[%s] at %s,"
            "error during the parsing of parameters\n",
            PROGRAM, __FUNCTION__);
    return 1;
  }

  /* create reader based on anchorfile passed as parameter */
  OTF2_Reader* reader = OTF2_Reader_Open (arguments.input[0]);
  if (reader == NULL){
    fprintf(stderr,
            "[%s] at %s, "
            "creation of OTF2_Reader_New failed\n",
            PROGRAM, __FUNCTION__);
    return 1;
  }

  /* read global definitions */
  /* User data for callbacks. */
  otf2paje_t *user_data = (otf2paje_t*) malloc (sizeof (otf2paje_t));
  user_data->reader = reader;

  /* Define definition callbacks. */
  OTF2_GlobalDefReaderCallbacks *def_callbacks = OTF2_GlobalDefReaderCallbacks_New();
  OTF2_GlobalDefReaderCallbacks_SetStringCallback (def_callbacks, otf22paje_global_def_string);
  OTF2_GlobalDefReaderCallbacks_SetRegionCallback (def_callbacks, otf22paje_global_def_region);
  OTF2_GlobalDefReaderCallbacks_SetClockPropertiesCallback (def_callbacks, otf22paje_global_def_clock_properties);

  /* Read global definitions. */
  OTF2_GlobalDefReader* glob_def_reader  = OTF2_Reader_GetGlobalDefReader (reader);
  OTF2_Reader_RegisterGlobalDefCallbacks (reader, glob_def_reader,
                                        def_callbacks,
                                        user_data);
  uint64_t definitions_read = 0;
  OTF2_Reader_ReadGlobalDefinitions (reader, glob_def_reader,
                                     OTF2_UNDEFINED_UINT64,
                                     &definitions_read);
  /* Check if number of global definitions read matches the number of global
   * definitions defined in the anchor file. */
  uint64_t defs_anchor = 0;
  OTF2_Reader_GetNumberOfGlobalDefinitions (reader, &defs_anchor);
  if (definitions_read != defs_anchor){
    fprintf(stderr,
            "[%s] at %s, "
            "Number of global definitions read and "
            "defined in anchor file do not match.\n",
            PROGRAM, __FUNCTION__);   
    return 1;
  }

  /* we start here to output the paje converted file */
  if (!arguments.dummy){
    /* start output with comments */
    if (arguments.comment){
      aky_dump_comment (PROGRAM, arguments.comment);
    }
    if (arguments.comment_file){
      if (aky_dump_comment_file (PROGRAM, arguments.comment_file) == 1){
        return 1;
      }
    }

    /* output build version, date and conversion for aky in the trace */
    aky_dump_version (PROGRAM, argv, argc);
    poti_header(arguments.basic, 0);
    aky_paje_hierarchy();
    poti_CreateContainer (0, "root", "ROOT", "0", "root");
  }

  /* Get number of locations from the anchor file. */
  uint64_t          num_locations = 0;
  OTF2_Reader_GetNumberOfLocations (reader, &num_locations);
  size_t i;
  for ( i = 0; i < num_locations; i++ ){
    OTF2_DefReader* def_reader = OTF2_Reader_GetDefReader (reader, i);
    uint64_t definitions_read = 0;
    OTF2_Reader_ReadAllLocalDefinitions (reader, def_reader, &definitions_read);
    OTF2_Reader_CloseDefReader (reader, def_reader);

    char mpi_process[100];
    snprintf(mpi_process, 100, "rank%zu", i);
    if (!arguments.dummy){
      poti_CreateContainer(0, mpi_process, "PROCESS", "root", mpi_process);
    }
  }

  /* Define event callbacks. */
  OTF2_GlobalEvtReaderCallbacks* evt_callbacks = OTF2_GlobalEvtReaderCallbacks_New();
  OTF2_GlobalEvtReaderCallbacks_SetEnterCallback( evt_callbacks, otf22paje_enter );
  OTF2_GlobalEvtReaderCallbacks_SetLeaveCallback( evt_callbacks, otf22paje_leave );

  /* Get global event reader. */
  OTF2_GlobalEvtReader *glob_evt_reader = OTF2_Reader_GetGlobalEvtReader (reader);

  /* Register the above defined callbacks to the global event reader. */
  OTF2_Reader_RegisterGlobalEvtCallbacks (reader,
                                          glob_evt_reader,
                                          evt_callbacks,
                                          user_data);


  /* Read until events are all read. */
  uint64_t events_read = UINT64_MAX;
  while (events_read == UINT64_MAX){
    OTF2_Reader_ReadGlobalEvents (reader,
                                  glob_evt_reader,
                                  UINT64_MAX,
                                  &events_read );
  }

  for (i = 0; i < num_locations; i++){
    char mpi_process[100];
    snprintf(mpi_process, 100, "rank%zu", i);
    if (!arguments.dummy){
      poti_DestroyContainer(user_data->last_timestamp, "PROCESS", mpi_process);
    }
  }

  if (!arguments.dummy){
    poti_DestroyContainer(user_data->last_timestamp, "ROOT", "root");
  }

  OTF2_Reader_Close (reader);
  return 0;
}
