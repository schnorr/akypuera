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

int main(int argc, char **argv)
{
  bzero (&arguments, sizeof(struct arguments));
  if (argp_parse (&argp, argc, argv, 0, 0, &arguments) == ARGP_KEY_ERROR){
    fprintf(stderr,
            "[%s] at %s,"
            "error during the parsing of parameters\n",
            PROGRAM, __FUNCTION__);
    return 1;
  }

  if (aky_key_init() == 1){
    fprintf(stderr,
            "[%s] at %s,"
            "error during hash table allocation\n",
            PROGRAM, __FUNCTION__);
    return 1;
  }

  /* allocating hash tables */
  if (hcreate_r (1000, &state_name_hash) == 0){
    fprintf (stderr,
             "[%s] at %s,"
             "hash table allocation for state names failed.",
             PROGRAM, __FUNCTION__);
    return 1;
  }

  /* open trace file */
  Ttf_FileHandleT fh;
  fh = Ttf_OpenFileForInput(arguments.input[0], arguments.input[1]);
  if (!fh) {
    fprintf(stderr,
            "[%s] at %s,"
            "Ttf_OpenFileForInput fails\n",
            PROGRAM, __FUNCTION__);
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
  cb.EnterState = EnterState;
  cb.LeaveState = LeaveState;
  cb.DefUserEvent = DefUserEvent;
  cb.EventTrigger = EventTrigger;
  cb.SendMessage = SendMessage;
  cb.RecvMessage = RecvMessage;

  /* output build version, date and conversion for aky in the trace */
  if (!arguments.dummy){
    /* start output with comments */
    if (arguments.comment){
      aky_dump_comment (PROGRAM, arguments.comment);
    }
    if (arguments.comment_file){
      if (aky_dump_comment_file (PROGRAM, arguments.comment_file) == 1){
        return 1;
      }
    }

    /* output build version, date and conversion for aky in the trace */
    aky_dump_version (PROGRAM, argv, argc);
    poti_header (arguments.basic);
    aky_paje_hierarchy();
    poti_CreateContainer (0, "root", "ROOT", "0", "root");
  }

  int recs_read;
  do {
    recs_read = Ttf_ReadNumEvents(fh, cb, 100);
  } while (recs_read >= 0 && !EndOfTrace);

  Ttf_CloseFile(fh);
  aky_key_free();
  hdestroy_r (&state_name_hash);

  if (total_number_of_links_not_translated){
    fprintf (stderr,
             "[%s] at %s, %lld links not translated due to synchronization problems.\n",
             PROGRAM, __FUNCTION__, total_number_of_links_not_translated);
  }

  if (!arguments.dummy){
    poti_DestroyContainer (last_time, "ROOT", "root");
  }
  return 0;
}
