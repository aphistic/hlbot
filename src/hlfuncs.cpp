// HLBot
//
// hlfuncs.cpp - Functions for HL server comm
//
// $Id: hlfuncs.cpp,v 1.2 2002/07/15 07:10:14 yodatoad Exp $

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
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include "config.h"

char* hlParsePlayers(char* szHLPacket) {
 int iTotalPlayers = 0, iLoop = 0;
 int iHLPlayerFrags = 0;
 char *ret, *szTWordP = NULL, *pkt, *szTFrag = NULL;
 
 if (szHLPacket[4] == 'D') {
  pkt = szHLPacket;
  pkt += 5;
  iTotalPlayers = (unsigned int)pkt[0];
  pkt++;
  ret = new char[MAXRECVSIZE];
  szTWordP = new char[MAXRECVSIZE];
  szTFrag = new char[33];
  memset(ret, '\0', MAXRECVSIZE);
  memset(szTWordP, '\0', MAXRECVSIZE);
  memset(szTFrag, '\0', 33);
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
   strcat(ret, "\\");
   iHLPlayerFrags = (int)*(pkt+1);
   pkt += 9;
   sprintf(szTFrag, "%d", iHLPlayerFrags);
   strcat(ret, szTFrag);
   
   memset(szTFrag, '\0', 33);
   memset(szTWordP, '\0', MAXRECVSIZE);

   if (iLoop+1 < iTotalPlayers) {
    strcat(ret, "\\");
   }
  }
  if (szTFrag != NULL)
   delete [] szTFrag;
  if (szTWordP != NULL)
   delete [] szTWordP;
  
  return ret;
 } else {
  if (szTFrag != NULL)
   delete [] szTFrag;
  if (szTWordP != NULL) 
   delete [] szTWordP;
  
  return NULL;
 }
}

