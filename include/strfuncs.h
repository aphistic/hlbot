// HLBot
//
// strfuncs.h - Header for string functions
//
// $Id: strfuncs.h,v 1.1 2002/06/18 00:17:45 yodatoad Exp $

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

#ifndef STRFUNCS_H
#define STRFUNCS_H

char* getPhrase(char*, int, char);
char* getWord(char*, int, char);
void scanner_init(char*);
char* scanner_token(char);
bool wordExists(char*, int, char);
void clearStr(char*, int iLength);
char* trim(char*);

#endif
