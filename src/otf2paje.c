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
#include <scorep_utility/SCOREP_Hashtab.h>
#include <scorep_utility/SCOREP_Vector.h>
#include "aky_private.h"

/* time_to_seconds */
static double time_to_seconds(double time)
{
  static double first_time = -1;
  if (first_time == -1){
    first_time = time;
  }
  return (time - first_time) / 1000000;
}

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

/* Data utilities for convertion */
struct otf2paje_s
{
    OTF2_Reader*    reader;
    SCOREP_Vector*  locations;
    SCOREP_Hashtab* regions;
    SCOREP_Hashtab* strings;
    SCOREP_Hashtab* groups;
};
typedef struct otf2paje_s otf2paje_t;

struct otf2paje_region_s
{
    uint32_t region_id;
    uint32_t string_id;
};
typedef struct otf2paje_region_s otf2paje_region_t;

struct otf2paje_string_s
{
    uint32_t string_id;
    char*    content;
};
typedef struct otf2paje_string_s otf2paje_string_t;

/* Definition callbacks */
static SCOREP_Error_Code GlobDefString_print (void* userData,
                                              uint32_t stringID,
                                              char* string)
{
  otf2paje_t* data = (otf2paje_t*) userData;
  size_t hint;
  SCOREP_Hashtab_Entry* entry;
  entry = SCOREP_Hashtab_Find (data->strings, &stringID, &hint);
  if (entry) {
    /* already defined? */
    return SCOREP_SUCCESS;
  }

  otf2paje_string_t* new_string;
  new_string = (otf2paje_string_t*) malloc (sizeof (otf2paje_string_t));
  new_string->string_id = stringID;
  new_string->content = (char*) malloc (strlen(string)+1);
  strcpy (new_string->content, string);
  SCOREP_Hashtab_Insert (data->strings,
                         &new_string->string_id,
                         new_string,
                         &hint);
  return SCOREP_SUCCESS;
}

static SCOREP_Error_Code GlobDefRegion_print (void* userData,
                                              uint32_t regionID,
                                              uint32_t stringID,
                                              uint32_t description,
                                              OTF2_GlobRegionType regionType,
                                              uint32_t sourceFile,
                                              uint32_t beginLineNumber,
                                              uint32_t endLineNumber)
{
  otf2paje_t* data = (otf2paje_t*) userData;
  size_t hint;
  SCOREP_Hashtab_Entry* entry;
  entry = SCOREP_Hashtab_Find (data->regions, &regionID, &hint);
  if (entry) {
    /* already defined? */
    return SCOREP_SUCCESS;
  }
  otf2paje_region_t* new_region = (otf2paje_region_t*) malloc (sizeof(otf2paje_region_t));
  new_region->region_id = regionID;
  new_region->string_id = stringID;
  SCOREP_Hashtab_Insert( data->regions,
                         &new_region->region_id,
                         new_region,
                         &hint );
  return SCOREP_SUCCESS;
}

/* Events callbacks */
static SCOREP_Error_Code Enter_print (uint64_t locationID,
                                      uint64_t time,
                                      void *userData,
                                      OTF2_AttributeList* attributes,
                                      uint32_t regionID)
{
  otf2paje_t* data = (otf2paje_t*) userData;
  SCOREP_Hashtab_Entry *e1 = SCOREP_Hashtab_Find (data->regions, &regionID, NULL);
  otf2paje_region_t *region = (otf2paje_region_t*) e1->value;
  SCOREP_Hashtab_Entry *e2 = SCOREP_Hashtab_Find (data->strings, &region->string_id,NULL);
  otf2paje_string_t *string = (otf2paje_string_t*) e2->value;
  char *state_name = string->content;

  char mpi_process[100];
  snprintf(mpi_process, 100, "rank%lu", locationID);
  pajePushState(time_to_seconds(time), mpi_process, "STATE", state_name);
  return SCOREP_SUCCESS;
}

