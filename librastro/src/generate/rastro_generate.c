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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>

int validate_argtypes(char *linha)
{
    return strspn(linha, XSTR(LETRA_DOUBLE) XSTR(LETRA_UINT64) XSTR(LETRA_FLOAT) XSTR(LETRA_UINT32) XSTR(LETRA_UINT16) XSTR(LETRA_UINT8) XSTR(LETRA_STRING)) == strlen(linha);
}

void break_types(char *argtypes, char *arglist, char *varlist, char * arglist_f, char * params_f, counters_t *ct)
{
    char *s = argtypes;
    char c;
    int na, nv, nf, np;
    
    ct->n_uint16 = ct->n_uint8 = ct->n_uint64 = ct->n_uint32 = 0;
    ct->n_float = ct->n_double = ct->n_string = 0;

    na = sprintf(arglist,   "u_int16_t type");
    nf = sprintf(arglist_f, "int16_t *type");
    np = sprintf(params_f,  "(u_int16_t)*type");
    nv = sprintf(varlist, "type");
    while ((c = *s++) != '\0') {
	switch (c) {
	case LETRA_UINT32_ASPA:
	    na += sprintf(arglist+na,   ", u_int32_t "   XSTR(LETRA_UINT32) "%d",  ct->n_uint32);
	    nf += sprintf(arglist_f+nf, ", int32_t *"    XSTR(LETRA_UINT32) "%d",  ct->n_uint32);
            np += sprintf(params_f +np, ", (u_int32_t)*" XSTR(LETRA_UINT32) "%d",  ct->n_uint32);
	    nv += sprintf(varlist+nv,   ", "             XSTR(LETRA_UINT32) "%d",  ct->n_uint32);
            ct->n_uint32++;
	    break;
	case LETRA_UINT16_ASPA:
	    na += sprintf(arglist+na,    ", u_int16_t "  XSTR(LETRA_UINT16) "%d", ct->n_uint16);
	    nf += sprintf(arglist_f+nf,  ", int16_t *" XSTR(LETRA_UINT16) "%d", ct->n_uint16);
            np += sprintf(params_f +np,  ", (u_int16_t)*" XSTR(LETRA_UINT16) "%d", ct->n_uint16);
	    nv += sprintf(varlist+nv,    ", "            XSTR(LETRA_UINT16) "%d", ct->n_uint16);
	    ct->n_uint16++;
	    break;
	case LETRA_UINT8_ASPA:
	    na += sprintf(arglist+na,    ", u_int8_t "  XSTR(LETRA_UINT8) "%d",   ct->n_uint8);
	    nf += sprintf(arglist_f+nf,  ", int8_t *" XSTR(LETRA_UINT8) "%d",   ct->n_uint8);
            np += sprintf(params_f +np,  ", (u_int8_t)*" XSTR(LETRA_UINT8) "%d",   ct->n_uint8);
	    nv += sprintf(varlist+nv,    ", "           XSTR(LETRA_UINT8) "%d",   ct->n_uint8);
            ct->n_uint8++;
	    break;
	case LETRA_FLOAT_ASPA:
	    na += sprintf(arglist+na,    ", float "  XSTR(LETRA_FLOAT) "%d",      ct->n_float);
	    nf += sprintf(arglist_f+nf,  ", float *" XSTR(LETRA_FLOAT) "%d",      ct->n_float);
            np += sprintf(params_f +np,  ", (float)*" XSTR(LETRA_FLOAT) "%d",      ct->n_float);
	    nv += sprintf(varlist+nv,    ", "        XSTR(LETRA_FLOAT) "%d",      ct->n_float);
            ct->n_float++;
	    break;
	case LETRA_DOUBLE_ASPA:
	    na += sprintf(arglist+na,    ", double "  XSTR(LETRA_DOUBLE) "%d",    ct->n_double);
	    nf += sprintf(arglist_f+nf,  ", double *" XSTR(LETRA_DOUBLE) "%d",    ct->n_double);
              np += sprintf(params_f +np,  ", (double)* " XSTR(LETRA_DOUBLE) "%d",    ct->n_double);
	    nv += sprintf(varlist+nv,    ", "         XSTR(LETRA_DOUBLE) "%d",    ct->n_double);
            ct->n_double++;
	    break;
	case LETRA_UINT64_ASPA:
	    na += sprintf(arglist+na,    ", u_int64_t "  XSTR(LETRA_UINT64) "%d", ct->n_uint64);
	    nf += sprintf(arglist_f+nf,  ", int64_t *" XSTR(LETRA_UINT64) "%d", ct->n_uint64);
            np += sprintf(params_f +np,  ", (u_int64_t)*" XSTR(LETRA_UINT64) "%d", ct->n_uint64);
	    nv += sprintf(varlist+nv,    ", "            XSTR(LETRA_UINT64) "%d", ct->n_uint64);
            ct->n_uint64++;
	    break;
	case LETRA_STRING_ASPA:
	    na += sprintf(arglist+na,    ", char *" XSTR(LETRA_STRING) "%d",      ct->n_string);
	    nf += sprintf(arglist_f+nf,  ", char *" XSTR(LETRA_STRING) "%d",      ct->n_string);
            np += sprintf(params_f +np,  ", "       XSTR(LETRA_STRING) "%d",      ct->n_string);
	    nv += sprintf(varlist+nv,    ", "       XSTR(LETRA_STRING) "%d",      ct->n_string);
            ct->n_string++;
	    break;
	}
    }
}

