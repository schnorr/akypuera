/*
    Copyright (c) 1998--2006 Benhur Stein
    
    This file is part of librastro.

    librastro is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    librastro is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
    for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with librastro; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02111 USA.
*/
#include "rastro_private.h"

#define RST_UINT8_ID  0
#define RST_UINT16_ID 1
#define RST_UINT32_ID 2
#define RST_UINT64_ID 3
#define RST_FLOAT_ID  4
#define RST_DOUBLE_ID 5
#define RST_STRING_ID 6

static char *c_types[] = {
  "u_int8_t",
  "u_int16_t",
  "u_int32_t",
  "u_int64_t",
  "float",
  "double",
  "char*"
};

static char *fortran_types[] = {
  "int8*",
  "int16*",
  "int32*",
  "int64*",
  "float*",
  "double*",
  "char*"
};

static char *fortran_casts[] = {
  "(u_int8_t)*",
  "(u_int16_t)*",
  "(u_int32_t)*",
  "(u_int64_t)*",
  "(float)*",
  "(double)*",
  ""
};

static int rst_generate_validate_types (char *types)
{
  size_t accept = strspn (types,
                          XSTR(LETRA_DOUBLE)
                          XSTR(LETRA_UINT64)
                          XSTR(LETRA_FLOAT)
                          XSTR(LETRA_UINT32)
                          XSTR(LETRA_UINT16)
                          XSTR(LETRA_UINT8)
                          XSTR(LETRA_STRING));
  size_t len = strlen (types);
  return accept == len;
}

static int rst_add_space (char *output, int len)
{
  return snprintf (output, len, " ");
}

static int rst_add_comma (char *output, int len)
{
  return snprintf (output, len, ", ");
}

static int rst_add_var (char c, int id, char *output, int len)
{
  return snprintf (output, len, "%c%d", c, id);
}

static int rst_add_type (char c, char **types, char *output, int len)
{
  int res = 0;
  switch (c){
  case LETRA_UINT8_ASPA:  res = snprintf (output, len, "%s", types[RST_UINT8_ID]); break;
  case LETRA_UINT16_ASPA: res = snprintf (output, len, "%s", types[RST_UINT16_ID]); break;
  case LETRA_UINT32_ASPA: res = snprintf (output, len, "%s", types[RST_UINT32_ID]); break;
  case LETRA_UINT64_ASPA: res = snprintf (output, len, "%s", types[RST_UINT64_ID]); break;
  case LETRA_FLOAT_ASPA:  res = snprintf (output, len, "%s", types[RST_FLOAT_ID]); break;
  case LETRA_DOUBLE_ASPA: res = snprintf (output, len, "%s", types[RST_DOUBLE_ID]); break;
  case LETRA_STRING_ASPA: res = snprintf (output, len, "%s", types[RST_STRING_ID]); break;
  }
  return res;
}

static int rst_get_id (char c, counters_t *ct)
{
  switch (c){
  case LETRA_UINT8_ASPA:  return ct->n_uint8;
  case LETRA_UINT16_ASPA: return ct->n_uint16;
  case LETRA_UINT32_ASPA: return ct->n_uint32;
  case LETRA_UINT64_ASPA: return ct->n_uint64;
  case LETRA_FLOAT_ASPA:  return ct->n_float;
  case LETRA_DOUBLE_ASPA: return ct->n_double;
  case LETRA_STRING_ASPA: return ct->n_string;
  }
  return -1;
}

static void rst_add_id (char c, counters_t *ct)
{
  switch (c){
  case LETRA_UINT8_ASPA:  ct->n_uint8++; break;
  case LETRA_UINT16_ASPA: ct->n_uint16++; break;
  case LETRA_UINT32_ASPA: ct->n_uint32++; break;
  case LETRA_UINT64_ASPA: ct->n_uint64++; break;
  case LETRA_FLOAT_ASPA:  ct->n_float++; break;
  case LETRA_DOUBLE_ASPA: ct->n_double++; break;
  case LETRA_STRING_ASPA: ct->n_string++; break;
  }
}

static int rst_add_type_and_var (char c, char **types, counters_t *ct, char *output, int len)
{
  int res = 0;
  res += rst_add_type (c, types, output+res, len-res);
  res += rst_add_space (output+res, len-res);
  res += rst_add_var (c, rst_get_id(c, ct), output+res, len-res);
  return res;
}

int rst_generate_function_header (char *types, char *header, int header_len)
{
  if (!rst_generate_validate_types (types)){
    /* types are not validated */
    return -1;
  }

  counters_t ct;
  bzero (&ct, sizeof(counters_t));

  int len = 1000;
  char *af, *ap;
  af = (char*)malloc(len*sizeof(char));
  ap = (char*)malloc(len*sizeof(char));
  bzero (af, 1000);
  bzero (ap, 1000);
  strncat (af, "u_int16_t type", len);
  strncat (ap, "type", len);
  int afn = strlen(af);
  int apn = strlen(ap);

  char *index = NULL;
  for (index = types; *index != '\0'; index++){
    afn += rst_add_comma (af+afn, len-strlen(af));
    afn += rst_add_type_and_var (*index, c_types, &ct, af+afn, len-strlen(af));

    apn += rst_add_comma (ap+apn, len-strlen(ap));
    apn += rst_add_var (*index, rst_get_id(*index, &ct), ap+apn, len-strlen(ap));
    rst_add_id (*index, &ct);
  }

  int res;
  res = snprintf (header,
                  header_len,
                  "/* Rastro function prototype for '%s' */\n"
                  "void rst_event_%s_ptr(rst_buffer_t *ptr, %s);\n"
                  "#define rst_event_%s(%s) rst_event_%s_ptr(RST_PTR, %s)\n\n",
                  types,
                  types, af,
                  types, ap, types, af);
  free (af);
  free (ap);
  return res;
}

int rst_generate_header (char *types[], int types_len, char *header, int header_len)
{
  int i;
  int n = 0;

  n += snprintf (header+n,
                 header_len-strlen(header),
                 "/* Do not edit. File generated by rastro. */\n\n"
                 "#ifndef __AUTO_RASTRO_FILE_H__\n"
                 "#define __AUTO_RASTRO_FILE_H__\n"
                 "#include <rastro.h>\n\n");
  for (i = 0; i < types_len; i++){
    int nt = rst_generate_function_header (types[i], header+n, header_len-strlen(header));
    if (nt == -1){
      /* types[i] has some unrecognized letters, ignore */
      continue;
    }
    n += nt;
  }
  n += snprintf (header+n,
                 header_len-strlen(header),
                 "#endif //__AUTO_RASTRO_FILE_H__\n");
  return n;
}