static SCOREP_Error_Code Leave_print (uint64_t locationID,
                                      uint64_t time,
                                      void *userData,
                                      OTF2_AttributeList* attributes,
                                      uint32_t regionID)
{
  char mpi_process[100];
  snprintf(mpi_process, 100, "rank%lu", locationID);
  pajePopState(time_to_seconds(time), mpi_process, "STATE");
  return SCOREP_SUCCESS;
}


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

  /* User data for callbacks. */
  otf2paje_t *user_data = (otf2paje_t*) malloc (sizeof (otf2paje_t));
  user_data->reader    = reader;
  user_data->locations = SCOREP_Vector_Create();
  user_data->regions   = SCOREP_Hashtab_CreateSize( 128,
                                                    SCOREP_Hashtab_HashInt32,
                                                    SCOREP_Hashtab_CompareUint32 );
  user_data->strings = SCOREP_Hashtab_CreateSize( 256,
                                                  SCOREP_Hashtab_HashInt32,
                                                  SCOREP_Hashtab_CompareUint32 );
  user_data->groups = SCOREP_Hashtab_CreateSize( 128,
                                                 SCOREP_Hashtab_HashInt64,
                                                 SCOREP_Hashtab_CompareUint64 );

  /* Define definition callbacks. */
  OTF2_GlobDefReaderCallbacks register_defs;
  bzero(&register_defs, sizeof (OTF2_GlobDefReaderCallbacks));
  register_defs.GlobDefString         = GlobDefString_print;
  /* register_defs.GlobDefLocation       = GlobDefLocation_print; */
  register_defs.GlobDefRegion         = GlobDefRegion_print;
  /* register_defs.GlobDefGroup          = GlobDefGroup_print; */
  /* register_defs.GlobDefSystemTreeNode = GlobDefSystemTreeNode_print; */
  /* register_defs.GlobDefLocationGroup  = GlobDefLocationGroup_print; */

  /* Read global definitions. */
  OTF2_GlobDefReader* glob_def_reader  = OTF2_Reader_GetGlobDefReader (reader);
  OTF2_Reader_RegisterGlobDefCallbacks (reader, glob_def_reader,
                                        &register_defs,
                                        sizeof (register_defs),
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
            "[otf2paje] at %s, "
            "Number of global definitions read and "
            "defined in anchor file do not match.\n",
            __FUNCTION__);   
    return 1;
  }

  /* we start here to output the paje converted file */
  paje_header();
  paje_hierarchy();

  /* Get number of locations from the anchor file. */
  uint64_t          num_locations = 0;
  SCOREP_Error_Code status = OTF2_Reader_GetNumberOfLocations (reader, &num_locations);
  for ( size_t i = 0; i < num_locations; i++ ){
    OTF2_EvtReader *evt_reader = OTF2_Reader_GetEvtReader (reader, i);
    OTF2_DefReader* def_reader = OTF2_Reader_GetDefReader (reader, i);
    uint64_t definitions_read = 0;
    OTF2_Reader_ReadAllLocalDefinitions (reader, def_reader, &definitions_read);
    OTF2_Reader_CloseDefReader (reader, def_reader);

    char mpi_process[100];
    snprintf(mpi_process, 100, "rank%lu", i);
    pajeCreateContainer(0, mpi_process, "PROCESS", "0", mpi_process);
  }

  /* Define event callbacks. */
  OTF2_GlobEvtReaderCallbacks print;
  bzero(&print, sizeof(OTF2_GlobEvtReaderCallbacks));
  print.Enter                = Enter_print;
  print.Leave                = Leave_print;

  /* Get global event reader. */
  OTF2_GlobEvtReader *glob_evt_reader = OTF2_Reader_GetGlobEvtReader (reader);

  /* Register the above defined callbacks to the global event reader. */
  OTF2_Reader_RegisterGlobEvtCallbacks (reader,
                                        glob_evt_reader,
                                        &print,
                                        sizeof (print),
                                        user_data);


  /* Read until events are all read. */
  uint64_t events_read = UINT64_MAX;
  while (events_read == UINT64_MAX){
    SCOREP_Error_Code status = OTF2_Reader_ReadGlobalEvents (reader,
                                                             glob_evt_reader,
                                                             UINT64_MAX,
                                                             &events_read );
  }

  for (size_t i = 0; i < num_locations; i++){
    char mpi_process[100];
    snprintf(mpi_process, 100, "rank%lu", i);
    /* pajeDestroyContainer(0, "PROCESS", mpi_process); */
  }

  OTF2_Reader_Delete (reader);
  return 0;
}
