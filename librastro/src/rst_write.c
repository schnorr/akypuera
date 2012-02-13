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
#include "rst_private.h"

#ifndef LIBRASTRO_THREADED
rst_buffer_t *rst_global_buffer;
#else
pthread_key_t rst_key;
#endif

static char rst_dirname[FILENAME_MAX];
static void rst_event_lls_ptr(rst_buffer_t * ptr, u_int16_t type,
                              u_int64_t l0, u_int64_t l1, char *s0);

#ifdef HAVE_CLOCKGETTIME
int (*rastro_gettimeofday) (clockid_t clk_id, struct timespec *tp) = NULL;
#elif HAVE_GETTIMEOFDAY
int (*rastro_gettimeofday) (struct timeval *tv, struct timezone *tz) = NULL;
#endif

void rst_destroy_buffer(void *p)
{
  rst_buffer_t *ptr = (rst_buffer_t *) p;

  if (ptr != NULL) {
    int fd;
    rst_event_ptr(ptr, RST_EVENT_STOP);
    rst_flush(ptr);
    free(ptr->rst_buffer);
    fd = RST_FD(ptr);
    close(fd);
    free(ptr);
  }
}

void rst_init(u_int64_t id1, u_int64_t id2)
{
#ifdef HAVE_CLOCKGETTIME
  rst_init_timestamp (id1, id2, &clock_gettime);
#elif HAVE_GETTIMEOFDAY
  rst_init_timestamp (id1, id2, &gettimeofday);
#endif
}

#ifdef HAVE_CLOCKGETTIME
void rst_init_timestamp(u_int64_t id1, u_int64_t id2, int (*timestamp) (clockid_t clk_id, struct timespec *tp))
#elif HAVE_GETTIMEOFDAY
void rst_init_timestamp(u_int64_t id1, u_int64_t id2, int (*timestamp) (struct timeval *tv, struct timezone *tz))
#endif
{
  rst_buffer_t *ptr;
  ptr = (rst_buffer_t *) malloc(sizeof(rst_buffer_t));

  rst_init_ptr_timestamp (ptr, id1, id2, timestamp);
}

void rst_init_ptr(rst_buffer_t *ptr, u_int64_t id1, u_int64_t id2)
{
#ifdef HAVE_CLOCKGETTIME
  rst_init_ptr_timestamp(ptr, id1, id2, &clock_gettime);
#elif HAVE_GETTIMEOFDAY
  rst_init_ptr_timestamp(ptr, id1, id2, &gettimeofday);
#endif
}

#ifdef HAVE_CLOCKGETTIME
void rst_init_ptr_timestamp(rst_buffer_t * ptr,
                            u_int64_t id1,
                            u_int64_t id2,
                            int (*timestamp) (clockid_t clk_id, struct timespec *tp))
#elif HAVE_GETTIMEOFDAY
void rst_init_ptr_timestamp(rst_buffer_t * ptr,
                            u_int64_t id1,
                            u_int64_t id2,
                            int (*timestamp) (struct timeval *tv, struct timezone *tz))
#endif
{
  int fd;
  char fname[30];
  char hostname[MAXHOSTNAMELEN + 1];

  if (ptr == NULL) {
    fprintf(stderr, "[rastro] error inicializing - invalid pointer\n");
    return;
  }
#ifdef LIBRASTRO_THREADED
  static int rst_key_initialized = 0;
  if (!rst_key_initialized) {
    pthread_key_create(&rst_key, NULL);
    rst_key_initialized = 1;
  }
#endif

  //define the timestamp function to be used by librastro
  rastro_gettimeofday = timestamp;

  RST_SET_PTR(ptr);
  ptr->rst_buffer_size = 100000;
  ptr->rst_buffer = malloc(ptr->rst_buffer_size);
  bzero(ptr->rst_buffer, ptr->rst_buffer_size);
  RST_RESET(ptr);

  sprintf(fname, "rastro-%" PRIu64 "-%" PRIu64 ".rst", /* dirname, */ id1,
          id2);
  fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if (fd == -1) {
    fprintf(stderr, "[rastro] cannot open file %s: %s\n",
            fname, strerror(errno));
    return;
  }

  RST_SET_FD(ptr, fd);

  // this will force first event to register sync time
  RST_SET_T0(ptr, 0);

  gethostname(hostname, sizeof(hostname));

  XCAT(rst_event_, LETTER_UINT64, LETTER_STRING, _ptr) (ptr, RST_EVENT_INIT,
                                                      id1, id2, hostname);
}

void rst_flush(rst_buffer_t * ptr)
{
  size_t nbytes;
  size_t n;

  nbytes = RST_BUF_COUNT(ptr);
  n = write(RST_FD(ptr), RST_BUF_DATA(ptr), nbytes);
  if (n != nbytes) {
    fprintf(stderr, "[rastro] error writing rastro file\n");
  }
  RST_RESET(ptr);
}

void rst_finalize(void)
{
  rst_buffer_t *ptr = RST_PTR;
  rst_destroy_buffer(ptr);
}

void rst_finalize_ptr(rst_buffer_t * ptr)
{
  rst_destroy_buffer(ptr);
}

void rst_startevent(rst_buffer_t *ptr, u_int32_t header)
{
    timestamp_t deltasec;
    timestamp_t sec;
    timestamp_t precision;
#ifdef HAVE_CLOCKGETTIME
    struct timespec tp;
    clock_gettime (CLOCK_REALTIME, &tp);
    sec = tp.tv_sec;
    precision = tp.tv_nsec;
#elif HAVE_GETTIMEOFDAY
    struct timeval tp;
    rastro_gettimeofday(&tp, NULL);
    sec = tp.tv_sec;
    precision = tp.tv_usec;
#endif
    deltasec = sec - RST_T0(ptr);
    if (deltasec > 3600) {
        RST_SET_T0(ptr, sec);
        deltasec = 0;
        RST_PUT(ptr, u_int32_t, header | RST_TIME_SET);
        RST_PUT(ptr, u_int32_t, sec);
    } else {
        RST_PUT(ptr, u_int32_t, header);
    }
    timestamp_t resolution = RST_CLOCK_RESOLUTION;
    RST_PUT(ptr, u_int64_t, deltasec * resolution + precision);
}

void rst_endevent(rst_buffer_t * ptr)
{
    ptr->rst_buffer_ptr = ALIGN_PTR(ptr->rst_buffer_ptr);
    if (RST_BUF_COUNT(ptr) > (RST_BUF_SIZE(ptr) - RST_MAX_EVENT_SIZE)) {
        rst_flush(ptr);
    }
}

/*
 * basic event only with a type
 */
void rst_event(u_int16_t type)
{
  rst_event_ptr (RST_PTR, type);
}

void rst_event_ptr(rst_buffer_t * ptr, u_int16_t type)
{
/*...2 para finalizar esse rastro que so tem o tipo...*/
  rst_startevent(ptr, type << 18 | 0x20000);
  rst_endevent(ptr);
}

/*
 * lls event to be used by rastro itself (for the first event)
 */ 
static void rst_event_lls_ptr(rst_buffer_t * ptr, u_int16_t type,
                              u_int64_t l0, u_int64_t l1, char *s0)
{
  rst_startevent(ptr, type << 18 | 0x24410);
  RST_PUT(ptr, u_int64_t, l0);
  RST_PUT(ptr, u_int64_t, l1);
  RST_PUT_STR(ptr, s0);
  rst_endevent(ptr);
}
