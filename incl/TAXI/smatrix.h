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


#ifndef __SMATRIX_H__
#define __SMATRIX_H__

#include <TAXI/Tbase.h>
#include <TAXI/Tsvector.h>
#define EPSILON (1E-9)

static void Divise(svector<double> *v, double d)
{ for (int i=0; i<v->n(); i++)
    (*v)[i]/=d;
}
static void SubtractMultiple(svector<double> *a,double d, svector<double> *b)
{ for (int i=0; i<a->n(); i++)
    (*a)[i] -= d*(*b)[i];
}
class SMatrix 
{private:
  int n;
  svector<svector<double> *> rows;
  
  void InitRows()
      {for (int i=0; i<n; i++)
          rows[i] = new svector<double> (n);
      }
  void DeleteRows()
      {for (int i=0; i<n; i++)
          delete rows[i];
      }
  void InitDuplicateRows(const SMatrix &SM)
      {for (int i=0; i<n; i++)
          rows[i] = new svector<double> (*(SM.rows[i]));
      }
  void DuplicateRows(const SMatrix &SM)
      {for (int i=0; i<n; i++)
          (*rows[i]) = *(SM.rows[i]);
      }
  int FindPivot(int col)
      {double maxv = fabs((*this)[col][col]);
      int maxi=col;
      double d;
      for (int i=col+1;i<n; i++)
          {d=fabs((*this)[i][col]);
          if (d>maxv)
              {maxv=d;
              maxi=i;
              }
          }
      if (maxv<EPSILON) return -1;
      return maxi;
      }

public:
  SMatrix(int nn):n(nn), rows(nn)
      { InitRows();}
  SMatrix(const SMatrix &SM) : n(SM.n), rows(SM.n)
      {InitDuplicateRows(SM);}
  ~SMatrix() {DeleteRows();}
  
  svector<double> & operator[] (int i)
      {return *rows[i];}
  const svector<double> & operator[] (int i) const
      {return *rows[i];}
  SMatrix & operator=(const SMatrix &SM)
      {if (&SM == this)
          return *this;
      if (SM.n==n)
          DuplicateRows(SM);
      else
          {DeleteRows();
          n = SM.n;
          InitDuplicateRows(SM);
          }
      return *this;
      }
  void SwapRows(int i, int j)
      { svector<double> *tmp = rows[i];
      rows[i]=rows[j];
      rows[j]=tmp;
      }
  void clear()
      {for (int i=0; i<n; i++)
          rows[i]->clear();
      }	
  void SetIdentity()
      {for (int i=0; i<n; i++)
          {rows[i]->clear();
          (*rows[i])[i]=1;
          }
      }	
  SMatrix Inverse(int &ok)
  {//SMatrix &Result = *new SMatrix(n);
    SMatrix Result(n);
      SMatrix &M = *new SMatrix(*this);
      double d;
      int icol;
      ok=0;
      Result.SetIdentity();
      for (icol=0; icol<n; icol++)
          {int p = M.FindPivot(icol);
          if (p<0) 
              {delete &M;
              return Result;
              }
          // echange lignes p et icol
          M.SwapRows(p,icol);
          Result.SwapRows(p,icol);
          d = M[icol][icol];
          Divise(M.rows[icol],d);
          Divise(Result.rows[icol],d);
          for (int j=0; j<n; j++)
              {if (j==icol) continue;
              d = M[j][icol];
              SubtractMultiple(M.rows[j],d,M.rows[icol]);
              SubtractMultiple(Result.rows[j],d,Result.rows[icol]);
              }
          }
      ok=1;
      delete &M;
      return Result;
      }
  svector<double> operator*(svector<double> x)
    {//svector<double> &result = *new svector<double>(n);
      svector <double> result(n);
      double z;
      for (int i=0; i<n; i++)
          {svector<double>&v = *rows[i];
          z=0;
          for (int j=0; j<n; j++)
              z+=v[j]*x[j];
          result[i]=z;
          }
      return result;
      }
};


#endif
