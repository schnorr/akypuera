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
#include "rastro_private.h"

/*
  Internal functions
 */

// read an event from the buffer
static char *trd_event(timestamp_t * hora_global, rst_event_t * evento,
                       char *ptr)
{
  u_int32_t header;
  char field_types[100];
  int field_ctr = 0;
  int i;
  int fields_in_header;

  header = RST_GET(ptr, u_int32_t);

  evento->type = header >> 18;

  evento->ct.n_uint64 = evento->ct.n_uint16 = evento->ct.n_double =
      evento->ct.n_uint32 = 0;
  evento->ct.n_float = evento->ct.n_string = evento->ct.n_uint8 = 0;

  if (header & RST_TIME_SET) {
    long long resolution = RST_CLOCK_RESOLUTION;
    long long seconds = (long long) RST_GET(ptr, u_int32_t);
    *hora_global = seconds * resolution;
  }
  evento->timestamp = (timestamp_t)RST_GET(ptr, u_int64_t);
  evento->timestamp += *hora_global;

  fields_in_header = RST_FIELDS_IN_FIRST;
  for (;;) {
    for (i = 0; i < fields_in_header; i++) {
      char type;
      int bits_to_shift = RST_BITS_PER_FIELD * (fields_in_header - i - 1);
      type = (header >> bits_to_shift) & RST_FIELD_MASK;
      if (type == RST_NO_TYPE) {
        break;
      }
      field_types[field_ctr++] = type;
    }
    if (!((header >> (fields_in_header * RST_BITS_PER_FIELD)) & RST_LAST)) {
      // there are more headers -- process next one
      header = RST_GET(ptr, u_int32_t);
      fields_in_header = RST_FIELDS_IN_OTHERS;
    } else {
      // it was last header -- stop
      break;
    }
  }
  for (i = 0; i < field_ctr; i++) {
    switch (field_types[i]) {
    case RST_STRING_TYPE:
      RST_GET_STR(ptr, evento->v_string[evento->ct.n_string++]);
      break;
    case RST_DOUBLE_TYPE:
      evento->v_double[evento->ct.n_double++] = RST_GET(ptr, double);
      break;
    case RST_LONG_TYPE:
      evento->v_uint64[evento->ct.n_uint64++] = RST_GET(ptr, u_int64_t);
      break;
    case RST_FLOAT_TYPE:
      evento->v_float[evento->ct.n_float++] = RST_GET(ptr, float);
      break;
    case RST_INT_TYPE:
      evento->v_uint32[evento->ct.n_uint32++] = RST_GET(ptr, u_int32_t);
      break;
    case RST_SHORT_TYPE:
      evento->v_uint16[evento->ct.n_uint16++] = RST_GET(ptr, u_int16_t);
      break;
    case RST_CHAR_TYPE:
      evento->v_uint8[evento->ct.n_uint8++] = RST_GET(ptr, u_int8_t);
      break;
    }
  }

  ptr = ALIGN_PTR(ptr);

  return ptr;
}

// correct a timestamp according to synchronization data 
static timestamp_t rst_correct_time(timestamp_t remote, ct_t * ct)
{
  timestamp_t local;

  local = (timestamp_t) (ct->a * (double) (remote - ct->loc0)) + ct->ref0;

  return local;
}

static void find_timesync_data(char *filename, rst_one_file_t * of_data)
{
  FILE *ct_file;
  char refhost[MAXHOSTNAMELEN + 1];
  char host[MAXHOSTNAMELEN + 1];
  timestamp_t time;
  timestamp_t reftime;
  int first = 1;

  of_data->sync_time.a = 1;
  of_data->sync_time.loc0 = 0;
  of_data->sync_time.ref0 = 0;

  if (filename == NULL) {
    return;
  }
  ct_file = fopen(filename, "r");
  if (ct_file == NULL) {
    return;
  }

  while (!feof(ct_file)) {
    fscanf(ct_file, "%s %lld %s %lld", refhost, &reftime, host, &time);
    if (strcmp(refhost, of_data->hostname) == 0) {
      // this is the reference host
      of_data->sync_time.ref0 = reftime;
      of_data->sync_time.loc0 = reftime;
      break;
    }
    if (strcmp(host, of_data->hostname) == 0) {
      if (first) {
        of_data->sync_time.ref0 = reftime;
        of_data->sync_time.loc0 = time;
        first = 0;
      } else {
        of_data->sync_time.a = (double) (reftime - of_data->sync_time.ref0)
            / (double) (time - of_data->sync_time.loc0);
      }
    }
  }
  fclose(ct_file);
}

