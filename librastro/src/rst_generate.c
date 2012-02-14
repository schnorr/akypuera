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
#include "rst_private.h"

#define RST_UINT8_ID  0
#define RST_UINT16_ID 1
#define RST_UINT32_ID 2
#define RST_UINT64_ID 3
#define RST_FLOAT_ID  4
#define RST_DOUBLE_ID 5
#define RST_STRING_ID 6

static const char *c_types[] = {
  "u_int8_t",
  "u_int16_t",
  "u_int32_t",
  "u_int64_t",
  "float",
  "double",
  "const char*"
};

static const char *fortran_types[] = {
  "int8_t*",
  "int16_t*",
  "int32_t*",
  "int64_t*",
  "float*",
  "double*",
  "const char*"
};

static const char *fortran_casts[] = {
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
                          XSTR(LETTER_DOUBLE)
                          XSTR(LETTER_UINT64)
                          XSTR(LETTER_FLOAT)
                          XSTR(LETTER_UINT32)
                          XSTR(LETTER_UINT16)
                          XSTR(LETTER_UINT8)
                          XSTR(LETTER_STRING));
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

static int rst_add_type (char c, const char **types, char *output, int len)
{
  int res = 0;
  switch (c){
  case LETTER_UINT8_QUOTE:  res = snprintf (output, len, "%s", types[RST_UINT8_ID]); break;
  case LETTER_UINT16_QUOTE: res = snprintf (output, len, "%s", types[RST_UINT16_ID]); break;
  case LETTER_UINT32_QUOTE: res = snprintf (output, len, "%s", types[RST_UINT32_ID]); break;
  case LETTER_UINT64_QUOTE: res = snprintf (output, len, "%s", types[RST_UINT64_ID]); break;
  case LETTER_FLOAT_QUOTE:  res = snprintf (output, len, "%s", types[RST_FLOAT_ID]); break;
  case LETTER_DOUBLE_QUOTE: res = snprintf (output, len, "%s", types[RST_DOUBLE_ID]); break;
  case LETTER_STRING_QUOTE: res = snprintf (output, len, "%s", types[RST_STRING_ID]); break;
  }
  return res;
}

static int rst_get_id (char c, rst_counters_t *ct)
{
  switch (c){
  case LETTER_UINT8_QUOTE:  return ct->n_uint8;
  case LETTER_UINT16_QUOTE: return ct->n_uint16;
  case LETTER_UINT32_QUOTE: return ct->n_uint32;
  case LETTER_UINT64_QUOTE: return ct->n_uint64;
  case LETTER_FLOAT_QUOTE:  return ct->n_float;
  case LETTER_DOUBLE_QUOTE: return ct->n_double;
  case LETTER_STRING_QUOTE: return ct->n_string;
  }
  return -1;
}

static void rst_add_id (char c, rst_counters_t *ct)
{
  switch (c){
  case LETTER_UINT8_QUOTE:  ct->n_uint8++; break;
  case LETTER_UINT16_QUOTE: ct->n_uint16++; break;
  case LETTER_UINT32_QUOTE: ct->n_uint32++; break;
  case LETTER_UINT64_QUOTE: ct->n_uint64++; break;
  case LETTER_FLOAT_QUOTE:  ct->n_float++; break;
  case LETTER_DOUBLE_QUOTE: ct->n_double++; break;
  case LETTER_STRING_QUOTE: ct->n_string++; break;
  }
}

static void rst_counters (char *types, rst_counters_t *ct)
{
  char *index = NULL;
  bzero (ct, sizeof(rst_counters_t));
  for (index = types; *index != '\0'; index++){
    rst_add_id (*index, ct);
  }
}

static int rst_add_type_and_var (char c, const char **types, rst_counters_t *ct, char *output, int len)
{
  int res = 0;
  res += rst_add_type (c, types, output+res, len-res);
  res += rst_add_space (output+res, len-res);
  res += rst_add_var (c, rst_get_id(c, ct), output+res, len-res);
  return res;
}

static int rst_generate_arg_fortran_types (char *types, char *str, int len)
{
  int n = 0;
  rst_counters_t ct;
  char *index = NULL;
  bzero (&ct, sizeof(rst_counters_t));
  n += snprintf (str+n, len-n, "int16_t *type");
  for (index = types; *index != '\0'; index++){
    n += rst_add_comma (str+n, len-n);
    n += rst_add_type_and_var (*index, fortran_types, &ct, str+n, len-strlen(str));
    rst_add_id (*index, &ct);
  }
  return n;
}

static int rst_generate_arg_fortran_casts (char *types, char *str, int len)
{
  int n = 0;
  rst_counters_t ct;
  char *index = NULL;
  bzero (&ct, sizeof(rst_counters_t));
  n += snprintf (str+n, len-n, "(u_int16_t)* type");
  for (index = types; *index != '\0'; index++){
    n += rst_add_comma (str+n, len-strlen(str));
    n += rst_add_type_and_var (*index, fortran_casts, &ct, str+n, len-strlen(str));
    rst_add_id (*index, &ct);
  }
  return n;
}

