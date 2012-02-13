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
#include "auto-generated.h"

static timestamp_t my_timestamping (void)
{
  static timestamp_t counter = 0;
  return counter++;
}

static timestamp_t my_timeresolution (void)
{
  return 0;
}

int main(int argc, char *argv[])
{
  rst_init_timestamp(10, 20, &my_timestamping, &my_timeresolution);
  rst_event(1);
  rst_event_lws(2, 1, 2, "string_3");
  rst_event_wlsfcd(3, 1, 2, "string_3", 4, '5', 6);
  rst_event_iwlsifcd(4, 1, 2, 3, "string_4", 5, 6, '7', 8);
  rst_finalize();
  return 0;
}
