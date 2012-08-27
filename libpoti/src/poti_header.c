/*
    This file is part of Poti

    Poti is free software: you can redistribute it and/or modify it
    under the terms of the GNU Lesser Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Poti is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser Public License for more details.

    You should have received a copy of the GNU Lesser Public License
    along with Poti. If not, see <http://www.gnu.org/licenses/>.
*/
#include "poti_private.h"

extern FILE *paje_file;
extern int paje_extended;

static void poti_header_PajeDefineContainerType (int basic)
{
  fprintf(paje_file, "%%EventDef PajeDefineContainerType %d\n", PAJE_DefineContainerType);
  fprintf(paje_file, "%%       Alias string\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%       Name string\n");
  fprintf(paje_file, "%%EndEventDef\n");
}

static void poti_header_PajeDefineVariableType (int basic)
{
  fprintf(paje_file, "%%EventDef PajeDefineVariableType %d\n", PAJE_DefineVariableType);
  fprintf(paje_file, "%%       Alias string\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%       Name string\n");
  fprintf(paje_file, "%%       Color color\n");
  fprintf(paje_file, "%%EndEventDef\n");
}

static void poti_header_PajeDefineStateType (int basic)
{
  fprintf(paje_file, "%%EventDef PajeDefineStateType %d\n", PAJE_DefineStateType);
  fprintf(paje_file, "%%       Alias string\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%       Name string\n");
  fprintf(paje_file, "%%EndEventDef\n");
}

static void poti_header_PajeDefineEventType (int basic)
{
  fprintf(paje_file, "%%EventDef PajeDefineEventType %d\n", PAJE_DefineEventType);
  fprintf(paje_file, "%%       Alias string\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%       Name string\n");
  fprintf(paje_file, "%%EndEventDef\n");
}

static void poti_header_PajeDefineLinkType (int basic)
{
  fprintf(paje_file, "%%EventDef PajeDefineLinkType %d\n", PAJE_DefineLinkType);
  fprintf(paje_file, "%%       Alias string\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%       StartContainerType string\n");
  fprintf(paje_file, "%%       EndContainerType string\n");
  fprintf(paje_file, "%%       Name string\n");
  fprintf(paje_file, "%%EndEventDef\n");
}

static void poti_header_PajeDefineEntityValue (int basic)
{
  fprintf(paje_file, "%%EventDef PajeDefineEntityValue %d\n", PAJE_DefineEntityValue);
  fprintf(paje_file, "%%       Alias string\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%       Name string\n");
  fprintf(paje_file, "%%       Color color\n");
  fprintf(paje_file, "%%EndEventDef\n");
}

static void poti_header_PajeCreateContainer (int basic)
{
  fprintf(paje_file, "%%EventDef PajeCreateContainer %d\n", PAJE_CreateContainer);
  fprintf(paje_file, "%%       Time date\n");
  fprintf(paje_file, "%%       Alias string\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%       Container string\n");
  fprintf(paje_file, "%%       Name string\n");
  fprintf(paje_file, "%%EndEventDef\n");
}

static void poti_header_PajeDestroyContainer (int basic)
{
  fprintf(paje_file, "%%EventDef PajeDestroyContainer %d\n", PAJE_DestroyContainer);
  fprintf(paje_file, "%%       Time date\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%       Name string\n");
  fprintf(paje_file, "%%EndEventDef\n");
}

static void poti_header_PajeSetVariable (int basic)
{
  fprintf(paje_file, "%%EventDef PajeSetVariable %d\n", PAJE_SetVariable);
  fprintf(paje_file, "%%       Time date\n");
  fprintf(paje_file, "%%       Container string\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%       Value double\n");
  fprintf(paje_file, "%%EndEventDef\n");
}

static void poti_header_PajeAddVariable (int basic)
{
  fprintf(paje_file, "%%EventDef PajeAddVariable %d\n", PAJE_AddVariable);
  fprintf(paje_file, "%%       Time date\n");
  fprintf(paje_file, "%%       Container string\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%       Value double\n");
  fprintf(paje_file, "%%EndEventDef\n");
}

static void poti_header_PajeSubVariable (int basic)
{
  fprintf(paje_file, "%%EventDef PajeSubVariable %d\n", PAJE_SubVariable);
  fprintf(paje_file, "%%       Time date\n");
  fprintf(paje_file, "%%       Container string\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%       Value double\n");
  fprintf(paje_file, "%%EndEventDef\n");
}


static void poti_header_PajeSetState (int basic)
{
  fprintf(paje_file, "%%EventDef PajeSetState %d\n", PAJE_SetState);
  fprintf(paje_file, "%%       Time date\n");
  fprintf(paje_file, "%%       Container string\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%       Value string\n");
  fprintf(paje_file, "%%EndEventDef\n");
}

static void poti_header_PajePushState (int basic)
{
  fprintf(paje_file, "%%EventDef PajePushState %d\n", PAJE_PushState);
  fprintf(paje_file, "%%       Time date\n");
  fprintf(paje_file, "%%       Container string\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%       Value string\n");
  fprintf(paje_file, "%%EndEventDef\n");
}

