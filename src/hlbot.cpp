// HLBot
//
// hlbot.cpp - Main HLBot source
// 
// $Id: hlbot.cpp,v 1.5 2002/06/25 21:39:51 yodatoad Exp $

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
#include <string>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include "config.h"
#include "strfuncs.h"
#include "logfuncs.h"
#include "ircfuncs.h"
#include "CSocket.h"
#include "CIRC.h"
#include "CHLServ.h"

void catchInt(int s);
void catchAlrm(int s);
bool readConfig();

CIRC IRCServer;
CHLServ HLServer;
CSocket ChildIPCSock, ParentIPCSock;

//Config Vars
char *szCfgHLServ, *szCfgIRCServ, *szCfgIRCChan, *szCfgIRCNick;
char *szCfgRconPass;
int iCfgHLPort, iCfgHLClientPort, iCfgIRCPort, iCfgLogPort, iCfgAdvertise;



int main(int argc, char *argv[]) {
 char *szActWordP, 
      szRecvBuf[MAXDATASIZE],
      szSendBuf[MAXDATASIZE],
      *szQueryServer,
      *szCurrentLineP,
      *szNickname,
      *szTBufP,
      *szTBuf2P,
      *szTWordP;
 int numbytes, looptest, iTLoop, iTNum, iTNum2, iHLPort;
 pid_t pid;
 fd_set readfds, creadfds;
 struct timeval tv;
 
 szCfgHLServ = new char[MAXDATASIZE];
 szCfgIRCServ = new char[MAXDATASIZE];
 szCfgIRCChan = new char[MAXDATASIZE];
 szCfgIRCNick = new char[MAXDATASIZE];
 szCfgRconPass = new char[MAXDATASIZE];
 iCfgHLPort = 27015;
 iCfgHLClientPort = 0;
 iCfgIRCPort = 6667;
 iCfgLogPort = 27008;
 
 if (!readConfig()) {
  printf("\nError reading config. Exiting.\n");
  exit(1);
 }

 if (szCfgHLServ[0] == '\0') {
  printf("You must enter a Half-Life Server.\n");
  exit(1);
 } else if (szCfgIRCServ[0] == '\0') {
  printf("You must enter an IRC Server.\n");
  exit(1);
 } else if (szCfgIRCChan[0] == '\0') {
  printf("You must enter an IRC Channel.\n");
  exit(1);
 } else if (szCfgIRCNick[0] == '\0') {
  printf("You must enter an IRC Nickname.\n");
  exit(1);
 }
 if (szCfgRconPass[0] == '\0') {
  printf("No RCON Password was supplied. Functions using rcon will not work.\n");
 }

 if (iCfgAdvertise > 0) {
  alarm(iCfgAdvertise*60);
 }
 
 looptest = 0;
 signal(SIGINT, catchInt);
 signal(SIGALRM, catchAlrm);
 
 ChildIPCSock.BindDGRAMIPC(IPCSOCKNAMEC);
 ParentIPCSock.BindDGRAMIPC(IPCSOCKNAMEP);

 if ((pid = fork()) == -1) {
  perror("fork");
  exit(1);
 } else if (pid == 0) {
// -- CHILD --  
  char *pkt, *hlNickname, *hlDoing, *hlTeam, *hlNickname2, *hlTeam2, *hlKilledWith;
  char *hlFullInfo, *hlFullInfo2;
  char *hlCTWins, *hlTWins;
  CSocket HLLogSock;
 
  HLLogSock.BindDGRAM(iCfgLogPort);
  
  FD_ZERO(&creadfds);

  tv.tv_sec = 1;
  tv.tv_usec = 0;
 
  FD_SET(ChildIPCSock.iSockfd, &creadfds);
  FD_SET(HLLogSock.iSockfd, &creadfds);
  
  hlTeam = new char[3];
  hlTeam2 = new char[3];
  
  while (1) {
   select(ChildIPCSock.iHighestSock+1, &creadfds, NULL, NULL, &tv);

   if (FD_ISSET(ChildIPCSock.iSockfd, &creadfds)) {
    clearStr(szRecvBuf, MAXDATASIZE);
    ChildIPCSock.RecvfromIPC(szRecvBuf);
#ifdef DEBUG    
    printf("CHILD: Parent sent something - \"%s\"\n", szRecvBuf);
#endif
    if (!strcmp(szRecvBuf, "kill")) {
     exit(0);
    }
    tv.tv_sec = 1;
   } else if (FD_ISSET(HLLogSock.iSockfd, &creadfds)) {
    clearStr(szRecvBuf, MAXDATASIZE);
    HLLogSock.Recvfrom(szRecvBuf);

    pkt = szRecvBuf;
    pkt += 33;
    
    if (pkt[strlen(pkt)-1] == '\n') {
     pkt[strlen(pkt)-1] = '\0';
    }

#ifdef DEBUG
    printf("HL Log: %s\n", szRecvBuf);
#endif
    
    szActWordP = getWord(pkt, 1, ' ');
    if (!strcmp(szActWordP, "Log")) {
     //Log stuff
    } else if (!strcmp(szActWordP, "Server")) {
     //Server stuff
     if (wordExists(pkt, 2, ' ')) {
      szTWordP = getWord(pkt, 2, ' ');
      if (!strcmp(szTWordP, "say")) {
       clearStr(szSendBuf, MAXDATASIZE);
       if (wordExists(pkt, 2, '\"')) {
        szTBuf2P = getWord(pkt, 2, '\"');
	sprintf(szSendBuf, "<Server> %s", szTBuf2P);
       }
       ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
      }
     }
    } else if (!strcmp(szActWordP, "World")) {
     //World stuff
     if (wordExists(pkt, 3, ' ')) {
      szTWordP = getWord(pkt, 3, ' ');
      if (!strcmp(szTWordP, "\"Round_Start\"")) {
       //New Round started
       ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, "Round Started");
      }
      if (!strcmp(szTWordP, "\"Round_End\"")) {
       //Round ended
       ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, "Round Ended");
      }
      if (!strcmp(szTWordP, "\"Round_Draw\"")) {
       ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, "Round Draw");
       hlCTWins = getWord(pkt, 5, ' ');
       hlTWins = getWord(pkt, 7, ' ');
       clearStr(szSendBuf, MAXDATASIZE);
       sprintf(szSendBuf, "Wins: (CT %s (T %s", hlCTWins, hlTWins);
       ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
      }
      if (!strcmp(szTWordP, "\"Game_Commencing\"")) {
       ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, "Game Commencing");
      }
     }
    } else if (!strcmp(szActWordP, "Team")) {
     //Team stuff
     if (wordExists(pkt, 3, ' ')) {
      szTWordP = getWord(pkt, 3, ' ');
      if (!strcmp(szTWordP, "triggered")) {
       szTBufP = getWord(pkt, 4, ' ');
       hlCTWins = getWord(pkt, 6, ' ');
       hlTWins = getWord(pkt, 8, ' ');
       if (!strcmp(szTBufP, "\"Target_Saved\"")) {
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, "CT's saved the target.");
        clearStr(szSendBuf, MAXDATASIZE);
	sprintf(szSendBuf, "Wins: (CT %s (T %s", hlCTWins, hlTWins);
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
       }
       if (!strcmp(szTBufP, "\"Bomb_Defused\"")) {
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, "CT's defused the bomb.");
	clearStr(szSendBuf, MAXDATASIZE);
	sprintf(szSendBuf, "Wins: (CT %s (T %s", hlCTWins, hlTWins);
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
       }
       if (!strcmp(szTBufP, "\"Target_Bombed\"")) {
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, "Terrorists bombed the target.");
	clearStr(szSendBuf, MAXDATASIZE);
	sprintf(szSendBuf, "Wins: (CT %s (T %s", hlCTWins, hlTWins);
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
       }
       if (!strcmp(szTBufP, "\"All_Hostages_Rescued\"")) {
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, "CT's rescued all the hostages.");
	clearStr(szSendBuf, MAXDATASIZE);
	sprintf(szSendBuf, "Wins: (CT %s (T %s", hlCTWins, hlTWins);
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
       }
       if (!strcmp(szTBufP, "\"Hostages_Not_Rescued\"")) {
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, "Hostages have not been rescued.");
	clearStr(szSendBuf, MAXDATASIZE);
	sprintf(szSendBuf, "Wins: (CT %s (T %s", hlCTWins, hlTWins);
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
       }
       if (!strcmp(szTBufP, "\"CTs_Win\"")) {
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, "Counter-Terrorists Win.");
	clearStr(szSendBuf, MAXDATASIZE);
	sprintf(szSendBuf, "Wins: (CT %s (T %s", hlCTWins, hlTWins);
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
       }
       if (!strcmp(szTBufP, "\"Terrorists_Win\"")) {
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, "Terrorists Win.");
	clearStr(szSendBuf, MAXDATASIZE);
	sprintf(szSendBuf, "Wins: (CT %s (T %s", hlCTWins, hlTWins);
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
       }
       if (!strcmp(szTBufP, "\"VIP_Escaped\"")) {
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, "VIP escaped.");
	clearStr(szSendBuf, MAXDATASIZE);
	sprintf(szSendBuf, "Wins: (CT %s (T %s", hlCTWins, hlTWins);
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
       }
       if (!strcmp(szTBufP, "\"VIP_Not_Escaped\"")) {
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, "VIP didn't escape.");
	clearStr(szSendBuf, MAXDATASIZE);
	sprintf(szSendBuf, "Wins: (CT %s (T %s", hlCTWins, hlTWins);
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
       }
       if (!strcmp(szTBufP, "\"VIP_Assassinated\"")) {
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, "VIP assassinated.");
	clearStr(szSendBuf, MAXDATASIZE);
	sprintf(szSendBuf, "Wins: (CT %s (T %s", hlCTWins, hlTWins);
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
       }
       delete [] szTBufP;
      }
      delete [] szTWordP;
     }
    } else if (!strcmp(szActWordP, "Started")) {
     if (wordExists(pkt, 2, ' ')) {
      szTWordP = getWord(pkt, 2, ' ');
      if (!strcmp(szTWordP, "map")) {
       szTBufP = getWord(pkt, 3, ' ');
       clearStr(szSendBuf, MAXDATASIZE);
       sprintf(szSendBuf, "Started map %s.", szTBufP);
       ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
      }
     }
    } else if (pkt[0] == '\"') {
     szTBufP = getWord(pkt, 1, ' ');
     hlFullInfo = getWord(pkt, 1, '\"');
     hlNickname = getHLNick(hlFullInfo);
     if (wordExists(pkt, 2, '\"')) {
      hlDoing = getWord(pkt, 2, '\"');
       
      switch(getHLTeam(hlFullInfo)) {
       case 'S':
        strcpy(hlTeam, "S\0\0");
        break;
       case 'C':
        strcpy(hlTeam, "CT\0");
        break;
       case 'T':
        strcpy(hlTeam, "T\0\0");
        break;
      }
      
      szTWordP = getWord(hlDoing, 1, ' ');
     
      if (!strcmp(szTWordP, "connected,")) {
       szTBufP = getWord(pkt, 3, '\"');
       clearStr(szSendBuf, MAXDATASIZE);
       sprintf(szSendBuf, "%s connected from %s", hlNickname, szTBufP);
       ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
      }
      if (!strcmp(szTWordP, "changed")) {
       szTBufP = getWord(pkt, 3, '\"');
       clearStr(szSendBuf, MAXDATASIZE);
       sprintf(szSendBuf, "%s changed name to %s", hlNickname, szTBufP);
       ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
      }
      if (!strcmp(szTWordP, "committed")) {
       hlKilledWith = getWord(pkt, 3, '\"');
       clearStr(szSendBuf, MAXDATASIZE);
       sprintf(szSendBuf, "%s committed suicide with %s", hlNickname, hlKilledWith);
       ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
      }
      if (!strcmp(szTWordP, "triggered")) {
       szTBufP = getWord(pkt, 3, '\"');
       if (!strcmp(szTBufP, "Planted_The_Bomb")) {
        clearStr(szSendBuf, MAXDATASIZE);
	sprintf(szSendBuf, "%s planted the bomb.", hlNickname);
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
       }
       if (!strcmp(szTBufP, "Dropped_The_Bomb")) {
	clearStr(szSendBuf, MAXDATASIZE);
	sprintf(szSendBuf, "%s dropped the bomb.", hlNickname);
	ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
       }
       delete [] szTBufP;
      }
      if (!strcmp(szTWordP, "disconnected")) {
       clearStr(szSendBuf, MAXDATASIZE);
       sprintf(szSendBuf, "%s disconnected.", hlNickname);
       ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
      }
      if (!strcmp(szTWordP, "entered")) {
       clearStr(szSendBuf, MAXDATASIZE);
       sprintf(szSendBuf, "%s entered the game.", hlNickname);
       ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
      }
      if (!strcmp(szTWordP, "say")) {
       clearStr(szSendBuf, MAXDATASIZE);
       if (wordExists(pkt, 4, '\"')) {
	szTBuf2P = getWord(pkt, 4, '\"');
	if (szTBuf2P != NULL) {
	 if (!strcmp(szTBuf2P, " (dead)")) {
	  strcat(szSendBuf, "*DEAD* ");
	 }
	}
	delete [] szTBuf2P;
       }
       szTBuf2P = getWord(pkt, 3, '\"');
       sprintf(szTWordP, "%s (%s): %s\n", hlNickname, hlTeam, szTBuf2P);
       strcat(szSendBuf, szTWordP);
       ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
       delete [] szTBuf2P;
      }
      if (!strcmp(szTWordP, "joined")) {
       szTBufP = getWord(pkt, 3, '\"');
       clearStr(szSendBuf, MAXDATASIZE);
       sprintf(szSendBuf, "%s joined team %s", hlNickname, szTBufP);
       ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
       delete []szTBufP;
      }
      if (!strcmp(szTWordP, "killed")) {
       hlFullInfo2 = getWord(pkt, 3, '\"');
       clearStr(hlNickname2, MAXDATASIZE);
       hlNickname2 = getHLNick(hlFullInfo2);
       switch(getHLTeam(hlFullInfo2)) {
	case 'C':
	 strcpy(hlTeam2, "CT\0");
	 break;
	case 'T':
	 strcpy(hlTeam2, "T\0\0");
	 break;
       }
       hlKilledWith = getWord(pkt, 5, '\"');
       clearStr(szSendBuf, MAXDATASIZE);
       sprintf(szSendBuf, "%s (%s) killed %s (%s) with %s", hlNickname, hlTeam, hlNickname2, hlTeam2, hlKilledWith);
       ChildIPCSock.SendtoIPC(IPCSOCKNAMEP, szSendBuf);
       delete [] szTBufP;
      }
     }

    } else {
     //Ignore for now
    }
    tv.tv_sec = 1;
   } else {
    FD_SET(ChildIPCSock.iSockfd, &creadfds);
    FD_SET(HLLogSock.iSockfd, &creadfds);
    tv.tv_sec = 1;
   }
  }
