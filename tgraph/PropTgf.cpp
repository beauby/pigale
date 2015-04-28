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



#define PRIVATE
#include <TAXI/Tproptgf.h>
#include <TAXI/Tpropio.h>
#undef PRIVATE
#include <TAXI/Tpoint.h>
void WriteTGF(PSet &X,Tgf &file,int PNum)
  {pset_header h;
  h.start = X.start();
  h.finish = X.finish();
  file.FieldWrite(PSetHeaderTag(PNum),(char *)&h, sizeof(h));
  for (int i=X.PStart(); i< X.PEnd(); i++)
      {if (i==128) break;
      if (!X.exist(i)) continue;
      _svector & v = *X[i];
      file.FieldWrite(PSetTag(PNum,i),(char *)v.begin(),v.getsize());
      }
  }
void WriteTGF(PSet1 &X,Tgf &file,int PNum)
  {for (int i=X.PStart(); i< X.PEnd(); i++)
      {if (i==128) break;
      if (!X.exist(i)) continue;
      if (X.declared(i))
          {_svector * exp = X(i)->Export(X[i]);
          file.FieldWrite(PSet1Tag(PNum,i),(char *)(exp->begin()),exp->getsize());
          delete exp;
          }
      else
          {_svector * exp = (_svector *)X[i];
          file.FieldWrite(PSet1Tag(PNum,i),(char *)(exp->begin()),exp->getsize());
          }
      }
  }
void ReadTGF(PSet &X, Tgf &file, int PNum)
  {pset_header h;
  if (!file.FieldRead(PSetHeaderTag(PNum),(char *)&h, sizeof(h)))
      {DebugPrintf("could not readTGF::PNum;%d",PNum);return;
      }
  X.clear();
  X.resize(h.start,h.finish);
  //cout<<"PSET **********************"<<endl;
  for (int i=X.PStart(); i< X.PEnd(); i++)
      {if (i==128) break;
      long size = file.GetTagLength(PSetTag(PNum,i));
      long nelmt = size / (h.finish-h.start+1);
      if (!size) continue;
      _svector v(h.start,h.finish, (int)nelmt);
      file.FieldRead(PSetTag(PNum,i),(char *)v.begin());
      X.swload(i,v);
      }
  }
void ReadTGF(PSet1 &X, Tgf &file, int PNum)
  {X.clear();
  //cout<<"PSET1 ********************** PNum:"<<PNum<<endl;
  for (int i=X.PStart(); i< X.PEnd(); i++)
      {if (i==128) break;
      long size = file.GetTagLength(PSet1Tag(PNum,i));      
      if (!size) continue;
      svector<char> v((int)size);
      file.FieldRead(PSet1Tag(PNum,i),v.begin());
      X.swload(i,v);
      }
  }

