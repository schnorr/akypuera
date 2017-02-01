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
#include "tau2paje.h"

struct hsearch_data state_name_hash;
static double *rank_last_time = NULL;
static int total_number_of_ranks = 0;
int EndOfTrace = 0;
long long total_number_of_links_not_translated = 0;
double last_time = 0;

static double time_to_seconds(double time)
{
  double resolution = arguments.resolution;
  if (!resolution){
    resolution = 1e6;
  }
  return time / resolution;

}

/* implementation of callback routines */
int EnterState(void *userData, double time,
               unsigned int nodeid, unsigned int tid, unsigned int stateid)
{
  /* Find state name */
  char *state_name = NULL;
  {
    char state_key[AKY_DEFAULT_STR_SIZE];
    bzero(state_key, AKY_DEFAULT_STR_SIZE);
    snprintf (state_key, AKY_DEFAULT_STR_SIZE, "%d", stateid);

    ENTRY e, *ep = NULL;
    e.key = state_key;
    e.data = NULL;
    hsearch_r (e, FIND, &ep, &state_name_hash);
    if (ep == NULL){
      return 1;
    }
    state_name = (char*)ep->data;
  }

  /* if state name is not defined, don't convert it */
  if (state_name == NULL){
    return 0;
  }

  /* try to normalize mpi names */
  if (arguments.normalize_mpi && strstr(state_name, "MPI")){
    char *first = strtok (state_name, "\""); /* removes the first " */
    /* locate first '(' */
    char *first_bracket = strchr (first, '(');
    if (first_bracket){
      char *x = state_name+(first_bracket-state_name);
      x[0] = '\0';
      state_name = first;
    }
    state_name = first;
  }

  rank_last_time[nodeid] = time_to_seconds(time);
  char mpi_process[AKY_DEFAULT_STR_SIZE];
  snprintf(mpi_process, AKY_DEFAULT_STR_SIZE, "rank%d", nodeid);

  if (!arguments.dummy && !arguments.no_states){
    poti_PushState(rank_last_time[nodeid], mpi_process, "STATE", state_name);
  }
  last_time = rank_last_time[nodeid];
  return 0;
}

int LeaveState(void *userData, double time, unsigned int nodeid,
               unsigned int tid, unsigned int stateid)
{
  /* Find state name */
  char *state_name = NULL;
  {
    char state_key[AKY_DEFAULT_STR_SIZE];
    bzero(state_key, AKY_DEFAULT_STR_SIZE);
    snprintf (state_key, AKY_DEFAULT_STR_SIZE, "%d", stateid);

    ENTRY e, *ep = NULL;
    e.key = state_key;
    e.data = NULL;
    hsearch_r (e, FIND, &ep, &state_name_hash);
    if (ep == NULL){
      return 1;
    }
    state_name = (char*)ep->data;
  }

  /* if state name is not defined, don't convert it */
  if (state_name == NULL){
    return 0;
  }
  rank_last_time[nodeid] = time_to_seconds(time);
  char mpi_process[AKY_DEFAULT_STR_SIZE];
  snprintf(mpi_process, AKY_DEFAULT_STR_SIZE, "rank%d", nodeid);
  if (!arguments.dummy && !arguments.no_states){
    poti_PopState(rank_last_time[nodeid], mpi_process, "STATE");
  }
  last_time = rank_last_time[nodeid];
  return 0;
}


int ClockPeriod(void *userData, double clkPeriod)
{
  arguments.resolution = 1/clkPeriod;
  return 0;
}

int DefThread(void *userData, unsigned int nodeid,
              unsigned int threadToken, const char *threadName)
{
  char mpi_process[AKY_DEFAULT_STR_SIZE];
  snprintf(mpi_process, AKY_DEFAULT_STR_SIZE, "rank%d", nodeid);
  if (!arguments.dummy){
    poti_CreateContainer(0, mpi_process, "PROCESS", "root", mpi_process);
  }
  if (nodeid + 1 > total_number_of_ranks) {
    total_number_of_ranks = nodeid + 1;
    rank_last_time =
        realloc(rank_last_time, sizeof(double) * total_number_of_ranks);
  }
  rank_last_time[nodeid] = 0;
  return 0;
}