static int rst_generate_arg_c (char *types, char *str, int len)
{
  int n = 0;
  rst_counters_t ct;
  char *index = NULL;
  bzero (&ct, sizeof(rst_counters_t));
  n += snprintf (str+n, len-n, "u_int16_t type");
  for (index = types; *index != '\0'; index++){
    n += rst_add_comma (str+n, len-strlen(str));
    n += rst_add_type_and_var (*index, c_types, &ct, str+n, len-strlen(str));
    rst_add_id (*index, &ct);
  }
  return n;
}

static int rst_generate_arg_prep (char *types, char *str, int len)
{
  int n = 0;
  rst_counters_t ct;
  char *index = NULL;
  bzero (&ct, sizeof(rst_counters_t));
  n += snprintf (str+n, len-n, "type");
  for (index = types; *index != '\0'; index++){
    n += rst_add_comma (str+n, len-strlen(str));
    n += rst_add_var (*index, rst_get_id(*index, &ct), str+n, len-strlen(str));
    rst_add_id (*index, &ct);
  }
  return n;
}

int rst_generate_function_header (char *types, char *header, int header_len)
{
  if (!rst_generate_validate_types (types)){
    /* types are not validated */
    return -1;
  }

  rst_counters_t ct;
  bzero (&ct, sizeof(rst_counters_t));

  int len = 1000, res = 0;
  char *af, *ap;
  af = (char*)malloc(len*sizeof(char));
  ap = (char*)malloc(len*sizeof(char));
  bzero (af, 1000);
  bzero (ap, 1000);
  rst_generate_arg_c (types, af, len);
  rst_generate_arg_prep (types, ap, len);

  /* Fortran prototypes */
  char *arg_list_fortran;
  arg_list_fortran = (char*)malloc(len*sizeof(char));
  rst_generate_arg_fortran_types (types, arg_list_fortran, len);

  res = snprintf (header,
                  header_len,
                  "/* Rastro function prototype for '%s' */\n"
                  "void rst_event_%s_ptr(rst_buffer_t *ptr, %s);\n"
                  "void rst_event_%s_f_ (%s);\n"
                  "#define rst_event_%s(%s) rst_event_%s_ptr(RST_PTR, %s)\n\n",
                  types,
                  types, af,
                  types, arg_list_fortran,
                  types, ap, types, ap);
  free (af);
  free (ap);
  return res;
}

static int rst_generate_function_start (rst_counters_t *ct, char *implem, int implem_len)
{
  rst_counters_t ct_done;
  bzero (&ct_done, sizeof(rst_counters_t));

  int bits = 16;
  int done = 0;
  int n = 0;
  n += snprintf (implem+n, implem_len-n, "  rst_startevent(ptr, type<<18|");
  for (;;) {
    int remaining_bits = bits;
    int res = 0;

    while (done < RST_TYPE_CTR && remaining_bits > 0) {
      int nibble_type = 0;
      switch (done) {
#define     CASE(n, ctr, type)                                  \
        case n:                                                 \
          if (ct_done.ctr < ct->ctr && nibble_type == 0) {      \
            nibble_type = type;                                 \
            ct_done.ctr++;                                      \
          }                                                     \
          if (ct_done.ctr < ct->ctr)                            \
            break;                                              \
          done++
        CASE(0, n_double, RST_DOUBLE_TYPE);
        CASE(1, n_uint64, RST_LONG_TYPE);
        CASE(2, n_float,  RST_FLOAT_TYPE);
        CASE(3, n_uint32, RST_INT_TYPE);
        CASE(4, n_uint16, RST_SHORT_TYPE);
        CASE(5, n_uint8,  RST_CHAR_TYPE);
        CASE(6, n_string, RST_STRING_TYPE);
#undef      CASE
      }
      if (nibble_type != 0) {
        res = res << 4 | nibble_type;
        remaining_bits -= 4;
      }
    }

    if (done < RST_TYPE_CTR) {
      n += snprintf (implem+n, implem_len-n, "0x%x);\n", res);
      //Verify
      //n += snprintf (implem+n, implem_len-n, "  RST_PUT(ptr, u_int64_t, ");
      n += snprintf (implem+n, implem_len-n, "  RST_PUT(ptr, u_int32_t, ");
      bits = 28;
    } else {
      n += snprintf (implem+n, implem_len-n, "0x%x);\n", RST_LAST<<bits | res<<remaining_bits);
      break;
    }
  }
  return n;
}

