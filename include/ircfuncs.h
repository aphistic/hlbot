// HLBot
//
// ircfuncs.h - IRC functions for HLBot
//
// $Id: ircfuncs.h,v 1.4 2003/07/03 17:02:02 yodatoad Exp $

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

#ifndef HLBOT_IRCFUNCS_H
#define HLBOT_IRCFUNCS_H

bool ircConnect(int iSockIRC, const char *szNickname);
bool ircJoin(int iSockIRC, const char *szChannel);
bool ircAuthUser(int, const char*, const char*, const char*);
#endif
