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

#ifndef _TBASE_H_INCLUDED_
#define _TBASE_H_INCLUDED_

#ifdef __GNUC__
#ifndef  GCC_VERSION
#define GCC_VERSION (__GNUC__ * 10000 \
                              + __GNUC_MINOR__ * 100 \
                              + __GNUC_PATCHLEVEL__)
#endif
#endif 

#if (defined(__GNUC__) && GCC_VERSION < 30000)
#include <stdlib.h> 
#include <stdio.h> 
#include <stdarg.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include <setjmp.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <new.h>
#else
#include <cstdlib> 
#include <cstdio> 
#include <cstring>
#include <cstdarg>
#include <cassert>
#include <iostream>
#include <fstream>
#include <csetjmp>
#include <cmath>
#include <ctime>
#include <new>
#endif

#define T_STD
using namespace std; 

#ifndef ALGOBASE_H
template <class T,class U> inline T Min(const T a,const U b)
    {if(a <= (T)b) return a;
    return (T)b;
    }
template <class T,class U> inline T Max(const T a,const U b)
    {if(a >= (T)b)return a;
    return (T)b;
    }
inline double Max(const double a, const double b)
    {if(a >= b)return a;
    return b;
    }
inline double Min(const double a, const double b)
    {if(a <= b)return a;
    return b;
    }
#endif
inline void Tswap(int &a,int &b) {int c=a; a=b; b=c;}
inline void Tswap(float &a,float &b) {float c=a; a=b; b=c;}
inline void Tswap(double &a,double &b) {double c=a; a=b; b=c;}
template <class T> inline void Tswap(T &a,T &b)
    {char buf[sizeof(T)];
    memcpy(buf,&a,sizeof(T));
    memcpy(&a,&b,sizeof(T));
    memcpy(&b,buf,sizeof(T));
    }
template <class T,class U,class V> inline T Min(const T a,const U b,const V c)
    {return(a<=(T)b ? (a<=(T)c ? a : (T)c) : ((T)b<=(T)c ? (T)b : (T)c) );}
template <class T,class U,class V> inline T Max(const T a,const U b,const V c)
    {return(a>=(T)b ? (a>=(T)c ? a : (T)c) : ((T)b>=(T)c ? (T)b : (T)c) );}
template <class T,class U,class V> inline T bound(T a,U mini,V maxi)
    {return( a<(T)mini ? (T)mini : (a>(T)maxi ? (T)maxi : a) );}
template <class T> inline T Abs(const T a)
    {return (a >= (T)0  ? a : -a);}

//Standart constants
#ifndef DBL_MAX
//maximal double
#define DBL_MAX 	1.79769313486231500e+308
#endif
//minimal double
#ifndef DBL_MIN
#define DBL_MIN 	2.22507385850720200e-308
#endif
//minimal double such that DBL_EPSILON + 1. != 1.
#ifndef DBL_EPSILON
#define DBL_EPSILON	2.22044604925031300e-016
#endif

#endif

