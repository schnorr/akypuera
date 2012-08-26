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

static double paje_event_timestamp(double timestamp)
{
  static double first_timestamp = -1;
  if (first_timestamp == -1) {
    first_timestamp = timestamp;
  }
  return timestamp - first_timestamp;
}

static FILE* paje_file = 0;

int pajeOpen (const char* filename)
{
  FILE* fout = fopen(filename,"w");
  if (fout ==0) return -1;
  paje_file = fout;
  return 0;
}

void pajeClose ()
{
  if (paje_file != stdout)
  {
    fclose( paje_file );
  }
  paje_file = 0;
}


void pajeHeader (void)
{
  if (paje_file ==0)
    paje_file = stdout;

  fprintf(paje_file,"#POTI_GIT_VERSION %s\n", POTI_GITVERSION);
  fprintf(paje_file,"#POTI_GIT_DATE (date of the cmake configuration) %s\n", POTI_GITDATE);
  fprintf(paje_file,"%%EventDef PajeDefineContainerType %d\n"
         "%%       Alias string\n"
         "%%       Type string\n"
         "%%       Name string\n"
         "%%EndEventDef\n"
         "%%EventDef PajeDefineVariableType %d\n"
         "%%       Alias string\n"
         "%%       Type string\n"
         "%%       Name string\n"
         "%%       Color color\n"
         "%%EndEventDef\n"
         "%%EventDef PajeDefineStateType %d\n"
         "%%       Alias string\n"
         "%%       Type string\n"
         "%%       Name string\n"
         "%%EndEventDef\n"
         "%%EventDef PajeDefineEventType %d\n"
         "%%       Alias string\n"
         "%%       Type string\n"
         "%%       Name string\n"
         "%%       Color color\n"
         "%%EndEventDef\n"
         "%%EventDef PajeDefineLinkType %d\n"
         "%%       Alias string\n"
         "%%       Type string\n"
         "%%       StartContainerType string\n"
         "%%       EndContainerType string\n"
         "%%       Name string\n"
         "%%EndEventDef\n"
         "%%EventDef PajeDefineEntityValue %d\n"
         "%%       Alias string\n"
         "%%       Type string\n"
         "%%       Name string\n"
         "%%       Color color\n"
         "%%EndEventDef\n"
         "%%EventDef PajeCreateContainer %d\n"
         "%%       Time date\n"
         "%%       Alias string\n"
         "%%       Type string\n"
         "%%       Container string\n"
         "%%       Name string\n"
         "%%EndEventDef\n"
         "%%EventDef PajeDestroyContainer %d\n"
         "%%       Time date\n"
         "%%       Type string\n"
         "%%       Name string\n"
         "%%EndEventDef\n"
         "%%EventDef PajeSetVariable %d\n"
         "%%       Time date\n"
         "%%       Type string\n"
         "%%       Container string\n"
         "%%       Value double\n"
         "%%EndEventDef\n"
         "%%EventDef PajeAddVariable %d\n"
         "%%       Time date\n"
         "%%       Type string\n"
         "%%       Container string\n"
         "%%       Value double\n"
         "%%EndEventDef\n"
         "%%EventDef PajeSubVariable %d\n"
         "%%       Time date\n"
         "%%       Type string\n"
         "%%       Container string\n"
         "%%       Value double\n"
         "%%EndEventDef\n"
         "%%EventDef PajeSetState %d\n"
         "%%       Time date\n"
         "%%       Container string\n"
         "%%       Type string\n"
         "%%       Value string\n"
         "%%EndEventDef\n"
         "%%EventDef PajePushState %d\n"
         "%%       Time date\n"
         "%%       Container string\n"
         "%%       Type string\n"
         "%%       Value string\n"
         "%%EndEventDef\n"
         "%%EventDef PajePopState %d\n"
         "%%       Time date\n"
         "%%       Container string\n"
         "%%       Type string\n"
         "%%EndEventDef\n"
         "%%EventDef PajeStartLink %d\n"
         "%%       Time date\n"
         "%%       Container string\n"
         "%%       Type string\n"
         "%%       StartContainer string\n"
         "%%       Value string\n"
         "%%       Key string\n"
         "%%EndEventDef\n"
         "%%EventDef PajeEndLink %d\n"
         "%%       Time date\n"
         "%%       Container string\n"
         "%%       Type string\n"
         "%%       EndContainer string\n"
         "%%       Value string\n"
         "%%       Key string\n"
         "%%EndEventDef\n"
         "%%EventDef PajeNewEvent %d\n"
         "%%       Time date\n"
         "%%       Container string\n"
         "%%       Type string\n"
         "%%       Value string\n"
         "%%EndEventDef\n",
         PAJE_DefineContainerType,
         PAJE_DefineVariableType,
         PAJE_DefineStateType,
         PAJE_DefineEventType,
         PAJE_DefineLinkType,
         PAJE_DefineEntityValue,
         PAJE_CreateContainer,
         PAJE_DestroyContainer,
         PAJE_SetVariable,
         PAJE_AddVariable,
         PAJE_SubVariable,
         PAJE_SetState,
         PAJE_PushState,
         PAJE_PopState,
         PAJE_StartLink,
         PAJE_EndLink,
         PAJE_NewEvent);
}

