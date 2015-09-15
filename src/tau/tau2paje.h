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
#ifndef __TAU2PAJE_H
#define __TAU2PAJE_H
#include <TAU_tf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <poti.h>
#include "aky_private.h"
#define _GNU_SOURCE
#define __USE_GNU
#include <search.h>

#define PROGRAM "tau2paje"

struct arguments {
  char *input[AKY_INPUT_SIZE];
  int input_size;
  int ignore_errors, no_links, no_states, only_mpi, normalize_mpi, basic, dummy;
  int literal_timestamps;
  char *comment;
  char *comment_file;
  double resolution;
};
extern struct arguments arguments;
extern struct argp argp;

int EnterState(void *userData, double time,
               unsigned int nodeid, unsigned int tid, unsigned int stateid);
int LeaveState(void *userData, double time, unsigned int nodeid,
               unsigned int tid, unsigned int stateid);
int ClockPeriod(void *userData, double clkPeriod);
int DefThread(void *userData, unsigned int nodeid,
              unsigned int threadToken, const char *threadName);
int EndTrace(void *userData, unsigned int nodeid, unsigned int threadid);
int DefStateGroup(void *userData, unsigned int stateGroupToken,
                  const char *stateGroupName);
int DefState(void *userData, unsigned int stateid, const char *statename,
             unsigned int stateGroupToken);
int DefUserEvent(void *userData, unsigned int userEventToken,
                 const char *userEventName, int monotonicallyIncreasing);
int EventTrigger(void *userData, double time,
                 unsigned int nodeToken,
                 unsigned int threadToken,
                 unsigned int userEventToken, long long userEventValue);
int SendMessage(void *userData,
                double time,
                unsigned int sourceNodeToken,
                unsigned int sourceThreadToken,
                unsigned int destinationNodeToken,
                unsigned int destinationThreadToken,
                unsigned int messageSize,
                unsigned int messageTag, unsigned int messageComm);
int RecvMessage(void *userData, double time,
                unsigned int sourceNodeToken,
                unsigned int sourceThreadToken,
                unsigned int destinationNodeToken,
                unsigned int destinationThreadToken,
                unsigned int messageSize,
                unsigned int messageTag, unsigned int messageComm);

extern int EndOfTrace;
extern long long total_number_of_links_not_translated;
extern double last_time;
extern struct hsearch_data state_name_hash;

#endif
