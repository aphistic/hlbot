// HLBot
//
// hlfuncs.cpp - Functions for HL server comm
//
// $Id: hlfuncs.cpp,v 1.1 2002/07/09 06:45:18 yodatoad Exp $

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

char* hlParsePlayers(char* szHLPacket) {
 int iTotalPlayers = 0, iLoop = 0;
 char *ret, *szTWordP, *pkt;
 
 if (szHLPacket[4] == 'D') {
  pkt = szHLPacket;
  pkt += 5;
  iTotalPlayers = (unsigned int)pkt[0];
  pkt++;
  ret = new char[MAXRECVSIZE];
  szTWordP = new char[MAXRECVSIZE];
  memset(ret, '\0', MAXRECVSIZE);
  memset(szTWordP, '\0', MAXRECVSIZE);
  if (pkt[0] == '\0')
   return NULL;
  for (iLoop = 0; iLoop < iTotalPlayers; iLoop++) {

   // Client Number/Index
   sprintf(szTWordP, "%d\\", (unsigned int)pkt[0]);
   strcat(ret, szTWordP);
   memset(szTWordP, '\0', MAXRECVSIZE);
   pkt++;

   // Client Name
   strcat(ret, pkt);
   pkt += strlen(pkt);
   pkt += 9;
   
   memset(szTWordP, '\0', MAXRECVSIZE);

   if (iLoop+1 < iTotalPlayers) {
    strcat(ret, "\\");
   }
  }
  return ret;
 } else {
  return NULL;
 }
}

