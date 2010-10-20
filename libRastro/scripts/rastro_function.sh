#!/bin/sh
#/*
#    Copyright (c) 1998--2006 Benhur Stein
#    
#    This file is part of Pajé.
#
#    Pajé is free software; you can redistribute it and/or modify it under
#    the terms of the GNU Lesser General Public License as published by the
#    Free Software Foundation; either version 2 of the License, or (at your
#    option) any later version.
#
#    Pajé is distributed in the hope that it will be useful, but WITHOUT ANY
#    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
#    FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
#    for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with Pajé; if not, write to the Free Software Foundation, Inc.,
#	51 Franklin Street, Fifth Floor, Boston, MA 02111 USA.
#*/
#
#
#//////////////////////////////////////////////////
#/*      Author: Geovani Ricardo Wiedenhoft      */
#/*      Email: grw@inf.ufsm.br                  */
#//////////////////////////////////////////////////


grep [a-z,A-Z,0-9] > .tmp.rastro
./bin/rastro_generate .tmp.rastro rastro.c rastro.h
rm -f .tmp.rastro

