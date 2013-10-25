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


/* time_to_seconds */
static double time_to_seconds(double time, double resolution)
{
  static double first_time = -1;
  if (first_time == -1){
    first_time = time;
  }
  return (time - first_time) / resolution;
}

/* Definition callbacks */
SCOREP_Error_Code GlobDefUnknown_print (void* userData)
{
    ( void )userData;
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code GlobDefClockProperties_print (void *userData, uint64_t timer_resolution, uint64_t global_offset, uint64_t trace_length)
{
  otf2paje_t* data = (otf2paje_t*) userData;
  data->time_resolution = timer_resolution;
  return SCOREP_SUCCESS;
}

SCOREP_Error_Code GlobDefString_print (void *userData, uint32_t stringID, const char* string)
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

SCOREP_Error_Code GlobDefRegion_print (void* userData, uint32_t regionID, uint32_t stringID, uint32_t description, OTF2_RegionType regionType, uint32_t sourceFile, uint32_t beginLineNumber, uint32_t endLineNumber)
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


SCOREP_Error_Code GlobDefLocation_print (void* userData, uint64_t locationID, uint32_t name, OTF2_LocationType locationType, uint64_t numberOfEvents, uint32_t locationGroup)
{
  return SCOREP_SUCCESS;
}


/* Events callbacks */
SCOREP_Error_Code Enter_print (uint64_t locationID, uint64_t time, void *userData, OTF2_AttributeList* attributes, uint32_t regionID)
{
  otf2paje_t* data = (otf2paje_t*) userData;
  SCOREP_Hashtab_Entry *e1 = SCOREP_Hashtab_Find (data->regions, &regionID, NULL);
  otf2paje_region_t *region = (otf2paje_region_t*) e1->value;
  SCOREP_Hashtab_Entry *e2 = SCOREP_Hashtab_Find (data->strings, &region->string_id,NULL);
  otf2paje_string_t *string = (otf2paje_string_t*) e2->value;
  char *state_name = string->content;

  if (arguments.only_mpi && strstr(state_name, "MPI_") == NULL){
    return SCOREP_SUCCESS;
  }

  char mpi_process[100];
  snprintf(mpi_process, 100, "rank%"PRId64"", locationID);
  if (!arguments.dummy && !arguments.no_states){
    poti_PushState(time_to_seconds(time, data->time_resolution),
                   mpi_process, "STATE", state_name);
  }
  data->last_timestamp = time_to_seconds(time, data->time_resolution);
  return SCOREP_SUCCESS;
}

SCOREP_Error_Code Leave_print (uint64_t locationID, uint64_t time, void *userData, OTF2_AttributeList* attributes, uint32_t regionID)
{
  otf2paje_t* data = (otf2paje_t*) userData;
  SCOREP_Hashtab_Entry *e1 = SCOREP_Hashtab_Find (data->regions, &regionID, NULL);
  otf2paje_region_t *region = (otf2paje_region_t*) e1->value;
  SCOREP_Hashtab_Entry *e2 = SCOREP_Hashtab_Find (data->strings, &region->string_id,NULL);
  otf2paje_string_t *string = (otf2paje_string_t*) e2->value;
  char *state_name = string->content;

  if (arguments.only_mpi && strstr(state_name, "MPI_") == NULL){
    return SCOREP_SUCCESS;
  }

  char mpi_process[100];
  snprintf(mpi_process, 100, "rank%"PRId64"", locationID);
  if (!arguments.dummy && !arguments.no_states){
    poti_PopState(time_to_seconds(time, data->time_resolution),
                  mpi_process, "STATE");
  }
  data->last_timestamp = time_to_seconds(time, data->time_resolution);
  return SCOREP_SUCCESS;
}