void rst_fill_buffer(rst_one_file_t * of_data)
{
  int bytes_processed;
  int bytes_remaining;
  int bytes_free;
  int bytes_read;

  bytes_processed = of_data->rst_buffer_ptr - of_data->rst_buffer;
  bytes_remaining = of_data->rst_buffer_used - bytes_processed;
  if (bytes_remaining >= RST_MAX_EVENT_SIZE) {
    return;
  }

  bytes_free = of_data->rst_buffer_size - of_data->rst_buffer_used;
  if (bytes_free < RST_MAX_EVENT_SIZE) {
    memmove(of_data->rst_buffer, of_data->rst_buffer_ptr, bytes_remaining);
    of_data->rst_buffer_used = bytes_remaining;
    of_data->rst_buffer_ptr = of_data->rst_buffer;
    bytes_free += bytes_processed;
  }

  bytes_read =
      read(of_data->fd, of_data->rst_buffer + of_data->rst_buffer_used,
           bytes_free);
  if (bytes_read > 0) {
    of_data->rst_buffer_used += bytes_read;
  }
}


/*
  Functions to read a single trace file
 */


// Reads the buffer and decodes an event
static int rst_decode_one_event(rst_one_file_t * of_data,
                                rst_event_t * event)
{
  int bytes_remaining, bytes_processed;

  rst_fill_buffer(of_data);

  bytes_processed = of_data->rst_buffer_ptr - of_data->rst_buffer;
  bytes_remaining = of_data->rst_buffer_used - bytes_processed;
  if (bytes_remaining <= 0) {
    return RST_NOK;
  }

  of_data->rst_buffer_ptr =
      trd_event(&of_data->hora_global, event, of_data->rst_buffer_ptr);

  event->timestamp =
      rst_correct_time(event->timestamp, &of_data->sync_time);
  event->id1 = of_data->id1;
  event->id2 = of_data->id2;
  if (event->type == RST_EVENT_STOP) {
    return RST_NOK;
  }
  event->file = of_data;
  return RST_OK;
}

//Open one trace file
static int rst_open_one_file(char *f_name, rst_one_file_t * of_data,
                             char *syncfilename, int buffer_size)
{
  of_data->fd = open(f_name, O_RDONLY);
  if (of_data->fd == -1) {
    fprintf(stderr, "[rastro] cannot open file %s: %s\n",
            f_name, strerror(errno));
    return RST_NOK;
  }
  of_data->sync_time.a = 1.0;
  of_data->sync_time.ref0 = 0;
  of_data->sync_time.loc0 = 0;

  if (buffer_size < 2 * RST_MAX_EVENT_SIZE) {
    buffer_size = 2 * RST_MAX_EVENT_SIZE;
  }
  of_data->rst_buffer_size = buffer_size;
  of_data->rst_buffer = (char *) malloc(of_data->rst_buffer_size);
  if (of_data->rst_buffer == NULL) {
    fprintf(stderr, "[rastro] cannot allocate buffer memory");
    close(of_data->fd);
    of_data->fd = -1;
    return RST_NOK;
  }
  of_data->rst_buffer_ptr = of_data->rst_buffer;
  of_data->rst_buffer_used = 0;

  if (!rst_decode_one_event(of_data, &of_data->event)
      || of_data->event.type != RST_EVENT_INIT
      || of_data->event.ct.n_uint64 != 2
      || of_data->event.ct.n_string != 1) {
    fprintf(stderr, "[rastro] invalid rastro file %s\n", f_name);
    close(of_data->fd);
    of_data->fd = -1;
    free(of_data->rst_buffer);
    of_data->rst_buffer = NULL;
    return RST_NOK;
  }
  of_data->id1 = of_data->event.v_uint64[0];
  of_data->id2 = of_data->event.v_uint64[1];
  of_data->hostname = strdup(of_data->event.v_string[0]);
  of_data->event.id1 = of_data->id1;
  of_data->event.id2 = of_data->id2;
  find_timesync_data(syncfilename, of_data);

  /* Synchronize the first event */
  of_data->event.timestamp =
      rst_correct_time(of_data->event.timestamp, &of_data->sync_time);
  return RST_OK;
}

//End a trace file
static void rst_close_one_file(rst_one_file_t * of_data)
{
  close(of_data->fd);
  of_data->fd = -1;
  free(of_data->rst_buffer);
  of_data->rst_buffer = NULL;
  free(of_data->hostname);
  of_data->hostname = NULL;
}


/* 
   Functions related to the reading of multiple trace files
 */

static void smallest_first(rst_file_t * f_data, int dead, int son)
{
  rst_one_file_t *aux;
  if (f_data->of_data[dead - 1]->event.timestamp >
      f_data->of_data[son - 1]->event.timestamp) {
    aux = f_data->of_data[dead - 1];
    f_data->of_data[dead - 1] = f_data->of_data[son - 1];
    f_data->of_data[son - 1] = aux;
  }
}



static void reorganize_bottom_up(rst_file_t * f_data, int son)
{
  int dead;
  dead = son / 2;
  if (dead == 0)
    return;

  smallest_first(f_data, dead, son);
  reorganize_bottom_up(f_data, dead);
}

static void reorganize_top_down(rst_file_t * f_data, int dead)
{
  int son1, son2;
  son1 = dead * 2;
  son2 = (dead * 2) + 1;

  //empty
  if (dead > f_data->quantity)
    return;
  //son1 does not belong
  if (son1 > f_data->quantity)
    return;
  //son2 does not belong
  if (son2 > f_data->quantity)
    smallest_first(f_data, dead, son1);

  //both belong
  else {
    if (f_data->of_data[son1 - 1]->event.timestamp <
        f_data->of_data[son2 - 1]->event.timestamp) {
      smallest_first(f_data, dead, son1);
      reorganize_top_down(f_data, son1);
    } else {
      smallest_first(f_data, dead, son2);
      reorganize_top_down(f_data, son2);
    }
  }
}



