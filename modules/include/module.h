// HLBot
//
// modules.h - Some defs and functions for HLBot modules
//
// $Id: module.h,v 1.2 2002/07/20 01:51:33 yodatoad Exp $

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

#ifndef HLBOT_MODULE_H
#define HLBOT_MODULE_H

#include <vector>
#include <string>
#include <string.h>

#define MAXDATASIZE 1024
#define MAXRECVSIZE 2048

#define HLBOT_NOIPS 	0x0001
#define HLBOT_NOCOLOR	0x0002

// Thanks to Alavoor Vasudevan <alavoor[AT]yahoo.com> for the tokenizer
void tokenize(const string& str,
              vector<string>& tokens,
              const string& delimiters = " ") {
 string::size_type lastPos = str.find_first_not_of(delimiters, 0);
 string::size_type pos = str.find_first_of(delimiters, lastPos);

 while (string::npos != pos || string::npos != lastPos) {
  tokens.push_back(str.substr(lastPos, pos - lastPos));
  lastPos = str.find_first_not_of(delimiters, pos);
  pos = str.find_first_of(delimiters, lastPos);
 }
}

void strReplace(string& sReplace, char *szFind, char *szRepWith) {
 unsigned int iSearchLoc = sReplace.find(szFind);
 while (iSearchLoc < string::npos) {
  sReplace.replace(iSearchLoc, 1, szRepWith);
  iSearchLoc = sReplace.find(szFind, iSearchLoc+1);
 }
}

void parsePlayer(char *playerStr, vector<string>& storeVec) {
 int iLoop, iLoop2, iCharCount;
 char *szPointer, *szString;
 string sTemp;
 vector<string> sTStorage;

 szString = new char[MAXDATASIZE];
 strcpy(szString, playerStr);
 
 iCharCount = 0;
 iLoop = 0;
 szPointer = (char *)szString+(strlen(szString)-1);
 
 while (iCharCount < 3 && szPointer[0] != '\0') {
  if (szPointer[0] == '<') {
   szPointer[0] = '\001';
   iCharCount++;
  }

  szPointer--;

 }
 
 sTemp = szString;
 
 delete [] szString;
 
 storeVec.clear();
 tokenize(sTemp, storeVec, "\001");
 
 if (storeVec.size() != 4) {
  storeVec.clear();
 } else {
  strReplace(storeVec[1], ">", "");
  strReplace(storeVec[2], ">", "");
  strReplace(storeVec[3], ">", "");
 }
}


#endif
