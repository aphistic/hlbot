// HLBot
//
// strfuncs.h - Header for string functions
//
// $Id: strfuncs.h,v 1.3 2002/07/09 06:38:49 yodatoad Exp $

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

#ifndef HLBOT_STRFUNCS_H
#define HLBOT_STRFUNCS_H

#include <vector>

void tokenize(const string& str, vector<string>& tokens, const string& delimiters = " ");
void strReplace(string& sReplace, char *szFind, char *szRepWith);

#endif
