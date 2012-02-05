/*
    Copyright (c) 1998--2006 Benhur Stein
    
    This file is part of Pajé.

    Pajé is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    Pajé is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
    for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Pajé; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02111 USA.
*/
#ifndef _RASTRO_PRIVATE_H_
#define _RASTRO_PRIVATE_H_
#include "rastro.h"
#include "rastro_config.h"

//the timestamp function used by librastro
extern int (*rastro_gettimeofday) (struct timeval *tv, struct timezone *tz);

#define RST_MAX_EVENT_SIZE 1000

#define RST_RESET(ptr) (ptr->rst_buffer_ptr = ptr->rst_buffer)
#define RST_FD(ptr) (ptr->rst_fd)
#define RST_SET_FD(ptr, fd) (ptr->rst_fd = fd)
#define RST_T0(ptr) (ptr->rst_t0)
#define RST_SET_T0(ptr, t) (ptr->rst_t0 = t)
#define RST_BUF_COUNT(ptr) (ptr->rst_buffer_ptr - ptr->rst_buffer)
#define RST_BUF_DATA(ptr) (ptr->rst_buffer)
#define RST_BUF_SIZE(ptr) (ptr->rst_buffer_size)


//#define RST_GET(ptr, type) (*((type *)(ptr))++)
#define RST_GET(ptr, type) ((ptr += sizeof(type)),			\
			    (*(type *)(ptr - sizeof(type))))
#define RST_GET_STR(ptr, str)               \
    do {                                    \
        char *__s1 = ptr;                     \
        char *__s2 = str;                     \
        while ((*__s2++ = *__s1++) != '\0')     \
            ;                               \
        ptr = ALIGN_PTR(__s1);   \
    } while(0)


//#endif


#define RST_NO_TYPE     0
#define RST_STRING_TYPE 1
#define RST_DOUBLE_TYPE 2
#define RST_FLOAT_TYPE  3
#define RST_LONG_TYPE   4
#define RST_SHORT_TYPE  5
#define RST_CHAR_TYPE   6
#define RST_INT_TYPE    7
#define RST_TYPE_CTR    7

#define RST_TIME_SET    0x10000
#define RST_LAST        0x2

#define RST_BITS_PER_FIELD   4
#define RST_FIELD_MASK       0xf
#define RST_FIELDS_IN_FIRST  4
#define RST_FIELDS_IN_OTHERS 7

void rst_fill_buffer(rst_one_file_t * of_data);
void rst_destroy_buffer(void *p);
void extract_arguments(int *argcp, char ***argvp);
void rst_flush(rst_buffer_t * ptr);




#endif  /*    _RASTRO_PRIVATE_H_   */
