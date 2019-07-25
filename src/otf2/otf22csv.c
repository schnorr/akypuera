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

int main (int argc, char **argv)
{
  memset (&arguments, 0, sizeof(struct arguments));
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

  /* parameter check about hierarchy */
  if (arguments.hostfile && arguments.flat){
    fprintf(stderr,
            "[%s] at %s, "
            "Choose only one between -h and -f (see ./otf22csv --help)\n",
            PROGRAM, __FUNCTION__);
    return 1;
  }

  /* Get number of locations from the anchor file. */
  uint64_t num_locations;
  OTF2_Reader_GetNumberOfLocations (reader, &num_locations);

  /* read global definitions */
  /* User data for callbacks. */
  otf2paje_t *user_data = (otf2paje_t*) malloc (sizeof (otf2paje_t));
  user_data->reader = reader;

  /* zeroe everything regarding metric vectors */
  bzero(user_data->metrics_n, MAX_RANKS*sizeof(int));
  bzero(user_data->metrics, MAX_RANKS*sizeof(metric_t));

  bzero(user_data->enter_metrics_n, MAX_RANKS*sizeof(int));
  bzero(user_data->leave_metrics_n, MAX_RANKS*sizeof(int));
  bzero(user_data->enter_metrics, MAX_RANKS*sizeof(metric_t));
  bzero(user_data->leave_metrics, MAX_RANKS*sizeof(metric_t));

  /* prepare the rest of user data */
  user_data->locations = malloc( sizeof( *(user_data->locations) )
                                     + num_locations
				 * sizeof( *(user_data->locations)->members ) );
  user_data->locations->capacity = num_locations;
  user_data->locations->size     = 0;
  // Keep the last timestamp of each location on each level (one stack per location)
  int s = sizeof(double*) * num_locations;
  user_data->last_timestamp = malloc (s);
  for (uint8_t j = 0; j < num_locations; j++){
    int s = sizeof(double) * MAX_IMBRICATION;
    user_data->last_timestamp[j] = malloc(s);
    bzero(user_data->last_timestamp[j], s);
  }
  // Keep the imbrication level of each location
  s = sizeof(unsigned int) * num_locations;
  user_data->last_imbric = malloc (s);
  bzero(user_data->last_imbric, s);

  // Keep values of performance metrics
  user_data->number_of_metrics = 0; //It is defined as soon as possible, in the callback
  size_t sp = sizeof(uint64_t) * num_locations;
  user_data->last_metric = malloc (sp);
  bzero(user_data->last_metric, sp);
  user_data->last_enter_metric = malloc (sp);
  bzero(user_data->last_enter_metric, sp);

  /* Define definition callbacks. */
  OTF2_GlobalDefReaderCallbacks *def_callbacks = OTF2_GlobalDefReaderCallbacks_New();
  OTF2_GlobalDefReaderCallbacks_SetLocationCallback (def_callbacks, otf22csv_global_def_location);
  OTF2_GlobalDefReaderCallbacks_SetStringCallback (def_callbacks, otf22csv_global_def_string);
  OTF2_GlobalDefReaderCallbacks_SetRegionCallback (def_callbacks, otf22csv_global_def_region);
  OTF2_GlobalDefReaderCallbacks_SetClockPropertiesCallback (def_callbacks, otf22csv_global_def_clock_properties);

  if (arguments.hostfile){
    OTF2_GlobalDefReaderCallbacks_SetLocationGroupCallback (def_callbacks, otf22csv_global_def_location_group_hostfile);
    OTF2_GlobalDefReaderCallbacks_SetSystemTreeNodeCallback (def_callbacks, otf22csv_global_def_system_tree_node_hostfile);
  }else if (arguments.flat){
    OTF2_GlobalDefReaderCallbacks_SetLocationGroupCallback (def_callbacks, otf22csv_global_def_location_group_flat);
  }else{
    OTF2_GlobalDefReaderCallbacks_SetLocationGroupCallback (def_callbacks, otf22csv_global_def_location_group);
    OTF2_GlobalDefReaderCallbacks_SetLocationCallback (def_callbacks, otf22csv_global_def_location);
    OTF2_GlobalDefReaderCallbacks_SetSystemTreeNodeCallback (def_callbacks, otf22csv_global_def_system_tree_node);
    OTF2_GlobalDefReaderCallbacks_SetSystemTreeNodePropertyCallback (def_callbacks, otf22csv_global_def_system_tree_node_property);
    OTF2_GlobalDefReaderCallbacks_SetSystemTreeNodeDomainCallback (def_callbacks, otf22csv_global_def_system_tree_node_domain);
  }
 
  /* Read global definitions (locations). */
  OTF2_GlobalDefReader* glob_def_reader  = OTF2_Reader_GetGlobalDefReader (reader);
  OTF2_Reader_RegisterGlobalDefCallbacks (reader, glob_def_reader,
                                        def_callbacks,
                                        user_data);
  OTF2_GlobalDefReaderCallbacks_Delete(def_callbacks);
  
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

    /* Read definitions */
  size_t i;
  for ( i = 0; i < user_data->locations->size; i++ ){
    OTF2_DefReader* def_reader = OTF2_Reader_GetDefReader (reader, user_data->locations->members[i]);
    uint64_t definitions_read = 0;
    OTF2_Reader_ReadAllLocalDefinitions (reader, def_reader, &definitions_read);
    OTF2_Reader_CloseDefReader (reader, def_reader);
  }

  /* Create event readers */
  for (i = 0; i < user_data->locations->size; i++){
    OTF2_Reader_SelectLocation( reader, user_data->locations->members[i]);
    OTF2_Reader_GetEvtReader (reader, user_data->locations->members[i]);
  }

  /* Define event callbacks. */
  OTF2_GlobalEvtReaderCallbacks* evt_callbacks = OTF2_GlobalEvtReaderCallbacks_New();
  OTF2_GlobalEvtReaderCallbacks_SetEnterCallback( evt_callbacks, otf22csv_enter );
  OTF2_GlobalEvtReaderCallbacks_SetLeaveCallback( evt_callbacks, otf22csv_leave );
  OTF2_GlobalEvtReaderCallbacks_SetMetricCallback( evt_callbacks, otf22csv_print_metric );


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

  OTF2_Reader_Close (reader);
  return 0;
}
