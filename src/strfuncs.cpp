// HLBot
//
// strfuncs.cpp - String manipulation functions
//
// $Id: strfuncs.cpp,v 1.4 2003/07/10 02:12:31 sg1bc Exp $

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

#include <vector>
#include <string>
#include "config.h"

using namespace std;

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

