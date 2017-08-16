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
#ifndef __OTF22CSV_H
#define __OTF22CSV_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <otf2/otf2.h>
#include <poti.h>
#define _GNU_SOURCE
#define __USE_GNU
#include <search.h>
#include "aky_private.h"

#define PROGRAM "otf22csv"
#define MAX_NUMBER_OF_RANKS 100000

struct arguments {
  char *input[AKY_INPUT_SIZE];
  int input_size;
  int ignore_errors, no_links, no_states, only_mpi, normalize_mpi, basic, dummy, flat;
  char *comment;
  char *comment_file;
  char *hostfile;
};
extern struct arguments arguments;
extern struct argp argp;




/* Data utilities for convertion */
struct otf2paje_s
{
  OTF2_Reader*    reader;
  double time_resolution;

  struct vector
  {
    size_t   capacity;
    size_t   size;
    uint64_t members[];
  } *locations;

  double *last_timestamp;

  uint64_t **last_metric; //length is the number of locationIDs
  uint64_t **last_enter_metric;
  int number_of_metrics;
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

OTF2_CallbackCode otf22csv_enter (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList* attributes, OTF2_RegionRef regionID);
OTF2_CallbackCode otf22csv_leave (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList* attributes, OTF2_RegionRef regionID);

OTF2_CallbackCode otf22csv_unknown (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList* attributes);
OTF2_CallbackCode otf22csv_global_def_clock_properties (void *userData, uint64_t timerResolution, uint64_t globalOffset, uint64_t traceLength);
OTF2_CallbackCode otf22csv_global_def_string (void *userData, OTF2_StringRef self, const char *string);
OTF2_CallbackCode otf22csv_global_def_region (void *userData, OTF2_RegionRef self, OTF2_StringRef name, OTF2_StringRef canonicalName, OTF2_StringRef description, OTF2_RegionRole regionRole, OTF2_Paradigm paradigm, OTF2_RegionFlag regionFlags, OTF2_StringRef sourceFile, uint32_t beginLineNumber, uint32_t endLineNumber);
OTF2_CallbackCode otf22csv_global_def_location_group(void* userData, OTF2_LocationGroupRef self, OTF2_StringRef name, OTF2_LocationGroupType locationGroupType, OTF2_SystemTreeNodeRef systemTreeParent );
OTF2_CallbackCode otf22csv_global_def_location_group_hostfile(void* userData, OTF2_LocationGroupRef self, OTF2_StringRef name, OTF2_LocationGroupType locationGroupType, OTF2_SystemTreeNodeRef systemTreeParent );
OTF2_CallbackCode otf22csv_global_def_location_group_flat (void *userData, OTF2_LocationGroupRef self, OTF2_StringRef name, OTF2_LocationGroupType locationGroupType, OTF2_SystemTreeNodeRef systemTreeParen);

OTF2_CallbackCode otf22csv_global_def_location (void* userData, OTF2_LocationRef self, OTF2_StringRef name, OTF2_LocationType locationType, uint64_t numberOfEvents, OTF2_LocationGroupRef locationGroup);

OTF2_CallbackCode otf22csv_global_def_system_tree_node( void*      userData, OTF2_SystemTreeNodeRef self, OTF2_StringRef   name, OTF2_StringRef   className, OTF2_SystemTreeNodeRef parent );
OTF2_CallbackCode otf22csv_global_def_system_tree_node_hostfile( void*      userData, OTF2_SystemTreeNodeRef self, OTF2_StringRef   name, OTF2_StringRef   className, OTF2_SystemTreeNodeRef parent );

OTF2_CallbackCode otf22csv_global_def_system_tree_node_property(void *userData, unsigned int systemTreeNode, unsigned int ignore, unsigned char name, union OTF2_AttributeValue_union value);


OTF2_CallbackCode otf22csv_global_def_system_tree_node_domain( void* userData, OTF2_SystemTreeNodeRef systemTreeNode, OTF2_SystemTreeDomain  systemTreeDomain );

OTF2_CallbackCode otf22csv_print_metric( OTF2_LocationRef location, OTF2_TimeStamp time, void* userData, OTF2_AttributeList* attributes, OTF2_MetricRef metric, uint8_t numberOfMetrics, const OTF2_Type* typeIDs, const OTF2_MetricValue* metricValues );


#endif