void pajeDefineContainerType(const char *alias,
                             const char *containerType,
                             const char *name)
{
  fprintf(paje_file,"%d %s %s \"%s\"\n",
         PAJE_DefineContainerType,
         alias,
         containerType,
         name);
}

void pajeDefineStateType(const char *alias,
                         const char *containerType,
                         const char *name)
{
  fprintf(paje_file,"%d %s %s \"%s\"\n",
         PAJE_DefineStateType,
         alias,
         containerType,
         name);
}

void pajeDefineLinkType(const char *alias,
                        const char *containerType,
                        const char *sourceContainerType,
                        const char *destContainerType,
                        const char *name)
{
  fprintf(paje_file,"%d %s %s %s %s \"%s\"\n",
         PAJE_DefineLinkType,
         alias,
         containerType,
         sourceContainerType,
         destContainerType,
         name);
}

void pajeDefineEventType(const char *alias,
                        const char *containerType,
                        const char *name,
                        const char *color)
{
  fprintf(paje_file,"%d %s %s \"%s\" \"%s\"\n",
         PAJE_DefineEventType,
         alias,
         containerType,
         name,
         color);
}

void pajeDefineEntityValue(const char *alias,
                           const char *entityType,
                           const char *name,
                           const char *color)
{
  fprintf(paje_file,"%d %s %s \"%s\" \"%s\"\n",
          PAJE_DefineEntityValue,
          alias,
          entityType,
          name,
          color);
}

void pajeCreateContainer(double timestamp,
                         const char *alias,
                         const char *type,
                         const char *container,
                         const char *name)
{
  fprintf(paje_file,"%d %.9f %s %s %s \"%s\"\n",
         PAJE_CreateContainer,
         paje_event_timestamp(timestamp),
         alias,
         type,
         container,
         name);
}

void pajeDestroyContainer(double timestamp,
                          const char *type,
                          const char *container)
{
  fprintf(paje_file,"%d %.9f %s %s\n",
         PAJE_DestroyContainer,
         paje_event_timestamp(timestamp),
         type,
         container);
}

void pajeSetState(double timestamp,
                  const char *container,
                  const char *type,
                  const char *value)
{
  fprintf(paje_file,"%d %.9f %s %s %s\n",
         PAJE_SetState,
         paje_event_timestamp(timestamp),
         container,
         type,
         value);
}


void pajePushState(double timestamp,
                   const char *container,
                   const char *type,
                   const char *value)
{
  fprintf(paje_file,"%d %.9f %s %s %s\n",
         PAJE_PushState,
         paje_event_timestamp(timestamp),
         container,
         type,
         value);
}

void pajePopState(double timestamp,
                  const char *container,
                  const char *type)
{
  fprintf(paje_file,"%d %.9f %s %s\n",
         PAJE_PopState,
         paje_event_timestamp(timestamp),
         container,
         type);
}

void pajeStartLink(double timestamp,
                   const char *container,
                   const char *type,
                   const char *sourceContainer,
                   const char *value,
                   const char *key)
{
  fprintf(paje_file,"%d %.9f %s %s %s %s %s\n",
         PAJE_StartLink,
         paje_event_timestamp(timestamp),
         container,
         type,
         sourceContainer,
         value,
         key);
}

void pajeEndLink(double timestamp,
                 const char *container,
                 const char *type,
                 const char *endContainer,
                 const char *value,
                 const char *key)
{
  fprintf(paje_file,"%d %.9f %s %s %s %s %s\n",
         PAJE_EndLink,
         paje_event_timestamp(timestamp),
         container,
         type,
         endContainer,
         value,
         key);
}

void pajeNewEvent(double timestamp,
                 const char *container,
                 const char *type,
                 const char *value )
{
  fprintf(paje_file,"%d %.9f %s %s %s\n",
         PAJE_NewEvent,
         paje_event_timestamp(timestamp),
         container,
         type,
         value);
}
