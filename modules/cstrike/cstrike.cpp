// HLBot
//
// cstrike.cpp - Counter-Strike HLBot module
//
// $Id: cstrike.cpp,v 1.2 2002/07/15 20:29:41 yodatoad Exp $

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


#include "module.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

// Module version number
#define MODVERSION "v0.0.1"

// Module author
#define MODAUTHOR "Erik Davidson"

// Mod the module is for
#define MODFULLNAME "Counter-Strike"

// Filename of module (ie: cstrike.so would be cstrike here)
#define MODNAME "cstrike"


int iOptions = 0;

// Required function that parses the log files that are received
extern "C" char* parseLogLine(const char *argument) {
 char *ret;
 string sTeam1, sTeam2;
 string sBuffer, sSendBuf;
 vector<string> tokens, tokens2, tokens3, playerInfo, playerInfo2;
 
 ret = new char[MAXDATASIZE];
 memset(ret, '\0', MAXDATASIZE);
 
 sBuffer = argument+33;

 //cout << "CS: " << sBuffer << "\n";
 
 tokens.clear();

 tokenize(sBuffer, tokens);
 
 if (tokens[0] == "Server") {
  if (tokens.size() > 2) {
   if (tokens[1] == "say") {
    tokens2.clear();
    tokenize(sBuffer, tokens2, "\"");
    sSendBuf = "\001SAY\001<Server> ";
    sSendBuf += tokens2[1];
    sSendBuf += "\001";
   }
  }
 } else if (tokens[0] == "World") {
  if (tokens.size() > 2) {
   if (tokens[2] == "\"Round_Start\"") {
    sSendBuf = "\001SAY\001Round Started.\001";
   } else if (tokens[2] == "\"Round_End\"") {
    sSendBuf = "\001SAY\001Round Ended.\001";
   } else if (tokens[2] == "\"Round_Draw\"") {
    tokens2.clear();
    tokenize(sBuffer, tokens2, "\"");
    sSendBuf = "\001SAY\001Round Draw. Wins (T: ";
    sSendBuf += tokens2[5];
    sSendBuf += " CT: ";
    sSendBuf += tokens2[3];
    sSendBuf += ")\001";
   } else if (tokens[2] == "\"Game_Commencing\"") {
    sSendBuf = "\001SAY\001Game Commencing.\001";
   }
  }
 } else if (tokens[0] == "Team") {
  if (tokens.size() > 3) {
   if (tokens[2] == "triggered") {
    if (tokens[3] == "\"Target_Saved\"") {
     tokens2.clear();
     tokenize(sBuffer, tokens2, "\"");
     sSendBuf = "\001SAY\001CT's saved the target. Wins (T: ";
     sSendBuf += tokens2[7];
     sSendBuf += " CT: ";
     sSendBuf += tokens2[5];
     sSendBuf += ")\001";
    } else if (tokens[3] == "\"Bomb_Defused\"") {
     tokens2.clear();
     tokenize(sBuffer, tokens2, "\"");
     sSendBuf = "\001SAY\001CT's defused the bomb. Wins (T: ";
     sSendBuf += tokens2[7];
     sSendBuf += " CT: ";
     sSendBuf += tokens2[5];
     sSendBuf += ")\001";
    } else if (tokens[3] == "\"Target_Bombed\"") {
     tokens2.clear();
     tokenize(sBuffer, tokens2, "\"");
     sSendBuf = "\001SAY\001Terrorists bombed the target. Wins (T: ";
     sSendBuf += tokens2[7];
     sSendBuf += " CT: ";
     sSendBuf += tokens2[5];
     sSendBuf += ")\001";
    } else if (tokens[3] == "\"All_Hostages_Rescued\"") {
     tokens2.clear();
     tokenize(sBuffer, tokens2, "\"");
     sSendBuf = "\001SAY\001CT's rescued all the hostages. Wins (T: ";
     sSendBuf += tokens2[7];
     sSendBuf += " CT: ";
     sSendBuf += tokens2[5];
     sSendBuf += ")\001";
    } else if (tokens[3] == "\"Hostages_Not_Rescued\"") {
     tokens2.clear();
     tokenize(sBuffer, tokens2, "\"");
     sSendBuf = "\001SAY\001Hostages have not been rescued. Wins (T: ";
     sSendBuf += tokens2[7];
     sSendBuf += " CT: ";
     sSendBuf += tokens2[5];
     sSendBuf += ")\001";
    } else if (tokens[3] == "\"CTs_Win\"") {
     tokens2.clear();
     tokenize(sBuffer, tokens2, "\"");
     sSendBuf = "\001SAY\001Counter-Terrorists Win. Wins (T: ";
     sSendBuf += tokens2[7];
     sSendBuf += " CT: ";
     sSendBuf += tokens2[5];
     sSendBuf += ")\001";
    } else if (tokens[3] == "\"Terrorists_Win\"") {
     tokens2.clear();
     tokenize(sBuffer, tokens2, "\"");
     sSendBuf = "\001SAY\001Terrorists Win. Wins (T: ";
     sSendBuf += tokens2[7];
     sSendBuf += " CT: ";
     sSendBuf += tokens2[5];
     sSendBuf += ")\001";
    } else if (tokens[3] == "\"VIP_Escaped\"") {
     tokens2.clear();
     tokenize(sBuffer, tokens2, "\"");
     sSendBuf = "\001SAY\001VIP Escaped. Wins (T: ";
     sSendBuf += tokens2[7];
     sSendBuf += " CT: ";
     sSendBuf += tokens2[5];
     sSendBuf += ")\001";
    } else if (tokens[3] == "\"VIP_Not_Escaped\"") {
     tokens2.clear();
     tokenize(sBuffer, tokens2, "\"");
     sSendBuf = "\001SAY\001VIP didn't escape. Wins (T: ";
     sSendBuf += tokens2[7];
     sSendBuf += " CT: ";
     sSendBuf += tokens2[5];
     sSendBuf += ")\001";
    } else if (tokens[3] == "\"VIP_Assassinated\"") {
     tokens2.clear();
     tokenize(sBuffer, tokens2, "\"");
     sSendBuf = "\001SAY\001VIP assassinated. Wins (T: ";
     sSendBuf += tokens2[7];
     sSendBuf += " CT: ";
     sSendBuf += tokens2[5];
     sSendBuf += ")\001";
    }
   }
  }
 } else if (tokens[0] == "Started") {
  if (tokens.size() > 2) {
   if (tokens[1] == "map") {
    tokens2.clear();
    tokenize(sBuffer, tokens2, "\"");
    sSendBuf = "\001SAY\001Started map ";
    sSendBuf += tokens2[1];
    sSendBuf += ".\001";
   }
  }
 } else if (tokens[0].c_str()[0] == '\"') {
  tokens2.clear();
  tokenize(sBuffer, tokens2, "\"");
  parsePlayer((char *)tokens2[0].c_str(), playerInfo);
  if (playerInfo.size() == 4) {
   tokens3.clear();
   tokenize(tokens2[1], tokens3);
   if (tokens3[0] == "connected,") {
    sSendBuf = "\001SAY\001";
    sSendBuf += playerInfo[0];
    sSendBuf += " connected";
    if (iOptions & 0x01 == 0x01) {
     sSendBuf += ".";
    } else {
     sSendBuf += " from ";
    sSendBuf += tokens2[2];
    sSendBuf += ".";
   }
    sSendBuf += "\001";
   } else if (tokens3[0] == "changed") {
    sSendBuf = "\001SAY\001";
    sSendBuf += playerInfo[0];
    sSendBuf += " changed name to ";
    sSendBuf += tokens2[2];
    sSendBuf += ".\001";
   } else if (tokens3[0] == "committed") {
    sSendBuf = "\001SAY\001";
    sSendBuf += playerInfo[0];
    sSendBuf += " committed suicide with ";
    sSendBuf += tokens2[2];
    sSendBuf += ".\001";
   } else if (tokens3[0] == "triggered") {
    if (tokens2[2] == "Planted_The_Bomb") {
     sSendBuf = "\001SAY\001";
     sSendBuf += playerInfo[0];
     sSendBuf += " planted the bomb.\001";
    } else if (tokens2[2] == "Dropped_The_Bomb") {
     sSendBuf = "\001SAY\001";
     sSendBuf += playerInfo[0];
     sSendBuf += " dropped the bomb.\001";
    }
   } else if (tokens3[0] == "disconnected") {
    sSendBuf = "\001SAY\001";
    sSendBuf += playerInfo[0];
    sSendBuf += " disconnected.\001";
   } else if (tokens3[0] == "entered") {
    sSendBuf = "\001SAY\001";
    sSendBuf += playerInfo[0];
    sSendBuf += " entered the game.\001";
   } else if (tokens3[0] == "say") {
    sSendBuf = "\001SAY\001";
    if (tokens2[tokens2.size()-1] == " (dead)") {
     sSendBuf += "*DEAD* ";
    }
    sSendBuf += playerInfo[0];
    sSendBuf += " (";
    sSendBuf += playerInfo[3];
    sSendBuf += ") : ";
    sSendBuf += tokens2[2];
    sSendBuf += "\001";
   } else if (tokens3[0] == "joined") {
    sSendBuf = "\001SAY\001";
    sSendBuf += playerInfo[0];
    sSendBuf += " joined team ";
    sSendBuf += tokens2[2];
    sSendBuf += ".\001";
   } else if (tokens3[0] == "killed") {
    parsePlayer((char *)tokens2[2].c_str(), playerInfo2);
    sSendBuf = "\001SAY\001";
    sSendBuf += playerInfo[0];
    sSendBuf += " (";
    sSendBuf += playerInfo[3];
    sSendBuf += ") killed ";    
    sSendBuf += playerInfo2[0];
    sSendBuf += " (";
    sSendBuf += playerInfo2[3];
    sSendBuf += ") with ";
    sSendBuf += tokens2[4];
    sSendBuf += ".\001";
   }
  } else {
   //perror
  }
 }

 if (sSendBuf.empty()) {
  return NULL;
 } else {
  strcpy(ret, sSendBuf.c_str());
  return ret;
 }
}

// Required function that returns the version of the module
extern "C" char* modVersion() {
 char* ret;

 ret = new char[MAXDATASIZE];
 memset(ret, '\0', MAXDATASIZE);
 strcpy(ret, MODVERSION);
 return ret;
}

// Required function that returns the author of the module
extern "C" char* modAuthor() {
 char* ret;

 ret = new char[MAXDATASIZE];
 memset(ret, '\0', MAXDATASIZE);
 strcpy(ret, MODAUTHOR);
 return ret;
}

// Required function that returns the name of the mod the module
// is for
extern "C" char* modName() {
 char* ret;

 ret = new char[MAXDATASIZE];
 memset(ret, '\0', MAXDATASIZE);
 strcpy(ret, MODFULLNAME);
 return ret;
}

// Required function that sets the module run options
extern "C" void modSetOptions(int iOpt) {
 iOptions = iOpt;

 if (iOptions & 0x01 == 0x01) {
  cout << "Mod Hid IPs\n";
 }
}
