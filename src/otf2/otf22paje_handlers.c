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
#include <inttypes.h>

static char **string_hash = NULL;
static int string_hash_current_size = 0;
static int *region_name_map = NULL;
static int region_name_map_current_size = 0;
static char **type_tree_hash = NULL;
static int type_tree_current_size = 0;
static char **container_tree_hash = NULL;
static int container_tree_current_size = 0;

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
OTF2_CallbackCode otf22paje_unknown (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList* attributes)
{
    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode otf22paje_global_def_clock_properties (void *userData, uint64_t timerResolution, uint64_t globalOffset, uint64_t traceLength)
{
  otf2paje_t* data = (otf2paje_t*) userData;
  data->time_resolution = timerResolution;
  return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode otf22paje_global_def_string (void *userData, OTF2_StringRef self, const char *string)
{
  string_hash_current_size = self + 1;
  string_hash = (char**)realloc (string_hash, string_hash_current_size*sizeof(char*));
  string_hash[self] = (char*) malloc (strlen(string)+1);
  strcpy (string_hash[self], string);
  return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode otf22paje_global_def_region (void *userData, OTF2_RegionRef self, OTF2_StringRef name, OTF2_StringRef canonicalName, OTF2_StringRef description, OTF2_RegionRole regionRole, OTF2_Paradigm paradigm, OTF2_RegionFlag regionFlags, OTF2_StringRef sourceFile, uint32_t beginLineNumber, uint32_t endLineNumber)
{
  region_name_map_current_size = self + 1;
  region_name_map = (int*) realloc (region_name_map, region_name_map_current_size*sizeof(int));
  region_name_map[self] = name;
  return OTF2_CALLBACK_SUCCESS;
}

static const char *find_type_tree_name (const char *name)
{
  int i;
  for (i = 0; i < type_tree_current_size; i++){
    if (strcmp (type_tree_hash[i], name) == 0) return name;
  }
  return NULL;
}

OTF2_CallbackCode otf22paje_global_def_location_group (void *userData, OTF2_LocationGroupRef self, OTF2_StringRef name, OTF2_LocationGroupType locationGroupType, OTF2_SystemTreeNodeRef systemTreeParent)
{
  const char *name_str = string_hash[name];
  /* Define the type for the location group */

  const char *containerType = type_tree_hash[locationGroupType];
  const char *newTypeName = "process";
  int newTypeIndex = locationGroupType + 1;

  if (!find_type_tree_name (newTypeName)){
    type_tree_current_size++;
    type_tree_hash = (char**)realloc (type_tree_hash, type_tree_current_size*sizeof(char*));
    type_tree_hash[newTypeIndex] = strdup(newTypeName);

    if (!arguments.dummy){
      poti_DefineContainerType (newTypeName, containerType, newTypeName);
      poti_DefineStateType("STATE", newTypeName, "STATE");
      poti_DefineLinkType("LINK", "0", newTypeName, newTypeName, "LINK");

    }
  }

  /* create the container */
  int p = (int)systemTreeParent;
  const char *container = container_tree_hash[p];

  char mpi_process[100];
  snprintf(mpi_process, 100, "rank%d", (int)self);
  if (!arguments.dummy){
    poti_CreateContainer(0, mpi_process, newTypeName, container, name_str);
  }
  return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode otf22paje_global_def_location (void* userData, OTF2_LocationRef self, OTF2_StringRef name, OTF2_LocationType locationType, uint64_t numberOfEvents, OTF2_LocationGroupRef locationGroup)
{
  return OTF2_CALLBACK_SUCCESS;
}

static char *deblank (char* input)
{
  char *output = input;
  int i, j;
  for (i = 0, j = 0; i<strlen(input); i++, j++){
    if (input[i]!=' '){
      output[j]=input[i];
    }else{
      j--;
    }
  }
  output[j]=0;
  return output;
}

OTF2_CallbackCode otf22paje_global_def_system_tree_node (void *userData, OTF2_SystemTreeNodeRef self, OTF2_StringRef name, OTF2_StringRef className, OTF2_SystemTreeNodeRef parent)
{
  const char *name_str = deblank(string_hash[name]);
  const char *class_str = string_hash[className];

  /* handling the definition of the container type */

  //find parent container type and parent container
  const char *containerType, *container;
  int p = (int)parent;
  if (p < 0){
    containerType = "0";
    container = "0";
  }else{
    containerType = type_tree_hash[p];
    container = container_tree_hash[p];
  }

  //parent found, define new type if necessary
  if (!find_type_tree_name (class_str)){
    type_tree_current_size++;
    type_tree_hash = (char**)realloc (type_tree_hash, type_tree_current_size*sizeof(char*));
    type_tree_hash[self] = strdup(class_str);

    if (!arguments.dummy){
      poti_DefineContainerType (class_str, containerType, class_str);
    }
  }

  /* handling the create container */

  //create the container
  if (!arguments.dummy){
    poti_CreateContainer (0, name_str, class_str, container, name_str);
  }

  //registering the container creation
  int s = (int)self;
  container_tree_current_size++;
  container_tree_hash = (char**)realloc (container_tree_hash, container_tree_current_size*sizeof(char*));
  container_tree_hash[s] = strdup(name_str);

  return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode otf22paje_global_def_system_tree_node_property (void *userData, OTF2_SystemTreeNodeRef systemTreeNode, OTF2_StringRef name, OTF2_StringRef value)
{
  return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode otf22paje_global_def_system_tree_node_domain( void* userData, OTF2_SystemTreeNodeRef systemTreeNode, OTF2_SystemTreeDomain  systemTreeDomain )
{
  return OTF2_CALLBACK_SUCCESS;
}



/* Events callbacks */
OTF2_CallbackCode otf22paje_enter (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList* attributes, OTF2_RegionRef regionID)
{
  otf2paje_t* data = (otf2paje_t*) userData;
  const char *state_name = string_hash[region_name_map[regionID]];

  if (arguments.only_mpi && strstr(state_name, "MPI_") == NULL){
    return OTF2_CALLBACK_SUCCESS;
  }

  char mpi_process[100];
  snprintf(mpi_process, 100, "rank%"PRId64"", locationID);
  if (!arguments.dummy && !arguments.no_states){
    poti_PushState(time_to_seconds(time, data->time_resolution),
                   mpi_process, "STATE", state_name);
  }
  data->last_timestamp = time_to_seconds(time, data->time_resolution);
  return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode otf22paje_leave (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList* attributes, OTF2_RegionRef regionID)
{
  otf2paje_t* data = (otf2paje_t*) userData;
  const char *state_name = string_hash[region_name_map[regionID]];

  if (arguments.only_mpi && strstr(state_name, "MPI_") == NULL){
    return OTF2_CALLBACK_SUCCESS;
  }

  char mpi_process[100];
  snprintf(mpi_process, 100, "rank%"PRId64"", locationID);
  if (!arguments.dummy && !arguments.no_states){
    poti_PopState(time_to_seconds(time, data->time_resolution),
                  mpi_process, "STATE");
  }
  data->last_timestamp = time_to_seconds(time, data->time_resolution);
  return OTF2_CALLBACK_SUCCESS;
}



