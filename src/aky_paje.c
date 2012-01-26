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

//static timestamp_t first_timestamp = -1;
static double first_timestamp = -1;

static s_paje_event_t paje_events[] = {
  {"PajeDefineContainerType",
   "% Alias string\n"
   "% ContainerType string\n"
   "% Name string",
   -1},
  {"PajeDefineStateType",
   "% Alias string\n"
   "% ContainerType string\n"
   "% Name string",
   -1},
  {"PajeDefineLinkType",
   "% Alias string\n"
   "% ContainerType string\n"
   "% SourceContainerType string\n"
   "% DestContainerType string\n"
   "% Name string",
   -1},
  {"PajeCreateContainer",
   "% Time string\n"
   "% Alias string\n"
   "% Type string\n"
   "% Container string\n"
   "% Name string",
   -1},
  {"PajeDestroyContainer",
   "% Time string\n"
   "% Type string\n"
   "% Container string",
   -1},
  {"PajeSetState",
   "% Time string\n"
   "% Container string\n"
   "% EntityType string\n"
   "% Value string",
   -1},
  {"PajePushState",
   "% Time string\n"
   "% Container string\n"
   "% EntityType string\n"
   "% Value string\n"
   "% Mark string",
   -1},
  {"PajePopState",
   "% Time string\n"
   "% Container string\n"
   "% EntityType string",
   -1},
  {"PajeStartLink",
   "% Time string\n"
   "% Container string\n"
   "% EntityType string\n"
   "% SourceContainer string\n"
   "% Value string\n"
   "% Key string\n"
   "% MessageSize string\n"
   "% Mark string",
   -1},
  {"PajeEndLink",
   "% Time string\n"
   "% Container string\n"
   "% EntityType string\n"
   "% DestContainer string\n"
   "% Value string\n"
   "% Key string",
   -1},
  {NULL, NULL, -1}
};

static int paje_event_id(const char *name)
{
  int i;
  for (i = 0; paje_events[i].name; i++)
    if (strcmp(name, paje_events[i].name) == 0)
      return paje_events[i].id;
  return -1;
}

static double paje_event_timestamp(double timestamp)
{
  if (first_timestamp == -1) {
    first_timestamp = timestamp;
  }
  return timestamp - first_timestamp;
}

void pajeDefineContainerType(const char *alias,
                             const char *containerType, const char *name)
{
  printf("%d %s %s %s\n",
         paje_event_id("PajeDefineContainerType"),
         alias, containerType, name);
}

void pajeDefineStateType(const char *alias,
                         const char *containerType, const char *name)
{
  printf("%d %s %s %s\n",
         paje_event_id("PajeDefineStateType"), alias, containerType, name);
}

void pajeDefineLinkType(const char *alias,
                        const char *containerType,
                        const char *sourceContainerType,
                        const char *destContainerType, const char *name)
{
  printf("%d %s %s %s %s %s\n",
         paje_event_id("PajeDefineLinkType"),
         alias, containerType, sourceContainerType, destContainerType,
         name);
}

void pajeCreateContainer(double timestamp,
                         const char *alias,
                         const char *type,
                         const char *container, const char *name)
{
  printf("%d %f %s %s %s %s\n",
         paje_event_id("PajeCreateContainer"),
         paje_event_timestamp(timestamp), alias, type, container, name);
}

void pajeDestroyContainer(double timestamp,
                          const char *type, const char *container)
{
  printf("%d %f %s %s\n",
         paje_event_id("PajeDestroyContainer"),
         paje_event_timestamp(timestamp), type, container);
}

void pajeSetState(double timestamp,
                  const char *container,
                  const char *type, const char *value)
{
  printf("%d %f %s %s %s\n",
         paje_event_id("PajeSetState"),
         paje_event_timestamp(timestamp), container, type, value);
}

void pajePushState(double timestamp,
                   const char *container,
                   const char *type, const char *value, const int mark)
{
  printf("%d %f %s %s %s %d\n",
         paje_event_id("PajePushState"),
         paje_event_timestamp(timestamp), container, type, value, mark);
}

void pajePopState(double timestamp,
                  const char *container, const char *type)
{
  printf("%d %f %s %s\n",
         paje_event_id("PajePopState"),
         paje_event_timestamp(timestamp), container, type);
}

void pajeStartLink(double timestamp,
                   const char *container,
                   const char *type,
                   const char *sourceContainer,
                   const char *value,
                   const char *key,
                   const int messageSize,
                   const int mark)
{
  printf("%d %f %s %s %s %s %s %d %d\n",
         paje_event_id("PajeStartLink"),
         paje_event_timestamp(timestamp),
         container, type, sourceContainer, value, key, messageSize, mark);
}

void pajeEndLink(double timestamp,
                 const char *container,
                 const char *type,
                 const char *endContainer,
                 const char *value, const char *key)
{
  printf("%d %f %s %s %s %s %s\n",
         paje_event_id("PajeEndLink"),
         paje_event_timestamp(timestamp),
         container, type, endContainer, value, key);
}

void paje_header(void)
{
  int i;
  for (i = 0; paje_events[i].name; i++) {
    paje_events[i].id = i;
    printf("%%EventDef %s %d\n%s\n%%EndEventDef\n",
           paje_events[i].name, paje_events[i].id,
           paje_events[i].description);
  }
}

void paje_hierarchy(void)
{
  pajeDefineContainerType("PROCESS", "0", "PROCESS");
  pajeDefineStateType("STATE", "PROCESS", "STATE");
  pajeDefineLinkType("LINK", "0", "PROCESS", "PROCESS", "LINK");
}
