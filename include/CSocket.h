// HLBot
//
// CSocket.h - Header for socket class
//
// $Id: CSocket.h,v 1.1 2002/06/18 00:17:45 yodatoad Exp $

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

#ifndef CSOCKET_H
#define CSOCKET_H

#include <arpa/inet.h>
#include <sys/un.h>
#include "config.h"

class CSocket {
 public:
  CSocket();
  ~CSocket();
  bool Connect(char*, int);
  int Send(char*, int, int);
  int Recv(char*, int, int);
  int Bind(int); //port
  bool BindDGRAM(int);
  bool BindDGRAMIPC(char*);
  int Recvfrom(char*);
  int RecvfromIPC(char*);
  int Sendto(char*, int, char*);
  int SendtoN(char*, int, char*);
  int SendtoIPC(char*, char*);
  void Disconnect();
  static int iHighestSock;
  int iSockfd;
 private:
  static int iTotalSocks;
  struct sockaddr_un saConnectionIPC;
  struct sockaddr_in saConnection;
  int iSockType;
  char szIPCSockName[MAXDATASIZE];
};

#endif
