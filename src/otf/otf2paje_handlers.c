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

static double ticks_to_seconds (unsigned long long ticks)
{
  return (double)ticks/(double)time_resolution;
}

static void data_def (struct hsearch_data *hash, uint32_t id, const char *name)
{
  char key[100];
  bzero(key, 100);
  snprintf (key, 100, "%d", id);

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
    char key[100];
    bzero(key, 100);
    snprintf (key, 100, "%d", id);

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


int handleDefinitionComment(void *userData, uint32_t stream,
			    const char *comment, OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleDefTimerResolution(void *userData, uint32_t stream,
			     uint64_t ticksPerSecond,
			     OTF_KeyValueList * kvlist)
{
  if (!arguments.dummy){
    printf ("# OTF TimerResolution: TicksPerSecond %llu\n", (long long unsigned) ticksPerSecond);
  }
  time_resolution = ticksPerSecond;
  return OTF_RETURN_OK;
}


int handleDefProcess(void *userData, uint32_t stream, uint32_t process,
		     const char *name, uint32_t parent,
		     OTF_KeyValueList * kvlist)
{
  data_def (&process_name_hash, process, name);
  return OTF_RETURN_OK;
}


int handleDefProcessGroup(void *userData, uint32_t stream,
			  uint32_t group, const char *name,
			  uint32_t numberOfProcs, const uint32_t * procs,
			  OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleDefAttributeList(void *userData, uint32_t stream,
			   uint32_t attr_token, uint32_t num,
			   OTF_ATTR_TYPE * array,
			   OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}

int handleDefProcessOrGroupAttributes(void *userData, uint32_t stream,
				      uint32_t proc_token,
				      uint32_t attr_token,
				      OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}

int handleDefFunction(void *userData, uint32_t stream, uint32_t func,
		      const char *name, uint32_t funcGroup,
		      uint32_t source, OTF_KeyValueList * kvlist)
{
  char alias[100];
  bzero(alias, 100);
  snprintf (alias, 100, "f%d", func);
  if (!arguments.dummy){
    pajeDefineEntityValue (alias, "STATE", name, "0 0 0");
  }
  return OTF_RETURN_OK;
}


int handleDefFunctionGroup(void *userData, uint32_t stream,
			   uint32_t funcGroup, const char *name,
			   OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}

int handleDefCollectiveOperation(void *userData, uint32_t stream,
				 uint32_t collOp, const char *name,
				 uint32_t type, OTF_KeyValueList * kvlist)
{
  char alias[100];
  bzero(alias, 100);
  snprintf (alias, 100, "c%d", collOp);
  if (!arguments.dummy){
    pajeDefineEntityValue (alias, "STATE", name, "0 0 0");
  }
  return OTF_RETURN_OK;
}


int handleDefCounter(void *userData, uint32_t stream, uint32_t counter,
		     const char *name, uint32_t properties,
		     uint32_t counterGroup, const char *unit,
		     OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleDefCounterGroup(void *userData, uint32_t stream,
			  uint32_t counterGroup, const char *name,
			  OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleDefScl(void *userData, uint32_t stream, uint32_t source,
		 uint32_t sourceFile, uint32_t line,
		 OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleDefSclFile(void *userData, uint32_t stream,
		     uint32_t sourceFile, const char *name,
		     OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleDefCreator(void *userData, uint32_t stream,
		     const char *creator, OTF_KeyValueList * kvlist)
{
  if (!arguments.dummy){
    printf ("# OTF Creator %s\n", creator);
  }
  return OTF_RETURN_OK;
}


int handleDefUniqueId(void *userData, uint32_t stream, uint64_t uid)
{
  return OTF_RETURN_OK;
}


int handleDefVersion(void *userData, uint32_t stream, uint8_t major,
		     uint8_t minor, uint8_t sub, const char *string)
{
  if (!arguments.dummy){
    printf ("# OTF Version: %u.%u.%u %s\n", major, minor, sub, string);
  }
  return OTF_RETURN_OK;
}


int handleDefFile(void *userData, uint32_t stream,
		  uint32_t token, const char *name,
		  uint32_t group, OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleDefFileGroup(void *userData, uint32_t stream,
		       uint32_t token, const char *name,
		       OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleDefKeyValue(void *userData, uint32_t streamid, uint32_t token,
		      OTF_Type type, const char *name, const char *desc,
		      OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleDefTimeRange(void *userData, uint32_t streamid, uint64_t minTime,
		       uint64_t maxTime, OTF_KeyValueList * kvlist)
{
  if (!arguments.dummy){
    printf ("# OTF TimeRange: [%llu, %llu]\n", (long long unsigned) minTime, (long long unsigned) maxTime);
  }
  return OTF_RETURN_OK;
}


int handleDefCounterAssignments(void *userData, uint32_t streamid,
				uint32_t counter_token,
				uint32_t number_of_members,
				const uint32_t * procs_or_groups,
				OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleDefProcessSubstitutes(void *userData, uint32_t streamid,
				uint32_t representative,
				uint32_t numberOfProcs,
				const uint32_t * procs,
				OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}



int handleDefAuxSamplePoint(void *userData,
			    uint32_t streamid,
			    uint64_t time,
			    OTF_AuxSamplePointType type,
			    OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleNoOp(void *userData, uint64_t time, uint32_t process,
	       OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleEnter(void *userData, uint64_t time, uint32_t function,
		uint32_t process, uint32_t source,
		OTF_KeyValueList * kvlist)
{
  char palias[100];
  bzero(palias, 100);
  snprintf (palias, 100, "p%d", process);
  char falias[100];
  bzero(falias, 100);
  snprintf (falias, 100, "f%d", function);
  if (!arguments.dummy){
    pajePushState (ticks_to_seconds(time), palias, "STATE", falias);
  }
  return OTF_RETURN_OK;
}


int handleLeave(void *userData, uint64_t time, uint32_t function,
		uint32_t process, uint32_t source,
		OTF_KeyValueList * kvlist)
{
  char palias[100];
  bzero(palias, 100);
  snprintf (palias, 100, "p%d", process);
  if (!arguments.dummy){
    pajePopState (ticks_to_seconds(time), palias, "STATE");
  }
  return OTF_RETURN_OK;
}


int handleSendMsg(void *userData, uint64_t time, uint32_t sender,
		  uint32_t receiver, uint32_t group, uint32_t type,
		  uint32_t length, uint32_t source,
		  OTF_KeyValueList * kvlist)
{
  /* fprintf(stdout, */
  /*         "%llu SendMessage: sender %u, receiver %u, group %u, type %u, length %u, source %u\n", */
  /*         (long long unsigned) time, */
  /*         sender, receiver, group, type, length, source); */


  return OTF_RETURN_OK;
}


int handleRecvMsg(void *userData, uint64_t time, uint32_t recvProc,
		  uint32_t sendProc, uint32_t group, uint32_t type,
		  uint32_t length, uint32_t source,
		  OTF_KeyValueList * kvlist)
{
  /* fprintf(stdout, */
  /*         "%llu ReceiveMessage: receiver %u, sender %u, group %u, type %u, length %u, source %u\n", */
  /*         (long long unsigned) time, */
  /*         recvProc, sendProc, group, type, length, source); */


  return OTF_RETURN_OK;
}


int handleCounter(void *userData, uint64_t time, uint32_t process,
		  uint32_t counter, uint64_t value,
		  OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleCollectiveOperation(void *userData, uint64_t time,
			      uint32_t process, uint32_t collective,
			      uint32_t procGroup, uint32_t rootProc,
			      uint32_t sent, uint32_t received,
			      uint64_t duration, uint32_t source,
			      OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleBeginCollectiveOperation(void *userData, uint64_t time,
				   uint32_t process, uint32_t collOp,
				   uint64_t matchingId, uint32_t procGroup,
				   uint32_t rootProc, uint64_t sent,
				   uint64_t received, uint32_t scltoken,
				   OTF_KeyValueList * kvlist)
{
  char palias[100];
  bzero(palias, 100);
  snprintf (palias, 100, "p%d", process);
  char calias[100];
  bzero(calias, 100);
  snprintf (calias, 100, "c%d", collOp);
  if (!arguments.dummy){
    pajePushState (ticks_to_seconds(time), palias, "STATE", calias);
  }
  return OTF_RETURN_OK;
}


int handleEndCollectiveOperation(void *userData, uint64_t time,
				 uint32_t process, uint64_t matchingId,
				 OTF_KeyValueList * kvlist)
{
  char palias[100];
  bzero(palias, 100);
  snprintf (palias, 100, "p%d", process);
  if (!arguments.dummy){
    pajePopState (ticks_to_seconds(time), palias, "STATE");
  }
  return OTF_RETURN_OK;
}


int handleEventComment(void *userData, uint64_t time, uint32_t process,
		       const char *comment, OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleBeginProcess(void *userData, uint64_t time, uint32_t process,
		       OTF_KeyValueList * kvlist)
{
  char *name = data_get(&process_name_hash, process);
  if (name){
    char alias[100];
    bzero(alias, 100);
    snprintf (alias, 100, "p%d", process);
    if (!arguments.dummy){
      pajeCreateContainer (ticks_to_seconds(time), alias, "PROCESS", "root", name); 
    }
  }
  return OTF_RETURN_OK;
}


int handleEndProcess(void *userData, uint64_t time, uint32_t process,
		     OTF_KeyValueList * kvlist)
{
  char *name = data_get(&process_name_hash, process);
  if (name){
    char alias[100];
    bzero(alias, 100);
    snprintf (alias, 100, "p%d", process);
    if (!arguments.dummy){
      pajeDestroyContainer (ticks_to_seconds(time), "PROCESS", alias);
    }
  }
  return OTF_RETURN_OK;
}


int handleSnapshotComment(void *userData, uint64_t time,
			  uint32_t process, const char *comment,
			  OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleEnterSnapshot(void *userData, uint64_t time,
			uint64_t originaltime, uint32_t function,
			uint32_t process, uint32_t source,
			OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleSendSnapshot(void *userData, uint64_t time,
		       uint64_t originaltime, uint32_t sender,
		       uint32_t receiver, uint32_t procGroup, uint32_t tag,
		       uint32_t length, uint32_t source,
		       OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleOpenFileSnapshot(void *userData, uint64_t time,
			   uint64_t originaltime, uint32_t fileid,
			   uint32_t process, uint64_t handleid,
			   uint32_t source, OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleBeginCollopSnapshot(void *userData, uint64_t time,
			      uint64_t originaltime, uint32_t process,
			      uint32_t collOp, uint64_t matchingId,
			      uint32_t procGroup, uint32_t rootProc,
			      uint64_t sent, uint64_t received,
			      uint32_t scltoken, OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleBeginFileOpSnapshot(void *userData, uint64_t time,
			      uint64_t originaltime, uint32_t process,
			      uint64_t matchingId, uint32_t scltoken,
			      OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleCollopCountSnapshot(void *userData, uint64_t time,
			      uint32_t process, uint32_t communicator,
			      uint64_t count, OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleCounterSnapshot(void *userData,
			  uint64_t time,
			  uint32_t process,
			  uint64_t originaltime,
			  uint32_t counter,
			  uint64_t value, OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleSummaryComment(void *userData, uint64_t time,
			 uint32_t process, const char *comment,
			 OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleFunctionSummary(void *userData, uint64_t time,
			  uint32_t function, uint32_t process,
			  uint64_t invocations, uint64_t exclTime,
			  uint64_t inclTime, OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleFunctionGroupSummary(void *userData, uint64_t time,
			       uint32_t funcGroup, uint32_t process,
			       uint64_t invocations, uint64_t exclTime,
			       uint64_t inclTime,
			       OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleMessageSummary(void *userData, uint64_t time, uint32_t process,
			 uint32_t peer, uint32_t comm, uint32_t type,
			 uint64_t sentNumber, uint64_t receivedNumber,
			 uint64_t sentBytes, uint64_t receivedBytes,
			 OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleCollopSummary(void *userData, uint64_t time, uint32_t process,
			uint32_t comm, uint32_t collective,
			uint64_t sentNumber, uint64_t receivedNumber,
			uint64_t sentBytes, uint64_t receivedBytes,
			OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}



int handleFileOperation(void *userData, uint64_t time,
			uint32_t fileid, uint32_t process,
			uint64_t handleid, uint32_t operation,
			uint64_t bytes, uint64_t duration,
			uint32_t source, OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleBeginFileOperation(void *userData, uint64_t time,
			     uint32_t process, uint64_t matchingId,
			     uint32_t scltoken, OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleEndFileOperation(void *userData, uint64_t time,
			   uint32_t process, uint32_t fileid,
			   uint64_t matchingId, uint64_t handleId,
			   uint32_t operation, uint64_t bytes,
			   uint32_t scltoken, OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleRMAPut(void *userData, uint64_t time, uint32_t process,
		 uint32_t origin, uint32_t target, uint32_t communicator,
		 uint32_t tag, uint64_t bytes, uint32_t source,
		 OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleRMAPutRemoteEnd(void *userData, uint64_t time,
			  uint32_t process, uint32_t origin,
			  uint32_t target, uint32_t communicator,
			  uint32_t tag, uint64_t bytes, uint32_t source,
			  OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleRMAGet(void *userData, uint64_t time, uint32_t process,
		 uint32_t origin, uint32_t target, uint32_t communicator,
		 uint32_t tag, uint64_t bytes, uint32_t source,
		 OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleRMAEnd(void *userData, uint64_t time, uint32_t process,
		 uint32_t remote, uint32_t communicator, uint32_t tag,
		 uint32_t source, OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleUnknown(void *userData, uint64_t time, uint32_t process,
		  const char *record)
{
  return OTF_RETURN_OK;
}


int handleDefMarker(void *userData, uint32_t stream, uint32_t token,
		    const char *name, uint32_t type,
		    OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}


int handleMarker(void *userData, uint64_t time, uint32_t process,
		 uint32_t token, const char *text,
		 OTF_KeyValueList * kvlist)
{
  return OTF_RETURN_OK;
}
