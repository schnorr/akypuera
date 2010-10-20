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



# Script que gera de modo pratico os arquivos de saida com as 
#funcoes necessarias utilizadas e passadas para ele


if [ "$CC" == "" ]
then
	CC="gcc"
fi

SRCS=""
NENTRADA=$#
I=1
if [ 2 -gt $NENTRADA ]
then
	echo "ERRO: Parametros invalidos."
	echo "  USO: $0 <arquivos com funcoes...> <arquivo sem terminacao>" 
	echo " PS: Se quiser setar flags, sete a varialvel \"CFLAGS\" "
	exit
fi


while [ $I -lt $NENTRADA ]
do
	SRCS="$SRCS $1"
	shift
	I=$((I+1))
done

if [ -e $1 ]
then
	echo "ERRO: Arquivo $1 ja existente"
	exit
fi

$CC -E $CFLAGS $SRCS -I./include/ 2>/dev/null | ./bin/rastro_names.sh > $1
./bin/rastro_generate $1 $1.c $1.h
rm -f $1
echo "Inclua nos arquivos \"$SRCS\" a linha abaixo."
echo "#include \"$1.h\""

