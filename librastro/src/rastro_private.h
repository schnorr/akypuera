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
#include "rastro_list.h"

// Aligns pointer p to 4-byte-aligned address
#define ALIGN_PTR(p) ((void *)(((intptr_t)(p)+(4-1))&(~(4-1))))

#define RST_MAX_EVENT_SIZE 1000

#define RST_RESET(ptr) (ptr->rst_buffer_ptr = ptr->rst_buffer)
#define RST_FD(ptr) (ptr->rst_fd)
#define RST_SET_FD(ptr, fd) (ptr->rst_fd = fd)
#define RST_T0(ptr) (ptr->rst_t0)
#define RST_SET_T0(ptr, t) (ptr->rst_t0 = t)
#define RST_BUF_COUNT(ptr) (ptr->rst_buffer_ptr - ptr->rst_buffer)
#define RST_BUF_DATA(ptr) (ptr->rst_buffer)
#define RST_BUF_SIZE(ptr) (ptr->rst_buffer_size)

#ifndef LIBRASTRO_THREADED
extern rst_buffer_t *rst_global_buffer;
#define RST_PTR (rst_global_buffer)
#define RST_SET_PTR(ptr) (rst_global_buffer = ptr)
#else
extern int rst_key_initialized;
extern pthread_key_t rst_key;
#define RST_PTR ((rst_buffer_t *) pthread_getspecific(rst_key))
#define RST_SET_PTR(ptr) pthread_setspecific(rst_key, (void *) ptr)
#endif

#define RST_PUT(ptr, type, val)						\
	do {								\
		type *p = (type *)ptr->rst_buffer_ptr;			\
		*p++ = (type)(val);					\
		ptr->rst_buffer_ptr = (char *)p;			\
	} while (0)
#define RST_PUT_STR(ptr, str) 						\
	do {                  						\
		char *__s1 = (char *)ptr->rst_buffer_ptr;		\
		char *__s2 = (char *)str;                               \
		while ((*__s1++ = *__s2++) != '\0') 			\
			;						\
		ptr->rst_buffer_ptr = ALIGN_PTR(__s1); 	\
	} while(0)

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

void rst_flush(rst_buffer_t * ptr);

// finishes an event
static inline void rst_endevent(rst_buffer_t * ptr)
{
    ptr->rst_buffer_ptr = ALIGN_PTR(ptr->rst_buffer_ptr);
    if (RST_BUF_COUNT(ptr) > (RST_BUF_SIZE(ptr) - RST_MAX_EVENT_SIZE)) {
        rst_flush(ptr);
    }
}

// starts an event
static inline void rst_startevent(rst_buffer_t *ptr, u_int32_t header)
{
    struct timeval tp;
    u_int32_t deltasec;

    gettimeofday(&tp, NULL);
    deltasec = tp.tv_sec - RST_T0(ptr);
    if (deltasec > 3600) {
        RST_SET_T0(ptr, tp.tv_sec);
        deltasec = 0;
        RST_PUT(ptr, u_int32_t, header | RST_TIME_SET);
        RST_PUT(ptr, u_int32_t, tp.tv_sec);
    } else {
        RST_PUT(ptr, u_int32_t, header);
    }
    RST_PUT(ptr, u_int32_t, deltasec * RST_CLOCK_RESOLUTION + tp.tv_usec);
}
#endif  /*    _RASTRO_PRIVATE_H_   */
