// HLBot
//
// CSocket.cpp - Socket class
//
// $Id: CSocket.cpp,v 1.1 2002/06/18 00:17:45 yodatoad Exp $

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
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

#include "config.h"
#include "CSocket.h"

CSocket::CSocket() {
 iSockfd = 0;
 iSockType = 0;
 iTotalSocks++;
 //printf("Total Socks: %d\n", iTotalSocks);
}

CSocket::~CSocket() {
 //printf("I'm in ~CSocket\n");
 Disconnect();
}

int CSocket::iHighestSock = 0;
int CSocket::iTotalSocks = 0;

bool CSocket::Connect(char* szServerAddress, int iPort) {
 struct hostent *he;
 struct sockaddr_in haddr;
 
 if ((iSockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
  perror("socket");
  return false;
 }
 
 if ((he = gethostbyname(szServerAddress)) == NULL) {
  perror("gethostbyname");
  return false;
 }

 haddr.sin_family = AF_INET;
 haddr.sin_port = htons(iPort);
 haddr.sin_addr = *((struct in_addr *)he->h_addr);
 memset(&(haddr.sin_zero), '\0', 8);

 if (connect(iSockfd, (struct sockaddr *)&haddr, sizeof(struct sockaddr)) == -1) {
  perror("connect");
  return false;
 }
 
 if (iSockfd > iHighestSock)
  iHighestSock = iSockfd;
 iSockType = 1;
 //printf("Highest Sock: %d\n", iHighestSock);
 return true;
}

bool CSocket::BindDGRAMIPC(char* szSockPath) {
 int sockLen;
 
 if ((iSockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
  perror("socket");
  return false;
 }

 saConnectionIPC.sun_family = AF_UNIX;
 strcpy(saConnectionIPC.sun_path, szSockPath);
 unlink(szSockPath);
 
 sockLen = strlen(saConnectionIPC.sun_path) + sizeof(saConnectionIPC.sun_family);
 bind(iSockfd, (struct sockaddr *)&saConnectionIPC, sockLen);

 if (iSockfd > iHighestSock)
  iHighestSock = iSockfd;
 iSockType = 3;
 strcpy(szIPCSockName, szSockPath);
 
 return true;
}

bool CSocket::BindDGRAM(int iPort) {
 
 if ((iSockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
  perror("socket");
  return false;
 }

 saConnection.sin_family = AF_INET;
 saConnection.sin_port = htons(iPort);
 saConnection.sin_addr.s_addr = INADDR_ANY;
 memset(&(saConnection.sin_zero), '\0', 8);

 if (bind(iSockfd, (struct sockaddr *)&saConnection, sizeof(struct sockaddr)) == -1) {
  perror("bind");
  return false;
 }
 
 if (iSockfd > iHighestSock)
  iHighestSock = iSockfd;
 //printf("Highest Sock: %d\n", iHighestSock);
 return true;
}

int CSocket::Send(char* szBuffer, int iBufLen, int iFlags) {
 return send(iSockfd, szBuffer, iBufLen, iFlags);
}

int CSocket::Recv(char* szBuffer, int iBufLen, int iFlags) {
 return recv(iSockfd, szBuffer, iBufLen, iFlags);
}

void CSocket::Disconnect() {
 close(iSockfd);
 if (iSockType == 3) {
  unlink(szIPCSockName);
 }
 //printf("Socket Closed (CSocket - %d)\n", iSockfd);
}

int CSocket::Sendto(char* szAddress, int iPort, char* szBuffer) {
 struct hostent *he;
 
 if ((he = gethostbyname(szAddress)) == NULL) {
  perror("gethostbyname");
  return -1;
 }

 saConnection.sin_family = AF_INET;
 saConnection.sin_port = htons(iPort);
 saConnection.sin_addr = *((struct in_addr *)he->h_addr);
 memset(&(saConnection.sin_zero), '\0', 8);
 
 return sendto(iSockfd, szBuffer, strlen(szBuffer), 0, (struct sockaddr *)&saConnection, sizeof(struct sockaddr));

}

int CSocket::SendtoN(char* szAddress, int iPort, char* szBuffer) {
 struct hostent *he;

 if ((he = gethostbyname(szAddress)) == NULL) {
  perror("gethostbyname");
  return -1;
 }

 saConnection.sin_family = AF_INET;
 saConnection.sin_port = htons(iPort);
 saConnection.sin_addr = *((struct in_addr *)he->h_addr);
 memset(&(saConnection.sin_zero), '\0', 8);

 return sendto(iSockfd, szBuffer, strlen(szBuffer)+1, 0, (struct sockaddr *)&saConnection, sizeof(struct sockaddr));

}

int CSocket::SendtoIPC(char* szSockPath, char* szBuffer) {
 struct sockaddr_un saConnectionIPCSend;

 saConnectionIPCSend.sun_family = AF_UNIX;
 strcpy(saConnectionIPCSend.sun_path, szSockPath);
 
 return sendto(iSockfd, szBuffer, strlen(szBuffer), 0, (struct sockaddr *)&saConnectionIPCSend, sizeof(struct sockaddr));
}

int CSocket::RecvfromIPC(char* szBuffer) {
 int addr_len;

 addr_len = sizeof(struct sockaddr);
 return recvfrom(iSockfd, szBuffer, MAXRECVLEN-1, 0, (struct sockaddr *)&saConnection, (socklen_t *)&addr_len);
}
 
int CSocket::Recvfrom(char* szBuffer) {
 int addr_len;

 addr_len = sizeof(struct sockaddr);
 return recvfrom(iSockfd, szBuffer, MAXRECVLEN-1, 0, (struct sockaddr *)&saConnection, (socklen_t *)&addr_len);

}
