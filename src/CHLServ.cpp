// HLBot
// 
// CHLServ.cpp - Half-Life server class
//
// $Id: CHLServ.cpp,v 1.2 2002/06/18 17:55:09 yodatoad Exp $

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
#include <string.h>

#include "config.h"
#include "strfuncs.h"
#include "CHLServ.h"

CHLServ::CHLServ() {
 szRconPass = new char[MAXDATASIZE];
 szRconId = new char[MAXDATASIZE];
 clearStr(szRconPass, MAXDATASIZE);
 clearStr(szRconId, MAXDATASIZE);
}

CHLServ::~CHLServ() {
 delete [] szRconPass;
 delete [] szRconId;
}

bool CHLServ::ConnectRcon (char* szRconPassParm, char* szServerAddress, int iPort) {
 char* szSendBuf;
 char* szRecvBuf;
 char* rconId;
 int numbytes;
 struct timeval tv;
 fd_set readfds, readfds_orig;

 szSendBuf = new char[MAXDATASIZE];
 szRecvBuf = new char[MAXDATASIZE];
 
 clearStr(szRconPass, MAXDATASIZE);
 strcpy(szRconPass, szRconPassParm);
 
 sprintf(szSendBuf, "\377\377\377\377challenge rcon");
 strcat(szSendBuf, "\0");
 
 if ((numbytes = HLSocket.SendtoN(szServerAddress, iPort, szSendBuf)) == -1) {
  perror("sendto");
  return false;
 }

 FD_ZERO(&readfds_orig);
 FD_SET(HLSocket.iHighestSock, &readfds_orig);
 
 tv.tv_sec = 1;
 tv.tv_usec = 0;

 while (1) {
  bcopy(&readfds_orig, &readfds, sizeof(&readfds_orig));
  select(HLSocket.iHighestSock+1, &readfds, NULL, NULL, &tv);
  if (FD_ISSET(HLSocket.iHighestSock, &readfds)) {
   
   if ((numbytes = HLSocket.Recvfrom(szRecvBuf)) == -1) {
    perror("recvfrom");
    return false;
   }
  } else {
   return false;
  }
 }
 
 if (wordExists(szRecvBuf, 3, ' ')) {
  rconId = getWord(szRecvBuf, 3, ' ');
  strcpy(szRconId, rconId);
 } else {
  return false;
 }

 delete [] rconId;
 delete [] szSendBuf;
 delete [] szRecvBuf;

 return true;
}

bool CHLServ::SendRcon(char* szRconCommand, char* szServerAddress, int iPort) {
 char* szSendBuf;
 char* szRecvBuf;
 int numbytes;
 
 szSendBuf = new char[MAXDATASIZE];
 szRecvBuf = new char[MAXDATASIZE];

 sprintf(szSendBuf, "\377\377\377\377rcon %s \"%s\" %s", szRconId, szRconPass, szRconCommand);
 strcat(szSendBuf, "\0");

 if ((numbytes = HLSocket.SendtoN(szServerAddress, iPort, szSendBuf)) == -1) {
  perror("sendto");
  return false;
 }

 if ((numbytes == HLSocket.Recvfrom(szRecvBuf)) == -1) {
  perror("recvfrom");
  return false;
 }
 
 return false;
}

bool CHLServ::CreateListener(int iHLCPort) {
 if (!HLSocket.BindDGRAM(iHLCPort)) {
  return false;
 }
  
 return true;

}

char* CHLServ::GetServerInfo(char* szServerAddress, int iPort) {
 char* szSendBuf;
 char* szRecvBuf;
 int numbytes;
 
 szSendBuf = new char[MAXDATASIZE];
 szRecvBuf = new char[MAXDATASIZE];
 
 sprintf(szSendBuf, "\377\377\377\377infostring");
 szSendBuf[strlen(szSendBuf)] = '\0';
 if ((numbytes = HLSocket.Sendto(szServerAddress, iPort, szSendBuf)) == -1) {
  perror("sendto");
  return 0;
 }
 
 if ((numbytes = HLSocket.Recvfrom(szRecvBuf)) == -1) {
  perror("recvfrom");
  return 0;
 }

 delete [] szSendBuf;
 
 return szRecvBuf + 24;
}

char* CHLServ::GetPlayers(char* szServerAddress, int iPort) {
 char* szSendBuf;
 char* szRecvBuf;
 char* szTWordP;
 char* pkt;
 char* ret;
 int numbytes, total_players, iLoop;
 
 szSendBuf = new char[MAXDATASIZE];
 szRecvBuf = new char[MAXDATASIZE];

 sprintf(szSendBuf, "\377\377\377\377players");
 szSendBuf[strlen(szSendBuf)] = '\0';
 if ((numbytes = HLSocket.Sendto(szServerAddress, iPort, szSendBuf)) == -1) {
  perror("sendto");
  return 0;
 }

 if ((numbytes = HLSocket.Recvfrom(szRecvBuf)) == -1) {
  perror("recvfrom");
  return 0;
 }
 if (szRecvBuf[4] == 'D') {
  pkt = szRecvBuf;
  pkt += 5;
  total_players = (unsigned int)pkt[0];
  pkt++;
  ret = new char[MAXDATASIZE];
  szTWordP = new char[MAXDATASIZE];
  for (iLoop = 0; iLoop < total_players; iLoop++) {
   
   // Client Number/Index
   sprintf(szTWordP, "%d\\", (unsigned int)pkt[0]);
   strcat(ret, szTWordP);
   clearStr(szTWordP, MAXDATASIZE);
   pkt++;
   
   // Client Name
   strcat(ret, pkt);
   pkt += strlen(pkt);
   pkt += 9;
   clearStr(szTWordP, MAXDATASIZE);
   
   if (iLoop+1 < total_players) {
    strcat(ret, "\\");
   }
  }
  
  ret[strlen(ret)] = '\0';
 } else {
  return 0;
 }
 
 delete [] szSendBuf;
 delete [] szRecvBuf;
 
 return ret;
}
