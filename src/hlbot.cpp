// HLBot
//
// hlbot.cpp - Main HLBot source
//
// $Id: hlbot.cpp,v 1.9 2002/07/15 04:32:27 yodatoad Exp $

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

#include <string>
#include <vector>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <dlfcn.h>

#ifndef RTLD_NOW
#  define RTLD_NOW 2
#endif

#include "config.h"
#include "hlbot.h"
#include "sockets.h"
#include "strfuncs.h"
#include "hlfuncs.h"
#include "ircfuncs.h"

// Configuration Variables

string sConfigLoc, sCfgHLServ, sCfgRCONPass, sCfgIRCServ;
string sCfgIRCChan, sCfgIRCNick, sCfgModule, sCfgModuleDirectory;
string sCfgCommandPrefix;
int iCfgHLPort, iCfgHLClientPort, iCfgLogPort, iCfgIRCPort;
int iCfgTimeout, iCfgSendDelay, iCfgAdvertise;

// Socket Variables

int iSockIPCP, iSockIPCC, iSockHLClient, iSockIRC, iHighestSock;
int iSockHLLog, iSockRcon;

void dispVec(const vector<string>& vec) {
 unsigned int i=0;
 cout << "Vector:\n";
 for (i = 0; i < vec.size(); i++) {
  cout << i << ": " << vec[i] << "\n";
 }
}


int main(int argc, char *argv[]) {
 int iLoopTemp, iRunOptions = 0; 
 pid_t pid;
 
 signal(SIGINT, catchInt);
 
 for (iLoopTemp = 1; iLoopTemp < argc; iLoopTemp++) {
  if (!strcmp(argv[iLoopTemp], "--config") || !strcmp(argv[iLoopTemp], "-c")) {
   if (iLoopTemp + 1 > argc-1) {
    cout << "Error in command line options.\n\n";
    displayUsage();
    sexit(1);
   } else {
    sConfigLoc = argv[iLoopTemp+1];
    iLoopTemp += 1;
   }
  } else if (!strcmp(argv[iLoopTemp], "--no-background") || !strcmp(argv[iLoopTemp], "-n")) {
   iRunOptions++;
  } else if (!strcmp(argv[iLoopTemp], "--help") || !strcmp(argv[iLoopTemp], "-h")) {
   displayUsage();
   sexit(0);
  } else if (!strcmp(argv[iLoopTemp], "--module") || !strcmp(argv[iLoopTemp], "-o")) {
   if (iLoopTemp + 1 > argc-1) {
    cout << "Error in command line options.\n\n";
    displayUsage();
    sexit(1);
   } else {
    sCfgModule = argv[iLoopTemp+1];
    iLoopTemp += 1;
   }
  } else if (!strcmp(argv[iLoopTemp], "--version") || !strcmp(argv[iLoopTemp], "-v")) {
   cout << "HLBot " << HLBOTVERSION << " Copyright (C) 2002 Erik Davidson\n";
   sexit(0);
  } else {
   cout << "Error in command line options.\n\n";
   displayUsage();
   sexit(1); 
  }
 }

 iCfgHLPort = 27015;
 iCfgHLClientPort = 0;
 iCfgLogPort = 27008;
 iCfgIRCPort = 6667;
 iCfgTimeout = 5;
 iCfgAdvertise = 0;
 readConfig(sConfigLoc); 
 
 if (sCfgHLServ.empty()) {
  cout << "You must specify a Half-Life server in the configuration\n";
  sexit(1);
 } else if (sCfgIRCServ.empty()) {
  cout << "You must specify an IRC server in the configuration\n";
  sexit(1);
 } else if (sCfgIRCChan.empty()) {
  cout << "You must specify an IRC channel in the configuration\n";
  sexit(1);
 } else if (sCfgModule.empty()) {
  cout << "You must specify a module to load.\n";
  sexit(1);
 } else if (sCfgIRCNick.empty()) {
  cout << "You must an IRC nickname in the configuration\n";
  sexit(1);
 }
 if (sCfgModuleDirectory.empty()) {
  sCfgModuleDirectory = "./modules";
 }

 cout << "\nHLBot " << HLBOTVERSION << ", Copyright (C) 2002 Erik Davidson\n";
 cout << "HLBot comes with ABSOLUTELY NO WARRANTY; for details type\n";
 cout << "'show w'.  This is free software, and you are welcome to\n";
 cout << "redistribute it under certain conditions; type 'show c'\n";
 cout << "for details.\n\n";
 
 cout << "Connecting to " << sCfgIRCServ << ":" << iCfgIRCPort << "... ";
 if ((iSockIRC = sockConnect(sCfgIRCServ.c_str(), iCfgIRCPort)) < 0) {
  cout << "\nError connecting to IRC server: " << sCfgIRCServ << ":" << iCfgIRCPort << "\n";
  sexit(1);
 }
 if (iSockIRC > iHighestSock)
  iHighestSock = iSockIRC;
 if (!ircConnect(iSockIRC, sCfgIRCNick.c_str())) {
  cout << "\nError conneting to IRC server: "  << sCfgIRCServ << ":" << iCfgIRCPort << "\n";
  sexit(1);
 }
 cout << "done.\n";

 if (iRunOptions & 0x01 == 0x01) {
  forkParent();
 } else {
  if ((pid = fork()) == -1) {
   perror("fork");
   sexit(1);
  } else if (pid == 0) {
   forkParent();
  } else {
   cout << "Forked HLBot in the background. PID " << pid << "\n\n";
  }
 }

 return 0;
}

