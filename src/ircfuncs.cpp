// HLBot
//
// ircfuncs.cpp - IRC functions
//
// $Id: ircfuncs.cpp,v 1.1 2002/06/18 00:17:45 yodatoad Exp $

// Copyright (C) 2002  Erik Davidson
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <stdio.h>
#include <string.h>
#include "config.h"
#include "strfuncs.h"

char* getNickname(char* szBuffer) {
 char* ret;
 int curLoc, loopLoc;
 
 ret = new char[MAXDATASIZE];
 clearStr(ret, MAXDATASIZE);

 curLoc = 0;
 loopLoc = 0;
 szBuffer++;

 while (szBuffer[loopLoc] != '!') {
  ret[curLoc] = szBuffer[loopLoc];
  curLoc++;
  szBuffer++;
 }

 printf("Nick: %s\n", ret);
 
 return ret;
}
