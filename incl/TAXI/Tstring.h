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

#ifndef __TSTRING_H__
#define __TSTRING_H__

#include  <TAXI/Tbase.h>

class tstring
  {
  char *p;

  void copy(const char * str, bool del=true)
  { char *tmp = new char[strlen(str) + 1];
    strcpy(tmp,str);
    if (del) delete[] p;
    p = tmp;
  }
  public:
  tstring()
      {p = new char[1];
      *p = '\0';
      }
  tstring(const tstring &x)  {copy(x.p,false);}
  tstring(const char * str) {copy(str,false);}
  tstring(char c, int l=1)
      {p = new char[l + 1];
      memset(p,c,l);
      p[l] = '\0';
      }
  ~tstring()
      {delete[] p;
      }
  tstring & operator =(const tstring &x)
      {
	copy(x.p);
	return *this;
      }
  tstring & operator =(const char * str)
    { copy(str);
      return *this;
    }
  
  friend tstring operator + (tstring const &x, tstring const &y)
      {tstring tmp(x); tmp += y; return tmp;}
  friend tstring operator + (tstring const &x, char const *y)
      {tstring tmp(x); tmp += y; return tmp;}
  friend tstring operator + (tstring const &x, char c)
      {tstring tmp(x); tmp += c; return tmp;}
  friend tstring operator + (char const *x,tstring const &y)
      {tstring tmp(x); tmp += y; return tmp;}
  friend tstring operator + (char c,tstring const &y)
      {tstring tmp(c); tmp += y; return tmp;}
  
  tstring & operator +=(const tstring &x)
      {char *s = new char[length() + x.length()+1];
      strcpy(s,p);
      strcat(s,x.p);
      delete[] p;
      p = s;
      return *this;
      }
  tstring & operator += (char c)
      {operator +=(tstring(c)); return *this;}
  
  operator       char * ()       { return p;}
  operator const char * () const { return p;}
  char * operator ~()       { return p;}
  const char * operator ~() const { return p;}

  char  operator[] (int i) const
      {if(i > length())return '\0';
      return p[i];
      }
  char & operator[] (int i)  
    {if(i > length())return p[length()]; 
      return p[i];
    }

  int length() const {return (int)strlen(p);}
    
  friend int operator == (tstring const &x, tstring const &y)
      {return strcmp(x.p, y.p) == 0;}
  friend int operator == (tstring const &x, char const *y)
      {return strcmp(x.p, y) == 0;}
  friend int operator == (char const *x, tstring const &y)
      {return strcmp(x, y.p) == 0;}
  friend int operator != (tstring const &x, tstring const &y)
      {return strcmp(x.p, y.p);}
  friend int operator != (tstring const &x, char const *y)
      {return strcmp(x.p, y);}
  friend int operator != (char const *x, tstring const &y)
      {return strcmp(x, y.p);}
   
  friend T_STD ostream& operator <<(T_STD ostream &os,const tstring &x)
      {return os << x.p;}
      //{return os << x.p << T_STD endl;}
  friend T_STD istream& operator >> (T_STD istream &is, tstring & x)
      {char buff[256];
      is >> buff;
      x = buff;
      return is;
      }
   
  //substrings
  tstring operator () (int pos) const
    {tstring tmp;
    if(pos < length()) tmp = p + pos;
    else tmp=tstring();
    return tmp;
    }
  tstring operator () (int pos, int l) const
      {tstring tmp;
        if(pos < length())
            {tmp = p + pos;
            if(l+pos < tmp.length())tmp.p[l+1] = '\0';
            }
	else tmp=tstring();
        return tmp;
      }
 
  // Others
  int match(tstring const &x, int pos) const
      {tstring tmp;
      if(pos < length()) tmp = p + pos;
      else return 0;
      if(strcmp(x.p,tmp.p) <= 0)return 1;
      return 0;
      }
  int match(tstring const &x) const
      {int end = length() - x.length();
      
      for(int pos = 0;pos <= end;++pos)
          if(match(x,pos))return pos;
      return -1;
      }
  };
#endif

