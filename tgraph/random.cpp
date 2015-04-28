 
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
#include <TAXI/Tsvector.h>
#include <TAXI/graphs.h>
#include <TAXI/random.h>
#include <limits>

#if defined(_WINDOWS) || defined(_WIN32)
#define MAXP std::numeric_limits<unsigned int>::max()-1
#define lrand48 genrand
#define srand48 sgenrand
//#define srand48 srand
//#define lrand48 rand
//#define MAXP std::numeric_limits<int>::max()
#else
/*
#define MAXP std::numeric_limits<int>::max()-1
#define lrand48 random
#define srand48 srandom
*/
#define MAXP std::numeric_limits<unsigned int>::max()-1
#define lrand48 genrand
#define srand48 sgenrand


#endif


/* A C-program for MT19937: Integer version (1999/10/28)          */
/*  genrand() generates one pseudorandom unsigned integer (32bit) */
/* which is uniformly distributed among 0 to 2^32-1  for each     */
/* call. sgenrand(seed) sets initial values to the working area   */
/* of 624 words. Before genrand(), sgenrand(seed) must be         */
/* called once. (seed is any 32-bit integer.)                     */
/*   Coded by Takuji Nishimura, considering the suggestions by    */
/* Topher Cooper and Marc Rieffel in July-Aug. 1997.              */

/* This library is free software; you can redistribute it and/or   */
/* modify it under the terms of the GNU Library General Public     */
/* License as published by the Free Software Foundation; either    */
/* version 2 of the License, or (at your option) any later         */
/* version.                                                        */
/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of  */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.            */
/* See the GNU Library General Public License for more details.    */
/* You should have received a copy of the GNU Library General      */
/* Public License along with this library; if not, write to the    */
/* Free Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA   */ 
/* 02111-1307  USA                                                 */

/* Copyright (C) 1997, 1999 Makoto Matsumoto and Takuji Nishimura. */
/* Any feedback is very welcome. For any question, comments,       */
/* see http://www.math.keio.ac.jp/matumoto/emt.html or email       */
/* matumoto@math.keio.ac.jp                                        */

/* REFERENCE                                                       */
/* M. Matsumoto and T. Nishimura,                                  */
/* "Mersenne Twister: A 623-Dimensionally Equidistributed Uniform  */
/* Pseudo-Random Number Generator",                                */
/* ACM Transactions on Modeling and Computer Simulation,           */
/* Vol. 8, No. 1, January 1998, pp 3--30.                          */



/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0df   /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */   
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)

static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

/* Initializing the array with a seed */
void sgenrand(unsigned long seed)
   
{
    int i;

    for (i=0;i<N;i++) {
         mt[i] = seed & 0xffff0000;
         seed = 69069 * seed + 1;
         mt[i] |= (seed & 0xffff0000) >> 16;
         seed = 69069 * seed + 1;
    }
    mti = N;
}

/* Initialization by "sgenrand()" is an example. Theoretically,      */
/* there are 2^19937-1 possible states as an intial state.           */
/* This function allows to choose any of 2^19937-1 ones.             */
/* Essential bits in "seed_array[]" is following 19937 bits:         */
/*  (seed_array[0]&UPPER_MASK), seed_array[1], ..., seed_array[N-1]. */
/* (seed_array[0]&LOWER_MASK) is discarded.                          */ 
/* Theoretically,                                                    */
/*  (seed_array[0]&UPPER_MASK), seed_array[1], ..., seed_array[N-1]  */
/* can take any values except all zeros.                             */
void lsgenrand(unsigned long seed_array[])
    //unsigned long seed_array[]; 
    /* the length of seed_array[] must be at least N */
{
    int i;

    for (i=0;i<N;i++) 
      mt[i] = seed_array[i];
    mti=N;
}

unsigned long genrand()
{
    unsigned long y;
    static unsigned long mag01[2]={0x0, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        int kk;

        if (mti == N+1)   /* if sgenrand() has not been called, */
            sgenrand(4357); /* a default initial seed is used   */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        for (;kk<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1];

        mti = 0;
    }
  
    y = mt[mti++];
    y ^= TEMPERING_SHIFT_U(y);
    y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
    y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
    y ^= TEMPERING_SHIFT_L(y);

    return y; 
}



unsigned long randomMax()
	{return (unsigned long)MAXP;
	}

long & randomSetSeed()
  {static long _seed = 1;
  return _seed;
  }
void randomInitSeed()
// called once, when program  starts
  {time_t time_seed;
  time(&time_seed);
  randomSetSeed() = (long)time_seed; 
  }
void randomStart()
  {srand48(randomSetSeed());
  }
void randomEnd()
  {randomSetSeed() = (long)lrand48();
  }
long randomGet(long range) 
//returns an integer >= 1 && <= range
  {return (lrand48()%(long)range) +1;
  }
long randomGet() 
  {return lrand48();
  }
void randomShuffle(svector<int> &tab)
// randomly permute the elements of tab
 {int n = tab.n();
 if(n < 2)return;
 randomStart();
 for(int i = 0;i < n;i++)
     {int r = i + (int) (lrand48()%(long)(n-i));
     if(i == r)continue;
     tab.SwapIndex(i,r);
     }
 randomEnd();
 }
void shuffleCir(TopologicalGraph &G)
  {G.planarMap() = 0;
  for(tvertex v = 1; v <= G.nv();v++)
      {int degree = G.Degree(v);
      if(degree < 3)continue;
      svector<int> tab(degree);
      tbrin e0 = G.pbrin[v];
      tbrin e = e0;
      int d = 0;      
      do
          {tab[d++] = e();
          }while((e = G.cir[e]) != e0);
      randomShuffle(tab);
      for(d = 0;d < degree-1;d++)
          G.cir[tab[d]] = (tbrin)tab[d+1];
      G.cir[tab[degree-1]] = (tbrin)tab[0];
      G.pbrin[v] = (tbrin)tab[0];
      }
  // Compute acir
  for(tbrin b = -G.ne(); b<= G.ne(); b++)
      G.acir[G.cir[b]] = b;
  }