// -----------------------------------
//            Parent Process
// -----------------------------------

int forkParent() {
 char *szRecvBuf, *szTBufP, *szHLRecvBuf, *pkt;
 int numbytes, iLoopTemp, iAddrLen;
 int iSendDelayCounter = 0;
 int iTPort = 0;
 int iRunOptions = 0;
 pid_t childPID;
 string sRecvBuf, sSendBuf, sCurrentLine, sTServer, sTString;
 string sRconNumber, sCheckWord;
 vector<string> tokens, lTokens, tTokens, tTokens2, qIRC, qHLCommands, qHLServerHost;
 fd_set readfds, readfds_orig, treadfds, treadfds_orig;
 struct hostent *he;
 struct sockaddr_in saHLClient, saTConn, saRcon;
 struct sockaddr_un saIPCP, saTIPC;
 struct timeval tv, ttv;

 if ((iSockHLClient = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
  perror("socket");
  cout << "HLBot: Could not create Half-Life server communications socket.\n";
  sexit(1);
 }

 saHLClient.sin_family = AF_INET;
 saHLClient.sin_port = htons(iCfgHLClientPort);
 saHLClient.sin_addr.s_addr = INADDR_ANY;
 memset(&(saHLClient.sin_zero), '\0', 8);

 if (bind(iSockHLClient, (struct sockaddr *)&saHLClient, sizeof(struct sockaddr)) == -1) {
  perror("bind");
  cout << "HLBot: Could not bind Half-Life server communications port.\n";
  sexit(1);
 }
 
 if (iSockHLClient > iHighestSock)
  iHighestSock = iSockHLClient;
 
 if ((iSockIPCP = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
  perror("socket");
  cout << "HLBot: Could not create parent IPC socket.\n";
  sexit(1);
 }

 saIPCP.sun_family = AF_UNIX;
 strcpy(saIPCP.sun_path, IPCSOCKNAMEP);
 unlink(IPCSOCKNAMEP);

 iAddrLen = strlen(saIPCP.sun_path) + sizeof(saIPCP.sun_family);
 if (bind(iSockIPCP, (struct sockaddr *)&saIPCP, iAddrLen) == -1) {
  perror("bind");
  cout << "HLBot: Could not bind parent IPC socket.\n";
  sexit(1);
 }
 
 if (iSockIPCP > iHighestSock)
  iHighestSock = iSockIPCP;
 
 sSendBuf = "\377\377\377\377challenge rcon";

 if ((he = gethostbyname(sCfgHLServ.c_str())) == NULL) {
  perror("gethostbyname");
  cout << "HLBot: Could not get hostname of Half-Life server. Rcon will not function.\n";
 } else {
  saRcon.sin_family = AF_INET;
  saRcon.sin_port = htons(iCfgHLPort);
  saRcon.sin_addr = *((struct in_addr *)he->h_addr);
  memset(&(saRcon.sin_zero), '\0', 8);
  if ((numbytes = sendto(iSockHLClient, sSendBuf.c_str(), strlen(sSendBuf.c_str()), 0, (struct sockaddr *)&saRcon, sizeof(struct sockaddr))) == -1) {
   perror("sendto");
   cout << "HLBot: Could not send RCON challenge.  RCON functions will not work.\n";
  }
 } 
 
 FD_ZERO(&treadfds_orig);
 FD_SET(iSockHLClient, &treadfds_orig);

 ttv.tv_sec = iCfgTimeout;
 ttv.tv_usec = 0;

 while (1) {
  bcopy(&treadfds_orig, &treadfds, sizeof(&treadfds_orig));
  select(iHighestSock+1, &treadfds, NULL, NULL, &ttv);
  if (FD_ISSET(iSockHLClient, &treadfds)) {
   iAddrLen = sizeof(struct sockaddr);
   szRecvBuf = new char[MAXRECVSIZE];
   memset(szRecvBuf, '\0', MAXRECVSIZE);
   if ((numbytes = recvfrom(iSockHLClient, szRecvBuf, MAXRECVSIZE-1, 0, (struct sockaddr *)&saTConn, (socklen_t *)&iAddrLen)) == -1) {
    perror("recvfrom");
    cout << "HLBot: RCON challenge failed. RCON functions will not work.\n";
   }
   
   sTString = szRecvBuf+4;
   tokenize(sTString, tokens);
   sRconNumber = tokens[2];
   strReplace(sRconNumber, "\n", "");
   strReplace(sRconNumber, "\r", "");
   delete [] szRecvBuf;
   break;
  } else {
   cout << "HLBot: RCON challenge timed out. RCON functions will not work.\n";
   break;
   //perror
  }
 }

 // Ok, all the sockets are bound and working, fork the child.
 if ((childPID = fork()) == -1) {
  perror("fork");
  cout << "HLBot: Could not fork child process. Quitting.\n";
  sexit(1);
 } else if (childPID == 0) {
  forkChild();
 } else {
  cout << "HLBot: Forked Child. PID: " << childPID << "\n";
 }
 
 tv.tv_sec = 0;
 tv.tv_usec = 100000;

 FD_ZERO(&readfds_orig);
 FD_SET(iSockIRC, &readfds_orig);
 FD_SET(iSockIPCP, &readfds_orig);
 FD_SET(iSockHLClient, &readfds_orig);
 
 while (1) {
  bcopy(&readfds_orig, &readfds, sizeof(readfds_orig));
  select(iHighestSock+1, &readfds, NULL, NULL, &tv);
  if (FD_ISSET(iSockHLClient, &readfds)) {
   szHLRecvBuf = new char[MAXRECVSIZE];
   memset(szHLRecvBuf, '\0', MAXRECVSIZE);

   iAddrLen = sizeof(struct sockaddr);
   if ((numbytes = recvfrom(iSockHLClient, szHLRecvBuf, MAXRECVSIZE-1, 0, (struct sockaddr *)&saTConn, (socklen_t *)&iAddrLen)) == -1) {
    perror("recvfrom");
   }

   pkt = szHLRecvBuf;

   if (pkt[4] == 'D') {
    if (qHLCommands.size() > 0) {
     if (qHLCommands[0].c_str()[0] == 'D') {
      
      szTBufP = hlParsePlayers(szHLRecvBuf);
      
      if (szTBufP == NULL) {
       sSendBuf = "PRIVMSG ";
       sSendBuf += sCfgIRCChan;
       sSendBuf += " :No players on the server.\n";
       addQueue(sSendBuf, qIRC);
      } else {
       sTString = szTBufP;
       delete [] szTBufP;
      
       tTokens.clear();
       tokenize(sTString, tTokens, "\\");
       for (iLoopTemp = 0; (unsigned int)iLoopTemp < tTokens.size()-1; iLoopTemp += 2) {
	sSendBuf = "PRIVMSG ";
	sSendBuf += qHLCommands[0].c_str()+1;
	sSendBuf += " :";
	sSendBuf += tTokens[iLoopTemp];
	sSendBuf += " - ";
	sSendBuf += tTokens[iLoopTemp+1];
	sSendBuf += "\n";
	addQueue(sSendBuf, qIRC);
       }
      }
     }
     delQueueFirst(qHLCommands);
    }
   } else if (pkt[4] == 'i' && pkt[5] == 'n') {
    if (qHLCommands.size() > 0) {
     if (qHLCommands[0].c_str()[0] == 'i') {
     
      sTString = pkt+24;
      tTokens.clear();
      tokenize(sTString, tTokens, "\\");

      sSendBuf = "PRIVMSG ";
      sSendBuf += qHLCommands[0].c_str()+1;
      sSendBuf += " :Server Name: ";
      sSendBuf += tTokens[17];
      sSendBuf += "\n";
      addQueue(sSendBuf, qIRC);

      sSendBuf = "PRIVMSG ";
      sSendBuf += qHLCommands[0].c_str()+1;
      sSendBuf += " :Server Location: ";
      sSendBuf += qHLServerHost[0];
      sSendBuf += "\n";
      addQueue(sSendBuf, qIRC);

      sSendBuf = "PRIVMSG ";
      sSendBuf += qHLCommands[0].c_str()+1;
      sSendBuf += " :Mod: ";
      sSendBuf += tTokens[15];
      sSendBuf += "\n";
      addQueue(sSendBuf, qIRC);

      sSendBuf = "PRIVMSG ";
      sSendBuf += qHLCommands[0].c_str()+1;
      sSendBuf += " :Current Map: ";
      sSendBuf += tTokens[19];
      sSendBuf += "\n";
      addQueue(sSendBuf, qIRC);

      sSendBuf = "PRIVMSG ";
      sSendBuf += qHLCommands[0].c_str()+1;
      sSendBuf += " :Players: ";
      sSendBuf += tTokens[5];
      sSendBuf += "/";
      sSendBuf += tTokens[11];
      sSendBuf += "\n";
      addQueue(sSendBuf, qIRC);
     
     }
     delQueueFirst(qHLServerHost);
     delQueueFirst(qHLCommands);
    }
   } else if (pkt[4] == 'l') {
    if (pkt[5] == 'B' && pkt[6] == 'a' && pkt[7] == 'd') {
     //perror bad rcon pass
    }
   }

   delete [] szHLRecvBuf;
  
  }
  if (FD_ISSET(iSockIPCP, &readfds)) {
  
   szRecvBuf = new char[MAXRECVSIZE];
   memset(szRecvBuf, '\0', MAXRECVSIZE);
   iAddrLen = sizeof(struct sockaddr);
   if ((numbytes = recvfrom(iSockIPCP, szRecvBuf, MAXRECVSIZE-1, 0, (struct sockaddr *)&saIPCP, (socklen_t *)&iAddrLen)) == -1) {
    perror("recvfrom");
   }

   sTString = szRecvBuf;
   delete [] szRecvBuf;
   
   tokens.clear();
   tokenize(sTString, tokens, "\001");

   if (tokens[0] == "SAY") {
    sSendBuf = "PRIVMSG ";
    sSendBuf += sCfgIRCChan;
    sSendBuf += " :";
    sSendBuf += tokens[1];
    sSendBuf += "\n";
    
    if ((numbytes = send(iSockIRC, sSendBuf.c_str(), strlen(sSendBuf.c_str()), 0)) == -1)
     perror("send");
   } else if (tokens[0] == "MODINFOREPLY") {
    sSendBuf = "PRIVMSG ";
    sSendBuf += sCfgIRCChan;
    sSendBuf += " :HLBot ";
    sSendBuf += tokens[3];
    sSendBuf += " module ";
    sSendBuf += tokens[1];
    sSendBuf += " by ";
    sSendBuf += tokens[2];
    sSendBuf += "\n";
    addQueue(sSendBuf, qIRC);
   } else if (tokens[0] == "DIE") {
    sexit(0);
   }
  }
  if (FD_ISSET(iSockIRC, &readfds)) {
   szRecvBuf = new char[MAXRECVSIZE];
   memset(szRecvBuf, '\0', MAXRECVSIZE);
   if ((numbytes = recv(iSockIRC, szRecvBuf, MAXRECVSIZE-1, 0)) == -1) {
    delete [] szRecvBuf;
    close(iSockIRC);
    sexit(1);
   }

   if (numbytes == 0) {
    close(iSockIRC);
#ifdef DEBUG
    //perror
    cout << "Disconnected.\n";
#endif
    if ((iSockIRC = sockConnect(sCfgIRCServ.c_str(), iCfgIRCPort)) < 0) {
     cout << "\nError reconnecting to IRC server: " << sCfgIRCServ << ":" << iCfgIRCPort << "\n";
     sexit(1);
    }
    if (iSockIRC > iHighestSock)
     iHighestSock = iSockIRC;
    if (!ircConnect(iSockIRC, sCfgIRCNick.c_str())) {
     cout << "\nError reconneting to IRC server: "  << sCfgIRCServ << ":" << iCfgIRCPort << "\n";
     sexit(1);
    }
   } else {
    
    sRecvBuf = "";
    sRecvBuf = szRecvBuf;
    delete [] szRecvBuf;
    strReplace(sRecvBuf, "\r", "");
 
    tokens.clear();
    tokenize(sRecvBuf, tokens, "\n");
    iLoopTemp = 0;
    while ((unsigned int)iLoopTemp < tokens.size()) {
     sCurrentLine = tokens[iLoopTemp];
     lTokens.clear();
     tokenize(sCurrentLine, lTokens);
#ifdef DEBUG
     cout << "Line: " << sCurrentLine << "\n";
#endif
     sCheckWord = "";
     
     if (lTokens[0] == "PING") {
      sSendBuf = "";
      sSendBuf += "PONG ";
      sSendBuf += lTokens[1];
      sSendBuf += "\n";
      if ((numbytes = send(iSockIRC, (char *)sSendBuf.c_str(), strlen(sSendBuf.c_str()), 0)) == -1)
       perror("send");
     }
     if (lTokens[1] == "001") {
      ircJoin(iSockIRC, sCfgIRCChan.c_str());
     }
     if (lTokens.size() > 3) {
      sCheckWord = ":";
      sCheckWord += sCfgCommandPrefix;
     
      if (lTokens[3].substr(0, sCheckWord.length()) == sCheckWord) {
       sCheckWord = lTokens[3].substr(sCfgCommandPrefix.length()+1, lTokens[3].length()-sCfgCommandPrefix.length()+1);
      } else {
       sCheckWord = "";
      }
      if (lTokens[3] == ":\001VERSION\001") {
       tTokens.clear();
       tokenize(lTokens[0], tTokens, "!");
       sSendBuf = "";
       sSendBuf += "NOTICE ";
       sSendBuf += tTokens[0].c_str()+1;
       sSendBuf += " :\001VERSION HLBot ";
       sSendBuf += HLBOTVERSION;
       sSendBuf += " [http://hlbot.erikd.org]\001\n";
       if ((numbytes = send(iSockIRC, (char *)sSendBuf.c_str(), strlen(sSendBuf.c_str()), 0)) == -1)
        perror("send");
      }
      if (lTokens[3] == ":\001PING") {
       tTokens.clear();
       tokenize(lTokens[0], tTokens, "!");
       sSendBuf = "";
       sSendBuf += "NOTICE ";
       sSendBuf += tTokens[0].c_str()+1;
       sSendBuf += " :\001PING ";
       sSendBuf += lTokens[4];
       sSendBuf += " ";
       sSendBuf += lTokens[5];
       sSendBuf += "\n";
       if ((numbytes = send(iSockIRC, (char *)sSendBuf.c_str(), strlen(sSendBuf.c_str()), 0)) == -1)
        perror("send");
      }
      if (sCheckWord == "help") {
       sSendBuf = "PRIVMSG ";
       sSendBuf += sCfgIRCChan;
       sSendBuf += " :HLBot Help\n";
       addQueue(sSendBuf, qIRC);
 
       sSendBuf = "PRIVMSG ";
       sSendBuf += sCfgIRCChan;
       sSendBuf += " :----------\n";
       addQueue(sSendBuf, qIRC);
 
       sSendBuf = "PRIVMSG ";
       sSendBuf += sCfgIRCChan;
       sSendBuf += " :";
       sSendBuf += sCfgCommandPrefix;
       sSendBuf += "help - ...\n";
       addQueue(sSendBuf, qIRC);
 
       sSendBuf = "PRIVMSG ";
       sSendBuf += sCfgIRCChan;
       sSendBuf += " :";
       sSendBuf += sCfgCommandPrefix;
       sSendBuf += "version - Displays HLBot version information.\n";
       addQueue(sSendBuf, qIRC);
 
       sSendBuf = "PRIVMSG ";
       sSendBuf += sCfgIRCChan;
       sSendBuf += " :";
       sSendBuf += sCfgCommandPrefix;
       sSendBuf += "modinfo - Display info on the loaded HLBot module.\n";
       addQueue(sSendBuf, qIRC);
       
       sSendBuf = "PRIVMSG ";
       sSendBuf += sCfgIRCChan;
       sSendBuf += " :";
       sSendBuf += sCfgCommandPrefix;
       sSendBuf += "status [server] [port] - Display the HL server status.\n";
       addQueue(sSendBuf, qIRC);
 
       sSendBuf = "PRIVMSG ";
       sSendBuf += sCfgIRCChan;
       sSendBuf += " :";
       sSendBuf += sCfgCommandPrefix;
       sSendBuf += "players [server] [port] - Display the players on a HL server.\n";
       addQueue(sSendBuf, qIRC);
      
       sSendBuf = "PRIVMSG ";
       sSendBuf += sCfgIRCChan;
       sSendBuf += " :";
       sSendBuf += sCfgCommandPrefix;
       sSendBuf += "s <text> - Display <text> on the HL server.\n";
       addQueue(sSendBuf, qIRC);
 
      }
      if (sCheckWord == "s") {
       tTokens.clear();
       tokenize(lTokens[0], tTokens, "!");
       
       sSendBuf = "\377\377\377\377rcon ";
       sSendBuf += sRconNumber;
       sSendBuf += " \"";
       sSendBuf += sCfgRCONPass;
       sSendBuf += "\" say (IRC)<";
       sSendBuf += tTokens[0].c_str()+1;
       sSendBuf += "> ";
       for(iLoopTemp = 4; (unsigned int)iLoopTemp < lTokens.size(); iLoopTemp++) {
        sSendBuf += lTokens[iLoopTemp];
        sSendBuf += " ";
       }
      
       if ((he = gethostbyname(sCfgHLServ.c_str())) == NULL) {
        perror("gethostbyname");
       } else {
 
        saRcon.sin_family = AF_INET;
        saRcon.sin_port = htons(iCfgHLPort);
        saRcon.sin_addr = *((struct in_addr *)he->h_addr);
        memset(&(saRcon.sin_zero), '\0', 8);
        
        if ((numbytes = sendto(iSockHLClient, sSendBuf.c_str(), strlen(sSendBuf.c_str()), 0, (struct sockaddr *)&saRcon, sizeof(struct sockaddr))) == -1) {
         perror("sendto");
        }
       }
      }
      if (sCheckWord == "players") {
       sSendBuf = "\377\377\377\377players";

       iTPort = 0;
       sTServer = "";

       switch (lTokens.size()) {
        case 4:
 	 iTPort = iCfgHLPort;
 	 sTServer = sCfgHLServ;
 	 iRunOptions = 2;
 	 break;
        case 5:
 	 iTPort = iCfgHLPort;
 	 sTServer = lTokens[4];
 	 iRunOptions = 2;
 	 break;
        case 6:
 	 iTPort = atoi(lTokens[5].c_str());
 	 sTServer = lTokens[4];
 	 iRunOptions = 2;
 	 break;
       }
     
       if ((he = gethostbyname(sTServer.c_str())) == NULL) {
        perror("gethostbyname");
        cout << "Error Connecting\n";
       } else {
 
        saTConn.sin_family = AF_INET;
        saTConn.sin_port = htons(iTPort);
        saTConn.sin_addr = *((struct in_addr *)he->h_addr);
        memset(&(saTConn.sin_zero), '\0', 8);
        
        if ((numbytes == sendto(iSockHLClient, sSendBuf.c_str(), strlen(sSendBuf.c_str()), 0, (struct sockaddr *)&saTConn, sizeof(struct sockaddr))) == -1) {
 	 perror("sendto");
         cout << "Error Connecting\n";
        }
      
        tTokens.clear();
        sTString = "D";
        tokenize(lTokens[0], tTokens, "!");
        sTString += tTokens[0].c_str()+1;
        addQueue(sTString, qHLCommands);
      
       }
      }
      if (sCheckWord == "status") {
       sSendBuf = "\377\377\377\377infostring";
 
       iTPort = 0;
       sTServer = "";
 
       switch (lTokens.size()) {
        case 4:
         iTPort = iCfgHLPort;
         sTServer = sCfgHLServ;
         iRunOptions = 2;
         break;
        case 5:
         iTPort = iCfgHLPort;
         sTServer = lTokens[4];
         iRunOptions = 2;
         break;
        case 6:
         iTPort = atoi(lTokens[5].c_str());
         sTServer = lTokens[4];
         iRunOptions = 2;
         break;
       }
 
       if ((he = gethostbyname(sTServer.c_str())) == NULL) {
        perror("gethostbyname");
        cout << "Error Connecting\n";
       } else {
 
        saTConn.sin_family = AF_INET;
        saTConn.sin_port = htons(iTPort);
        saTConn.sin_addr = *((struct in_addr *)he->h_addr);
        memset(&(saTConn.sin_zero), '\0', 8);
 
        if ((numbytes == sendto(iSockHLClient, sSendBuf.c_str(), strlen(sSendBuf.c_str()), 0, (struct sockaddr *)&saTConn, sizeof(struct sockaddr))) == -1) {
         perror("sendto");
         cout << "Error Connecting\n";
        }
        
        tTokens.clear();
        sTString = "i";
        tokenize(lTokens[0], tTokens, "!");
        sTString += tTokens[0].c_str()+1;
        addQueue(sTString, qHLCommands);
        addQueue(sTServer, qHLServerHost);
       }
      }
      if (sCheckWord == "version") {
       sSendBuf = "PRIVMSG ";
       sSendBuf += sCfgIRCChan;
       sSendBuf += " :HLBot ";
       sSendBuf += HLBOTVERSION;
       sSendBuf += " [ http://hlbot.erikd.org ]\n";
       if ((numbytes = send(iSockIRC, (char *)sSendBuf.c_str(), strlen(sSendBuf.c_str()), 0)) == -1)
        perror("send");
      }
      if (sCheckWord == "modinfo") {
       sSendBuf = "\001MODINFO\001";
       saTIPC.sun_family = AF_UNIX;
       strcpy(saTIPC.sun_path, IPCSOCKNAMEC);
 
       if ((numbytes = sendto(iSockIPCP, sSendBuf.c_str(), strlen(sSendBuf.c_str()), 0, (struct sockaddr *)&saTIPC, sizeof(struct sockaddr))) == -1) {
        perror("sendto");
      
       }
      }
      if (lTokens[3] == ":die") {
       sexit(0);
      }
     }
     iLoopTemp++;
    }
   } // End disconnected if statement
  } else {
   tv.tv_sec = 0;
   tv.tv_usec = 10000;
   if (iSendDelayCounter == iCfgSendDelay) {
    if (qIRC.size() > 0) {
     if ((numbytes = send(iSockIRC, (char *)qIRC[0].c_str(), strlen(qIRC[0].c_str()), 0)) == -1)
      perror("send");
     delQueueFirst(qIRC);
    } 
     iSendDelayCounter = 0;
   } else {
    iSendDelayCounter++;
   }
   
   //cout << "Something wrong here\n";
  } // end iSockIRC ISSET
 }
 return 0;
}


// -----------------------------------
//            Child  Process
// ----------------------------------- 

int forkChild() {
 int sockLen, numbytes;
 string sSendBuf, sModulePath, sTString, sHLLine;
 vector<string> tokens;
 fd_set readfds, readfds_orig;
 char *szRecvBuf, *szTBufP;
 struct timeval tv;
 struct sockaddr_in saHLLog;
 struct sockaddr_un saIPCC, saTIPC;
 struct stat tStat;

 if ((iSockIPCC = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
  perror("socket");
  cout << "HLBot: Could not create child IPC socket.\n";
  sexit(1);
 }

 saIPCC.sun_family = AF_UNIX;
 strcpy(saIPCC.sun_path, IPCSOCKNAMEC);
 unlink(IPCSOCKNAMEC);

 sockLen = strlen(saIPCC.sun_path) + sizeof(saIPCC.sun_family);
 if (bind(iSockIPCC, (struct sockaddr *)&saIPCC, sockLen) == -1) {
  perror("bind");
  cout << "HLBot: Could not bind child IPC socket.\n";
  sexit(1);
 }

 if (iSockIPCC > iHighestSock)
  iHighestSock = iSockIPCC;
  
 if ((iSockHLLog = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
  perror("socket");
  cout << "HLBot: Could not create Half-Life log socket.\n";
  sexit(1);
 }

 saHLLog.sin_family = AF_INET;
 saHLLog.sin_port = htons(iCfgLogPort);
 saHLLog.sin_addr.s_addr = INADDR_ANY;
 memset(&(saHLLog.sin_zero), '\0', 8);

 if (bind(iSockHLLog, (struct sockaddr *)&saHLLog, sizeof(struct sockaddr)) == -1) {
  perror("bind");
  cout << "HLBot: Could not bind Half-Life log socket.\n";
  sexit(1);
 }

 if (iSockHLLog > iHighestSock)
  iHighestSock = iSockHLLog;
 
 // Checking to make sure the modules directory is right
 if (stat(sCfgModuleDirectory.c_str(), &tStat) == -1) {
  perror("stat");
  cout << "Failed to stat modules dir: " << sCfgModuleDirectory << ".\n";
  sexit(1);
 }

 if (S_ISDIR(tStat.st_mode)) {
 } else {
  cout << sCfgModuleDirectory << " is not a directory.\n";
  sexit(1);
 }
 
 // Checking to make sure the module is there
 sModulePath = sCfgModuleDirectory;
 if (sModulePath.c_str()[strlen(sModulePath.c_str())] != '/') {
  sModulePath += "/";
 }
 sModulePath += sCfgModule;
 sModulePath += ".so";

 if (stat(sModulePath.c_str(), &tStat) == -1) {
  perror("stat");
  cout << "Failed to stat module: " << sCfgModule << ".so\n";
  sexit(1);
 }

 if (S_ISREG(tStat.st_mode)) {
 } else {
  cout << sCfgModule << ".so is not a file.\n";
  sexit(1);
 }
 
 void* handle = dlopen(sModulePath.c_str(), RTLD_LAZY);

 if (!handle) {
  cout << "Cannot open module: " << dlerror() << '\n';
  sexit(1);
 }

 typedef char* (*parseLogLine_t)(const char *arguement);
 parseLogLine_t parseLogLine = (parseLogLine_t) dlsym(handle, "parseLogLine");
 if (!parseLogLine) {
  cout << "Cannot load symbol 'parseLogLine': " << dlerror() << '\n';
  dlclose(handle);
  sexit(1);
 }

 typedef char* (*modVersion_t)();
 modVersion_t modVersion = (modVersion_t) dlsym(handle, "modVersion");
 if (!modVersion) {
  cout << "Cannot load symbol 'modVersion': " << dlerror() << '\n';
  dlclose(handle);
  sexit(1);
 }

 typedef char* (*modAuthor_t)();
 modAuthor_t modAuthor = (modAuthor_t) dlsym(handle, "modAuthor");
 if (!modAuthor) {
  cout << "Cannot load symbol 'modAuthor': " << dlerror() << '\n';
  dlclose(handle);
  sexit(1);
 }

 typedef char* (*modName_t)();
 modName_t modName = (modVersion_t) dlsym(handle, "modName");
 if (!modName) {
  cout << "Cannot load symbol 'modName': " << dlerror() << '\n';
  dlclose(handle);
  sexit(1);
 }

 tv.tv_sec = 0;
 tv.tv_usec = 10000;
 
 FD_ZERO(&readfds_orig);
 FD_SET(iSockIPCC, &readfds_orig);
 FD_SET(iSockHLLog, &readfds_orig);
 
 while (1) {
  bcopy(&readfds_orig, &readfds, sizeof(&readfds_orig));
  select(iHighestSock+1, &readfds, NULL, NULL, &tv);
  if (FD_ISSET(iSockHLLog, &readfds)) {
   szRecvBuf = new char[MAXRECVSIZE];
   memset(szRecvBuf, '\0', MAXRECVSIZE);
   
   sockLen = sizeof(struct sockaddr);
   if ((numbytes = recvfrom(iSockHLLog, szRecvBuf, MAXRECVSIZE-1, 0, (struct sockaddr *)&saHLLog, (socklen_t *)&sockLen)) == -1)
    perror("recvfrom");
   
   sHLLine = szRecvBuf;
   delete [] szRecvBuf;
   strReplace(sHLLine, "\n", "");
   strReplace(sHLLine, "\r", "");
   szTBufP = parseLogLine(sHLLine.c_str());
   
   
   if (szTBufP != NULL) {
   
    saTIPC.sun_family = AF_UNIX;
    strcpy(saTIPC.sun_path, IPCSOCKNAMEP);

    if ((numbytes = sendto(iSockIPCC, szTBufP, strlen(szTBufP), 0, (struct sockaddr *)&saTIPC, sizeof(struct sockaddr))) == -1) 
     perror("sendto");
    
    delete [] szTBufP;
   }
  }
  if (FD_ISSET(iSockIPCC, &readfds)) {
   szRecvBuf = new char[MAXRECVSIZE];
   memset(szRecvBuf, '\0', MAXRECVSIZE);
   
   sockLen = sizeof(struct sockaddr);
   if ((numbytes = recvfrom(iSockIPCC, szRecvBuf, MAXRECVSIZE-1, 0, (struct sockaddr *)&saTIPC, (socklen_t *)&sockLen)) == -1)
    perror("recvfrom");
   
   sTString = szRecvBuf;
   delete [] szRecvBuf;

   tokenize(sTString, tokens, "\001");

   if (tokens[0] == "DIE") {
    sexit(0);
   }

   if (tokens[0] == "MODINFO") {
    sSendBuf = "\001MODINFOREPLY\001";
    szTBufP = modVersion();
    sSendBuf += szTBufP;
    delete [] szTBufP;
    
    sSendBuf += "\001";
    szTBufP = modAuthor();
    sSendBuf += szTBufP;
    delete [] szTBufP;
 
    sSendBuf += "\001";
    szTBufP = modName();
    sSendBuf += szTBufP;
    delete [] szTBufP;

    sSendBuf += "\001";
 
    saTIPC.sun_family = AF_UNIX;
    strcpy(saTIPC.sun_path, IPCSOCKNAMEP);

    if ((numbytes = sendto(iSockIPCC, sSendBuf.c_str(), strlen(sSendBuf.c_str()), 0, (struct sockaddr *)&saTIPC, sizeof(struct sockaddr))) == -1) {
     perror("sendto");
    }
   }
   
  } else {
   tv.tv_sec = 0;
   tv.tv_usec = 10000;
  }
 }
 
 dlclose(handle);
 
 return 0;
}

void sexit(int exitLevel) {
 close(iSockIRC);
 close(iSockHLClient);
 close(iSockHLLog);
 close(iSockIPCP);
 close(iSockIPCC);
 
 exit(exitLevel);
}

bool readConfig(string sConfigFile) {
 FILE *configFile;
 int iCurrentFileLine;
 string sCurrentWord, sCurrentLine;
 vector<string> tokens;
 char *szConfigLine;

 szConfigLine = new char[MAXDATASIZE];
 
 if ((configFile = fopen(sConfigFile.c_str(), "r")) <= 0) {
  cout << "Error Opening Config File:\n";
  perror("fopen");
  return false;
 }

 iCurrentFileLine = 1;
 
 while ((fgets(szConfigLine, MAXDATASIZE-1, configFile)) != NULL) {
  sCurrentLine = szConfigLine;
  if (szConfigLine[0] != '#' && szConfigLine[0] != '\n') {
   strReplace(sCurrentLine, "\n", "");
   strReplace(sCurrentLine, "\t", "");
   tokens.clear();
   tokenize(sCurrentLine, tokens);
   if (tokens[0] == "HLServer") {
    sCfgHLServ = tokens[1];
   } else if (tokens[0] == "RCONPassword") {
    sCfgRCONPass = tokens[1];
   } else if (tokens[0] == "IRCServer") {
    sCfgIRCServ = tokens[1];
   } else if (tokens[0] == "IRCChannel") {
    sCfgIRCChan = tokens[1];
   } else if (tokens[0] == "IRCNickname") {
    sCfgIRCNick = tokens[1];
   } else if (tokens[0] == "HLPort") {
    iCfgHLPort = atoi(tokens[1].c_str());
   } else if (tokens[0] == "HLClientPort") {
    iCfgHLClientPort = atoi(tokens[1].c_str());
   } else if (tokens[0] == "LogPort") {
    iCfgLogPort = atoi(tokens[1].c_str());
   } else if (tokens[0] == "IRCPort") {
    iCfgIRCPort = atoi(tokens[1].c_str());
   } else if (tokens[0] == "Timeout") {
    iCfgTimeout = atoi(tokens[1].c_str());
   } else if (tokens[0] == "SendDelay") {
    iCfgSendDelay = atoi(tokens[1].c_str());
   } else if (tokens[0] == "Module" && sCfgModule.empty()) {
    sCfgModule = tokens[1];
   } else if (tokens[0] == "ModuleDirectory") {
    sCfgModuleDirectory = tokens[1];
   } else if (tokens[0] == "CommandPrefix") {
    sCfgCommandPrefix = tokens[1];
   } else if (tokens[0] == "Advertise") {
    iCfgAdvertise = atoi(tokens[1].c_str());
   }
  }
  iCurrentFileLine++;
 }

 fclose(configFile);
 
 delete [] szConfigLine; 

 return false; 
}

void addQueue(const string& line, vector<string>& queue) {
 queue.push_back(line);
}

void delQueueFirst(vector<string>& queue) {
 vector<string>::iterator startIterator;
 startIterator = queue.begin();
 queue.erase(startIterator);
}

void displayUsage() {
 cout << "Usage: hlbot [OPTION]\n";
 cout << "Options:\n";
 cout << "-h, --help                \t- Display this text.\n";
 cout << "-c, --config <config file>\t- Use alternate config file.\n";
 cout << "-o, --module <module>     \t- HLBot module to use.\n";
 //cout << "-m, --mode <play mode>    \t- Set the bot mode (see README).\n";
 cout << "-n, --no-background       \t- Don't launch into background.\n";
 cout << "-v, --version             \t- Display HLBot version information.\n";
}

void catchInt(int signum) {
 string sSendBuf;
 sSendBuf = "QUIT :HLBot ";
 sSendBuf += HLBOTVERSION;
 sSendBuf += " [ http://hlbot.erikd.org ]\n";
 send(iSockIRC, (char *)sSendBuf.c_str(), strlen(sSendBuf.c_str()), 0);
 sexit(0);
}
