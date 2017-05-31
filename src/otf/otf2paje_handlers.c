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
#include <otf.h>
#include <poti.h>
#include "otf2paje.h"
#define _GNU_SOURCE
#define __USE_GNU
#include <search.h>

static long long unsigned time_resolution;
struct hsearch_data process_name_hash;
double last_time = 0;

static double ticks_to_seconds (unsigned long long ticks)
{
  last_time = (double)ticks/(double)time_resolution;
  return last_time;
}

static void data_def (struct hsearch_data *hash, uint32_t id, const char *name)
{
  char key[AKY_DEFAULT_STR_SIZE];
  memset(key, 0, AKY_DEFAULT_STR_SIZE);
  snprintf (key, AKY_DEFAULT_STR_SIZE, "%d", id);

  ENTRY e, *ep = NULL;
  e.key = key;
  e.data = NULL;
  hsearch_r (e, FIND, &ep, hash);
  if (ep == NULL){
    e.key = strdup(key);
    e.data = strdup(name);
    hsearch_r (e, ENTER, &ep, hash);
  }
}

static char *data_get (struct hsearch_data *hash, uint32_t id)
{
  /* Find process name */
  char *name = NULL;
  {
    char key[AKY_DEFAULT_STR_SIZE];
    memset(key, 0, AKY_DEFAULT_STR_SIZE);
    snprintf (key, AKY_DEFAULT_STR_SIZE, "%d", id);

    ENTRY e, *ep = NULL;
    e.key = key;
    e.data = NULL;
    hsearch_r (e, FIND, &ep, hash);
    if (ep == NULL){
      return NULL;
    }
    name = (char*)ep->data;
  }

  if (name == NULL){
    return NULL;
  }else{
    return name;
  }
}

int OTF_Handler_DefinitionComment( void* userData,
                                   uint32_t stream,
                                   const char* comment )
{
  return OTF_RETURN_OK;
}

int OTF_Handler_DefTimerResolution( void* userData,
                                    uint32_t stream,
                                    uint64_t ticksPerSecond )
{
  if (!arguments.dummy){
    printf ("# OTF TimerResolution: TicksPerSecond %llu\n", (long long unsigned) ticksPerSecond);
  }
  time_resolution = ticksPerSecond;
  return OTF_RETURN_OK;
}

int OTF_Handler_DefProcess( void* userData,
                            uint32_t stream,
                            uint32_t process,
                            const char* name,
                            uint32_t parent )
{
  data_def (&process_name_hash, process, name);
  return OTF_RETURN_OK;
}

int OTF_Handler_DefProcessGroup( void* userData,
                                 uint32_t stream,
                                 uint32_t procGroup,
                                 const char* name,
                                 uint32_t numberOfProcs,
                                 const uint32_t* procs )
{
  return OTF_RETURN_OK;
}

int OTF_Handler_DefFunction( void* userData,
                             uint32_t stream,
                             uint32_t func,
                             const char* name,
                             uint32_t funcGroup,
                             uint32_t source )
{
  char alias[AKY_DEFAULT_STR_SIZE];
  memset(alias, 0, AKY_DEFAULT_STR_SIZE);
  snprintf (alias, AKY_DEFAULT_STR_SIZE, "f%d", func);
  if (!arguments.dummy && !arguments.no_states){
    poti_DefineEntityValue (alias, "STATE", name, "0 0 0");
  }
  return OTF_RETURN_OK;
}

int OTF_Handler_DefFunctionGroup( void* userData,
                                  uint32_t stream,
                                  uint32_t funcGroup,
                                  const char* name )
{
  return OTF_RETURN_OK;
}

int OTF_Handler_DefCollectiveOperation( void* userData,
                                        uint32_t stream,
                                        uint32_t collOp,
                                        const char* name,
                                        uint32_t type )
{
  char alias[AKY_DEFAULT_STR_SIZE];
  memset(alias, 0, AKY_DEFAULT_STR_SIZE);
  snprintf (alias, AKY_DEFAULT_STR_SIZE, "c%d", collOp);
  if (!arguments.dummy && !arguments.no_states){
    poti_DefineEntityValue (alias, "STATE", name, "0 0 0");
  }
  return OTF_RETURN_OK;
}

