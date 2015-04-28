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
#ifndef _MAX_PATH_
#define _MAX_PATH_
#include <TAXI/Tdebug.h>
#include <TAXI/Tsvector.h>
#include <TAXI/Tbase.h>
class MaxPath  
{
   int n_max;
   int m_max;
   int m;
   svector<int> link;
   svector<int> cir;
   svector<int> top;
   svector<int> ctop;
   svector<int> length;
   svector<int> vin;

 public:
   MaxPath(int nmax, int mmax): n_max(nmax), m_max(mmax), m(0),
     link(0,mmax), cir(0,mmax), top(0,nmax), ctop(0,nmax),
     length(0,mmax), vin(-mmax,mmax)
     {
       link.SetName("MaxPath link");
       cir.SetName("MaxPath cir");
       top.SetName("MaxPath top");
       ctop.SetName("MaxPath ctop");
       length.SetName("MaxPath length");
       vin.SetName("MaxPath vin");
       link.clear();
       cir.clear();
       top.clear();
       length.clear();
       vin.clear();
     }

   int insert(int iv1, int iv2, int len)
      {//if(!iv1)return(m);
     ++m;
#ifdef TDEBUG
     if(iv1 <= 0 || iv2 <= 0 ||iv1 > n_max || iv2 > n_max )
       { DPRINTF(("constraint between out of range value\n")); myabort();}
     if(m > m_max)
       {DPRINTF(("too many constraints\n")); myabort();}
#else
     if(m > m_max)LogPrintf("too many constraints/n");
#endif
     length[m] = len;
     vin[m] = iv1;
     vin[-m] = iv2;
     
     cir[m] = top[iv2];
     top[iv2] = m;
     return(m);
     }

void solve(svector<int> &potentiel)
    {int iv,vi;
    int je;
    int top_link;
/*Resoud les contraintes en effectuant un DFS … reculons
Les aretes decouvertes sont empilees (link,top_link) pour backtracker
en resolvant les contraintes
*/
    top_link = 0;
    ctop = top;
    for(vi = n_max;vi!=0;--vi)
        {iv = vi;
        for(;;)
            {while((je = ctop[iv]) != 0)
                {ctop[iv] = cir[je];
                iv = vin[je];
                link[je] = top_link;
                top_link = je;
                }
            if(!top_link)break;
            iv = vin[-top_link];
            potentiel[iv] = Max(potentiel[iv],potentiel[vin[top_link]] + length[top_link]);
            top_link = link[top_link];
            }
        }
    }
bool verify(svector<int> &potentiel)
      {bool ok = true;
      for(int i = 1; i <= m;i++)
          if(potentiel[vin[i]] + length[i]  > potentiel[vin[-i]] )
              {printf("%d  > %d\n",vin[i],vin[-i]);ok = false;}
      return ok;
      } 
};
#endif
