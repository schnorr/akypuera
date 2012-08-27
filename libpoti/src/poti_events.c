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

extern FILE *paje_file;
extern int paje_extended;

void poti_DefineContainerType(const char *alias,
                             const char *containerType,
                             const char *name)
{
  fprintf(paje_file,"%d %s %s \"%s\"\n",
         PAJE_DefineContainerType,
         alias,
         containerType,
         name);
}

void poti_DefineVariableType(const char *alias,
                             const char *containerType,
                             const char *name,
                             const char *color)
{
  fprintf (paje_file,"%d %s %s %s \"%s\"\n",
           PAJE_DefineVariableType,
           alias,
           containerType,
           name,
           color);
}

void poti_DefineStateType(const char *alias,
                         const char *containerType,
                         const char *name)
{
  fprintf(paje_file,"%d %s %s \"%s\"\n",
         PAJE_DefineStateType,
         alias,
         containerType,
         name);
}

void poti_DefineEventType(const char *alias,
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

void poti_DefineLinkType(const char *alias,
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

void poti_DefineEntityValue(const char *alias,
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

void poti_CreateContainer(double timestamp,
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

void poti_DestroyContainer(double timestamp,
                          const char *type,
                          const char *container)
{
  fprintf(paje_file,"%d %.9f %s %s\n",
         PAJE_DestroyContainer,
         paje_event_timestamp(timestamp),
         type,
         container);
}

void poti_SetVariable (double timestamp,
                       const char *container,
                       const char *type,
                       double value)
{
  fprintf(paje_file,"%d %.9f %s %s %f\n",
          PAJE_SetVariable,
          paje_event_timestamp(timestamp),
          container,
          type,
          value);
}

void poti_AddVariable (double timestamp,
                       const char *container,
                       const char *type,
                       double value)
{
  fprintf(paje_file,"%d %.9f %s %s %f\n",
          PAJE_AddVariable,
          paje_event_timestamp(timestamp),
          container,
          type,
          value);
}

void poti_SubVariable (double timestamp,
                       const char *container,
                       const char *type,
                       double value)
{
  fprintf(paje_file,"%d %.9f %s %s %f\n",
          PAJE_SubVariable,
          paje_event_timestamp(timestamp),
          container,
          type,
          value);
}

void poti_SetState(double timestamp,
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


void poti_PushState(double timestamp,
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

void poti_PushStateMark(double timestamp,
                        const char *container,
                        const char *type,
                        const char *value,
                        const int mark)
{
  fprintf(paje_file,"%d %.9f %s %s %s %d\n",
          PAJE_PushStateMark,
          paje_event_timestamp(timestamp),
          container,
          type,
          value,
          mark);
}

void poti_PopState(double timestamp,
                  const char *container,
                  const char *type)
{
  fprintf(paje_file,"%d %.9f %s %s\n",
         PAJE_PopState,
         paje_event_timestamp(timestamp),
         container,
         type);
}

void poti_ResetState(double timestamp,
                   const char *container,
                   const char *type)
{
  fprintf(paje_file,"%d %.9f %s %s\n",
         PAJE_ResetState,
         paje_event_timestamp(timestamp),
         container,
         type);
}

void poti_StartLink(double timestamp,
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

void poti_StartLinkSize(double timestamp,
                        const char *container,
                        const char *type,
                        const char *sourceContainer,
                        const char *value,
                        const char *key,
                        const int size)
{
  if (paje_extended){
    fprintf(paje_file,"%d %.9f %s %s %s %s %s %d\n",
            PAJE_StartLinkSize,
            paje_event_timestamp(timestamp),
            container,
            type,
            sourceContainer,
            value,
            key,
            size);
  }else{
    poti_StartLink (timestamp, container, type, sourceContainer, value, key);
  }
}

void poti_StartLinkSizeMark(double timestamp,
                            const char *container,
                            const char *type,
                            const char *sourceContainer,
                            const char *value,
                            const char *key,
                            const int size,
                            const int mark)
{
  if (paje_extended){
    fprintf(paje_file,"%d %.9f %s %s %s %s %s %d %d\n",
            PAJE_StartLinkSizeMark,
            paje_event_timestamp(timestamp),
            container,
            type,
            sourceContainer,
            value,
            key,
            size,
            mark);
  }else{
    poti_StartLink (timestamp, container, type, sourceContainer, value, key);
  }
}

void poti_EndLink(double timestamp,
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

void poti_NewEvent(double timestamp,
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
