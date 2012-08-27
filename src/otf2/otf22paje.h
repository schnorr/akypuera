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
#ifndef __OTF22PAJE_H
#define __OTF22PAJE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <otf2/otf2.h>
#include <scorep_utility/SCOREP_Hashtab.h>
#include <scorep_utility/SCOREP_Vector.h>
#include <poti.h>
#define _GNU_SOURCE
#define __USE_GNU
#include <search.h>
#include "aky_private.h"

#define PROGRAM "otf22paje"

struct arguments {
  char *input[AKY_INPUT_SIZE];
  int input_size;
  int ignore_errors, no_links, no_states, only_mpi, normalize_mpi, basic, dummy;
  char *comment;
  char *comment_file;
};
extern struct arguments arguments;
extern struct argp argp;


/* Data utilities for convertion */
struct otf2paje_s
{
  OTF2_Reader*    reader;
  SCOREP_Vector*  locations;
  SCOREP_Hashtab* regions;
  SCOREP_Hashtab* strings;
  SCOREP_Hashtab* groups;
  double last_timestamp;
  double time_resolution;
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

SCOREP_Error_Code GlobDefUnknown_print (void* userData);
SCOREP_Error_Code GlobDefClockProperties_print (void *userData, uint64_t timer_resolution, uint64_t global_offset, uint64_t trace_length);
SCOREP_Error_Code GlobDefString_print (void *userData, uint32_t stringID, const char* string);
SCOREP_Error_Code GlobDefRegion_print (void* userData, uint32_t regionID, uint32_t stringID, uint32_t description, OTF2_RegionType regionType, uint32_t sourceFile, uint32_t beginLineNumber, uint32_t endLineNumber);
SCOREP_Error_Code GlobDefLocation_print (void* userData, uint64_t locationID, uint32_t name, OTF2_LocationType locationType, uint64_t numberOfEvents, uint32_t locationGroup);
SCOREP_Error_Code Enter_print (uint64_t locationID, uint64_t time, void *userData, OTF2_AttributeList* attributes, uint32_t regionID);
SCOREP_Error_Code Leave_print (uint64_t locationID, uint64_t time, void *userData, OTF2_AttributeList* attributes, uint32_t regionID);

#endif
