// HLBot
//
// strfuncs.cpp - String manipulation functions
//
// $Id: strfuncs.cpp,v 1.1 2002/06/18 00:17:46 yodatoad Exp $

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
#include <sys/types.h>
#include <errno.h>
#include "config.h"

static char *str;
static char *c;
char strbuf[MAXRECVLEN];

void scanner_init(char* szInputStr) {
 str = c = szInputStr;
 memset(strbuf, '\0', strlen(strbuf));
}

char* scanner_token(char cSeperator) {
 char *ret;
 int i = 0;

 while (*c == cSeperator) {
  c++;
 }

 do {
  strbuf[i++] = *c++;
 } while (*c != '\0' && *c != cSeperator && *c != '\r' && *c != '\n');
 strbuf[i++] = '\0';

 if (strlen(strbuf) == 0) {
  return 0;
 }

 ret = new char[strlen(strbuf)+1];
 strcpy(ret, strbuf);
 return ret;
}

char* getWord(char* szBuffer, int iWordNum, char cSeperator) {
 int iLoopNum;
 char *ret;
 
 scanner_init(szBuffer);
 
 iLoopNum = 0;
 
 do {
  ret = scanner_token(cSeperator);
  iLoopNum++;
 } while (iLoopNum < iWordNum && ret != 0);

 return ret;
}

char* getPhrase(char* szBuffer, int iWordNum, char cSeperator) {
 int iLoopNum;
 char *ret, *tWord, *tSepChar;
 ret = new char[MAXDATASIZE];
 tSepChar = new char[2];
 memset(tSepChar, '\0', 2);
 memset(ret, '\0', MAXDATASIZE);
 memset(tSepChar, cSeperator, 1);
 
 scanner_init(szBuffer);

 iLoopNum = 0;
 
 do {
  tWord = scanner_token(cSeperator);
  iLoopNum++;
 } while (iLoopNum < iWordNum && tWord != 0);

 if (tWord == 0) {
  return 0;
 }

 do {
  tWord = scanner_token(cSeperator);
  iLoopNum++;
  if (tWord != 0) {
   strcat(ret, tSepChar);
   strcat(ret, tWord);
  }
 } while (tWord != 0);

 return ret;
}
 
bool wordExists(char* szBuffer, int iWordNum, char cSeperator) {
 char prevChar;
 int iWordCount;
 
 while (*szBuffer == cSeperator) {
  szBuffer++;
 }
 
 iWordCount = 1;
 
 while (*szBuffer != '\0') {
  if (*szBuffer == cSeperator && prevChar != cSeperator) {
   iWordCount++;
  }
  
  prevChar = *szBuffer;
  szBuffer++;
 
 }
 if (iWordCount < iWordNum) {
  return false;
 } else {
  return true;
 }
}

void clearStr(char *szBuffer, int iLength = 0) {
 if (iLength == 0) {
  iLength = strlen(szBuffer);
 }
 memset(szBuffer, '\0', MAXDATASIZE);
}

char* trim(char *szBuffer) {
  int iCurCharPos;
  char cCurChar;

  cCurChar = ' ';
  iCurCharPos = 0;
  while (cCurChar == ' ' || cCurChar == '\t') {
   cCurChar = szBuffer[iCurCharPos];
   iCurCharPos++;
  }

  iCurCharPos--;
  return szBuffer + iCurCharPos;
}
