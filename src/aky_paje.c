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
#include "aky_private.h"

static double paje_event_timestamp(long timestamp)
{
  static long first_timestamp = -1;
  if (first_timestamp == -1) {
    first_timestamp = timestamp;
  }
  long dif = timestamp - first_timestamp;
  return (double)dif/RST_CLOCK_RESOLUTION;
}

void pajeDefineContainerType(const char *alias,
                             const char *containerType, const char *name)
{
  printf("%d %s %s %s\n", PAJE_DefineContainerType,
         alias, containerType, name);
}

void pajeDefineStateType(const char *alias,
                         const char *containerType, const char *name)
{
  printf("%d %s %s %s\n", PAJE_DefineStateType, alias, containerType, name);
}

void pajeDefineLinkType(const char *alias,
                        const char *containerType,
                        const char *sourceContainerType,
                        const char *destContainerType, const char *name)
{
  printf("%d %s %s %s %s %s\n", PAJE_DefineLinkType,
         alias, containerType, sourceContainerType, destContainerType,
         name);
}

void pajeCreateContainer(long timestamp,
                         const char *alias,
                         const char *type,
                         const char *container, const char *name)
{
  printf("%d %g %s %s %s %s\n", PAJE_CreateContainer,
         paje_event_timestamp(timestamp), alias, type, container, name);
}

void pajeDestroyContainer(long timestamp,
                          const char *type, const char *container)
{
  printf("%d %g %s %s\n", PAJE_DestroyContainer,
         paje_event_timestamp(timestamp), type, container);
}

void pajeSetState(long timestamp,
                  const char *container,
                  const char *type, const char *value)
{
  printf("%d %g %s %s %s\n", PAJE_SetState,
         paje_event_timestamp(timestamp), container, type, value);
}


void pajePushState(long timestamp,
                   const char *container,
                   const char *type, const char *value)
{
  printf("%d %g %s %s %s\n", PAJE_PushState,
         paje_event_timestamp(timestamp), container, type, value);
}

void pajePushStateWithMark(long timestamp,
                   const char *container,
                   const char *type, const char *value, const int mark)
{
  printf("%d %g %s %s %s %d\n", PAJE_PushStateWithMark,
         paje_event_timestamp(timestamp), container, type, value, mark);
}

void pajePopState(long timestamp,
                  const char *container, const char *type)
{
  printf("%d %g %s %s\n", PAJE_PopState,
         paje_event_timestamp(timestamp), container, type);
}

void pajeStartLink(long timestamp,
                   const char *container,
                   const char *type,
                   const char *sourceContainer,
                   const char *value,
                   const char *key)
{
  printf("%d %g %s %s %s %s %s\n", PAJE_StartLink,
         paje_event_timestamp(timestamp),
         container, type, sourceContainer, value, key);
}

void pajeStartLinkWithMessageSize(long timestamp,
                   const char *container,
                   const char *type,
                   const char *sourceContainer,
                   const char *value,
                   const char *key,
                   const int messageSize)
{
  printf("%d %g %s %s %s %s %s %d\n", PAJE_StartLinkWithMessageSizeAndMark,
         paje_event_timestamp(timestamp),
         container, type, sourceContainer, value, key, messageSize);
}

void pajeStartLinkWithMessageSizeAndMark(long timestamp,
                   const char *container,
                   const char *type,
                   const char *sourceContainer,
                   const char *value,
                   const char *key,
                   const int messageSize,
                   const int mark)
{
  printf("%d %g %s %s %s %s %s %d %d\n", PAJE_StartLinkWithMessageSizeAndMark,
         paje_event_timestamp(timestamp),
         container, type, sourceContainer, value, key, messageSize, mark);
}

void pajeEndLink(long timestamp,
                 const char *container,
                 const char *type,
                 const char *endContainer,
                 const char *value, const char *key)
{
  printf("%d %g %s %s %s %s %s\n", PAJE_EndLink,
         paje_event_timestamp(timestamp),
         container, type, endContainer, value, key);
}

void paje_header(void)
{
  printf("%%EventDef PajeDefineContainerType %d\n"
         "%%       Alias string\n"
         "%%       ContainerType string\n"
         "%%       Name string\n"
         "%%EndEventDef\n"
         "%%EventDef PajeDefineVariableType %d\n"
         "%%       Alias string\n"
         "%%       ContainerType string\n"
         "%%       Name string\n"
         "%%       Color color\n"
         "%%EndEventDef\n"
         "%%EventDef PajeDefineStateType %d\n"
         "%%       Alias string\n"
         "%%       ContainerType string\n"
         "%%       Name string\n"
         "%%EndEventDef\n"
         "%%EventDef PajeDefineEventType %d\n"
         "%%       Alias string\n"
         "%%       ContainerType string\n"
         "%%       Name string\n"
         "%%       Color color\n"
         "%%EndEventDef\n"
         "%%EventDef PajeDefineLinkType %d\n"
         "%%       Alias string\n"
         "%%       ContainerType string\n"
         "%%       SourceContainerType string\n"
         "%%       DestContainerType string\n"
         "%%       Name string\n"
         "%%EndEventDef\n"
         "%%EventDef PajeDefineEntityValue %d\n"
         "%%       Alias string\n"
         "%%       EntityType string\n"
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
         "%%EventDef PajePushState %d\n"
         "%%       Time date\n"
         "%%       Container string\n"
         "%%       Type string\n"
         "%%       Value string\n"
         "%%       Mark string\n"
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
         "%%       SourceContainer string\n"
         "%%       Value string\n"
         "%%       Key string\n"
         "%%EndEventDef\n"
         "%%EventDef PajeStartLink %d\n"
         "%%       Time date\n"
         "%%       Container string\n"
         "%%       Type string\n"
         "%%       SourceContainer string\n"
         "%%       Value string\n"
         "%%       Key string\n"
         "%%       MessageSize string\n"
         "%%EndEventDef\n"
         "%%EventDef PajeStartLink %d\n"
         "%%       Time date\n"
         "%%       Container string\n"
         "%%       Type string\n"
         "%%       SourceContainer string\n"
         "%%       Value string\n"
         "%%       Key string\n"
         "%%       MessageSize string\n"
         "%%       Mark string\n"
         "%%EndEventDef\n"
         "%%EventDef PajeEndLink %d\n"
         "%%       Time date\n"
         "%%       Container string\n"
         "%%       Type string\n"
         "%%       DestContainer string\n"
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
         PAJE_PushStateWithMark,
         PAJE_PopState,
         PAJE_StartLink,
         PAJE_StartLinkWithMessageSize,
         PAJE_StartLinkWithMessageSizeAndMark,
         PAJE_EndLink,
         PAJE_NewEvent);
}

void paje_hierarchy(void)
{
  pajeDefineContainerType("PROCESS", "0", "PROCESS");
  pajeDefineStateType("STATE", "PROCESS", "STATE");
  pajeDefineLinkType("LINK", "0", "PROCESS", "PROCESS", "LINK");
}
