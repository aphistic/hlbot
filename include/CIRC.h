// HLBot
//
// CIRC.h - Header for IRC Server class
//
// $Id: CIRC.h,v 1.2 2002/06/22 07:21:09 yodatoad Exp $

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

#ifndef CIRC_H
#define CIRC_H

#include "CSocket.h"

class CIRC {
 public:
  CIRC();
  ~CIRC();
  bool Connect(char*, int, char*);
  void Quit(char*);
  void Join(char*);
  void Part();
  void SendPrivMsg(char*);
  void SendPrivMsgUser(char* szMessage, char* szUser);
  int Send(char* szBuffer);
  int Recv(char* szBuffer);
  int GetHighestSock();
  int GetSockfd();
 private:
  char szChanName[100];
  CSocket IRCSocket;
};
#endif
