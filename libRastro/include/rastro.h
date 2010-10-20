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



#ifndef _RASTRO_PUBLIC_H_
#define _RASTRO_PUBLIC_H_

#define RST_CLOCK_RESOLUTION 1000000

#define RST_EVENT_TYPE_MASK 0x3fff  /* 14 bits */
#define RST_EVENT_INIT (-1 & RST_EVENT_TYPE_MASK)
#define RST_EVENT_STOP (-2 & RST_EVENT_TYPE_MASK)

#include <sys/types.h>
#include <pthread.h>

/*Se for mudar alguma letra deve-se mudar tambem no script*/
/* ./bin/.rastro_names.sh tirando a letra mudada e colocando a nova*/

#define LETRA_UINT8 c
#define LETRA_UINT16 w
#define LETRA_UINT32 i
#define LETRA_UINT64 l
#define LETRA_FLOAT f
#define LETRA_DOUBLE d
#define LETRA_STRING s


#define LETRA_UINT8_ASPA 'c'
#define LETRA_UINT16_ASPA 'w'
#define LETRA_UINT32_ASPA 'i'
#define LETRA_UINT64_ASPA 'l'
#define LETRA_FLOAT_ASPA 'f'
#define LETRA_DOUBLE_ASPA 'd'
#define LETRA_STRING_ASPA 's'


/*CAT concatena a funcao rst_event_+LETRA_UINT64+LETRA_UINT64+LETRA_STRING+_ptr */
#define CAT(x,y,z,w) x##y##y##z##w
#define XCAT(x,y,z,w) CAT(x,y,z,w)
#define STR(x) #x
#define XSTR(x) STR(x)


/* defines utilizado para a inicializacao de uma estrutura do tipo rst_file_t */
#define FDATAINITIALIZED -239847237

typedef u_int16_t type_t;
typedef long long timestamp_t;

typedef struct {
    int n_uint8;
    int n_uint16;
    int n_uint32;
    int n_uint64;
    int n_float;
    int n_double;
    int n_string;
} counters_t;

#define RST_MAX_FIELDS_PER_TYPE 15
#define RST_MAX_STRLEN 100

typedef struct {
	counters_t ct;
	char v_string[RST_MAX_FIELDS_PER_TYPE][RST_MAX_STRLEN];
	u_int8_t v_uint8[RST_MAX_FIELDS_PER_TYPE];
	u_int16_t v_uint16[RST_MAX_FIELDS_PER_TYPE];
	u_int32_t v_uint32[RST_MAX_FIELDS_PER_TYPE];
	u_int64_t v_uint64[RST_MAX_FIELDS_PER_TYPE];
	float v_float[RST_MAX_FIELDS_PER_TYPE];
	double v_double[RST_MAX_FIELDS_PER_TYPE];
	type_t type;
        u_int64_t id1;
        u_int64_t id2;
	timestamp_t timestamp;
} rst_event_t;

typedef struct {
	double a;
	timestamp_t loc0;
	timestamp_t ref0;
} ct_t;

typedef struct {
	int fd;
	ct_t sync_time;
        char *rst_buffer_ptr;
        char *rst_buffer;
        int rst_buffer_size;
        int rst_buffer_used;
        char *hostname;
        u_int64_t id1;
        u_int64_t id2;
	timestamp_t hora_global;
	rst_event_t event;
} rst_one_file_t;

typedef struct {
        rst_one_file_t **of_data;
        int quantity;
	int initialized;
} rst_file_t;
		


typedef struct {
	long  rst_t0;
	int   rst_fd;
	char *rst_buffer_ptr;
        char *rst_buffer;
        int rst_buffer_size;
} rst_buffer_t;

extern int rst_debug_mask;

#define RST_OK  (1==1)
#define RST_NOK (0==1)

/*****************FUNCOES DE GERACAO DE RASTRO********************/
/*Funcoes de inicializacao*/
void rst_initialize(u_int64_t id1, u_int64_t id2, int *argc, char ***argv);
void rst_init(u_int64_t id1, u_int64_t id2);
void rst_init_ptr(rst_buffer_t *ptr, u_int64_t id1, u_int64_t id2);
void rst_finalize(void);
void rst_finalize_ptr(rst_buffer_t *ptr);
void rst_flush_all(void);

void rst_event(u_int16_t type);
void rst_event_ptr(rst_buffer_t *ptr, u_int16_t type);

/****************FUNCOES DE LEITURA DE RASTRO*******************/

/*Funcoes para leitura de multiplos arquivos de rastro*/
int rst_open_file(char *f_name, rst_file_t *f_data, char *syncfilename, int buffer_size);
void rst_close_file(rst_file_t *f_data);
int rst_decode_event(rst_file_t *f_data, rst_event_t *event);

/*Funcoes para leitura de um unico arquivo de rastro*/
int rst_open_one_file(char *f_name, rst_one_file_t *of_data, char *syncfilename, int buffer_size);
void rst_close_one_file(rst_one_file_t *of_data);
int rst_decode_one_event(rst_one_file_t *of_data, rst_event_t *event);

/*Imprime um evento*/
void rst_print_event(rst_event_t *event);

/*Funcoes internas*/
void reorganize_bottom_up (rst_file_t *f_data, int son);
void reorganize_top_down (rst_file_t *f_data, int dead);
void smallest_first (rst_file_t *f_data, int dead, int son);

#endif //_RASTRO_PUBLIC_H_