// -- /CHILD --
 } else {
// -- PARENT --
  
  FD_ZERO(&readfds);
  
  tv.tv_sec = 5;
  tv.tv_usec = 0;
  
  printf("Creating HL Server Bind\n");
  if (!HLServer.CreateListener(iCfgHLClientPort)) {
   printf("Failed to Bind HL Server socket\n");
   exit(1);
  }

  printf("Challenging RCON\n");
  if (!HLServer.ConnectRcon(szCfgRconPass, szCfgHLServ, iCfgHLPort)) {
   printf("Rcon challenge failed. Rcon functions will not work.\n");
  }
  
  printf("Connecting to IRC Server\n");
  if (!IRCServer.Connect(szCfgIRCServ, iCfgIRCPort, szCfgIRCNick)) {
   printf("Failed to connect to IRC Server %s:%d\n", szCfgIRCServ, iCfgIRCPort);
   ParentIPCSock.SendtoIPC(IPCSOCKNAMEC, "kill");
   exit(1);
  }
  
  FD_SET(IRCServer.GetSockfd(), &readfds);
  FD_SET(ParentIPCSock.iSockfd, &readfds);
  
  while (1) {
   select(ParentIPCSock.iHighestSock+1, &readfds, NULL, NULL, &tv);
 
   if (FD_ISSET(IRCServer.GetSockfd(), &readfds)) {
    if ((numbytes = IRCServer.Recv(szRecvBuf)) == -1) {
     perror("recv");
     exit(1);
    }
    
    looptest++;
    
    if (numbytes == 0) {
     printf("Disconnected from server.\n");
     IRCServer.Connect(szCfgIRCServ, iCfgIRCPort, szCfgIRCNick);
    } else {
    
     szTBufP = szRecvBuf;
     
     while ((szCurrentLineP = strtok(szTBufP, "\n")) != NULL) {

#ifdef DEBUG
      printf("Current Line: %s\n", szCurrentLineP);      
#endif
      
      if ((szActWordP = getWord(szCurrentLineP, 1, ' ')) != 0) {    
       if (!strcmp("PING", szActWordP)) {
#ifdef DEBUG
	printf("Recived PING. Sending PONG\n");
#endif
        szTWordP = getWord(szCurrentLineP, 2, ' ');
        sprintf(szSendBuf, "PONG %s\n", szTWordP);
        IRCServer.Send(szSendBuf);
        clearStr(szSendBuf, MAXDATASIZE);
        delete [] szTWordP;
       }
       delete [] szActWordP;
      }
      
      if ((szActWordP = getWord(szCurrentLineP, 2, ' ')) != 0) {
       if (!strcmp("001", szActWordP)) {
	printf("Connected to IRC Server\n");
        IRCServer.Join(szCfgIRCChan);
       }
       delete [] szActWordP;
      }
 
      if ((szActWordP = getWord(szCurrentLineP, 4, ' ')) != 0) {
       if (!strcmp(":\001VERSION\001", szActWordP)) {
        szTWordP = new char[MAXDATASIZE];
        
        szNickname = getNickname(szCurrentLineP);
        sprintf(szTWordP, "NOTICE %s :\001VERSION HLBOT %s [http://hlbot.erikd.org]\001\n", szNickname, VERSION);
        IRCServer.Send(szTWordP);
        
        delete [] szTWordP;
        delete [] szNickname;
        
       }
       if (!strcmp(":\001PING", szActWordP)) {
        szTWordP = new char[MAXDATASIZE];
        szTBufP = new char[MAXDATASIZE];
        szTBuf2P = new char[MAXDATASIZE];
       
        clearStr(szSendBuf, MAXDATASIZE);
        
        szNickname = getNickname(szCurrentLineP);
        
        szTWordP = getWord(szCurrentLineP, 5, ' ');
        if (wordExists(szCurrentLineP, 6, ' ')) {
         szTBufP = getWord(szCurrentLineP, 6, ' ');
         sprintf(szTBuf2P, "%s %s", szTWordP, szTBufP);
        } else {
         sprintf(szTBuf2P, "%s", szTWordP);
        }
        
        sprintf(szSendBuf, "NOTICE %s :\001PING %s\n", szNickname, szTBuf2P);
        
        IRCServer.Send(szSendBuf);
        
        delete [] szTWordP;
        delete [] szTBufP;
        delete [] szTBuf2P;
        delete [] szNickname;
       }
       if (!strcmp(":version", szActWordP)) {
        szTWordP = new char[MAXDATASIZE];
        sprintf(szTWordP, "HLBot %s [http://hlbot.erikd.org]", VERSION);
        IRCServer.SendPrivMsg(szTWordP);
        delete [] szTWordP;
       }
       if (!strcmp(":help", szActWordP)) {
        szTWordP = new char[MAXDATASIZE];
        sprintf(szTWordP, "HLBot %s Commands", VERSION);
        IRCServer.SendPrivMsg(szTWordP);
        IRCServer.SendPrivMsg("---");
        IRCServer.SendPrivMsg("help - ...");
        IRCServer.SendPrivMsg("version - Displays HLBot version information.");
        IRCServer.SendPrivMsg("status [server] [port] - Displays server status information.");
        IRCServer.SendPrivMsg("players [server] [port] - Displays the current players.");
	IRCServer.SendPrivMsg("!s <text> - Displays <text> on the Half-Life server.");
        delete [] szTWordP;
       }
       if (!strcmp(":!s", szActWordP)) {
	char *szIRCNick;
        szQueryServer = new char[MAXDATASIZE];

	szIRCNick = getNickname(szCurrentLineP);
	strcpy(szQueryServer, szCfgHLServ);
	iHLPort = iCfgHLPort;
        szTBufP = getPhrase(szCurrentLineP, 4, ' ');
	clearStr(szSendBuf, MAXDATASIZE);
	sprintf(szSendBuf, "say (IRC)<%s>%s", szIRCNick, szTBufP);
	HLServer.SendRcon(szSendBuf, szQueryServer, iHLPort);
        
	delete [] szIRCNick;
       }
       if (!strcmp(":status", szActWordP)) {
        szTWordP = new char[MAXDATASIZE];
	clearStr(szSendBuf, MAXDATASIZE);
        szQueryServer = new char[MAXDATASIZE];
        
        if (wordExists(szCurrentLineP, 5, ' ')) {
         szQueryServer = getWord(szCurrentLineP, 5, ' ');
        } else {
         strcpy(szQueryServer, szCfgHLServ);
        }
 
        if (wordExists(szCurrentLineP, 6, ' ')) {
  	 szTBufP = getWord(szCurrentLineP, 6, ' ');
 	 iHLPort = atoi(szTBufP);
        } else {
 	 iHLPort = iCfgHLPort;
        }
        
        szTBuf2P = getNickname(szCurrentLineP);
	
        if ((szTWordP = HLServer.GetServerInfo(szQueryServer, iHLPort)) != 0) {
        
         sprintf(szSendBuf, "Server Name: %s", getWord(szTWordP, 18, '\\'));
         IRCServer.SendPrivMsgUser(szSendBuf, szTBuf2P);
         clearStr(szSendBuf, MAXDATASIZE);
   
         sprintf(szSendBuf, "Server Location: %s", getWord(szTWordP, 4, '\\'));
         IRCServer.SendPrivMsgUser(szSendBuf, szTBuf2P);
         clearStr(szSendBuf, MAXDATASIZE);
   
         sprintf(szSendBuf, "Current Map: %s", getWord(szTWordP, 20, '\\'));
         IRCServer.SendPrivMsgUser(szSendBuf, szTBuf2P);
         clearStr(szSendBuf, MAXDATASIZE);
   
         sprintf(szSendBuf, "Players: %s/%s", getWord(szTWordP, 6, '\\'), getWord(szTWordP, 12, '\\'));
         IRCServer.SendPrivMsgUser(szSendBuf, szTBuf2P);
         clearStr(szSendBuf, MAXDATASIZE);
        } else {
         IRCServer.SendPrivMsgUser("Error connecting to server.", szTBuf2P);
        }
        
	delete [] szTBuf2P;
        delete [] szQueryServer;
        delete [] szTWordP;
       }
       if (!strcmp(":players", szActWordP)) {
        szTBufP = new char[MAXDATASIZE];
        szTBuf2P = new char[MAXDATASIZE];
        szQueryServer = new char[MAXDATASIZE];
  
        if (wordExists(szCurrentLineP, 5, ' ')) {
         szQueryServer = getWord(szCurrentLineP, 5, ' ');
        } else {
         strcpy(szQueryServer, szCfgHLServ);
        }
        
        if (wordExists(szCurrentLineP, 6, ' ')) {
 	 szTBufP = getWord(szCurrentLineP, 6, ' ');
 	 iHLPort = atoi(szTBufP);
        } else {
	 iHLPort = iCfgHLPort;
        }
       
        clearStr(szSendBuf, MAXDATASIZE);
  
        if ((szTBufP = HLServer.GetPlayers(szQueryServer, iHLPort)) != 0) {
        
	 szNickname = getNickname(szCurrentLineP);
  
	 scanner_init(szTBufP);
         
         iTLoop = 1;
         iTNum2 = 0;
         
         if (szTBufP[0] == '\0') {
	  sprintf(szTBuf2P, "PRIVMSG %s :There are no players on the server.\n", szNickname);
	  IRCServer.Send(szTBuf2P);
	 }
	 while ((szTWordP = getWord(szTBufP, iTLoop, '\\')) != 0) {
          if ((iTLoop % 2) == 1) {
           iTNum = atoi(szTWordP);
          } else if((iTLoop % 2) == 0) {
   	   sprintf(szTBuf2P, "PRIVMSG %s :%d - %s\n", szNickname, iTNum, szTWordP);
 	   strcat(szSendBuf, szTBuf2P);
 	   iTNum2++;
	   if (iTNum2 == 4) {
	    IRCServer.Send(szSendBuf);
 	    clearStr(szSendBuf, MAXDATASIZE);
	    iTNum2 = 0;
	   }
	   clearStr(szTBuf2P, MAXDATASIZE);
          }
         
          iTLoop++;
         }
	 IRCServer.Send(szSendBuf);
	 clearStr(szSendBuf, MAXDATASIZE);
	 iTNum2 = 0;
        } else {
         IRCServer.SendPrivMsg("Error connecting to server.");
        }
        delete [] szQueryServer;
        delete [] szTWordP;
        delete []szTBufP;
       }
      }
  
      delete [] szActWordP;
      szTBufP = NULL;
     }
    }
    tv.tv_sec = 5; 
    FD_SET(IRCServer.GetSockfd(), &readfds);
    FD_SET(ParentIPCSock.iSockfd, &readfds);
    clearStr(szRecvBuf, MAXDATASIZE);
   } else if (FD_ISSET(ParentIPCSock.iSockfd, &readfds)) {
    clearStr(szRecvBuf, MAXDATASIZE);
#ifdef DEBUG
    printf("PARENT: Child sent something.\n");
#endif
    ParentIPCSock.RecvfromIPC(szRecvBuf);
#ifdef DEBUG
    printf("PARENT: Child sent \"%s\"\n", szRecvBuf);
#endif
    IRCServer.SendPrivMsg(szRecvBuf);
#ifdef DEBUG
    printf("PARENT: Nothing big.\n");
#endif
   } else {
    tv.tv_sec = 5;
    FD_SET(IRCServer.GetSockfd(), &readfds);
    FD_SET(ParentIPCSock.iSockfd, &readfds);
   } //END FD_ISSET()
  } //END while(1)
 } //END fork()
}

