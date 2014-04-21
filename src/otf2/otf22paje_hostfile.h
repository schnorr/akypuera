/*
    This file is part of Akypuera

    Akypuera is free software: you can redistribute it and/or modify
    it under the terms of the GNU Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Akypuera is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Public License for more details.

    You should have received a copy of the GNU Public License
    along with Akypuera. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef __OTF22PAJE_HOSTFILE_H
#define __OTF22PAJE_HOSTFILE_H

int nf_hostnames_search (char *host);
void nf_hostnames_clear (void);
int nf_read_and_create_hierarchy (char *filename);
void nf_container_type_declare (const char *new_container_type, const char *container_type);

#endif
