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
#include "otf2paje.h"

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

  /* allocating hash tables */
  if (hcreate_r (1000, &process_name_hash) == 0){
    fprintf (stderr,
             "[%s] at %s,"
             "hash table allocation for process names failed.",
             PROGRAM, __FUNCTION__);
    return 1;
  }

  OTF_FileManager* manager = OTF_FileManager_open (100);
  OTF_Reader* reader = OTF_Reader_open (arguments.input[0], manager);

  /* configure all callbacks */
  OTF_HandlerArray* handlers = OTF_HandlerArray_open();
  OTF_HandlerArray_setHandler( handlers, 
                               (OTF_FunctionPointer*) OTF_Handler_DefinitionComment,
                               OTF_DEFINITIONCOMMENT_RECORD );
  OTF_HandlerArray_setHandler( handlers, 
                               (OTF_FunctionPointer*) OTF_Handler_DefTimerResolution,
                               OTF_DEFTIMERRESOLUTION_RECORD );
  OTF_HandlerArray_setHandler( handlers, 
                               (OTF_FunctionPointer*) OTF_Handler_DefProcess,
                               OTF_DEFPROCESS_RECORD );
  OTF_HandlerArray_setHandler( handlers,
                               (OTF_FunctionPointer*) OTF_Handler_DefProcessGroup,
                               OTF_DEFPROCESSGROUP_RECORD );
  OTF_HandlerArray_setHandler( handlers, 
                               (OTF_FunctionPointer*) OTF_Handler_DefFunction,
                               OTF_DEFFUNCTION_RECORD );
  OTF_HandlerArray_setHandler( handlers, 
                               (OTF_FunctionPointer*) OTF_Handler_DefFunctionGroup,
                               OTF_DEFFUNCTIONGROUP_RECORD );
  OTF_HandlerArray_setHandler( handlers, 
                               (OTF_FunctionPointer*) OTF_Handler_DefCounter,
                               OTF_DEFCOUNTER_RECORD );
  OTF_HandlerArray_setHandler( handlers, 
                               (OTF_FunctionPointer*) OTF_Handler_DefCounterGroup,
                               OTF_DEFCOUNTERGROUP_RECORD );
  OTF_HandlerArray_setHandler( handlers, 
                               (OTF_FunctionPointer*) OTF_Handler_DefScl,
                               OTF_DEFSCL_RECORD );
  OTF_HandlerArray_setHandler( handlers, 
                               (OTF_FunctionPointer*) OTF_Handler_DefSclFile,
                               OTF_DEFSCLFILE_RECORD );
  OTF_HandlerArray_setHandler( handlers,
                               (OTF_FunctionPointer*) OTF_Handler_DefCreator,
                               OTF_DEFCREATOR_RECORD );
  OTF_HandlerArray_setHandler( handlers,
                               (OTF_FunctionPointer*) OTF_Handler_DefCollectiveOperation,
                               OTF_DEFCOLLOP_RECORD );
  OTF_HandlerArray_setHandler( handlers, 
                               (OTF_FunctionPointer*) OTF_Handler_EventComment,
                               OTF_EVENTCOMMENT_RECORD );
  OTF_HandlerArray_setHandler( handlers, 
                               (OTF_FunctionPointer*) OTF_Handler_Counter,
                               OTF_COUNTER_RECORD );
  OTF_HandlerArray_setHandler( handlers, 
                               (OTF_FunctionPointer*) OTF_Handler_Enter,
                               OTF_ENTER_RECORD );
  OTF_HandlerArray_setHandler( handlers, 
                               (OTF_FunctionPointer*) OTF_Handler_CollectiveOperation,
                               OTF_COLLOP_RECORD );
  OTF_HandlerArray_setHandler( handlers, 
                               (OTF_FunctionPointer*) OTF_Handler_RecvMsg,
                               OTF_RECEIVE_RECORD );
  OTF_HandlerArray_setHandler( handlers, 
                               (OTF_FunctionPointer*) OTF_Handler_SendMsg,
                               OTF_SEND_RECORD );
  OTF_HandlerArray_setHandler( handlers, 
                               (OTF_FunctionPointer*) OTF_Handler_Leave,
                               OTF_LEAVE_RECORD );
  OTF_HandlerArray_setHandler( handlers, 
                               (OTF_FunctionPointer*) OTF_Handler_BeginProcess,
                               OTF_BEGINPROCESS_RECORD );
  OTF_HandlerArray_setHandler( handlers, 
                               (OTF_FunctionPointer*) OTF_Handler_EndProcess,
                               OTF_ENDPROCESS_RECORD );

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
    poti_init();
    poti_header();
    aky_paje_hierarchy();
    poti_CreateContainer (0, "root", "ROOT", "0", "root");
  }

  /* read everything */
  OTF_Reader_readDefinitions (reader, handlers);
  OTF_Reader_readEvents (reader, handlers);
  OTF_Reader_readStatistics (reader, handlers);
  OTF_Reader_readSnapshots (reader, handlers);
  /* OTF_Reader_readMarkers (reader, handlers); */

  OTF_Reader_close (reader);
  OTF_HandlerArray_close (handlers);
  OTF_FileManager_close (manager);
  if (!arguments.dummy){
    poti_DestroyContainer (last_time, "ROOT", "root");
  }
  return 0;
}
