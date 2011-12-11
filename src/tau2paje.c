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
#include <argp.h>
#include "aky_private.h"
#define _GNU_SOURCE
#define __USE_GNU
#include <search.h>

static struct hsearch_data state_name_hash;
static struct hsearch_data state_group_hash;

static double *rank_last_time = NULL;
static int total_number_of_ranks = 0;
static int nrank = 0;
static int EndOfTrace = 0;

/* Parameter handling */
static char doc[] = "Converts _merged_ TAU trace files to the Paje file format";
static char args_doc[] = "<tau.trc> <tau.edf>";

struct arguments {
  char *input[AKY_INPUT_SIZE];
  int input_size;
  int ignore_errors, no_links, no_states, only_mpi;
};
static struct arguments arguments;

static struct argp_option options[] = {
  {"ignore-errors", 'i', 0, OPTION_ARG_OPTIONAL, "Ignore errors"},
  {"no-links", 'l', 0, OPTION_ARG_OPTIONAL, "Don't convert links"},
  {"no-states", 's', 0, OPTION_ARG_OPTIONAL, "Don't convert states"},
  {"only-mpi", 'm', 0, OPTION_ARG_OPTIONAL, "Only convert MPI states"},
  { 0 }
};

static int parse_options (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;
  switch (key){
  case 'i': arguments->ignore_errors = 1; break;
  case 'l': arguments->no_links = 1; break;
  case 's': arguments->no_states = 1; break;
  case 'm': arguments->only_mpi = 1; break;
  case ARGP_KEY_ARG:
    if (arguments->input_size == AKY_INPUT_SIZE) {
      /* Too many arguments. */
      argp_usage (state);
    }
    arguments->input[state->arg_num] = arg;
    arguments->input_size++;
    break;
  case ARGP_KEY_END:
    if (state->arg_num < 2)
      /* Not enough arguments. */
      argp_usage (state);
    break;
  default: return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = { options, parse_options, args_doc, doc };


/* Rest */
static double time_to_seconds(double time)
{
  return time / 1000000;
}

/* implementation of callback routines */
int EnterState(void *userData, double time,
               unsigned int nodeid, unsigned int tid, unsigned int stateid)
{
  /* Find state name */
  char *state_name = NULL;
  {
    char state_key[100];
    bzero(state_key, 100);
    snprintf (state_key, 100, "%d", stateid);

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
  char mpi_process[100];
  snprintf(mpi_process, 100, "rank%d", nodeid);
  pajePushState(rank_last_time[nodeid], mpi_process, "STATE", state_name);
  return 0;
}

int LeaveState(void *userData, double time, unsigned int nodeid,
               unsigned int tid, unsigned int stateid)
{
  /* Find state name */
  char *state_name = NULL;
  {
    char state_key[100];
    bzero(state_key, 100);
    snprintf (state_key, 100, "%d", stateid);

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
  char mpi_process[100];
  snprintf(mpi_process, 100, "rank%d", nodeid);
  pajePopState(rank_last_time[nodeid], mpi_process, "STATE");
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
  char group_key[100];
  bzero(group_key, 100);
  snprintf (group_key, 100, "%d", stateGroupToken);

  if (arguments.only_mpi && strcmp(stateGroupName, "MPI")!=0){
    /* If we are supposed to only convert MPI states,
       don't register group names different from MPI. */
    return 0;
  }

  ENTRY e, *ep = NULL;
  e.key = group_key;
  e.data = NULL;
  hsearch_r (e, FIND, &ep, &state_group_hash);
  if (ep == NULL){
    e.key = strdup(group_key);
    e.data = strdup(stateGroupName);
    hsearch_r (e, ENTER, &ep, &state_group_hash);
  }
  return 0;
}

int DefState(void *userData, unsigned int stateid, const char *statename,
             unsigned int stateGroupToken)
{
  /* group check */
  {
    char group_key[100];
    bzero(group_key, 100);
    snprintf (group_key, 100, "%d", stateGroupToken);

    ENTRY e, *ep = NULL;
    e.key = group_key;
    e.data = NULL;
    hsearch_r (e, FIND, &ep, &state_group_hash);
    if (ep == NULL){
      /* The group is not registered, just ignore its state definitions */
      return 0;
    }
  }

  char state_key[100];
  bzero(state_key, 100);
  snprintf (state_key, 100, "%d", stateid);

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
  bzero (&arguments, sizeof(struct arguments));
  if (argp_parse (&argp, argc, argv, 0, 0, &arguments) == ARGP_KEY_ERROR){
    fprintf(stderr,
            "[tau2paje] at %s,"
            "error during the parsing of parameters\n",
            __FUNCTION__);
    return 1;
  }

  if (aky_key_init() == 1){
    fprintf(stderr,
            "[tau2paje] at %s,"
            "error during hash table allocation\n",
            __FUNCTION__);
    return 1;
  }

  /* allocating hash tables */
  if (hcreate_r (1000, &state_name_hash) == 0){
    fprintf (stderr,
             "[aky_converter] at %s,"
             "hash table allocation for state names failed.",
             __FUNCTION__);
    return 1;
  }
  if (hcreate_r (10, &state_group_hash) == 0){
    fprintf (stderr,
             "[aky_converter] at %s,"
             "hash table allocation for state groups failed.",
             __FUNCTION__);
    return 1;
  }

  /* open trace file */
  Ttf_FileHandleT fh;
  fh = Ttf_OpenFileForInput(arguments.input[0], arguments.input[1]);
  if (!fh) {
    fprintf(stderr,
            "[tau2paje] at %s,"
            "Ttf_OpenFileForInput fails\n",
            __FUNCTION__);
    return 1;
  }

  /* Fill the callback struct */
  Ttf_CallbacksT cb;
  cb.UserData = NULL;
  cb.DefClkPeriod = ClockPeriod;
  cb.DefThread = DefThread;
  cb.DefStateGroup = DefStateGroup;
  cb.DefState = DefState;
  cb.EndTrace = EndTrace;
  if (arguments.no_states){
    cb.EnterState = NULL;
    cb.LeaveState = NULL;
  }else{
    cb.EnterState = EnterState;
    cb.LeaveState = LeaveState;
  }
  cb.DefUserEvent = DefUserEvent;
  cb.EventTrigger = EventTrigger;
  if (arguments.no_links){
    cb.SendMessage = NULL;
    cb.RecvMessage = NULL;
  }else{
    cb.SendMessage = SendMessage;
    cb.RecvMessage = RecvMessage;
  }
  paje_header();
  paje_hierarchy();

  int recs_read, pos;
  do {
    recs_read = Ttf_ReadNumEvents(fh, cb, 100);
  } while (recs_read >= 0 && !EndOfTrace);

  Ttf_CloseFile(fh);
  aky_key_free();
  hdestroy_r (&state_name_hash);
  hdestroy_r (&state_group_hash);
  return 0;
}
