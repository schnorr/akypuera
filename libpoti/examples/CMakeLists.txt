# This file is part of Poti
#
# Poti is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Poti is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser Public License for more details.
#
# You should have received a copy of the GNU Lesser Public License
# along with Poti. If not, see <http://www.gnu.org/licenses/>.
ADD_EXECUTABLE(main_example main_example.c)
TARGET_LINK_LIBRARIES(main_example poti)

ADD_EXECUTABLE(vite-1.2 vite-1.2.c)
TARGET_LINK_LIBRARIES(vite-1.2 poti)

ADD_EXECUTABLE(vite-svn vite-svn.c)
TARGET_LINK_LIBRARIES(vite-svn poti)
