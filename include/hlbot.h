// HLBot
//
// hlbot.h - Main header for HLBot
//
// $Id: hlbot.h,v 1.2 2002/07/20 01:51:33 yodatoad Exp $

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

#ifndef HLBOT_HLBOT_H
#define HLBOT_HLBOT_H

#include <vector>

#define HLBOT_NOIPS	0x0001
#define HLBOT_NOCOLOR	0x0002

bool readConfig(string);
void displayUsage();
void addQueue(const string&, vector<string>&);
void delQueueFirst(vector<string>&);
int forkParent();
int forkChild();
void sexit(int);

void catchInt(int);

typedef char* modentry (const char *arguement);

#endif