int OTF_Handler_DefCounter( void* userData,
                            uint32_t stream,
                            uint32_t counter,
                            const char* name,
                            uint32_t properties,
                            uint32_t counterGroup,
                            const char* unit )
{
  return OTF_RETURN_OK;
}

int OTF_Handler_DefCounterGroup( void* userData,
                                 uint32_t stream,
                                 uint32_t counterGroup,
                                 const char* name )
{
  return OTF_RETURN_OK;
}

int OTF_Handler_DefScl( void* userData,
                        uint32_t stream,
                        uint32_t source,
                        uint32_t sourceFile,
                        uint32_t line )
{
  return OTF_RETURN_OK;
}

int OTF_Handler_DefSclFile( void* userData,
                            uint32_t stream,
                            uint32_t sourceFile,
                            const char* name )
{
  return OTF_RETURN_OK;
}

int OTF_Handler_DefCreator( void* userData,
                            uint32_t stream,
                            const char* creator )
{
  if (!arguments.dummy){
    printf ("# OTF Creator %s\n", creator);
  }
  return OTF_RETURN_OK;
}

int OTF_Handler_Enter( void* userData,
                       uint64_t time,
                       uint32_t function,
                       uint32_t process,
                       uint32_t source )
{
  char palias[AKY_DEFAULT_STR_SIZE];
  memset(palias, 0, AKY_DEFAULT_STR_SIZE);
  snprintf (palias, AKY_DEFAULT_STR_SIZE, "p%d", process);
  char falias[AKY_DEFAULT_STR_SIZE];
  memset(falias, 0, AKY_DEFAULT_STR_SIZE);
  snprintf (falias, AKY_DEFAULT_STR_SIZE, "f%d", function);
  if (!arguments.dummy && !arguments.no_states){
    poti_PushState (ticks_to_seconds(time), palias, "STATE", falias);
  }
  return OTF_RETURN_OK;
}

int OTF_Handler_Leave( void* userData,
                       uint64_t time,
                       uint32_t function,
                       uint32_t process,
                       uint32_t source )
{
  char palias[AKY_DEFAULT_STR_SIZE];
  memset(palias, 0, AKY_DEFAULT_STR_SIZE);
  snprintf (palias, AKY_DEFAULT_STR_SIZE, "p%d", process);
  if (!arguments.dummy && !arguments.no_states){
    poti_PopState (ticks_to_seconds(time), palias, "STATE");
  }
  return OTF_RETURN_OK;
}

int OTF_Handler_SendMsg( void* userData,
                         uint64_t time,
                         uint32_t sender,
                         uint32_t receiver,
                         uint32_t group,
                         uint32_t type,
                         uint32_t length,
                         uint32_t source )
{
  return OTF_RETURN_OK;
}

int OTF_Handler_RecvMsg( void* userData,
                         uint64_t time,
                         uint32_t recvProc,
                         uint32_t sendProc,
                         uint32_t group,
                         uint32_t type,
                         uint32_t length,
                         uint32_t source )
{
  return OTF_RETURN_OK;
}

int OTF_Handler_Counter( void* userData,
                         uint64_t time,
                         uint32_t process,
                         uint32_t counter,
                         uint64_t value )
{
  return OTF_RETURN_OK;
}

int OTF_Handler_CollectiveOperation( void* userData,
                                     uint64_t time,
                                     uint32_t process,
                                     uint32_t collective,
                                     uint32_t procGroup,
                                     uint32_t rootProc,
                                     uint32_t sent,
                                     uint32_t received,
                                     uint64_t duration,
                                     uint32_t source )
{
  //TODO
  //In OTF 1.12.* we had BeginCollective and EndCollective
  
  /* char palias[AKY_DEFAULT_STR_SIZE]; */
  /* memset(palias, 0, AKY_DEFAULT_STR_SIZE); */
  /* snprintf (palias, AKY_DEFAULT_STR_SIZE, "p%d", process); */
  /* char calias[AKY_DEFAULT_STR_SIZE]; */
  /* memset(calias, 0, AKY_DEFAULT_STR_SIZE); */
  /* snprintf (calias, AKY_DEFAULT_STR_SIZE, "c%d", collOp); */
  /* if (!arguments.dummy && !arguments.no_states){ */
  /*   poti_PushState (ticks_to_seconds(time), palias, "STATE", calias); */
  /* } */

  /* char palias[AKY_DEFAULT_STR_SIZE]; */
  /* memset(palias, 0, AKY_DEFAULT_STR_SIZE); */
  /* snprintf (palias, AKY_DEFAULT_STR_SIZE, "p%d", process); */
  /* if (!arguments.dummy && !arguments.no_states){ */
  /*   poti_PopState (ticks_to_seconds(time), palias, "STATE"); */
  /* } */
  
  return OTF_RETURN_OK;
}

