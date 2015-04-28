/****************************************************************************
**
** Copyright (C) 2001 Hubert de Fraysseix, Patrice Ossona de Mendez.
** All rights reserved.
** This file is part of the PIGALE Toolkit.
**
** This file may be distributed under the terms of the GNU Public License
** appearing in the file LICENSE.HTML included in the packaging of this file.
**
*****************************************************************************/
#ifndef COLOR_H
#define COLOR_H
enum {
  White  =  0,
  Black  =  1,
  Red    =  2,
  Green  =  3,
  Blue   =  4,
  Yellow =  5,
  Violet =  6,
  Orange =  7,
  Cyan   =  8,
  Brown  =  9,
  Pink   = 10,
  Green2 = 11,
  Blue2  = 12,
  Grey1  = 13,
  Grey2  = 14,
  Grey3  = 15,
  Ivory  = 16 
};

// Functions to code and decode an rgb value into an unsigned int
inline int GetRed(unsigned int rgb)              // get red part of RGB
{return (int)((rgb >> 16) & 0xff);}
inline int GetGreen(unsigned int rgb)            // get green part of RGB
{return (int)((rgb >> 8) & 0xff);}
inline int GetBlue(unsigned int rgb)             // get blue part of RGB
{return (int)(rgb & 0xff);}
inline unsigned int SetRgb(int r,int g,int b )   // set RGB value
{return (0xff << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);}

#endif
