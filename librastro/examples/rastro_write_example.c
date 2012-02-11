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

      Author: Geovani Ricardo Wiedenhoft
      Email: grw@inf.ufsm.fr
*/
#include "saida.h"

int main(int argc, char *argv[])
{
	rst_init(10, 20);
	rst_event(1);
	rst_event_lws(2, 1, 2, (u_int8_t*)"string_3");
	rst_event_wlsfcd(3, 1, 2, (u_int8_t*)"string_3", 4, '5', 6);
	rst_event_iwlsifcd(4, 1, 2, 3, (u_int8_t*)"string_4", 5, 6, '7', 8);
        int i;
        for (i = 0; i < 10; i++){
          rst_event (1);
          sleep(1);
        }
	rst_finalize();
	return 0;
}
