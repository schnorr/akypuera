#!/usr/bin/env julia

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

if length(ARGS) != 1 || !isfile(ARGS[1])
    println("Wrong usage.")
    println("Provide the traces.otf2 file within a scorep directory.")
    exit(1)
end

header = """
#This trace was generated with: otf2ompprint2paje.jl $(ARGS[1])
#otf2ompprint2paje.jl is available at https://github.com/schnorr/akypuera/
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
%EndEventDef"""

# print header
println(header)

# print type hierarchy
println("0 THREAD 0 \"THREAD\"")
println("2 STATE THREAD \"STATE\"")

# convert events
# declare -A tids # ?
tids = AbstractString[]

FIRST_TIMESTAMP = -Inf
RESOLUTION = 1000000

for l in split(readall(pipeline(`otf2-print $(ARGS[1])`, `tail -n +6`)), '\n')
    splitline = split(l)
    if length(splitline) >= 3
        code = splitline[1]
        tid = splitline[2]
        timestamp = parse(Float64, splitline[3])

        if FIRST_TIMESTAMP < 0
            FIRST_TIMESTAMP = timestamp
        end
        
        timestamp = (timestamp - FIRST_TIMESTAMP)/RESOLUTION

        # TID equals 0 should be renamed to "zero" so Paje can accept it
        if tid == "0"
            tid = "zero"
        end

        # check if TID was already PajeCreateContainer
        if !(tid in tids)
            println("6 $timestamp $tid THREAD 0 $tid") #PajeCreateContainer
            push!(tids, tid)
        end

        if code == "ENTER"
            state = replace(join(splitline[5:end], " "), "\"", "")
            println("12 $timestamp $tid STATE \"$state\"") #PajePushState
        elseif code == "LEAVE"
            println("14 $timestamp $tid STATE") #PajePopState
        end
    end
end

exit(0)
