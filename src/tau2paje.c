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
#include <TAU_tf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aky_private.h"

#define MAX_MPI_STATE 10000
static char **state_name = NULL;
static double *rank_last_time = NULL;
static int total_number_of_ranks = 0;
static int nrank = 0;
static int EndOfTrace = 0;

static double time_to_seconds(double time)
{
  return time / 1000000;
}

static void xbt_str_subst(char *str, char from, char to, int occurence)
{
  char *p = str;
  while (*p != '\0') {
    if (*p == from) {
      *p = to;
      if (occurence == 1)
        return;
      occurence--;
    }
    p++;
  }
}

/* implementation of callback routines */
int EnterState(void *userData, double time,
               unsigned int nodeid, unsigned int tid, unsigned int stateid)
{
  rank_last_time[nodeid] = time_to_seconds(time);
  char mpi_process[100];
  snprintf(mpi_process, 100, "rank%d", nodeid);
  char *state = NULL;
  state = state_name[stateid - 1];
  char *y = strstr(state, "addr");
  if (y)
    return 0;                   //ignore if it is not a MPI function
  if (strcmp(state, "MPI_Init") == 0)
    return 0;                   //ignore MPI_Init state (we don't have it in akypuera
  if (!strstr(state, "MPI_"))
    return 0;                   //ignore if the state does not start by a MPI_

  char nstate[1000], nstate2[1000];
  snprintf(nstate, 1000, "%s", state);
  xbt_str_subst(nstate, '"', ' ', 0);
  snprintf(nstate2, 1000, "\"%s\"", nstate);
  pajePushState(rank_last_time[nodeid], mpi_process, "STATE", nstate2);
  //printf("Entered state %d(%s) time %g nodeid %d tid %d\n",  stateid, state_name[stateid], time, nodeid, tid);
  return 0;
}

int LeaveState(void *userData, double time, unsigned int nodeid,
               unsigned int tid, unsigned int stateid)
{
  rank_last_time[nodeid] = time_to_seconds(time);
  if (stateid == 1)
    return 0;
  char mpi_process[100];
  snprintf(mpi_process, 100, "rank%d", nodeid);
  char *state = state_name[stateid - 1];
  char *y = strstr(state, "addr");
  if (y)
    return 0;                   //ignore if it is not a MPI function
  if (strcmp(state, "MPI_Init") == 0)
    return 0;                   //ignore MPI_Init state (we don't have it in akypuera
  if (!strstr(state, "MPI_"))
    return 0;                   //ignore if the state does not start by a MPI_
  pajePopState(rank_last_time[nodeid], mpi_process, "STATE");
  //printf("Leaving state %d time %g nodeid %d tid %d\n", stateid, time, nodeid, tid);
  return 0;
}


int ClockPeriod(void *userData, double clkPeriod)
{
  //printf("Clock period %g\n", clkPeriod);
  return 0;
}

int DefThread(void *userData, unsigned int nodeid,
              unsigned int threadToken, const char *threadName)
{
  char mpi_process[100];
  snprintf(mpi_process, 100, "rank%d", nodeid);
  pajeCreateContainer(0, mpi_process, "PROCESS", "0", mpi_process);
  if (nodeid + 1 > total_number_of_ranks) {
    total_number_of_ranks = nodeid + 1;
    rank_last_time =
        realloc(rank_last_time, sizeof(double) * total_number_of_ranks);
  }
  rank_last_time[nodeid] = 0;
  //printf("DefThread nodeid %d tid %d, thread name %s\n", nodeid, threadToken, threadName);
  return 0;
}

int EndTrace(void *userData, unsigned int nodeid, unsigned int threadid)
{
  char mpi_process[100];
  snprintf(mpi_process, 100, "rank%d", nodeid);
  pajeDestroyContainer(rank_last_time[nodeid], "PROCESS", mpi_process);
  //printf("EndTrace nodeid %d tid %d\n", nodeid, threadid);
  EndOfTrace = 1;
  return 0;
}

int DefStateGroup(void *userData, unsigned int stateGroupToken,
                  const char *stateGroupName)
{
  //printf("StateGroup groupid %d, group name %s\n", stateGroupToken,  stateGroupName);
  return 0;
}

int DefState(void *userData, unsigned int stateid, const char *statename,
             unsigned int stateGroupToken)
{
  char *x = strdup(statename);
  char *y = strchr(x, '(');
  if (y)
    *y = '\0';
  state_name[stateid - 1] = strdup(x);
  free(x);
//  printf("DefState stateid %d stateName %s stategroup id %d\n",
//                stateid, statename, stateGroupToken);
  return 0;
}

int DefUserEvent(void *userData, unsigned int userEventToken,
                 const char *userEventName, int monotonicallyIncreasing)
{

  //printf("DefUserEvent event id %d user event name %s, monotonically increasing = %d\n", userEventToken,
  //        userEventName, monotonicallyIncreasing);
  return 0;
}

