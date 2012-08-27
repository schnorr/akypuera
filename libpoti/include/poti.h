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
#ifndef __POTI_H
#define __POTI_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
 * Function to open output into a file, else output to stdout
 */
int poti_open (const char* filename);

/*
 * Function to close opened file
 */
void poti_close (void);

/*
 * Function to generate the header 
 */
void poti_header (int basic);

/*
 * Functions to define the type hierarchy of a Paje trace file 
 */
void poti_DefineContainerType(const char *alias,
                             const char *containerType,
                             const char *name);
void poti_DefineVariableType(const char *alias,
                             const char *containerType,
                             const char *name,
                             const char *color);
void poti_DefineStateType(const char *alias,
                         const char *containerType,
                         const char *name);
void poti_DefineEventType(const char *alias,
                         const char *containerType,
                         const char *name,
                         const char *color);
void poti_DefineLinkType(const char *alias,
                        const char *containerType,
                        const char *sourceContainerType,
                        const char *destContainerType,
                        const char *name);
void poti_DefineEntityValue(const char *alias,
                           const char *entityType,
                           const char *name,
                           const char *color);

/*
 * Functions related to containers 
 */
void poti_CreateContainer(double timestamp,
                         const char *alias,
                         const char *type,
                         const char *container,
                         const char *name);
void poti_DestroyContainer(double timestamp,
                          const char *type,
                          const char *container);

/*
 * Functions related to variables
 */

void poti_SetVariable (double timestamp,
                       const char *type,
                       const char *container,
                       double value);
void poti_AddVariable (double timestamp,
                       const char *type,
                       const char *container,
                       double value);
void poti_SubVariable (double timestamp,
                       const char *type,
                       const char *container,
                       double value);

/*
 * Functions related to states
 */
void poti_SetState(double timestamp,
                  const char *container,
                  const char *type,
                  const char *value);
void poti_PushState(double timestamp,
                   const char *container,
                   const char *type,
                   const char *value);
void poti_PushStateMark(double timestamp,
                        const char *container,
                        const char *type,
                        const char *value,
                        const int mark);
void poti_PopState(double timestamp,
                  const char *container,
                  const char *type);
void poti_ResetState(double timestamp,
                    const char *container,
                    const char *type);


/* 
 * Functions related to links
 */
void poti_StartLink(double timestamp,
                   const char *container,
                   const char *type,
                   const char *sourceContainer,
                   const char *value,
                   const char *key);
void poti_StartLinkSize(double timestamp,
                        const char *container,
                        const char *type,
                        const char *sourceContainer,
                        const char *value,
                        const char *key,
                        const int size);
void poti_StartLinkSizeMark(double timestamp,
                            const char *container,
                            const char *type,
                            const char *sourceContainer,
                            const char *value,
                            const char *key,
                            const int size,
                            const int mark);
void poti_EndLink(double timestamp,
                 const char *container,
                 const char *type,
                 const char *endContainer,
                 const char *value,
                 const char *key);

/*
 * Functions related to variables, instantaneous events
 */
void poti_NewEvent(double timestamp,
                  const char *container,
                  const char *type,
                  const char *value );

#if defined(__cplusplus)
}
#endif

#endif