void generate_header(char *argtypes, char *arglist, char *varlist, FILE *hfile)
{
    fprintf(hfile, "void rst_event_%s_ptr(rst_buffer_t *ptr, %s);\n",
                        argtypes, arglist);
    fprintf(hfile, "#define rst_event_%s(%s) rst_event_%s_ptr(RST_PTR, %s)\n",
                        argtypes, varlist, argtypes, varlist);
}

void generate_function_start(counters_t *ct, FILE *cfile)
{
    int bits;
    int done;
    int res;
    counters_t ct_done = {0};
    
    bits = 16;
    done = 0;
    fprintf(cfile, "\trst_startevent(ptr, type<<18|");
    for (;;) {
        int remaining_bits = bits;
        res = 0;
        
        while (done < RST_TYPE_CTR && remaining_bits > 0) {
            int nibble_type = 0;
            switch (done) {
#define     CASE(n, ctr, type)                                      \
            case n:                                                 \
                if (ct_done.ctr < ct->ctr && nibble_type == 0) {    \
                    nibble_type = type;                             \
                    ct_done.ctr++;                                  \
                }                                                   \
                if (ct_done.ctr < ct->ctr)                          \
                    break;                                          \
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
            fprintf(cfile, "0x%x);\n", res);
	    //Verificar
	   // fprintf(cfile, "\tRST_PUT(ptr, u_int64_t, ");
	    fprintf(cfile, "\tRST_PUT(ptr, u_int32_t, ");
            bits = 28;
        } else {
            fprintf(cfile, "0x%x);\n", RST_LAST<<bits | res<<remaining_bits);
            break;
        }
    }
}

void generate_body(char *argtypes, char *arglist, counters_t *ct, FILE *cfile)
{
    int i;
    
    fprintf(cfile, "void rst_event_%s_ptr(rst_buffer_t *ptr, %s)\n",
                        argtypes, arglist);
    fprintf(cfile, "{\n");
    
    generate_function_start(ct, cfile);

    /* order must be the same as in generate_function_start */
    for (i = 0; i < ct->n_double; i++)
        fprintf(cfile, "\tRST_PUT(ptr, double, " XSTR(LETRA_DOUBLE) "%d);\n", i);
    for (i = 0; i < ct->n_uint64; i++)
        fprintf(cfile, "\tRST_PUT(ptr, u_int64_t, " XSTR(LETRA_UINT64) "%d);\n", i);
    for (i = 0; i < ct->n_float; i++)
        fprintf(cfile, "\tRST_PUT(ptr, float, " XSTR(LETRA_FLOAT) "%d);\n", i);
    for (i = 0; i < ct->n_uint32; i++)
        fprintf(cfile, "\tRST_PUT(ptr, u_int32_t, " XSTR(LETRA_UINT32) "%d);\n", i);
    for (i = 0; i < ct->n_uint16; i++)
        fprintf(cfile, "\tRST_PUT(ptr, u_int16_t, " XSTR(LETRA_UINT16) "%d);\n", i);
    for (i = 0; i < ct->n_uint8; i++)
        fprintf(cfile, "\tRST_PUT(ptr, u_int8_t, " XSTR(LETRA_UINT8) "%d);\n", i);
    for (i = 0; i < ct->n_string; i++)
        fprintf(cfile, "\tRST_PUT_STR(ptr, " XSTR(LETRA_STRING) "%d);\n", i);
    
    fprintf(cfile, "\trst_endevent(ptr);\n");

    fprintf(cfile, "}\n\n");
}

void generate_body_f(char *argtypes, char *arglist, char *params_f, counters_t *ct, FILE *cfile)
{
    int i;
    
    fprintf(cfile, "void rst_event_%s_f_( %s)\n",
                        argtypes, arglist);
    fprintf(cfile, "{\n");

    fprintf(cfile, "\t" "rst_event_%s(  %s);"  "\n", argtypes, params_f);
    fprintf(cfile, "}\n");

}



void generate_function(char *argtypes, FILE *hfile, FILE *cfile, FILE *ffile)
{
    char arglist[1000], varlist[1000],arglist_f[1000], params_f[1000];
    counters_t ct;
    
    break_types(argtypes, arglist, varlist, arglist_f, params_f, &ct);
    if (hfile)
      generate_header(argtypes, arglist, varlist, hfile);
    if(cfile)
      generate_body(argtypes, arglist, &ct, cfile);
    if(ffile)
    {
      generate_body_f(argtypes,arglist_f, params_f, &ct,ffile);
    }
}

void generate_hfile_start(FILE *hfile, char *hfilename)
{
    int n;
    char *dot;
    dot = strchr(hfilename, '.');
    if (dot != NULL) {
        n = dot - hfilename;
    } else {
        n = strlen(hfilename);
    }
    fprintf(hfile,
	    "/* Do not edit. File generated by rastro_generate. */\n\n");
    fprintf(hfile, "#ifndef _RASTRO_%.*s_H_\n", n, hfilename);
    fprintf(hfile, "#define _RASTRO_%.*s_H_\n\n", n, hfilename);
    fprintf(hfile, "#include \"rastro.h\"\n");
    fprintf(hfile, "#include \"rastro_private.h\"\n\n");
}

void generate_hfile_end(FILE *hfile)
{
    fprintf(hfile, "\n#endif\n");
}

void generate_cfile_start(FILE *cfile, char *hfilename)
{
    fprintf(cfile,
	    "/* Do not edit. File generated by rastro_generate. */\n\n");
    fprintf(cfile, "#include \"%s\"\n\n", hfilename);
}

void generate_ffile_start(FILE *ffile, char *hfilename)
{
    fprintf(ffile,
	    "/* Do not edit. File generated by rastro_generate. */\n\n");
    fprintf(ffile, "#include \"%s\"\n\n", hfilename);
}

FILE *openfile(char *name, char *mode)
{
    FILE *f;
    f = fopen(name, mode);
    if (f == NULL) {
        fprintf(stderr, "Error opening file %s: %s\n", name, strerror(errno));
        exit(1);
    }
    return f;
}

void usage (char *program)
{
  printf("Usage: %s -h file.h -c file.c -f file_f.c inputfile \n"
         "      where file.c and file.h are header and functions \n"
         "      for C programs file_f.c contains functions for \n"
         "      FORTRAN applications, needs file.c and file.c\n", program);
  exit(0);
}

/** Inicio do programa
* @version 0.2
* @author Gabriela e Lucas
* @param argv[1] arquivo de entrada
* @param argv[2] arquivo de saida_pthreads
* @param argv[3] arquivo de saida_ptr
*/
int main(int argc, char **argv)
{
    char *hfilename = NULL;
    char *cfilename = NULL;
    char *ffilename = NULL;
    FILE *entrada;
    FILE *hfile = NULL;
    FILE *cfile = NULL;
    FILE *ffile = NULL;
    char argtypes[1000], opt;

    while ((opt = getopt(argc,argv,"c:h:f:")) != -1 ) {
      switch (opt) {
        case 'f':
          ffilename = strdup(optarg);
          break;
        case 'c':
          cfilename = strdup(optarg);
          break;
        case 'h':
          hfilename = strdup(optarg);
          break;
        default:
          usage (argv[0]);
          break;
      }
    }

    if (optind >= argc) {
      usage(argv[0]);
    }




    /* Ugly.
     if ( argc != 4 ) {
        printf("rastro_generate functions name.c name.h\n");
        exit(0);
    } 
    cfilename = argv[2];
    hfilename = argv[3];
    */
    
    entrada = openfile(argv[optind], "r");

    // Only generates file names as required
    if (cfilename)
      cfile = openfile(cfilename, "w");
    if (hfilename)
      hfile = openfile(hfilename, "w");
    if (ffilename)
      ffile = openfile(ffilename, "w");

    // Make headers
    if (hfilename)
      generate_hfile_start(hfile, hfilename);
    if (cfilename)
      generate_cfile_start(cfile, hfilename);
    if (ffilename)
      generate_ffile_start(ffile, hfilename);
    
    while (fscanf(entrada, "%s", argtypes) == 1) {
	if (!validate_argtypes(argtypes)) {
	    fprintf(stderr, "Invalid event types \"%s\" ignored.\n", argtypes);
            continue;
        }
        generate_function(argtypes, hfile, cfile, ffile);
    }
    
    generate_hfile_end(hfile);

    if (ffile) {
      fprintf(ffile, "void rst_init_f_(int64_t * id1, int64_t * id2) { \n");
      fprintf(ffile, " rst_init((u_int64_t)*id1, (u_int64_t)*id2);\n}\n\n");
      fprintf(ffile, "void rst_finalize_f_ () {  rst_finalize(); }\n");
    }
    
    fclose(entrada);
    if (cfile)
      fclose(cfile);
    if (hfile)
      fclose(hfile);
    if (ffile)
      fclose(ffile);
    return 0;
}
