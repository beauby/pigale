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

#ifndef TPROPIO_H
#define TPROPIO_H

#include <TAXI/Tsvector.h>
#include <TAXI/Tbase.h>
#include <TAXI/Tsvector.h>
#include <TAXI/Tstring.h>


// Par defaut, export/import par copie
//

/*
 *  Principe de l'import/export
 *
 * Export prend un pointeur sur un objet du type et renvoie un _svector *
 * obtenu par un new. Seul le buffer du _svector est signifiant.
 *
 * Import recoit un _svector * et l'utilise pour initialiser un objet.
 * Seul le buffer du _svector est signifiant.
 */

//#ifndef _MSC_VER
//    inline void* operator new(size_t, void* p) {return p;}
//#else
//#include <new.h>
//#endif

template <class T>
struct TypeHandler
{
    static void Import(T& obj, const _svector *p) {obj = *(T *)(p->begin());}
    static _svector *Export(const void *pobj)
        {svector<T> *v = new svector<T>(1);
        new (v->begin()) T(*(const T *)pobj);
        return v;
        }
};

template <>
struct TypeHandler<tstring>
{
    static void Import(tstring& obj, const _svector *v)
        {obj = (char *)(v->begin());}
    static _svector *Export(const void *pobj)
        {
        const tstring & obj = *(const tstring *)pobj;
        int l = obj.length();
        svector<char> *v = new svector<char>(l+1);
        
        memcpy(v->begin(), (void *)~obj,l);
        (*v)[l] = '\0';
        return v;
        }
};

template <>
struct TypeHandler<svector<tstring *> >
{
  static void Import(svector<tstring *>& obj, const _svector *v)
  {
    int s,f;
    char *ptr=(char *)(v->begin());
    memcpy((void *)&s,ptr,sizeof(int));ptr+=sizeof(int);
    memcpy((void *)&f,ptr,sizeof(int));ptr+=sizeof(int);
    obj.resize(s,f-1);
    for (int i=s; i<f; i++)
      { if (*ptr++=='\0') obj[i]=(tstring *)0;
	else {
	  obj[i]=new tstring;
	  *obj[i] = (char *)ptr;
	  ptr += obj[i]->length()+1;
	}
      }
  }
  static _svector *Export(const void *pobj)
  {
    const svector<tstring *> & v = *(const svector<tstring *> *)pobj;
    int l = 0;
    int s=v.starti();
    int f=v.stopi();
    for (int i=s; i<f; i++)
      if (v[i]!=(tstring *)0) l+=v[i]->length()+2;
      else l+=1;
    svector<char> *w = new svector<char>(l+2*sizeof(int));
    char *ptr=(char *)(w->begin());
    memcpy(ptr, (void *)&s,sizeof(int));ptr+=sizeof(int);
    memcpy(ptr, (void *)&f,sizeof(int));ptr+=sizeof(int);
    for (int i=s; i<f; i++)
      { if (v[i]==(tstring *)0) *ptr++='\0'; 
	else 
	  {*ptr++='\"';
	    int len=v[i]->length()+1;
	    memcpy(ptr, (void *)~(*v[i]),len); ptr+=len;
	  }
      }
    return w;
  }
};

#endif
