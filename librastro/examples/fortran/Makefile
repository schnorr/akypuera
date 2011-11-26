# Makefile
# File: "/home/kassick/Work/akypuera/libRastro/examples/fortran/Makefile"
# Created: "Qua, 23 Fev 2011 17:36:49 +0100 (kassick)"
# Updated: "Qui, 24 Fev 2011 13:12:33 +0100 (kassick)"
# $Id$
# Copyright (C) 2011, Rodrigo Virote Kassick <rvkassick@inf.ufrgs.br> 

CFLAGS=-g -I/home/kassick/Work/akypuera/libRastro/include/ -I/home/kassick/Work/akypuera/libRastro/src
FFLAGS=-g

LDFLAGS=-L/home/kassick/Work/akypuera/libRastro/ -lRastro

all: rst_fort

rst_functions.c: rst_functions.dsc
	../../rastro_generate -c rst_functions.c -h rst_functions.h -f rst_functions_f.c rst_functions.dsc

%.o: %.f90
	ifort $(FFLAGS) -c -o $@  $<

%.o: %.c
	icc $(CFLAGS) -c -o $@ $<

rst_fort: rst_fort.o rst_functions.o rst_functions_f.o
	ifort $(LDFLAGS) $^ -o $@

