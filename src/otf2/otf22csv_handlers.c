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
#include "otf22csv.h"
#include <inttypes.h>

static char **string_hash = NULL;
static int string_hash_current_size = 0;
static int *region_name_map = NULL;
static int region_name_map_current_size = 0;
static char **type_tree_hash = NULL;
static int type_tree_current_size = 0;

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
OTF2_CallbackCode otf22csv_unknown (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList* attributes)
{
    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode otf22csv_global_def_clock_properties (void *userData, uint64_t timerResolution, uint64_t globalOffset, uint64_t traceLength)
{
  otf2paje_t* data = (otf2paje_t*) userData;
  data->time_resolution = timerResolution;
  return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode otf22csv_global_def_string (void *userData, OTF2_StringRef self, const char *string)
{
  string_hash_current_size = self + 1;
  string_hash = (char**)realloc (string_hash, string_hash_current_size*sizeof(char*));
  string_hash[self] = (char*) malloc (strlen(string)+1);
  strcpy (string_hash[self], string);
  return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode otf22csv_global_def_region (void *userData, OTF2_RegionRef self, OTF2_StringRef name, OTF2_StringRef canonicalName, OTF2_StringRef description, OTF2_RegionRole regionRole, OTF2_Paradigm paradigm, OTF2_RegionFlag regionFlags, OTF2_StringRef sourceFile, uint32_t beginLineNumber, uint32_t endLineNumber)
{
  region_name_map_current_size = self + 1;
  region_name_map = (int*) realloc (region_name_map, region_name_map_current_size*sizeof(int));
  region_name_map[self] = name;
  return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode otf22csv_global_def_location_group_flat (void *userData, OTF2_LocationGroupRef self, OTF2_StringRef name, OTF2_LocationGroupType locationGroupType, OTF2_SystemTreeNodeRef systemTreeParen)
{
  return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode otf22csv_global_def_location_group_hostfile (void *userData, OTF2_LocationGroupRef self, OTF2_StringRef name, OTF2_LocationGroupType locationGroupType, OTF2_SystemTreeNodeRef systemTreeParent)
{
  return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode otf22csv_global_def_location_group (void *userData, OTF2_LocationGroupRef self, OTF2_StringRef name, OTF2_LocationGroupType locationGroupType, OTF2_SystemTreeNodeRef systemTreeParent)
{
  return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode otf22csv_global_def_location (void* userData, OTF2_LocationRef self, OTF2_StringRef name, OTF2_LocationType locationType, uint64_t numberOfEvents, OTF2_LocationGroupRef locationGroup)
{
  return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode otf22csv_global_def_system_tree_node_hostfile (void *userData, OTF2_SystemTreeNodeRef self, OTF2_StringRef name, OTF2_StringRef className, OTF2_SystemTreeNodeRef parent)
{
  return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode otf22csv_global_def_system_tree_node (void *userData, OTF2_SystemTreeNodeRef self, OTF2_StringRef name, OTF2_StringRef className, OTF2_SystemTreeNodeRef parent)
{
  return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode otf22csv_global_def_system_tree_node_property (void *userData, unsigned int systemTreeNode, unsigned int ignore, unsigned char name, union OTF2_AttributeValue_union value)
{
  return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode otf22csv_global_def_system_tree_node_domain( void* userData, OTF2_SystemTreeNodeRef systemTreeNode, OTF2_SystemTreeDomain  systemTreeDomain )
{
  return OTF2_CALLBACK_SUCCESS;
}

/* Events callbacks */
OTF2_CallbackCode otf22csv_enter (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList* attributes, OTF2_RegionRef regionID)
{
  otf2paje_t* data = (otf2paje_t*) userData;
  data->last_timestamp[locationID] = time_to_seconds(time, data->time_resolution);
  return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode otf22csv_leave (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList* attributes, OTF2_RegionRef regionID)
{
  otf2paje_t* data = (otf2paje_t*) userData;
  const char *state_name = string_hash[region_name_map[regionID]];

  double before = data->last_timestamp[locationID];
  double now = time_to_seconds(time, data->time_resolution);

  if (!arguments.dummy){
    printf("%d %f %f %s\n", locationID, before, now, state_name);
  }
  data->last_timestamp[locationID] = time_to_seconds(time, data->time_resolution);
  return OTF2_CALLBACK_SUCCESS;
}