int EndTrace(void *userData, unsigned int nodeid, unsigned int threadid)
{
  char mpi_process[AKY_DEFAULT_STR_SIZE];
  snprintf(mpi_process, AKY_DEFAULT_STR_SIZE, "rank%d", nodeid);
  if (!arguments.dummy){
    poti_DestroyContainer(rank_last_time[nodeid], "PROCESS", mpi_process);
  }
  EndOfTrace = 1;
  return 0;
}

int DefStateGroup(void *userData, unsigned int stateGroupToken,
                  const char *stateGroupName)
{
  return 0;
}

int DefState(void *userData, unsigned int stateid, const char *statename,
             unsigned int stateGroupToken)
{
  if (arguments.only_mpi && strstr(statename, "MPI_") == NULL){
    return 0;
  }

  char state_key[AKY_DEFAULT_STR_SIZE];
  bzero(state_key, AKY_DEFAULT_STR_SIZE);
  snprintf (state_key, AKY_DEFAULT_STR_SIZE, "%d", stateid);

  ENTRY e, *ep = NULL;
  e.key = state_key;
  e.data = NULL;
  hsearch_r (e, FIND, &ep, &state_name_hash);
  if (ep == NULL){
    e.key = strdup(state_key);
    e.data = strdup(statename);
    hsearch_r (e, ENTER, &ep, &state_name_hash);
  }
  return 0;
}

int DefUserEvent(void *userData, unsigned int userEventToken,
                 const char *userEventName, int monotonicallyIncreasing)
{
  return 0;
}

int EventTrigger(void *userData, double time,
                 unsigned int nodeToken,
                 unsigned int threadToken,
                 unsigned int userEventToken, long long userEventValue)
{
  rank_last_time[nodeToken] = time_to_seconds(time);
  last_time = rank_last_time[nodeToken];
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
  if (arguments.no_links){
    return 0;
  }

  char key[AKY_DEFAULT_STR_SIZE];
  bzero(key, AKY_DEFAULT_STR_SIZE);
  aky_put_key("n", sourceNodeToken, destinationNodeToken, key,
              AKY_DEFAULT_STR_SIZE);
  rank_last_time[sourceNodeToken] = time_to_seconds(time);
  rank_last_time[destinationNodeToken] = time_to_seconds(time);
  char mpi_process[AKY_DEFAULT_STR_SIZE];
  snprintf(mpi_process, AKY_DEFAULT_STR_SIZE, "rank%d", sourceNodeToken);
  if (!arguments.dummy){
    char messageSizeStr[AKY_DEFAULT_STR_SIZE], messageTagStr[AKY_DEFAULT_STR_SIZE];
    snprintf(messageSizeStr, AKY_DEFAULT_STR_SIZE, "%u", messageSize);
    snprintf(messageTagStr, AKY_DEFAULT_STR_SIZE, "%u", messageTag);
    poti_StartLinkSizeMark(rank_last_time[sourceNodeToken], "root", "LINK",
                           mpi_process, "PTP", key, messageSizeStr, messageTagStr);
  }
  last_time = rank_last_time[sourceNodeToken];
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

  if (arguments.no_links){
    return 0;
  }

  char key[AKY_DEFAULT_STR_SIZE];
  bzero(key, AKY_DEFAULT_STR_SIZE);
  char *result = aky_get_key("n", sourceNodeToken, destinationNodeToken, key,
                             AKY_DEFAULT_STR_SIZE);
  if (result == NULL){
    fprintf (stderr,
             "[%s] at %s, no key to generate a pajeEndLink,\n"
             "[%s] got a receive at dst = %u from src = %d\n"
             "[%s] but no send for this receive yet,\n"
             "[%s] do you synchronize your input traces?\n",
             PROGRAM, __FUNCTION__, PROGRAM, destinationNodeToken, sourceNodeToken, PROGRAM, PROGRAM);

    /* should we ignore this error */
    if (arguments.ignore_errors){
      total_number_of_links_not_translated++;
      return 0;
    }else{
      exit(1);
      return 1;
    }
  }
  rank_last_time[sourceNodeToken] = time_to_seconds(time);
  rank_last_time[destinationNodeToken] = time_to_seconds(time);
  char mpi_process[AKY_DEFAULT_STR_SIZE];
  snprintf(mpi_process, AKY_DEFAULT_STR_SIZE, "rank%d", destinationNodeToken);
  if (!arguments.dummy){
    poti_EndLink(rank_last_time[destinationNodeToken], "root", "LINK",
                mpi_process, "PTP", key);
  }
  last_time = rank_last_time[destinationNodeToken];
  return 0;
}
