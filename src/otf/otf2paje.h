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
#ifndef __OTF2PAJE_H
#define __OTF2PAJE_H
#include <otf.h>
#include <poti.h>
#include <argp.h>
#include <stdio.h>
#define _GNU_SOURCE
#define __USE_GNU
#include <search.h>
#include "aky_private.h"

#define PROGRAM "otf2paje"

struct arguments {
  char *input[AKY_INPUT_SIZE];
  int input_size;
  int ignore_errors, no_links, no_states, only_mpi, normalize_mpi, basic, dummy;
  char *comment;
  char *comment_file;
};
extern struct arguments arguments;
extern struct argp argp;
extern struct hsearch_data process_name_hash;

int OTF_Handler_DefinitionComment( void* userData,
                                   uint32_t stream,
                                   const char* comment );
int OTF_Handler_DefTimerResolution( void* userData,
                                    uint32_t stream,
                                    uint64_t ticksPerSecond );
int OTF_Handler_DefProcess( void* userData,
                            uint32_t stream,
                            uint32_t process,
                            const char* name,
                            uint32_t parent );
int OTF_Handler_DefProcessGroup( void* userData,
                                 uint32_t stream,
                                 uint32_t procGroup,
                                 const char* name,
                                 uint32_t numberOfProcs,
                                 const uint32_t* procs );
int OTF_Handler_DefFunction( void* userData,
                             uint32_t stream,
                             uint32_t func,
                             const char* name,
                             uint32_t funcGroup,
                             uint32_t source );
int OTF_Handler_DefFunctionGroup( void* userData,
                                  uint32_t stream,
                                  uint32_t funcGroup,
                                  const char* name );
int OTF_Handler_DefCollectiveOperation( void* userData,
                                        uint32_t stream,
                                        uint32_t collOp,
                                        const char* name,
                                        uint32_t type );
int OTF_Handler_DefCounter( void* userData,
                            uint32_t stream,
                            uint32_t counter,
                            const char* name,
                            uint32_t properties,
                            uint32_t counterGroup,
                            const char* unit );
int OTF_Handler_DefCounterGroup( void* userData,
                                 uint32_t stream,
                                 uint32_t counterGroup,
                                 const char* name );
int OTF_Handler_DefScl( void* userData,
                        uint32_t stream,
                        uint32_t source,
                        uint32_t sourceFile,
                        uint32_t line );
int OTF_Handler_DefSclFile( void* userData,
                            uint32_t stream,
                            uint32_t sourceFile,
                            const char* name );
int OTF_Handler_DefCreator( void* userData,
                            uint32_t stream,
                            const char* creator );
int OTF_Handler_Enter( void* userData,
                       uint64_t time,
                       uint32_t function,
                       uint32_t process,
                       uint32_t source );
int OTF_Handler_Leave( void* userData,
                       uint64_t time,
                       uint32_t function,
                       uint32_t process,
                       uint32_t source );
int OTF_Handler_SendMsg( void* userData,
                         uint64_t time,
                         uint32_t sender,
                         uint32_t receiver,
                         uint32_t group,
                         uint32_t type,
                         uint32_t length,
                         uint32_t source );
int OTF_Handler_RecvMsg( void* userData,
                         uint64_t time,
                         uint32_t recvProc,
                         uint32_t sendProc,
                         uint32_t group,
                         uint32_t type,
                         uint32_t length,
                         uint32_t source );
int OTF_Handler_Counter( void* userData,
                         uint64_t time,
                         uint32_t process,
                         uint32_t counter,
                         uint64_t value );
int OTF_Handler_CollectiveOperation( void* userData,
                                     uint64_t time,
                                     uint32_t process,
                                     uint32_t collective,
                                     uint32_t procGroup,
                                     uint32_t rootProc,
                                     uint32_t sent,
                                     uint32_t received,
                                     uint64_t duration,
                                     uint32_t source );
int OTF_Handler_EventComment( void* userData,
                              uint64_t time,
                              uint32_t process,
                              const char* comment );
int OTF_Handler_BeginProcess( void* userData,
                              uint64_t time,
                              uint32_t process );
int OTF_Handler_EndProcess( void* userData,
                            uint64_t time,
                            uint32_t process );
int OTF_Handler_SnapshotComment( void* userData,
                                     uint64_t time,
                                     uint32_t process,
                                     const char* comment );
int OTF_Handler_EnterSnapshot( void *userData,
                           uint64_t time,
                           uint64_t originaltime,
                           uint32_t function,
                           uint32_t process,
                           uint32_t source );
int OTF_Handler_SendSnapshot( void *userData,
                           uint64_t time,
                           uint64_t originaltime,
                           uint32_t sender,
                           uint32_t receiver,
                           uint32_t procGroup,
                           uint32_t tag,
                           uint32_t source );
int OTF_Handler_SummaryComment( void * userData,
                                uint64_t time,
                                uint32_t process,
                                const char* comment );
int OTF_Handler_FunctionSummary( void* userData,
                                 uint64_t time,
                                 uint32_t function,
                                 uint32_t process,
                                 uint64_t invocations,
                                 uint64_t exclTime,
                                 uint64_t inclTime );
int OTF_Handler_FunctionGroupSummary( void* userData,
                                      uint64_t time,
                                      uint32_t funcGroup,
                                      uint32_t process,
                                      uint64_t invocations,
                                      uint64_t exclTime,
                                      uint64_t inclTime );
int OTF_Handler_MessageSummary( void* userData,
                                uint64_t time,
                                uint32_t process,
                                uint32_t peer,
                                uint32_t comm,
                                uint32_t type,
                                uint64_t sentNumber,
                                uint64_t receivedNumber,
                                uint64_t sentBytes,
                                uint64_t receivedBytes );
int OTF_Handler_UnknownRecord( void *userData,
                              uint64_t time,
                              uint32_t process,
                              const char *record );

extern double last_time;

#endif