static void poti_header_PajePushStateMark (int basic)
{
  fprintf(paje_file, "%%EventDef PajePushState %d\n", PAJE_PushStateMark);
  fprintf(paje_file, "%%       Time date\n");
  fprintf(paje_file, "%%       Container string\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%       Value string\n");
  fprintf(paje_file, "%%       Mark string\n");
  fprintf(paje_file, "%%EndEventDef\n");
}

static void poti_header_PajePopState (int basic)
{
  fprintf(paje_file, "%%EventDef PajePopState %d\n", PAJE_PopState);
  fprintf(paje_file, "%%       Time date\n");
  fprintf(paje_file, "%%       Container string\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%EndEventDef\n");
}

static void poti_header_PajeResetState (int basic)
{
  if (basic) return;

  fprintf(paje_file, "%%EventDef PajeResetState %d\n", PAJE_ResetState);
  fprintf(paje_file, "%%       Time date\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%       Container string\n");
  fprintf(paje_file, "%%EndEventDef\n");
}

static void poti_header_PajeStartLink (int basic)
{
  fprintf(paje_file, "%%EventDef PajeStartLink %d\n", PAJE_StartLink);
  fprintf(paje_file, "%%       Time date\n");
  fprintf(paje_file, "%%       Container string\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%       StartContainer string\n");
  fprintf(paje_file, "%%       Value string\n");
  fprintf(paje_file, "%%       Key string\n");
  fprintf(paje_file, "%%EndEventDef\n");
}

static void poti_header_PajeStartLinkSize (int basic)
{
  fprintf(paje_file, "%%EventDef PajeStartLink %d\n", PAJE_StartLinkSize);
  fprintf(paje_file, "%%       Time date\n");
  fprintf(paje_file, "%%       Container string\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%       StartContainer string\n");
  fprintf(paje_file, "%%       Value string\n");
  fprintf(paje_file, "%%       Key string\n");
  fprintf(paje_file, "%%       Size double\n");
  fprintf(paje_file, "%%EndEventDef\n");
}


static void poti_header_PajeStartLinkSizeMark (int basic)
{
  fprintf(paje_file, "%%EventDef PajeStartLink %d\n", PAJE_StartLinkSizeMark);
  fprintf(paje_file, "%%       Time date\n");
  fprintf(paje_file, "%%       Container string\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%       StartContainer string\n");
  fprintf(paje_file, "%%       Value string\n");
  fprintf(paje_file, "%%       Key string\n");
  fprintf(paje_file, "%%       Size double\n");
  fprintf(paje_file, "%%       Mark string\n");
  fprintf(paje_file, "%%EndEventDef\n");
}

static void poti_header_PajeEndLink (int basic)
{
  fprintf(paje_file, "%%EventDef PajeEndLink %d\n", PAJE_EndLink);
  fprintf(paje_file, "%%       Time date\n");
  fprintf(paje_file, "%%       Container string\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%       EndContainer string\n");
  fprintf(paje_file, "%%       Value string\n");
  fprintf(paje_file, "%%       Key string\n");
  fprintf(paje_file, "%%EndEventDef\n");
}

static void poti_header_PajeNewEvent (int basic)
{
  fprintf(paje_file, "%%EventDef PajeNewEvent %d\n", PAJE_NewEvent);
  fprintf(paje_file, "%%       Time date\n");
  fprintf(paje_file, "%%       Container string\n");
  fprintf(paje_file, "%%       Type string\n");
  fprintf(paje_file, "%%       Value string\n");
  fprintf(paje_file, "%%EndEventDef\n");
}

/* entry point */
void _poti_header(int basic)
{
  poti_header_PajeDefineContainerType (basic);
  poti_header_PajeDefineVariableType (basic);
  poti_header_PajeDefineStateType (basic);
  poti_header_PajeDefineEventType (basic);
  poti_header_PajeDefineLinkType (basic);
  poti_header_PajeDefineEntityValue (basic);
  poti_header_PajeCreateContainer (basic);
  poti_header_PajeDestroyContainer (basic);
  poti_header_PajeSetVariable (basic);
  poti_header_PajeAddVariable (basic);
  poti_header_PajeSubVariable (basic);
  poti_header_PajeSetState (basic);
  poti_header_PajePushState (basic);
  poti_header_PajePopState (basic);
  poti_header_PajeResetState (basic);
  poti_header_PajeStartLink (basic);
  poti_header_PajeEndLink (basic);
  poti_header_PajeNewEvent (basic);

  if (basic){
    paje_extended = 0;
    fprintf (paje_file,
             "#\n"
             "# Trace created without extended events\n");
    return;
  }else{
    paje_extended = 1;
  }

  fprintf(paje_file,
          "#\n"
          "# Extended events with additional information\n"
          "# Note that not all paje file format parsers support this\n"
          "# If you have problems parsing this file with your visualization tool\n"
          "#\n");

  poti_header_PajePushStateMark (basic);
  poti_header_PajeStartLinkSize (basic);
  poti_header_PajeStartLinkSizeMark (basic);
}
