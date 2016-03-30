#!/bin/bash

# This file is part of Akypuera

# Akypuera is free software: you can redistribute it and/or modify
# it under the terms of the GNU Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# Akypuera is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Public License for more details.

# You should have received a copy of the GNU Public License
# along with Akypuera. If not, see <http://www.gnu.org/licenses/>.

INPUT=$1

if [ -z $INPUT ]
then
    echo "Provide the traces.otf2 file within a scorep directory."
    exit
fi

echo "#This trace was generated with: otf2ompprint2paje.sh $INPUT
#otf2ompprint2paje.sh is available at https://github.com/schnorr/akypuera/
#The script relies on the availability of otf2-print executable (ScoreP)
%EventDef PajeDefineContainerType 0
%       Alias string
%       Type string
%       Name string
%EndEventDef
%EventDef PajeDefineVariableType 1
%       Alias string
%       Type string
%       Name string
%       Color color
%EndEventDef
%EventDef PajeDefineStateType 2
%       Alias string
%       Type string
%       Name string
%EndEventDef
%EventDef PajeDefineEventType 3
%       Alias string
%       Type string
%       Name string
%EndEventDef
%EventDef PajeDefineLinkType 4
%       Alias string
%       Type string
%       StartContainerType string
%       EndContainerType string
%       Name string
%EndEventDef
%EventDef PajeDefineEntityValue 5
%       Alias string
%       Type string
%       Name string
%       Color color
%EndEventDef
%EventDef PajeCreateContainer 6
%       Time date
%       Alias string
%       Type string
%       Container string
%       Name string
%EndEventDef
%EventDef PajeDestroyContainer 7
%       Time date
%       Type string
%       Name string
%EndEventDef
%EventDef PajeSetVariable 8
%       Time date
%       Container string
%       Type string
%       Value double
%EndEventDef
%EventDef PajeAddVariable 9
%       Time date
%       Container string
%       Type string
%       Value double
%EndEventDef
%EventDef PajeSubVariable 10
%       Time date
%       Container string
%       Type string
%       Value double
%EndEventDef
%EventDef PajeSetState 11
%       Time date
%       Container string
%       Type string
%       Value string
%EndEventDef
%EventDef PajePushState 12
%       Time date
%       Container string
%       Type string
%       Value string
%EndEventDef
%EventDef PajePopState 14
%       Time date
%       Container string
%       Type string
%EndEventDef
%EventDef PajeResetState 15
%       Time date
%       Type string
%       Container string
%EndEventDef
%EventDef PajeStartLink 16
%       Time date
%       Container string
%       Type string
%       StartContainer string
%       Value string
%       Key string
%EndEventDef
%EventDef PajeEndLink 19
%       Time date
%       Container string
%       Type string
%       EndContainer string
%       Value string
%       Key string
%EndEventDef
%EventDef PajeNewEvent 20
%       Time date
%       Container string
%       Type string
%       Value string
%EndEventDef"

# print type hierarchy

echo "0 THREAD 0 \"THREAD\"
2 STATE THREAD \"STATE\""

# convert events
declare -A tids

FIRST_TIMESTAMP=""
RESOLUTION=1000000

otf2-print $INPUT | tail -n +6 |
    while read LINHA
    do
	CODE=`echo $LINHA | cut -d" " -f1`
	TID=`echo $LINHA | cut -d" " -f2`
	TIMESTAMP=`echo $LINHA | cut -d" " -f3`

	if [ -z $FIRST_TIMESTAMP ]; then
	    FIRST_TIMESTAMP=$TIMESTAMP
	fi

	TIMESTAMP=`echo "($TIMESTAMP - $FIRST_TIMESTAMP) / $RESOLUTION" | bc -l | sed "s/0*$/0/"`
	
	#TID equals 0 should be renamed to "zero" so Paje can accept it
	if [ $TID == "0" ] ; then
	    TID="zero"
	fi
	
	# check if TID was already PajeCreateContainer
	if [ -z "${tids[$TID]}" ] ; then
	    echo "6 $TIMESTAMP $TID THREAD 0 $TID" #PajeCreateContainer
	    tids[$TID]="1"
	fi

	case $CODE in
	    "ENTER" )
		STATE=`echo $LINHA | cut -d" " -f5-20 | sed "s/\"//g"`
		echo "12 $TIMESTAMP $TID STATE \"$STATE\"" #PajePushState
		;;

	    "LEAVE" )
		STATE=`echo $LINHA | cut -d" " -f5-20 | sed "s/\"//g"`
		echo "14 $TIMESTAMP $TID STATE" #PajePopState
		;;
	    "THREAD_FORK" )
		;;
	    "THREAD_TEAM_BEGIN" )
		;;
	    "THREAD_TEAM_END" )
		;;
	    "THREAD_JOIN" )
		;;
	    * )
	    ;;
	esac
    done