int EventTrigger(void *userData, double time,
                 unsigned int nodeToken,
                 unsigned int threadToken,
                 unsigned int userEventToken, long long userEventValue)
{
  rank_last_time[nodeToken] = time_to_seconds(time);

  //printf("EventTrigger: time %g, nodeid %d tid %d event id %d triggered value %lld \n", time, nodeToken, threadToken, userEventToken, userEventValue);
  return 0;
}

int SendMessage(void *userData,
                double time,
                unsigned int sourceNodeToken,
                unsigned int sourceThreadToken,
                unsigned int destinationNodeToken,
                unsigned int destinationThreadToken,
                unsigned int messageSize,
                unsigned int messageTag, unsigned int messageComm)
{
  rank_last_time[sourceNodeToken] = time_to_seconds(time);
  rank_last_time[destinationNodeToken] = time_to_seconds(time);

  char mpi_process[100];
  snprintf(mpi_process, 100, "rank%d", sourceNodeToken);

  char key[AKY_DEFAULT_STR_SIZE];
  bzero(key, AKY_DEFAULT_STR_SIZE);

  //try to get a "u" (unusual) key
  char *found =
      aky_get_key("u", sourceNodeToken, destinationNodeToken, key,
                  AKY_DEFAULT_STR_SIZE);
  if (!found) {
    //generate a "n" (normal) key
    aky_put_key("n", sourceNodeToken, destinationNodeToken, key,
                AKY_DEFAULT_STR_SIZE);
  }
  pajeStartLink(rank_last_time[sourceNodeToken], "0", "LINK", mpi_process,
                "PTP", key);

/*
    printf("Message Send: time %g, nodeid %d, tid %d dest nodeid %d dest tid %d messageSize %d messageComm %d messageTag %d \n", time, sourceNodeToken,
    sourceThreadToken, destinationNodeToken,
    destinationThreadToken, messageSize, messageComm, messageTag);
*/
  return 0;
}

int RecvMessage(void *userData, double time,
                unsigned int sourceNodeToken,
                unsigned int sourceThreadToken,
                unsigned int destinationNodeToken,
                unsigned int destinationThreadToken,
                unsigned int messageSize,
                unsigned int messageTag, unsigned int messageComm)
{
  rank_last_time[sourceNodeToken] = time_to_seconds(time);
  rank_last_time[destinationNodeToken] = time_to_seconds(time);

  char mpi_process[100];
  snprintf(mpi_process, 100, "rank%d", destinationNodeToken);

  char key[AKY_DEFAULT_STR_SIZE];
  bzero(key, AKY_DEFAULT_STR_SIZE);

  //try to get a "n" (normal) key
  char *found =
      aky_get_key("n", sourceNodeToken, destinationNodeToken, key,
                  AKY_DEFAULT_STR_SIZE);
  if (!found) {
    //generate a "u" (unusual) key
    aky_put_key("u", sourceNodeToken, destinationNodeToken, key,
                AKY_DEFAULT_STR_SIZE);
  }
  pajeEndLink(rank_last_time[destinationNodeToken], "0", "LINK",
              mpi_process, "PTP", key);

/*
    printf("Message Recv: time %g, nodeid %d, tid %d dest nodeid %d dest tid %d messageSize %d messageComm %d messageTag %d \n", time, sourceNodeToken,
    sourceThreadToken, destinationNodeToken,
    destinationThreadToken, messageSize, messageComm, messageTag);
*/
  return 0;
}

/* Reader module */
int main(int argc, char **argv)
{
  Ttf_FileHandleT fh;

  state_name = (char **) malloc(sizeof(char *) * MAX_MPI_STATE);

  if (aky_key_init() == 1){
    return 1;
  }

  int recs_read, pos;
  Ttf_CallbacksT cb;

  /* main program: Usage app <trc> <edf> */
  if (argc != 3) {
    printf("Usage: %s <TAU trace> <edf file>\n", argv[0]);
    return 1;
  }

  /* open trace file */
  fh = Ttf_OpenFileForInput(argv[1], argv[2]);
  if (!fh) {
    printf("ERROR: Ttf_OpenFileForInput fails");
    return 1;
  }

  /* Fill the callback struct */
  cb.UserData = NULL;
  cb.DefClkPeriod = ClockPeriod;
  cb.DefThread = DefThread;
  cb.DefStateGroup = DefStateGroup;
  cb.DefState = DefState;
  cb.EndTrace = EndTrace;
  cb.EnterState = EnterState;
  cb.LeaveState = LeaveState;
  cb.DefUserEvent = DefUserEvent;
  cb.EventTrigger = EventTrigger;
  cb.SendMessage = SendMessage;
  cb.RecvMessage = RecvMessage;

  paje_header();
  paje_hierarchy();

  do {
    recs_read = Ttf_ReadNumEvents(fh, cb, 100);
  } while (recs_read >= 0 && !EndOfTrace);

  Ttf_CloseFile(fh);
  aky_key_free();
  return 0;
}
