 
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

#ifndef _TIME_H_INCLUDED_
#define _TIME_H_INCLUDED_
#if defined(__GNUC__)
#include <sys/time.h>

class Timer
{public:
  Timer(){start();}
  ~Timer(){}

  void start(){gettimeofday(&t0,NULL);}
  double elapsed() //returns the elapsed time in milliseconds 
    {gettimeofday(&t1,NULL);
    return (t1.tv_usec/1000. + t1.tv_sec*1000. - t0.tv_usec/1000. - t0.tv_sec*1000.);
    }
private:
  timeval t0,t1;  
};

#else
class Timer
{public:
  void start(){return;}
  double elapsed(){return 1.;}
  
};
#endif
#endif
