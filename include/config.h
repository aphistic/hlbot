// HLBot
//
// config.h - Configuration header for HLBot
//
// $Id: config.h,v 1.2 2002/06/18 17:56:52 yodatoad Exp $

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

#ifndef CONFIG_H
#define CONFIG_H

// You may want to edit this location
#define CONFIGLOC "hlbot.cfg"

//#define DEBUG

#define USERNAME "HLBot"
#define REALNAME "HLBot"

#define IPCSOCKNAMEC ".hlbotsockc"
#define IPCSOCKNAMEP ".hlbotsockp"

#define MAXRECVLEN 2048
#define MAXDATASIZE 1024

#define VERSION "v0.2.1"

#endif
