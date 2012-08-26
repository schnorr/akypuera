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
int pajeOpen (const char* filename);

/*
 * Function to close opened file
 */
void pajeClose (void);

/*
 * Function to generate the header 
 */
void pajeHeader (void);

/*
 * Functions to define the type hierarchy of a Paje trace file 
 */
void pajeDefineContainerType(const char *alias,
                             const char *containerType,
                             const char *name);
void pajeDefineStateType(const char *alias,
                         const char *containerType,
                         const char *name);
void pajeDefineLinkType(const char *alias,
                        const char *containerType,
                        const char *sourceContainerType,
                        const char *destContainerType,
                        const char *name);
void pajeDefineEventType(const char *alias,
                         const char *containerType,
                         const char *name,
                         const char *color);
void pajeDefineEntityValue(const char *alias,
                           const char *entityType,
                           const char *name,
                           const char *color);

/*
 * Functions related to containers 
 */
void pajeCreateContainer(double timestamp,
                         const char *alias,
                         const char *type,
                         const char *container,
                         const char *name);
void pajeDestroyContainer(double timestamp,
                          const char *type,
                          const char *container);

/*
 * Functions related to states
 */
void pajeSetState(double timestamp,
                  const char *container,
                  const char *type,
                  const char *value);
void pajePushState(double timestamp,
                   const char *container,
                   const char *type,
                   const char *value);
void pajePopState(double timestamp,
                  const char *container,
                  const char *type);

/* 
 * Functions related to links
 */
void pajeStartLink(double timestamp,
                   const char *container,
                   const char *type,
                   const char *sourceContainer,
                   const char *value,
                   const char *key);
void pajeEndLink(double timestamp,
                 const char *container,
                 const char *type,
                 const char *endContainer,
                 const char *value,
                 const char *key);

/*
 * Functions related to variables, instantaneous events
 */
void pajeNewEvent(double timestamp,
                  const char *container,
                  const char *type,
                  const char *value );

#if defined(__cplusplus)
}
#endif

#endif
