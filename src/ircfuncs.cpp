// HLBot
//
// ircfuncs.cpp - HLBot IRC functions
//
// $Id: ircfuncs.cpp,v 1.6 2003/07/10 02:12:31 sg1bc Exp $

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

#include <sys/types.h>
#include <string>
#include <stdio.h>
#include <sys/socket.h>
#include "config.h"
#include "strfuncs.h"

bool ircConnect(int iSockIRC, const char *szNickname) {
 string sSendBuf;
 
 sSendBuf = "";
 sSendBuf += "NICK ";
 sSendBuf += szNickname;
 sSendBuf += "\n";

 if (send(iSockIRC, (char *)sSendBuf.c_str(), strlen(sSendBuf.c_str()), 0) == -1)
  perror("send");
 sSendBuf = "";
 sSendBuf += "USER hlbot 8 * :HLBot ";
 sSendBuf += HLBOTVERSION;
 sSendBuf += "\n";
 if (send(iSockIRC, (char *)sSendBuf.c_str(), strlen(sSendBuf.c_str()), 0) == -1)
  perror("send");
 
 return true;
}

bool ircJoin(int iSockIRC, const char *szChannel) {
 string sSendBuf;

 sSendBuf = "";
 sSendBuf += "JOIN ";
 sSendBuf += szChannel;
 sSendBuf += "\n";
 send(iSockIRC, (char *)sSendBuf.c_str(), strlen(sSendBuf.c_str()), 0);
 
 return true;
}

bool ircAuthUser(int iSock, const char *szAuthService, const char *szAuthUsername, const char *szAuthPassword) {
 string sSendBuf;
 sSendBuf = "";
 sSendBuf = "PRIVMSG ";
 sSendBuf += szAuthService;
 sSendBuf += " :auth ";
 sSendBuf += szAuthUsername;
 sSendBuf += " ";
 sSendBuf += szAuthPassword;
 sSendBuf += "\n";
 send(iSock, (char *)sSendBuf.c_str(), strlen(sSendBuf.c_str()), 0);

 return true;
}
