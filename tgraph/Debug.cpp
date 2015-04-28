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


#include <TAXI/Tbase.h>
#include <TAXI/Tdebug.h>
#include <TAXI/Tmessage.h>

#include <iomanip>
#include <cstring>

#ifdef OSX
char *tmp = new char[100];
char *LogName = strcat(strcpy(tmp,getenv("HOME")),"/pigalelog.txt");
#else
static char LogName[] = "pigalelog.txt";
#endif
static int Indent = 0;
static bool first = true;
static _Error MainError;

const char * logfile()
    {return (const char *)LogName;}

_T_Error &ErrorPositioner(const char *f, int l)
     {MainError.file=f; MainError.line=l; return MainError.error;}

int getPigaleError() 
  {return (int) MainError.error;}
const char * getPigaleErrorMsg()
  {return MainError.error.msg;}
const char * getPigaleErrorFile()
  {return MainError.file;}
int getPigaleErrorLine()
  {return MainError.line;}

bool& debug()
  {static bool i = 0;
  return i;
  }
void DebugIndent(int i)
  {Indent += i;
  } 
int CheckLogFile()
  {FILE *LogFile;
  LogFile = fopen(LogName, "a");
  if(LogFile == NULL)return -1;
  fclose(LogFile);
  return 0;
  }
void DebugPuts(const char *str)
  {FILE *LogFile;
  if(first)
      {LogFile = fopen(LogName, "w");first = false;}
  else
      LogFile = fopen(LogName, "a");
  if(LogFile == NULL)return;
  fprintf(LogFile, "%*s%s\n", Indent*2, "", str);
  fclose(LogFile);
  }

void DebugPrintf(const char *fmt,...)
  {FILE *LogFile;
  va_list arg_ptr;
  char buff[256];
  va_start(arg_ptr,fmt);
  vsprintf(buff,fmt,arg_ptr);
  va_end(arg_ptr);

  if(first)
      {LogFile = fopen(LogName, "w");first = false;}
  else
      LogFile = fopen(LogName, "a");
  if(LogFile == NULL)return;
  fprintf(LogFile, "%*s%s\n", Indent*2, "", buff);
  //cout<<setw(Indent*2)<<" "<<buff<<endl;
  Tprintf("%*s%s", Indent*2, "", buff);
  fclose(LogFile);
  }


void LogPrintf(const char *fmt,...)
  {FILE *LogFile;
  va_list arg_ptr;
  char buff[256];
  va_start(arg_ptr,fmt);
  vsprintf(buff,fmt,arg_ptr);
  va_end(arg_ptr);

  if(first)
      {LogFile = fopen(LogName, "w");first = false;}
  else
      LogFile = fopen(LogName, "a");
  if(LogFile == NULL)return;
  fprintf(LogFile,"%s",buff);
  fclose(LogFile);
  }
void myabort() {abort();}

GraphDebug *GraphDebug::gd=(GraphDebug *)0;
static GraphDebug DefaultGraphDebug;
