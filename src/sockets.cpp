// HLBot
//
// sockets.cpp - HLBot sockets functions
//
// $Id: sockets.cpp,v 1.1 2002/07/09 06:45:18 yodatoad Exp $

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
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include "config.h"

int sockConnect(const char *szHostname, int iPort) {
 int iSockfd;
 struct hostent *he;
 struct sockaddr_in haddr;

 if ((iSockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
  perror("socket");
  return -1;
 }

 if ((he = gethostbyname(szHostname)) == NULL) {
  perror("gethostbyname");
  return -1;
 }
 
 haddr.sin_family = AF_INET;
 haddr.sin_port = htons(iPort);
 haddr.sin_addr = *((struct in_addr *)he->h_addr);
 memset(&(haddr.sin_zero), '\0', 8);

 if (connect(iSockfd, (struct sockaddr *)&haddr, sizeof(struct sockaddr)) == -1) {
  perror("connect");
  return -1;
 }
 
 return iSockfd;
}

int sockSendto(int iSockfd, char *szBuffer, char* szAddress, int iPort, int iSize) {
 struct hostent *he;
 struct sockaddr_in saConn;
 
 if ((he = gethostbyname(szAddress)) == NULL) {
  perror("gethostbyname");
  return -1;
 }

 saConn.sin_family = AF_INET;
 saConn.sin_port = htons(iPort);
 saConn.sin_addr = *((struct in_addr *)he->h_addr);
 memset(&(saConn.sin_zero), '\0', 8);

 return sendto(iSockfd, szBuffer, iSize, 0, (struct sockaddr *)&saConn, sizeof(struct sockaddr));
}
