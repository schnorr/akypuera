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

struct arguments {
  char *input[AKY_INPUT_SIZE];
  int input_size;
  int basic, dummy;
  char *comment;
  char *comment_file;
};

extern struct arguments arguments;
extern struct hsearch_data process_name_hash;

int handleDefinitionComment( void* userData, uint32_t stream,
                             const char* comment, OTF_KeyValueList* kvlist );
int handleDefTimerResolution( void* userData, uint32_t stream,
                              uint64_t ticksPerSecond, OTF_KeyValueList* kvlist );
int handleDefProcess( void* userData, uint32_t stream, uint32_t process,
                      const char* name, uint32_t parent, OTF_KeyValueList* kvlist );
int handleDefProcessGroup( void* userData, uint32_t stream,
                           uint32_t group, const char* name, uint32_t numberOfProcs,
                           const uint32_t* procs, OTF_KeyValueList* kvlist );
int handleDefAttributeList( void* userData, uint32_t stream,
                            uint32_t attr_token, uint32_t num, OTF_ATTR_TYPE* array,
                            OTF_KeyValueList* kvlist );
int handleDefProcessOrGroupAttributes( void* userData, uint32_t stream,
                                       uint32_t proc_token, uint32_t attr_token, OTF_KeyValueList* kvlist );
int handleDefFunction( void* userData, uint32_t stream, uint32_t func,
                       const char* name, uint32_t funcGroup, uint32_t source, OTF_KeyValueList* kvlist );
int handleDefFunctionGroup( void* userData, uint32_t stream,
                            uint32_t funcGroup, const char* name, OTF_KeyValueList* kvlist );
int handleDefCollectiveOperation( void* userData, uint32_t stream,
                                  uint32_t collOp, const char* name, uint32_t type, OTF_KeyValueList* kvlist );
int handleDefCounter( void* userData, uint32_t stream, uint32_t counter,
                      const char* name, uint32_t properties, uint32_t counterGroup,
                      const char* unit, OTF_KeyValueList* kvlist );
int handleDefCounterGroup( void* userData, uint32_t stream,
                           uint32_t counterGroup, const char* name, OTF_KeyValueList* kvlist );
int handleDefScl( void* userData, uint32_t stream, uint32_t source,
                  uint32_t sourceFile, uint32_t line, OTF_KeyValueList* kvlist );
int handleDefSclFile( void* userData, uint32_t stream,
                      uint32_t sourceFile, const char* name, OTF_KeyValueList* kvlist );
int handleDefCreator( void* userData, uint32_t stream,
                      const char* creator, OTF_KeyValueList* kvlist );
int handleDefUniqueId( void* userData, uint32_t stream, uint64_t uid );
int handleDefVersion( void* userData, uint32_t stream, uint8_t major,
                      uint8_t minor, uint8_t sub, const char* string );
int handleDefFile( void* userData, uint32_t stream,
                   uint32_t token, const char *name,
                   uint32_t group, OTF_KeyValueList* kvlist );
int handleDefFileGroup( void* userData, uint32_t stream,
                        uint32_t token, const char *name, OTF_KeyValueList* kvlist );
int handleDefKeyValue( void *userData, uint32_t streamid, uint32_t token,
                       OTF_Type type, const char *name, const char* desc,
                       OTF_KeyValueList* kvlist );
int handleDefTimeRange( void* userData, uint32_t streamid, uint64_t minTime,
                        uint64_t maxTime, OTF_KeyValueList* kvlist );
int handleDefCounterAssignments( void* userData, uint32_t streamid,
                                 uint32_t counter_token, uint32_t number_of_members,
                                 const uint32_t* procs_or_groups, OTF_KeyValueList* kvlist );
int handleDefProcessSubstitutes( void* userData, uint32_t streamid,
                                 uint32_t representative, uint32_t numberOfProcs, const uint32_t* procs,
                                 OTF_KeyValueList* kvlist );
int handleDefAuxSamplePoint( void*                  userData,
                             uint32_t               streamid,
                             uint64_t               time,
                             OTF_AuxSamplePointType type,
                             OTF_KeyValueList*      kvlist );
int handleNoOp( void* userData, uint64_t time, uint32_t process,
                OTF_KeyValueList* kvlist );
int handleEnter( void* userData, uint64_t time, uint32_t function,
                 uint32_t process, uint32_t source, OTF_KeyValueList* kvlist );
int handleLeave( void* userData, uint64_t time, uint32_t function,
                 uint32_t process, uint32_t source, OTF_KeyValueList* kvlist );
int handleSendMsg( void* userData, uint64_t time, uint32_t sender,
                   uint32_t receiver, uint32_t group, uint32_t type, uint32_t length,
                   uint32_t source, OTF_KeyValueList* kvlist );
int handleRecvMsg( void* userData, uint64_t time, uint32_t recvProc,
                   uint32_t sendProc, uint32_t group, uint32_t type, uint32_t length, 
                   uint32_t source, OTF_KeyValueList* kvlist );
int handleCounter( void* userData, uint64_t time, uint32_t process,
                   uint32_t counter, uint64_t value, OTF_KeyValueList* kvlist );
int handleCollectiveOperation( void* userData, uint64_t time,
                               uint32_t process, uint32_t collective, uint32_t procGroup,
                               uint32_t rootProc, uint32_t sent, uint32_t received, uint64_t duration, 
                               uint32_t source, OTF_KeyValueList* kvlist );
int handleBeginCollectiveOperation( void* userData, uint64_t time,
                                    uint32_t process, uint32_t collOp, uint64_t matchingId,
                                    uint32_t procGroup, uint32_t rootProc, uint64_t sent,
                                    uint64_t received, uint32_t scltoken, OTF_KeyValueList* kvlist );
