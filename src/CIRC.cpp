// HLBot
//
// CIRC.cpp - IRC Server class
//
// $Id: CIRC.cpp,v 1.3 2002/06/22 07:21:09 yodatoad Exp $

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

#include "strfuncs.h"
#include "CIRC.h"

CIRC::CIRC() {

}

CIRC::~CIRC() {

}

bool CIRC::Connect(char* szServer, int iPort, char* szNickname) {
 char *tSendBufP, *tRecBufP;

 tSendBufP = new char[MAXDATASIZE];
 tRecBufP = new char[MAXDATASIZE];
 
 if (IRCSocket.Connect(szServer, iPort)) {
 } else {
  return false;
 }

 sprintf(tSendBufP, "NICK %s\n", szNickname);
 if (IRCSocket.Send(tSendBufP, strlen(tSendBufP), 0) == -1)
  perror("send");
 sprintf(tSendBufP, "USER csbot 8 * :CSBot %s\n", VERSION);
 if (IRCSocket.Send(tSendBufP, strlen(tSendBufP), 0) == -1)
  perror("send");

 delete [] tSendBufP, tRecBufP;
 return true;
}

int CIRC::Send(char* szBuffer) {
 return IRCSocket.Send(szBuffer, strlen(szBuffer), 0);
}

int CIRC::Recv(char* szBuffer) {
 return IRCSocket.Recv(szBuffer, MAXDATASIZE-1, 0);
}

void CIRC::Join(char* szChannel) {
 char *szSendBufP;
 
 szSendBufP = new char[MAXDATASIZE];
 sprintf(szSendBufP, "JOIN %s\n", szChannel);
 Send(szSendBufP);
 delete [] szSendBufP;
 strcpy(szChanName, szChannel);
}

void CIRC::SendPrivMsg(char* szMessage) {
 char *szSendBufP;

 szSendBufP = new char[MAXDATASIZE];
 sprintf(szSendBufP, "PRIVMSG %s :%s\n", szChanName, szMessage);
 Send(szSendBufP);
 delete [] szSendBufP;
}

void CIRC::SendPrivMsgUser(char* szMessage, char* szUser) {
 char *szSendBufP;

 szSendBufP = new char[MAXDATASIZE];
 sprintf(szSendBufP, "PRIVMSG %s :%s\n", szUser, szMessage);
 Send(szSendBufP);
 delete [] szSendBufP;
}

void CIRC::Quit(char* szQuitMessage) {
 char* szSendBufP;

 szSendBufP = new char[MAXDATASIZE];
 sprintf(szSendBufP, "QUIT :%s\n", szQuitMessage);
 Send(szSendBufP);

 delete [] szSendBufP;
}

int CIRC::GetHighestSock() {
 return IRCSocket.iHighestSock;
}

int CIRC::GetSockfd() {
 return IRCSocket.iSockfd;
}
