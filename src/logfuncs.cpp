// HLBot
//
// logfuncs.cpp - Half-Life log functions
//
// $Id: logfuncs.cpp,v 1.1 2002/06/18 00:17:46 yodatoad Exp $

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
#include "config.h"

char* getHLNick(char* szNickString) {
 char *szRetNick;
 int iLocInString, iCharsFound;

 iLocInString = strlen(szNickString);
 iCharsFound = 0;
 
 while (iCharsFound < 3) {
  if (szNickString[iLocInString] == '<') {
   iCharsFound++;
  }
  iLocInString--;
 }
 
 iLocInString++;
 szRetNick = new char[iLocInString+1];
 while (iLocInString > 0) {
  szRetNick[iLocInString-1] = szNickString[iLocInString-1];
  iLocInString--;
 }
 szRetNick[strlen(szRetNick)] = '\0';

 return szRetNick;
}

char getHLTeam(char* szNickString) {
 char cChar;
 int iLocInString;

 iLocInString = strlen(szNickString);

 while (cChar != '<') {
  cChar = szNickString[iLocInString];
  iLocInString--;
 }
 return szNickString[iLocInString+2];
}