int handleEndCollectiveOperation( void* userData, uint64_t time,
                                  uint32_t process, uint64_t matchingId, OTF_KeyValueList* kvlist );
int handleEventComment( void* userData, uint64_t time, uint32_t process,
                        const char* comment, OTF_KeyValueList* kvlist );
int handleBeginProcess( void* userData, uint64_t time, uint32_t process, OTF_KeyValueList* kvlist );
int handleEndProcess( void* userData, uint64_t time, uint32_t process, OTF_KeyValueList* kvlist );
int handleSnapshotComment( void* userData, uint64_t time,
                           uint32_t process, const char* comment, OTF_KeyValueList* kvlist );
int handleEnterSnapshot( void *userData, uint64_t time,
                         uint64_t originaltime, uint32_t function, uint32_t process,
                         uint32_t source, OTF_KeyValueList* kvlist );
int handleSendSnapshot( void *userData, uint64_t time,
                        uint64_t originaltime, uint32_t sender, uint32_t receiver,
                        uint32_t procGroup, uint32_t tag, uint32_t length, uint32_t source,
                        OTF_KeyValueList* kvlist );
int handleOpenFileSnapshot( void* userData, uint64_t time,
                            uint64_t originaltime, uint32_t fileid, uint32_t process, uint64_t handleid,
                            uint32_t source, OTF_KeyValueList* kvlist );
int handleBeginCollopSnapshot( void *userData, uint64_t time, uint64_t originaltime,
                               uint32_t process, uint32_t collOp, uint64_t matchingId, uint32_t procGroup,
                               uint32_t rootProc, uint64_t sent, uint64_t received, uint32_t scltoken,
                               OTF_KeyValueList *kvlist);
int handleBeginFileOpSnapshot( void *userData, uint64_t time,
                               uint64_t originaltime, uint32_t process, uint64_t matchingId,
                               uint32_t scltoken, OTF_KeyValueList *kvlist );
int handleCollopCountSnapshot( void *userData, uint64_t time,
                               uint32_t process, uint32_t communicator, uint64_t count,
                               OTF_KeyValueList *kvlist);
int handleCounterSnapshot( void             *userData,
                           uint64_t          time,
                           uint32_t          process,
                           uint64_t          originaltime,
                           uint32_t          counter,
                           uint64_t          value,
                           OTF_KeyValueList *kvlist );
int handleSummaryComment( void * userData, uint64_t time,
                          uint32_t process, const char* comment, OTF_KeyValueList* kvlist );
int handleFunctionSummary( void* userData, uint64_t time,
                           uint32_t function, uint32_t process, uint64_t invocations,
                           uint64_t exclTime, uint64_t inclTime, OTF_KeyValueList* kvlist );
int handleFunctionGroupSummary( void* userData, uint64_t time,
                                uint32_t funcGroup, uint32_t process, uint64_t invocations,
                                uint64_t exclTime, uint64_t inclTime, OTF_KeyValueList* kvlist );
int handleMessageSummary( void* userData, uint64_t time, uint32_t process,
                          uint32_t peer, uint32_t comm, uint32_t type, uint64_t sentNumber,
                          uint64_t receivedNumber, uint64_t sentBytes, uint64_t receivedBytes,
                          OTF_KeyValueList* kvlist );
int handleCollopSummary( void* userData, uint64_t time, uint32_t process,
                         uint32_t comm, uint32_t collective, uint64_t sentNumber, uint64_t receivedNumber,
                         uint64_t sentBytes, uint64_t receivedBytes, OTF_KeyValueList* kvlist);
int handleFileOperation( void* userData, uint64_t time,
                         uint32_t fileid, uint32_t process,
                         uint64_t handleid, uint32_t operation,
                         uint64_t bytes, uint64_t duration,
                         uint32_t source, OTF_KeyValueList* kvlist );
int handleBeginFileOperation( void* userData, uint64_t time,
                              uint32_t process, uint64_t matchingId, uint32_t scltoken,
                              OTF_KeyValueList* kvlist );
int handleEndFileOperation( void* userData, uint64_t time,
                            uint32_t process, uint32_t fileid, uint64_t matchingId,
                            uint64_t handleId, uint32_t operation, uint64_t bytes,
                            uint32_t scltoken, OTF_KeyValueList* kvlist );
int handleRMAPut( void* userData, uint64_t time, uint32_t process,
                  uint32_t origin, uint32_t target, uint32_t communicator, uint32_t tag,
                  uint64_t bytes, uint32_t source, OTF_KeyValueList* kvlist );
int handleRMAPutRemoteEnd( void* userData, uint64_t time,
                           uint32_t process, uint32_t origin, uint32_t target,
                           uint32_t communicator, uint32_t tag, uint64_t bytes,
                           uint32_t source, OTF_KeyValueList* kvlist );
int handleRMAGet( void* userData, uint64_t time, uint32_t process,
                  uint32_t origin, uint32_t target, uint32_t communicator, uint32_t tag,
                  uint64_t bytes, uint32_t source, OTF_KeyValueList* kvlist);
int handleRMAEnd( void* userData, uint64_t time, uint32_t process,
                  uint32_t remote, uint32_t communicator, uint32_t tag,
                  uint32_t source, OTF_KeyValueList* kvlist );
int handleUnknown( void* userData, uint64_t time, uint32_t process,
                   const char* record );
int handleDefMarker( void *userData, uint32_t stream, uint32_t token, const char* name,
                     uint32_t type, OTF_KeyValueList* kvlist );
int handleMarker( void *userData, uint64_t time, uint32_t process, 
                  uint32_t token, const char* text, OTF_KeyValueList* kvlist );

#endif
