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

/*
Principe : smap<T> effectue un map long -> T sans constructeur!!!
Creation : 
  smap<T> toto; 
  smap<T> toto(size); -> size key allouées au démarrage
Utilisation:
  smap[key]=value; // la valeur associée à "key" (long) est "value" (T)
  smap[key] // -> donne la valeur associée à key (n'importe quoi si
               pas défini)
  ExistingIndexByKey(key) // -> index (int) de la clé "key"/ -1 si
               existe pas
  ValueByIndex(index) // -> référence sur l'élément d'index "index"

  Utilisation simple:

      while...
          key= ..
          value=...
          pigaleMap[key]=value;
          ..
          toto=pigaleMap[autreKey];

  Utilisation avec test:

     int i;
     if ((i=pigaleMap.ExistingIndexByKey(key))>=0)
        pigaleMap.ValueByIndex(i)=...
     else
        ...
*/



#ifndef TMAP_H
#define TMAP_H


#include <TAXI/Tsvector.h>

template <class T>
class smap : public svector<T>
    {
    svector<int> Hash;
    svector<long> Key;
    svector<int> Next;
    int n;

    void init()
        {
        Hash.clear();
        n=0;
        }
    int ComputeHash(long l) 
        {
        return (int) (((l>>24)^(l>>16)^(l>>8)^l)&0xFF);
        }

    
    int IndexByKey(long l)
        {int i=ComputeHash(l);
        int j=Hash[i]-1;
        int k=0;
        
        if (j<0)
            {j=n;
            Hash[i]=++n;
            Key(j)=l;
            Next(j)=-1;
            return j;
            }
        else while (j>=0)
            {k=j;
            if (Key[j]==l) return j;
            else j=Next[j];
            }
        j=Next[k]=n++;
        Key(j)=l;
        Next(j)=-1;
        return j;
        }
    
    svector<T> & me() { return *this;}
    public :
        
        int ExistingIndexByKey(long l)
        { int i=ComputeHash(l);
        int j=Hash[i]-1;
        int k;
        
        if (j<0) return -1;
        else while (j>=0)
            {k=j;
            if (Key[j]==l) return j;
            else j=Next[j];
            }
        return -1;
        }
    smap() : svector<T>(), Hash(256), Key() {init();}
    smap(int nn) : svector<T>(nn),Hash(256), Key(nn) {init();}
    ~smap() {}
    T& operator[] (long i) {return ValueByIndex(IndexByKey(i));}
    T& ValueByIndex(int i) {return me()(i);}
    };

#endif