int rst_generate_function_implementation (char *types, char *implem, int implem_len)
{
  if (!rst_generate_validate_types (types)){
    /* types are not validated */
    return -1;
  }

  rst_counters_t ct;
  rst_counters (types, &ct);

  int i, n = 0;
  int len = 1000;
  char *arg_list = (char*)malloc(len*sizeof(char));
  rst_generate_arg_c (types, arg_list, len);

  /* C implementation */
  n += snprintf (implem+n,
                 implem_len-n,
                 "/* Rastro function implementation for '%s' */\n"
                 "void rst_event_%s_ptr(rst_buffer_t *ptr, %s)\n"
                 "{\n",
                 types,
                 types, arg_list);
  n += rst_generate_function_start (&ct, implem+n, implem_len-n);
  /* order must be the same as in generate_function_start */
  for (i = 0; i < ct.n_double; i++)
    n += snprintf (implem+n, implem_len-n, "  RST_PUT(ptr, double, " XSTR(LETTER_DOUBLE) "%d);\n", i);
  for (i = 0; i < ct.n_uint64; i++)
    n += snprintf (implem+n, implem_len-n, "  RST_PUT(ptr, u_int64_t, " XSTR(LETTER_UINT64) "%d);\n", i);
  for (i = 0; i < ct.n_float; i++)
    n += snprintf (implem+n, implem_len-n, "  RST_PUT(ptr, float, " XSTR(LETTER_FLOAT) "%d);\n", i);
  for (i = 0; i < ct.n_uint32; i++)
    n += snprintf (implem+n, implem_len-n, "  RST_PUT(ptr, u_int32_t, " XSTR(LETTER_UINT32) "%d);\n", i);
  for (i = 0; i < ct.n_uint16; i++)
    n += snprintf (implem+n, implem_len-n, "  RST_PUT(ptr, u_int16_t, " XSTR(LETTER_UINT16) "%d);\n", i);
  for (i = 0; i < ct.n_uint8; i++)
    n += snprintf (implem+n, implem_len-n, "  RST_PUT(ptr, u_int8_t, " XSTR(LETTER_UINT8) "%d);\n", i);
  for (i = 0; i < ct.n_string; i++)
    n += snprintf (implem+n, implem_len-n, "  RST_PUT_STR(ptr, " XSTR(LETTER_STRING) "%d);\n", i);
  n += snprintf (implem+n, implem_len-n,
                 "  rst_endevent(ptr);\n"
                 "}\n");

  /* Fortran implementation */
  char *arg_list_fortran, *casts_fortran;
  arg_list_fortran = (char*)malloc(len*sizeof(char));
  casts_fortran = (char*)malloc(len*sizeof(char));
  rst_generate_arg_fortran_types (types, arg_list_fortran, len);
  rst_generate_arg_fortran_casts (types, casts_fortran, len);

  n += snprintf (implem+n, implem_len-n,
                 "/* Rastro function implementation for '%s' - fortran support */\n"
                 "void rst_event_%s_f_( %s)\n"
                 "{\n",
                 types,
                 types, arg_list_fortran);
  n += snprintf (implem+n, implem_len-n,
                 "  rst_event_%s (%s);\n",
                 types, casts_fortran);
  n += snprintf (implem+n, implem_len-n,
                 "}\n\n");
  free (arg_list);
  free (arg_list_fortran);
  free (casts_fortran);
  return n;
}

int rst_generate_header (char *types[], int types_len, char *header, int header_len)
{
  int i;
  int n = 0;

  n += snprintf (header+n,
                 header_len-n,
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
                 "void rst_init_f_(int64_t *id1, int64_t *id2);\n"
                 "void rst_finalize_f_ (void);\n\n");

  n += snprintf (header+n,
                 header_len-strlen(header),
                 "#endif //__AUTO_RASTRO_FILE_H__\n");
  return n;
}

int rst_generate_functions (char *types[], int types_len, char *implem, int implem_len, char *header_filename)
{
  int i;
  int n = 0;
  n += snprintf (implem+n,
                 implem_len-n,
                 "/* Do not edit. File generated by rastro. */\n\n"
                 "#include \"%s\"\n\n", header_filename);
  for (i = 0; i < types_len; i++){
    int nt = rst_generate_function_implementation (types[i], implem+n, implem_len-n);
    if (nt == -1){
      /* types[i] has some unrecognized letters, ignore */
      continue;
    }
    n += nt;
  }

  //TODO: this should be incorportated in librastro
  n += snprintf (implem+n,
                 implem_len-n,
                 "void rst_init_f_(int64_t *id1, int64_t *id2)\n"
                 "{\n"
                 "  rst_init((u_int64_t)* id1, (u_int64_t)* id2);\n"
                 "}\n"
                 "void rst_finalize_f_ ()\n"
                 "{\n"
                 "  rst_finalize();\n"
                 "}\n");
  return n;
}

int rst_generate (char *types[], int types_len, FILE *header, FILE *implem, char *header_name)
{
  int len = 50000; /* 50 Kbytes, should work for most librastro use cases */
  char *str = (char*) malloc (len*sizeof(char));
  int n = rst_generate_header (types, types_len, str, len);
  size_t written = fwrite (str, sizeof(char), n, header);
  if (written != n){
    return 1;
  }

  n = rst_generate_functions (types, types_len, str, len, header_name);
  written = fwrite (str, sizeof(char), n, implem);
  if (written != n){
    return 1;
  }

  free (str);
  return 0;
}
