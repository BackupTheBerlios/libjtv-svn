/*
    Copyright (C) 1998 by Jorrit Tyberghein
  
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
  
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
  
    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef CSENDIAN_H
#define CSENDIAN_H

#include <stdio.h>

typedef unsigned char UByte;
typedef unsigned short UShort;
typedef unsigned int UInt;
typedef unsigned long ULong;

/// Read a little-endian short from address
inline UShort get_le_short (UByte *buff)
{
 return ((UShort)buff[0]) | ((UShort)buff[1] << 8);
}

/// Read a little-endian long from address
inline ULong get_le_long (UByte *buff)
{
 return ((ULong)buff[0])       | ((ULong)buff[1] << 8)
      | ((ULong)buff[2] << 16) | ((ULong)buff[3] << 24);
}

/// Set a little-endian short on a address
inline void set_le_short (UByte *buff, UShort value)
{
 buff[0] = (value)       & 0xff;
 buff[1] = (value >> 8)  & 0xff;
}

/// Set a little-endian long on a address
inline void set_le_long (UByte *buff, ULong value)
{
 buff[0] = (UByte)(value)       & 0xff;
 buff[1] = (UByte)(value >> 8)  & 0xff;
 buff[2] = (UByte)(value >> 16) & 0xff;
 buff[3] = (UByte)(value >> 24) & 0xff;
}

#endif
