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


 
#include <TAXI/Tprop.h>

void PSet::copy(const PSet &P)
  {int i=-1;
  while ((i=P.nextindx[i])>=0)
      {if (P.tab[i]->origin())
          load(i,*(P.tab[i]));
      else
          {tab[i] = new _svector;
          link(i);
          }
      if (P.vtab[i]!=(vProp *)0)
          reg(i, P.vtab[i]->dup());
      }
  keep = P.keep;
  }
void PSet::clear()
  {int i;
  while( (i = nextindx[-1]) >= 0)
      erase(i);
  }
void PSet::reset()
  {int i,j;
  j = -1;
  while ((i = nextindx[j]) >=0)
      if (keep[i]) j=i;
      else erase(i);
  }
void PSet::resize(int a, int b)
  {_start=a; _finish=b;
  int i = -1;
  while ((i=nextindx[i])>=0)
      if (tab[i]->origin())tab[i]->resize(_start,_finish);
  }
void PSet::SwapIndex(int a, int b)
  {int i = -1;
  while ((i=nextindx[i])>=0)
      if (tab[i]->origin())tab[i]->SwapIndex(a,b);
  }
void PSet::CopyIndex(int a,int b)
  {int i = -1;
  while ((i=nextindx[i]) >= 0)
      if (tab[i]->origin()) tab[i]->CopyIndex(a,b);
  }
void PSet1::clear()
  {for (int i=tab.starti(); i<tab.stopi(); i++)
      if (tab[i]!=(void *)0)erase(i);
  }
void PSet1::reset()
  {for (int i=tab.starti(); i<tab.stopi(); i++)
      if ((!keep[i]) && (tab[i]!=(void *)0)) erase(i);
  }

void PSet1::copy(const PSet1 &P)
  {vProp1 *vp;
  for (int i=tab.starti(); i<tab.stopi(); i++)
      {vp = P.vtab[i];
      if (vp != (vProp1 *)0)
	{ if (vtab[i] == (vProp1 *)0)
	    {vtab[i] = vp->dup();
	      tab[i] = vp->edup(P.tab[i]);
	    }
	  else { // kept property should REALLY be copied
	    vp->copy(P.tab[i],tab[i]);
	  }
	}    
      }
  keep = P.keep;
  }