bool readConfig() {
 FILE *configFile;
 int iCurrentFileLine;
 char *szConfigLine, *szConfigLineP, *szConfigWord;

 szConfigLine = new char[MAXDATASIZE];
 clearStr(szConfigLine, MAXDATASIZE);

 if ((configFile = fopen(CONFIGLOC, "r")) <= 0) {
  printf("Error Opening Config File\n");
  perror("fopen");
  return false;
 }
 
 iCurrentFileLine = 1;

 while ((fgets(szConfigLine, MAXDATASIZE-1, configFile)) != NULL) {
  szConfigLineP = trim(szConfigLine);
  scanner_init(szConfigLineP);
  szConfigWord = scanner_token(' ');
  if (!strcmp("HLServer", szConfigWord)) {
   delete [] szConfigWord;
   szConfigWord = scanner_token(' ');
   clearStr(szCfgHLServ, MAXDATASIZE);
   strcpy(szCfgHLServ, szConfigWord);
   if (szCfgHLServ[strlen(szCfgHLServ)-1] == '\n') {
    szCfgHLServ[strlen(szCfgHLServ)-1] = '\0';
   }
  } else if (!strcmp("IRCServer", szConfigWord)) {
   delete [] szConfigWord;
   szConfigWord = scanner_token(' ');
   clearStr(szCfgIRCServ, MAXDATASIZE);
   strcpy(szCfgIRCServ, szConfigWord);
   if (szCfgIRCServ[strlen(szCfgIRCServ)-1] == '\n') {
    szCfgIRCServ[strlen(szCfgIRCServ)-1] = '\0';
   }
  } else if (!strcmp("IRCChannel", szConfigWord)) {
   delete [] szConfigWord;
   szConfigWord = scanner_token(' ');
   clearStr(szCfgIRCChan, MAXDATASIZE);
   strcpy(szCfgIRCChan, szConfigWord);
   if (szCfgIRCChan[strlen(szCfgIRCChan)-1] == '\n') {
    szCfgIRCChan[strlen(szCfgIRCChan)-1] = '\0';
   }
  } else if (!strcmp("IRCNickname", szConfigWord)) {
   delete [] szConfigWord;
   szConfigWord = scanner_token(' ');
   clearStr(szCfgIRCNick, MAXDATASIZE);
   strcpy(szCfgIRCNick, szConfigWord);
   if (szCfgIRCNick[strlen(szCfgIRCNick)-1] == '\n') {
    szCfgIRCNick[strlen(szCfgIRCNick)-1] = '\0';
   }
  } else if (!strcmp("RCONPassword", szConfigWord)) {
   delete [] szConfigWord;
   szConfigWord = scanner_token(' ');
   clearStr(szCfgRconPass, MAXDATASIZE);
   strcpy(szCfgRconPass, szConfigWord);
   if (szCfgRconPass[strlen(szCfgRconPass)-1] == '\n') {
    szCfgRconPass[strlen(szCfgRconPass)-1] = '\0';
   }
  } else if (!strcmp("HLPort", szConfigWord)) {
   delete [] szConfigWord;
   szConfigWord = scanner_token(' ');
   iCfgHLPort = atoi(szConfigWord);
  } else if (!strcmp("HLClientPort", szConfigWord)) {
   delete [] szConfigWord;
   szConfigWord = scanner_token(' ');
   iCfgHLClientPort = atoi(szConfigWord);
  } else if (!strcmp("IRCPort", szConfigWord)) {
   delete [] szConfigWord;
   szConfigWord = scanner_token(' ');
   iCfgIRCPort = atoi(szConfigWord);
  } else if (!strcmp("LogPort", szConfigWord)) {
   delete [] szConfigWord;
   szConfigWord = scanner_token(' ');
   iCfgLogPort = atoi(szConfigWord);
  } else if (!strcmp("Advertise", szConfigWord)) {
   delete [] szConfigWord;
   szConfigWord = scanner_token(' ');
   iCfgAdvertise = atoi(szConfigWord);
  } else if (szConfigWord[0] == '#' || szConfigWord[0] == '\n') {
   delete [] szConfigWord;
  } else {
   printf("Config parse error on line %d:\n", iCurrentFileLine);
   printf("%s", szConfigLine);
   return false;
  }
  if (feof(configFile))
   break;
  iCurrentFileLine++;
 }

 return true;
}

void catchAlrm(int s) {
 char *szAlarmString;
 szAlarmString = new char[MAXDATASIZE];
 sprintf(szAlarmString, "say Monitored by HLBot %s [http://hlbot.erikd.org]", VERSION);
 HLServer.SendRcon(szAlarmString, szCfgHLServ, iCfgHLPort);
 alarm(iCfgAdvertise*60);
 printf("Alarm!\n");
}

void catchInt(int s) {
 IRCServer.Quit("HLBot - http://hlbot.erikd.org");
 exit(0);
}
