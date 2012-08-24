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
#include <otf.h>
#include <stdio.h>

int handleEnter (void *userData, uint64_t time, uint32_t
                 function, uint32_t process, uint32_t source) {
  return OTF_RETURN_OK;
}
int handleLeave (void *userData, uint64_t time, uint32_t
                 function, uint32_t process, uint32_t source) {
  return OTF_RETURN_OK;
}

int main( int argc, char** argv ) {
  OTF_FileManager* manager;
  OTF_Reader* reader;
  OTF_HandlerArray* handlers;
  manager= OTF_FileManager_open( 100 );
  handlers = OTF_HandlerArray_open();
  reader = OTF_Reader_open( "mytrace", manager );
  OTF_HandlerArray_setHandler( handlers,
                               (OTF_FunctionPointer*) handleEnter,
                               OTF_ENTER_RECORD );
  OTF_HandlerArray_setHandler( handlers,
                               (OTF_FunctionPointer*) handleLeave,
                               OTF_LEAVE_RECORD );
  OTF_Reader_readEvents( reader, handlers );

  OTF_Reader_close( reader );
  OTF_HandlerArray_close( handlers );
  OTF_FileManager_close( manager );
  return 0;
}