/*
  Public functions
 */

int rst_open_file(char *f_name, rst_file_t * f_data, char
                  *syncfilename, int buffer_size)
{
  if (f_data->initialized != FDATAINITIALIZED) {
    f_data->of_data = (rst_one_file_t **) malloc(sizeof(*f_data->of_data));
    f_data->quantity = 0;
    f_data->initialized = FDATAINITIALIZED;
  } else {
    f_data->of_data =
        (rst_one_file_t **) realloc(f_data->of_data,
                                    sizeof(*f_data->of_data) *
                                    (f_data->quantity + 1));
  }

  if (f_data->of_data == NULL) {
    fprintf(stderr, "[rastro] cannot allocate memory");
    return RST_NOK;
  }

  f_data->of_data[f_data->quantity] =
      (rst_one_file_t *) malloc(sizeof(rst_one_file_t));
  bzero(f_data->of_data[f_data->quantity], sizeof(rst_one_file_t));
  if (f_data->of_data[f_data->quantity] == NULL) {
    fprintf(stderr, "[rastro] cannot allocate memory");
    return RST_NOK;
  }

  if (rst_open_one_file
      (f_name, f_data->of_data[f_data->quantity], syncfilename,
       buffer_size)) {
    if (!rst_decode_one_event
        (f_data->of_data[f_data->quantity],
         &f_data->of_data[f_data->quantity]->event)) {

      rst_close_one_file(f_data->of_data[f_data->quantity]);
      free(f_data->of_data[f_data->quantity]);
    } else {
      f_data->quantity++;
      reorganize_bottom_up(f_data, f_data->quantity);
    }
    return RST_OK;
  } else
    return RST_NOK;
}

void rst_close_file(rst_file_t * f_data)
{
  free(f_data->of_data);
  f_data->quantity = 0;
}

int rst_decode_event(rst_file_t * f_data, rst_event_t * event)
{
  rst_one_file_t *aux;

  //empty
  if (f_data->quantity < 1)
    return RST_NOK;

  else {
    *event = f_data->of_data[0]->event;

    f_data->quantity--;

    //switch the last and the first
    aux = f_data->of_data[0];
    f_data->of_data[0] = f_data->of_data[f_data->quantity];
    f_data->of_data[f_data->quantity] = aux;

    //reorganize
    reorganize_top_down(f_data, 1);

    if (!rst_decode_one_event
        (f_data->of_data[f_data->quantity],
         &f_data->of_data[f_data->quantity]->event)) {

      rst_close_one_file(f_data->of_data[f_data->quantity]);
      free(f_data->of_data[f_data->quantity]);
    } else {
      f_data->quantity++;
      //reorganize
      reorganize_bottom_up(f_data, f_data->quantity);
    }
    return RST_OK;
  }
}


void rst_print_event(rst_event_t * event)
{
  int i;
  printf("type: %d ts: %lld (%.9f secs) (id1=%lu,id2=%lu)\n",
          event->type, event->timestamp,
          (double)event->timestamp/(double)RST_CLOCK_RESOLUTION,
          event->id1, event->id2);
  if (event->ct.n_uint64 > 0) {
    printf("\tu_int64_ts-> ");
    for (i = 0; i < event->ct.n_uint64; i++) {
      printf("(%" PRIu64 ") ", event->v_uint64[i]);
    }
    printf("\n");
  }
  if (event->ct.n_string > 0) {
    printf("\tstrings-> ");
    for (i = 0; i < event->ct.n_string; i++) {
      printf("(%s) ", event->v_string[i]);
    }
    printf("\n");
  }
  if (event->ct.n_float > 0) {
    printf("\tfloats-> ");
    for (i = 0; i < event->ct.n_float; i++) {
      printf("(%f) ", event->v_float[i]);
    }
    printf("\n");
  }
  if (event->ct.n_uint32 > 0) {
    printf("\tu_int32_ts-> ");
    for (i = 0; i < event->ct.n_uint32; i++) {
      printf("(%d) ", event->v_uint32[i]);
    }
    printf("\n");
  }
  if (event->ct.n_uint16 > 0) {
    printf("\tu_int16_ts-> ");
    for (i = 0; i < event->ct.n_uint16; i++) {
      printf("(%d) ", event->v_uint16[i]);
    }
    printf("\n");
  }
  if (event->ct.n_uint8 > 0) {
    printf("\tu_int8_ts-> ");
    for (i = 0; i < event->ct.n_uint8; i++) {
      printf("(%c) ", event->v_uint8[i]);
    }
    printf("\n");
  }
  if (event->ct.n_double > 0) {
    printf("\tdoubles-> ");
    for (i = 0; i < event->ct.n_double; i++) {
      printf("(%f) ", event->v_double[i]);
    }
    printf("\n");
  }
}