int OTF_Handler_EventComment( void* userData,
                              uint64_t time,
                              uint32_t process,
                              const char* comment )
{
  return OTF_RETURN_OK;
}

int OTF_Handler_BeginProcess( void* userData,
                              uint64_t time,
                              uint32_t process )
{
  char *name = data_get(&process_name_hash, process);
  if (name){
    char alias[AKY_DEFAULT_STR_SIZE];
    memset(alias, 0, AKY_DEFAULT_STR_SIZE);
    snprintf (alias, AKY_DEFAULT_STR_SIZE, "p%d", process);
    if (!arguments.dummy){
      poti_CreateContainer (ticks_to_seconds(time), alias, "PROCESS", "root", name);
    }
  }
  return OTF_RETURN_OK;
}

int OTF_Handler_EndProcess( void* userData,
                            uint64_t time,
                            uint32_t process )
{
  char *name = data_get(&process_name_hash, process);
  if (name){
    char alias[AKY_DEFAULT_STR_SIZE];
    memset(alias, 0, AKY_DEFAULT_STR_SIZE);
    snprintf (alias, AKY_DEFAULT_STR_SIZE, "p%d", process);
    if (!arguments.dummy){
      poti_DestroyContainer (ticks_to_seconds(time), "PROCESS", alias);
    }
  }
  return OTF_RETURN_OK;
}

int OTF_Handler_SnapshotComment( void* userData,
                                     uint64_t time,
                                     uint32_t process,
                                     const char* comment )
{
  return OTF_RETURN_OK;
}

int OTF_Handler_EnterSnapshot( void *userData,
                           uint64_t time,
                           uint64_t originaltime,
                           uint32_t function,
                           uint32_t process,
                           uint32_t source )
{
  return OTF_RETURN_OK;
}

int OTF_Handler_SendSnapshot( void *userData,
                           uint64_t time,
                           uint64_t originaltime,
                           uint32_t sender,
                           uint32_t receiver,
                           uint32_t procGroup,
                           uint32_t tag,
                           uint32_t source )
{
  return OTF_RETURN_OK;
}

int OTF_Handler_SummaryComment( void * userData,
                                uint64_t time,
                                uint32_t process,
                                const char* comment )
{
  return OTF_RETURN_OK;
}

int OTF_Handler_FunctionSummary( void* userData,
                                 uint64_t time,
                                 uint32_t function,
                                 uint32_t process,
                                 uint64_t invocations,
                                 uint64_t exclTime,
                                 uint64_t inclTime )
{
  return OTF_RETURN_OK;
}

int OTF_Handler_FunctionGroupSummary( void* userData,
                                      uint64_t time,
                                      uint32_t funcGroup,
                                      uint32_t process,
                                      uint64_t invocations,
                                      uint64_t exclTime,
                                      uint64_t inclTime )
{
  return OTF_RETURN_OK;
}


int OTF_Handler_MessageSummary( void* userData,
                                uint64_t time,
                                uint32_t process,
                                uint32_t peer,
                                uint32_t comm,
                                uint32_t type,
                                uint64_t sentNumber,
                                uint64_t receivedNumber,
                                uint64_t sentBytes,
                                uint64_t receivedBytes )
{
  return OTF_RETURN_OK;
}


int OTF_Handler_UnknownRecord( void *userData,
                              uint64_t time,
                              uint32_t process,
			       const char *record )
{
  return OTF_RETURN_OK;
}
