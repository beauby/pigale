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

#ifndef __MYDEBUG_H__
#define __MYDEBUG_H__

#include <TAXI/Tbase.h>
#include <TAXI/Errors.h>

bool & debug();
void myabort();
int CheckLogFile();
void DebugPuts(const char *str);
void DebugPrintf(const char *fmt,...);
void LogPrintf(const char *fmt,...);
void DebugIndent(int i);
const char * logfile();


struct _T_Error 
   {int code;
   const char *msg;

   _T_Error(int c=0, const char *m=(const char *)0) : code(c), msg(m) {}
   operator int () const { return code;}
   };

struct _Error {
   _T_Error error;
   const char *file;
   int line;
   };


_T_Error &ErrorPositioner(const char *f, int l);
int  getPigaleError();
const char * getPigaleErrorMsg();
const char * getPigaleErrorFile();
int getPigaleErrorLine();

#define setPigaleError ErrorPositioner((const char *)__FILE__,__LINE__)=_T_Error
#define _PRINTFL(arg)   DebugPrintf("%s line:%d -> %d",__FILE__, __LINE__,arg)

#ifdef TDEBUG

  #define DOPEN()   { DebugIndent(1); }
  #define DCLOSE()  { DebugIndent(-1); }

  #define DPRINTF(arg) { \
     DebugPrintf("%s(%d): ", __FILE__, __LINE__); \
     DebugPrintf arg; \
  }
  #define PRINTF(arg) { \
     DebugPrintf arg; \
  }
  #define DPUTS(str) { \
     DebugPrintf("%s(%d): %s", __FILE__, __LINE__, str); \
  }
  #define CHKNEW(p) { \
     if (p == 0) { \
        DebugPrintf("%s(%d): Short of memory.\n", __FILE__, __LINE__); \
        exit(1); \
     } \
  }
#else
  #define DOPEN()
  #define DCLOSE()
  #define DPRINTF(arg) {}
  #define DPUTS(str) {}
  #define CHKNEW(p)
#endif
#endif
