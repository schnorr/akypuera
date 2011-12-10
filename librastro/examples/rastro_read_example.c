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


//////////////////////////////////////////////////
/*      Author: Geovani Ricardo Wiedenhoft      */
/*      Email: grw@inf.ufsm.br                  */
//////////////////////////////////////////////////


#include "rastro.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
	rst_file_t data;
	rst_event_t ev;
        int i;
        bzero(&data,sizeof(rst_file_t));
        for (i=2; i<argc; i++) {
  	   if (rst_open_file(argv[i], &data, argv[1], 100000) == -1) {
		fprintf(stderr, "could not open rastro file %s\n", argv[i]);
		continue;
	   }
	}
	while (rst_decode_event(&data, &ev)){
		rst_print_event(&ev);
	}
	rst_close_file(&data);
	return 0;
}

