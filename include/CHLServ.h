// HLBot
//
// CHLServ.h - Header for Half-Life server class
//
// $Id: CHLServ.h,v 1.1 2002/06/18 00:17:44 yodatoad Exp $

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

#ifndef CHLSERV_H
#define CHLSERV_H

#include "CSocket.h"

class CHLServ {
 public:
  CHLServ();
  ~CHLServ();
  bool CreateListener(int iHLPort);
  char* GetServerInfo(char*, int);
  char* GetPlayers(char*, int);
  bool ConnectRcon(char*, char*, int);
  bool SendRcon(char*, char*, int);
 private:
  CSocket HLSocket;
  char* szRconPass;
  char* szRconId;
};

#endif
